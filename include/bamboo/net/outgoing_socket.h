#ifndef BAMBOO_NET_OUTGOING_SOCKET_H_
#define BAMBOO_NET_OUTGOING_SOCKET_H_

#include <mutex>

#include "bamboo/net/socket_error.h"

namespace bamboo {

class OutgoingSocket {
 public:
  OutgoingSocket(int sock);

  void send_message(const std::string& msg);

  void cleanshutdown();

 private:
  const int sock_;
  std::mutex mtx_;
};

}  // namespace bamboo

#endif
