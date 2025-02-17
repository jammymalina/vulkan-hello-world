#include "function_loader.h"

#include <stddef.h>

#include "./functions.h"

#define EXPORTED_VULKAN_FUNCTION(name) PFN_##name name = NULL;
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) PFN_##name name = NULL;
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name = NULL;
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name) \
  PFN_##name name = NULL;
#define DEVICE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name = NULL;
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) \
  PFN_##name name = NULL;

#include "function_list.inl"

Result(int, ErrorMessage)
    vulkan_load_external_function(PFN_vkGetInstanceProcAddr vk_get_proc) {
  if (!vk_get_proc) {
    return Err(int, ErrorMessage)("Unable to get vulkan proc address");
  }
  vkGetInstanceProcAddr = vk_get_proc;
  return Ok(int, ErrorMessage)(0);
}

Result(int, ErrorMessage) vulkan_load_global_functions() {
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                    \
  name = (PFN_##name)vkGetInstanceProcAddr(nullptr, #name);                   \
  if (!name) {                                                                \
    return Err(int,                                                           \
               ErrorMessage)("Could not load global level function: " #name); \
  }

#include "function_list.inl"

  return Ok(int, ErrorMessage)(0);
}

Result(int, ErrorMessage) vulkan_load_instance_functions(VkInstance instance) {
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                  \
  name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                  \
  if (!name) {                                                                \
    return Err(                                                               \
        int, ErrorMessage)("Could not load instance level function: " #name); \
  }

#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name)                   \
  name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                  \
  if (!name) {                                                                \
    return Err(                                                               \
        int, ErrorMessage)("Could not load instance level function: " #name); \
  }

#include "function_list.inl"

  return Ok(int, ErrorMessage)(0);
}

Result(int, ErrorMessage) vulkan_load_device_functions(VkDevice device) {
#define DEVICE_LEVEL_VULKAN_FUNCTION(name)                                    \
  name = (PFN_##name)vkGetDeviceProcAddr(device, #name);                      \
  if (!name) {                                                                \
    return Err(int,                                                           \
               ErrorMessage)("Could not load device level function: " #name); \
  }

#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)          \
  name = (PFN_##name)vkGetDeviceProcAddr(device, #name);                      \
  if (!name) {                                                                \
    return Err(int,                                                           \
               ErrorMessage)("Could not load device level function: " #name); \
  }

#include "function_list.inl"

  return Ok(int, ErrorMessage)(0);
}
