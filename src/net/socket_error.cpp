#include "byj/net/socket_error.h"
#include <sstream>
#include <string.h>

using namespace byj;

SocketError::SocketError(const char* msg, int err) {
    std::ostringstream oss;
    oss << msg << ": " << strerror(err);
    msg_ = oss.str();
}

SocketError::~SocketError() throw() {

}

const char* SocketError::what() const throw() {
    return msg_.c_str();
}
