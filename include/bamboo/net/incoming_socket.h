#ifndef BAMBOO_NET_INCOMING_SOCKET_H_
#define BAMBOO_NET_INCOMING_SOCKET_H_

#include <tbb/concurrent_queue.h>
#include <list>
#include <string>

#include "bamboo/net/buffered_socket_reader.h"
#include "bamboo/thread_interface.h"

namespace bamboo {

class IncomingSocket : public ThreadInterface {
 public:
  IncomingSocket(int sock);
  virtual ~IncomingSocket();

  std::list<std::string> get_messages();
  bool get_message(std::string& out);
  void cleanshutdown();
  bool is_stopped() const;

 protected:
  void Shutdown();
  virtual void Run();

 private:
  int sock_;
  BufferedSocketReader reader_;
  volatile bool stop_flag_;
  tbb::concurrent_queue<std::string> queue_;
  std::size_t bytes_last_checked_;
};

}  // namespace bamboo

#endif  // BAMBOO_NET_INCOMING_SOCKET_H_
