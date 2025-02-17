#ifndef VULKAN_BACKEND_FUNCTION_LOADER_H
#define VULKAN_BACKEND_FUNCTION_LOADER_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../result.h"

Result(int, ErrorMessage)
    vulkan_load_external_function(PFN_vkGetInstanceProcAddr vk_get_proc);
Result(int, ErrorMessage) vulkan_load_global_functions();
Result(int, ErrorMessage) vulkan_load_instance_functions(VkInstance instance);
Result(int, ErrorMessage) vulkan_load_device_functions(VkDevice device);

#endif
