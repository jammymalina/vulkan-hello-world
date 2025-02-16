#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <stdio.h>
#include <stdlib.h>

#include "./result.h"
#include "./utils/logger.h"

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

typedef struct ResourceManager {
  SDLResource sdl_resource;
} ResourceManager;

void resource_manager_reset(ResourceManager* resource_manager) {
  resource_manager->sdl_resource.is_sdl_window_init = false;
  resource_manager->sdl_resource.is_sdl_vulkan_init = false;
  resource_manager->sdl_resource.is_sdl_init = false;
}

static Result(int, ErrorMessage) init_SDL(SDLResource* sdl_resource) {
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

void destroy_resources(ResourceManager* resource_manager) {
  if (resource_manager->sdl_resource.is_sdl_window_init) {
    SDL_DestroyWindow(resource_manager->sdl_resource.window);
  }
  if (resource_manager->sdl_resource.is_sdl_init) {
    SDL_Quit();
  }
  resource_manager_reset(resource_manager);
}

int main(int argc, [[maybe_unused]] char* argv[argc + 1]) {
  // Init

  ResourceManager resource_manager = {};
  resource_manager_reset(&resource_manager);

  const auto sdl_result = init_SDL(&resource_manager.sdl_resource);
  if (!sdl_result.is_ok) {
    log_error("Error while initializing SDL: %s", sdl_result.error);
    destroy_resources(&resource_manager);
    return EXIT_FAILURE;
  }

  // Game loop

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
      double delta = lag / MS_PER_UPDATE;
      lag -= MS_PER_UPDATE;
    }

    bool success = true;
    if (!success) {
      is_running = false;
    }
  }

  // Destroy

  destroy_resources(&resource_manager);

  return EXIT_SUCCESS;
}
