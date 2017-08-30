#ifndef __BAMBOO_NET_INCOMING_SOCKET__
#define __BAMBOO_NET_INCOMING_SOCKET__

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
  void shutdown();
  virtual void run();

 private:
  const static char MSG_SEP = '&';
  int sock_;
  BufferedSocketReader reader_;
  volatile bool stop_flag_;
  tbb::concurrent_queue<std::string> queue_;
  std::size_t bytes_last_checked;
};

}  // namespace bamboo

#endif
