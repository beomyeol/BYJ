#include "byj/logger.h"
#include <sstream>

using namespace byj;

Logger::Logger(log_level level, std::ostream& os)
: os_ (os)
, level_ (level) {
}

void Logger::log(log_level level, const std::string& log_str) {
  if(level <= level_) {
    os_ << log_str << std::endl;
    flush();
  }
}

void Logger::flush() {
  os_.flush();
}
