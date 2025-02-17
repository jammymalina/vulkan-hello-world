#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

#include "./result.h"
#include "./utils/logger.h"
#include "./utils/memory.h"
#include "./vulkan_backend/debug.h"
#include "./vulkan_backend/function_loader.h"
#include "./vulkan_backend/functions.h"

#define MS_PER_UPDATE 16

typedef struct SDLResource {
  SDL_DisplayMode display_mode;
  SDL_Window* window;
  int drawable_width;
  int drawable_height;
  bool is_sdl_window_init;
  bool is_sdl_vulkan_init;
  bool is_sdl_init;
} SDLResource;

Result(int, ErrorMessage) sdl_resource_init(SDLResource* sdl_resource) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    return Err(int, ErrorMessage)(SDL_GetError());
  }
  sdl_resource->is_sdl_init = true;

  if (SDL_Vulkan_LoadLibrary(NULL) != 0) {
    return Err(int, ErrorMessage)(SDL_GetError());
  }
  sdl_resource->is_sdl_vulkan_init = true;

  sdl_resource->window = SDL_CreateWindow(
      "Hello Vulkan!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280,
      720, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
  if (!sdl_resource->window) {
    return Err(int, ErrorMessage)(SDL_GetError());
  }
  sdl_resource->is_sdl_window_init = true;
  SDL_GetCurrentDisplayMode(0, &sdl_resource->display_mode);
  SDL_Vulkan_GetDrawableSize(sdl_resource->window,
                             &sdl_resource->drawable_width,
                             &sdl_resource->drawable_height);

  log_info("Drawable Size: %d, %d", sdl_resource->drawable_width,
           sdl_resource->drawable_height);
  log_info("Screen BPP: %d",
           SDL_BITSPERPIXEL(sdl_resource->display_mode.format));

  return Ok(int, ErrorMessage)(0);
}

void sdl_resource_reset(SDLResource* sdl_resource) {
  sdl_resource->is_sdl_window_init = false;
  sdl_resource->is_sdl_vulkan_init = false;
  sdl_resource->is_sdl_init = false;
}

void sdl_resource_destroy(SDLResource* sdl_resource) {
  if (sdl_resource->is_sdl_window_init) {
    SDL_DestroyWindow(sdl_resource->window);
  }
  if (sdl_resource->is_sdl_init) {
    SDL_Quit();
  }
}

typedef struct VulkanResource {
  VkInstance instance;
  bool is_instance_init;
} VulkanResource;

Result(int, ErrorMessage)
    vulkan_resource_init(VulkanResource* vk_resource,
                         const SDLResource* sdl_resource) {
  PFN_vkGetInstanceProcAddr vk_get_proc = SDL_Vulkan_GetVkGetInstanceProcAddr();
  if (!vk_get_proc) {
    return Err(int, ErrorMessage)(SDL_GetError());
  }
  auto load_result = vulkan_load_external_function(vk_get_proc);
  if (!load_result.is_ok) {
    return load_result;
  }
  load_result = vulkan_load_global_functions();
  if (!load_result.is_ok) {
    return load_result;
  }

  const char** extensions = nullptr;
  unsigned extension_count = 0;

  if (!SDL_Vulkan_GetInstanceExtensions(sdl_resource->window, &extension_count,
                                        nullptr)) {
    return Err(int, ErrorMessage)(SDL_GetError());
  }

  uint32_t debug_extension_count = 0;
#ifdef DEBUG
  debug_extension_count = 1;
#endif

  extensions = mem_alloc(sizeof(const char*) *
                         (extension_count + debug_extension_count));
  CHECK_ALLOC(extensions, Err(int, ErrorMessage)(
                              "Unable to allocate memory for extensions"));

  if (!SDL_Vulkan_GetInstanceExtensions(sdl_resource->window, &extension_count,
                                        extensions)) {
    return Err(int, ErrorMessage)(SDL_GetError());
  }

  // push debug extension + enable validation layers
#ifdef DEBUG
  extension_count += debug_extension_count;

  const char* validation_layer_names[] = {"VK_LAYER_KHRONOS_validation"};
  uint32_t validation_layers_size = 1;
  extensions[extension_count - 1] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;

  if (!vulkan_layers_all_available(validation_layer_names,
                                   validation_layers_size)) {
    log_error("Unavailable validation layers");
    validation_layers_size = 0;
  }

#else
  const char* validation_layer_names[] = {};
  uint32_t validation_layers_size = 0;
#endif

  VkApplicationInfo vk_application_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = "Hello Vulkan!",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "Jammy Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_3,
  };

  VkInstanceCreateInfo vk_instance_create_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .pApplicationInfo = &vk_application_info,
      .enabledLayerCount = validation_layers_size,
      .ppEnabledLayerNames = validation_layer_names,
      .enabledExtensionCount = extension_count,
      .ppEnabledExtensionNames = extensions,
  };

  VkResult result = vkCreateInstance(&vk_instance_create_info, nullptr,
                                     &vk_resource->instance);

  if (result != VK_SUCCESS || vk_resource->instance == VK_NULL_HANDLE) {
    mem_free(extensions);
    return Err(int, ErrorMessage)(vulkan_result_to_string(result));
  }

  vulkan_load_instance_functions(vk_resource->instance);
  vk_resource->is_instance_init = true;
  log_debug("Initialized Vulkan instance");

  mem_free(extensions);

  return Ok(int, ErrorMessage)(0);
}

void vulkan_resource_reset(VulkanResource* vk_resource) {
  vk_resource->is_instance_init = false;
}

void vulkan_resource_destroy(VulkanResource* vk_resource) {
  if (vk_resource->is_instance_init) {
    vkDestroyInstance(vk_resource->instance, nullptr);
  }
}

typedef struct ResourceManager {
  SDLResource sdl_resource;
  VulkanResource vk_resource;
} ResourceManager;

void resource_manager_reset(ResourceManager* resource_manager) {
  vulkan_resource_reset(&resource_manager->vk_resource);
  sdl_resource_reset(&resource_manager->sdl_resource);
}

void resource_manager_destroy_resources(ResourceManager* resource_manager) {
  vulkan_resource_destroy(&resource_manager->vk_resource);
  sdl_resource_destroy(&resource_manager->sdl_resource);
  resource_manager_reset(resource_manager);
}

int main(int argc, [[maybe_unused]] char* argv[argc + 1]) {
#ifdef DEBUG
  SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
#endif

  // Init
  ResourceManager resource_manager = {0};
  resource_manager_reset(&resource_manager);

  auto sdl_result = sdl_resource_init(&resource_manager.sdl_resource);
  if (!sdl_result.is_ok) {
    log_error("Error while initializing SDL: %s", sdl_result.error);
    resource_manager_destroy_resources(&resource_manager);
    return EXIT_FAILURE;
  }

  auto vk_result = vulkan_resource_init(&resource_manager.vk_resource,
                                        &resource_manager.sdl_resource);
  if (!vk_result.is_ok) {
    log_error("Error while initializing Vulkan: %s", vk_result.error);
    resource_manager_destroy_resources(&resource_manager);
    return EXIT_FAILURE;
  }

  // Render loop
  bool is_running = true;

  uint32_t previous_time = SDL_GetTicks();
  double lag = 0.0;
  while (is_running) {
    uint32_t current_time = SDL_GetTicks();
    uint32_t elapsed_time = current_time - previous_time;

    previous_time = current_time;
    lag += elapsed_time;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          is_running = false;
          break;
        case SDL_KEYUP:
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            is_running = false;
          }
          break;
      }
    }

    while (lag >= MS_PER_UPDATE) {
      [[maybe_unused]] const double delta = lag / MS_PER_UPDATE;
      lag -= MS_PER_UPDATE;
    }

    bool success = true;
    if (!success) {
      is_running = false;
    }
  }

  // Destroy
  resource_manager_destroy_resources(&resource_manager);

  return EXIT_SUCCESS;
}
