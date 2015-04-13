#ifndef __LOGGER__
#define __LOGGER__

#include <fstream>
#include <iostream>

#include "boost/lexical_cast.hpp"

namespace byj {

template<typename Source>
std::string to_string(const Source& arg) {
  return boost::lexical_cast<std::string>(arg);
}

class Logger {
public:
  enum log_level {
    FATAL = 0,
    WARNING = 1,
    INFO = 2
  };

  Logger(log_level level = FATAL, std::ostream& os = std::cout);

  void log(log_level type, const std::string& log_str);

  void flush();

private:
  std::ostream& os_;
  log_level level_;
};

}

#endif
