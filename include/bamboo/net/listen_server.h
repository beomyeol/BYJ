#ifndef __BAMBOO_NET_LISTEN_SERVER__
#define __BAMBOO_NET_LISTEN_SERVER__

#include <tbb/concurrent_vector.h>
#include <memory>
#include <vector>

#include "bamboo/net/config.h"
#include "bamboo/net/incoming_socket.h"
#include "bamboo/thread_interface.h"

namespace bamboo {

class ListenServer : public ThreadInterface {
 public:
  typedef std::shared_ptr<IncomingSocket> IncomingSocketSptr;
  typedef tbb::concurrent_vector<IncomingSocketSptr> IncomingSocketSptrs;

  ListenServer(std::shared_ptr<Config> conf, IncomingSocketSptrs& insocks);
  ~ListenServer();

  virtual void run();
  bool is_stopped() const;
  void cleanshutdown();

  unsigned short get_port() const;

 private:
  volatile bool stop_flag_;
  std::shared_ptr<Config> conf_;
  IncomingSocketSptrs& insocks_;
  int sock_;
  unsigned short port_;
};

}  // namespace bamboo

#endif
