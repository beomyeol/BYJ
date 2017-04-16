#ifndef BAMBOO_NET_OUTGOING_SOCKET_H
#define BAMBOO_NET_OUTGOING_SOCKET_H

#include <mutex>

#include "bamboo/net/socket_error.h"

namespace bamboo {

class OutgoingSocket {
public:
  OutgoingSocket(int sock);

  void send_message(const std::string& msg);

  void cleanshutdown();

private:
  const static char MSG_SEP = '&';
  int sock_;
  std::mutex mtx_;
};

}

#endif
