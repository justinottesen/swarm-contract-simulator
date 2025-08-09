#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <mutex>
#include <sstream>
#include <thread>

#include <macros.hpp>
#include <result.hpp>
#include <singleton.hpp>

#define LOG(LEVEL)                                                             \
  if (false)                                                                   \
    ;                                                                          \
  else                                                                         \
    Logger(LogLevel::LEVEL, __FILE__, __LINE__, __func__)

ENUM(LogLevel, uint8_t, FINE, DEBUG, INFO, WARN, ERROR, FATAL);

struct LogMessage {
  LogLevel level;
  std::string msg;
  std::string_view file;
  int line;
  std::string_view func;
  std::chrono::system_clock::time_point timestamp;
  std::thread::id thread;
};

// TODO: Refactor to use a thread pool for dispatching handler work
class LogManager : public Singleton<LogManager> {
public:
  DEFAULT_CTOR_DTOR(LogManager);
  DELETE_COPY_MOVE(LogManager);

  using Handler = std::function<void(const LogMessage &)>;

  auto submit(const LogMessage &msg) noexcept -> void {
    std::scoped_lock lock(m_lock);
    for (const auto &handler : m_handlers) {
      handler(msg);
    }
  }

  auto addHandler(auto &&handler) noexcept -> Handler & {
    std::scoped_lock lock(m_lock);
    return m_handlers.emplace_back(std::forward<Handler>(handler));
  }

private:
  std::vector<Handler> m_handlers;
  std::mutex m_lock;
};

class Logger {
public:
  DELETE_CTOR_COPY_MOVE(Logger);
  Logger(LogLevel level, std::string_view file, int line, std::string_view func)
      : m_msg{.level = level,
              .msg = {},
              .file = file,
              .line = line,
              .func = func,
              .timestamp = std::chrono::system_clock::now(),
              .thread = std::this_thread::get_id()} {}

  ~Logger() {
    m_msg.msg = m_stream.str();
    LogManager::instance().submit(m_msg);
  }

  auto operator<<(const auto &value) -> Logger & {
    m_stream << value;
    return *this;
  }

private:
  std::ostringstream m_stream;
  LogMessage m_msg;
};
