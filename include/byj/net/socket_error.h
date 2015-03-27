#ifndef __BYJ_NET_SOCKET_ERROR__
#define __BYJ_NET_SOCKET_ERROR__

#include <exception>
#include <string>

namespace byj {

class SocketError: public std::exception {
public:
    SocketError(const char* msg, int err);
    virtual ~SocketError() throw();
    virtual const char* what() const throw();

private:
    std::string msg_;
};

}
#endif
