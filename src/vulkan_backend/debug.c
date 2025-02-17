#include "./debug.h"

#include <stddef.h>
#include <string.h>

#include "../utils/logger.h"
#include "../utils/memory.h"
#include "./functions.h"

const char* vulkan_result_to_string(VkResult result) {
  switch (result) {
    case VK_SUCCESS:
      return "VK_SUCCESS";
    case VK_NOT_READY:
      return "VK_NOT_READY";
    case VK_TIMEOUT:
      return "VK_TIMEOUT";
    case VK_EVENT_SET:
      return "VK_EVENT_SET";
    case VK_EVENT_RESET:
      return "VK_EVENT_RESET";
    case VK_INCOMPLETE:
      return "VK_INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED:
      return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST:
      return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED:
      return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT:
      return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT:
      return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS:
      return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
      return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL:
      return "VK_ERROR_FRAGMENTED_POOL";
    case VK_ERROR_UNKNOWN:
      return "VK_ERROR_UNKNOWN";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
      return "VK_ERROR_OUT_OF_POOL_MEMORY";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
      return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
    case VK_ERROR_FRAGMENTATION:
      return "VK_ERROR_FRAGMENTATION";
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
      return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
    case VK_PIPELINE_COMPILE_REQUIRED:
      return "VK_PIPELINE_COMPILE_REQUIRED";
    case VK_ERROR_NOT_PERMITTED:
      return "VK_ERROR_NOT_PERMITTED";
    case VK_ERROR_SURFACE_LOST_KHR:
      return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
      return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR:
      return "VK_SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR:
      return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
      return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT:
      return "VK_ERROR_VALIDATION_FAILED_EXT";
    case VK_ERROR_INVALID_SHADER_NV:
      return "VK_ERROR_INVALID_SHADER_NV";
    case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
      return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
      return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
      return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
      return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
      return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
      return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
      return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
      return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
    case VK_THREAD_IDLE_KHR:
      return "VK_THREAD_IDLE_KHR";
    case VK_THREAD_DONE_KHR:
      return "VK_THREAD_DONE_KHR";
    case VK_OPERATION_DEFERRED_KHR:
      return "VK_OPERATION_DEFERRED_KHR";
    case VK_OPERATION_NOT_DEFERRED_KHR:
      return "VK_OPERATION_NOT_DEFERRED_KHR";
    case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
      return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
    case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
      return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
    case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
      return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
    case VK_PIPELINE_BINARY_MISSING_KHR:
      return "VK_PIPELINE_BINARY_MISSING_KHR";
    case VK_ERROR_NOT_ENOUGH_SPACE_KHR:
      return "VK_ERROR_NOT_ENOUGH_SPACE_KHR";
    case VK_RESULT_MAX_ENUM:
      return "VK_RESULT_MAX_ENUM";
    default:
      return "VK_ERROR_<Unknown>";
  }
}

bool vulkan_layers_is_layer_available(const char* name,
                                      VkLayerProperties* available_layers,
                                      uint32_t layers_size) {
  if (layers_size == 0 || available_layers == nullptr) {
    return false;
  }
  for (size_t i = 0; i < layers_size; i++) {
    if (strcmp(name, available_layers[i].layerName) == 0) {
      return true;
    }
  }

  return false;
}

bool vulkan_layers_requested_layers_in_available(
    const char** names,
    uint32_t names_size,
    VkLayerProperties* available_layers,
    uint32_t layers_size) {
  if (names_size == 0) {
    return true;
  }
  if (layers_size == 0 || available_layers == nullptr) {
    return false;
  }
  bool success = true;
  for (size_t i = 0; i < names_size && success; i++) {
    success &= vulkan_layers_is_layer_available(names[i], available_layers,
                                                layers_size);
  }

  return success;
}

bool vulkan_layers_all_available(const char** names, uint32_t names_size) {
  uint32_t layers_size = 0;
  VkLayerProperties* available_layers =
      vulkan_layers_get_available_validation_layers(&layers_size);
  bool success = vulkan_layers_requested_layers_in_available(
      names, names_size, available_layers, layers_size);
  mem_free(available_layers);

  return success;
}

VkLayerProperties* vulkan_layers_get_available_validation_layers(
    uint32_t* layers_size) {
  *layers_size = 0;
  uint32_t count = 0;
  VkResult r = vkEnumerateInstanceLayerProperties(&count, nullptr);

  if (r != VK_SUCCESS) {
    log_error("Unable to get validation layers: %s",
              vulkan_result_to_string(r));
    return nullptr;
  }
  if (count == 0) {
    return nullptr;
  }

  VkLayerProperties* validation_layers =
      mem_alloc(count * sizeof(VkLayerProperties));
  r = vkEnumerateInstanceLayerProperties(&count, validation_layers);

  if (!validation_layers) {
    log_error("Unable to allocate validation layers");
    return nullptr;
  }

  if (r != VK_SUCCESS || count == 0) {
    log_error("Unable to get validation layers: %s",
              vulkan_result_to_string(r));
    mem_free(validation_layers);
    return nullptr;
  }

  *layers_size = count;
  return validation_layers;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
vulkan_debug_callback([[maybe_unused]] VkDebugReportFlagsEXT flags,
                      [[maybe_unused]] VkDebugReportObjectTypeEXT objType,
                      [[maybe_unused]] uint64_t obj,
                      [[maybe_unused]] size_t location,
                      [[maybe_unused]] int32_t code,
                      const char* layer_prefix,
                      const char* msg,
                      [[maybe_unused]] void* userData) {
  log_info("VALIDATION LAYER %s: %s", layer_prefix, msg);
  return VK_FALSE;
}
