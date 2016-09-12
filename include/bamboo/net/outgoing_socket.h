#ifndef __BAMBOO_NET_OUTGOING_SOCKET__
#define __BAMBOO_NET_OUTGOING_SOCKET__

#include "socket_error.h"
#include "boost/thread/mutex.hpp"

namespace bamboo {

class OutgoingSocket {
public:
  OutgoingSocket(int sock);

  void send_message(const std::string& msg);

  void cleanshutdown();

private:
  const static char MSG_SEP = '&';
  int sock_;
  boost::mutex mtx_;
};

}

#endif
