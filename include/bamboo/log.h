#ifndef BAMBOO_LOG_H_
#define BAMBOO_LOG_H_

#include <ostream>

#define LOG(level) LOG_##level

#define LOG_FATAL bamboo::LogMessageFatal(__FILE__, __LINE__, __func__).stream()
#define LOG_WARNING \
  bamboo::LogMessageWarning(__FILE__, __LINE__, __func__).stream()
#define LOG_INFO bamboo::LogMessageInfo(__FILE__, __LINE__, __func__).stream()

namespace bamboo {

class LogMessage {
 public:
  enum Level { FATAL = 0, WARNING = 1, INFO = 2 };

  LogMessage(const char* file, int line, const char* func);
  ~LogMessage();

  std::ostream& stream() { return stream_; }

 private:
  std::ostream& stream_;
};

class LogMessageInfo : public LogMessage {
 public:
  LogMessageInfo(const char* file, int line, const char* func);
};

class LogMessageWarning : public LogMessage {
 public:
  LogMessageWarning(const char* file, int line, const char* func);
};

class LogMessageFatal : public LogMessage {
 public:
  LogMessageFatal(const char* file, int line, const char* func);
  ~LogMessageFatal();
};

}  // namespace bamboo

#endif
