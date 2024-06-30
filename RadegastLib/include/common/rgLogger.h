#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>
#include <stdexcept>

#define LOGI(...) do { spdlog::info(__VA_ARGS__); } while (0)
#define LOGW(...) do { spdlog::warn(__VA_ARGS__); } while (0)
#define LOGE(...) do { spdlog::error(__VA_ARGS__); } while (0)

#define RT_THROW(msg) throw std::runtime_error(msg)

#define VK_CHECK(x, msg) if ((x) != VK_SUCCESS) { RT_THROW(msg); }

#endif //LOGGER_H
