#ifndef VULKAN_BACKEND_DEBUG_H
#define VULKAN_BACKEND_DEBUG_H

#include <stdint.h>
#include <vulkan/vulkan.h>

const char* vulkan_result_to_string(VkResult result);

bool vulkan_layers_all_available(const char** names, uint32_t names_size);
VkLayerProperties* vulkan_layers_get_available_validation_layers(
    uint32_t* layers_size);

VKAPI_ATTR VkBool32 VKAPI_CALL
vulkan_debug_callback(VkDebugReportFlagsEXT flags,
                      VkDebugReportObjectTypeEXT objType,
                      uint64_t obj,
                      size_t location,
                      int32_t code,
                      const char* layer_prefix,
                      const char* msg,
                      void* userData);

#endif
