// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <algorithm>
#include <fstream>
#include <optional>
#include <set>
#include <string>
#include <vector>


#include "framework.h"
#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>


#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>


#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/ext/scalar_common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#endif//PCH_H
