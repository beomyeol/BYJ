#include "bamboo/log.h"

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace bamboo {

LogMessage::LogMessage(const char* file, int line, const char* func)
    : stream_(std::cerr) {
  std::time_t t = std::time(nullptr);
  stream_ << std::put_time(std::localtime(&t), "%F %T ") << "[" << func
          << "() in " << file << ":" << line << "] ";
}

LogMessage::~LogMessage() {
  stream() << std::endl;
  stream().flush();
}

LogMessageInfo::LogMessageInfo(const char* file, int line, const char* func)
    : LogMessage(file, line, func) {
  stream() << "(INFO) ";
}

LogMessageWarning::LogMessageWarning(const char* file, int line,
                                     const char* func)
    : LogMessage(file, line, func) {
  stream() << "(WARNING) ";
}

LogMessageFatal::LogMessageFatal(const char* file, int line, const char* func)
    : LogMessage(file, line, func) {
  stream() << "(FATAL) ";
}

LogMessageFatal::~LogMessageFatal() {
  stream() << std::endl;
  stream().flush();
  abort();
}

}  // namespace bamboo
