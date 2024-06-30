#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>

#define LOGI(...) do { spdlog::info(__VA_ARGS__); } while (0)
#define LOGW(...) do { spdlog::warn(__VA_ARGS__); } while (0)
#define LOGE(...) do { spdlog::error(__VA_ARGS__); } while (0)

#endif //LOGGER_H
