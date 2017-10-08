#ifndef BAMBOO_NET_NETCONTROLLER_H_
#define BAMBOO_NET_NETCONTROLLER_H_

#include <tbb/concurrent_vector.h>
#include <list>
#include <memory>
#include <mutex>
#include <vector>

#include "bamboo/net/config.h"
#include "bamboo/net/incoming_socket.h"
#include "bamboo/net/listen_server.h"
#include "bamboo/net/outgoing_socket.h"

namespace bamboo {

class NetController {
 public:
  NetController(std::shared_ptr<Config> conf);

  typedef std::string Message;
  typedef std::list<Message> Messages;

  Messages get_recv_messages();
  bool get_recv_message(Message& out);
  bool send_message(int procid, const Message& msg);
  void shutdown();

  bool is_listening() const;
  unsigned short get_listening_port() const;

 private:
  bool init_outgoing_conn(int procid);

  std::shared_ptr<Config> conf_;
  std::mutex mtx_;
  ListenServer listener_;

  typedef std::shared_ptr<IncomingSocket> IncomingSocketStpr;
  typedef tbb::concurrent_vector<IncomingSocketStpr> IncomingSocketSptrs;
  IncomingSocketSptrs insocks_;

  typedef std::shared_ptr<OutgoingSocket> OutgoingSocketSptr;
  typedef std::vector<OutgoingSocketSptr> OutgoingSocketSptrs;
  OutgoingSocketSptrs outsocks_;
};

}  // namespace bamboo

#endif  // BAMBOO_NET_NETCONTROLLER_H_
