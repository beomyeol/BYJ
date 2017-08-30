#ifndef __BAMBOO_NET_BUFFERED_SOCKET_READER__
#define __BAMBOO_NET_BUFFERED_SOCKET_READER__

#include "socket_error.h"

namespace bamboo {

class BufferedSocketReader {
 public:
  BufferedSocketReader(int sock, std::size_t buffer_size = 8192);
  ~BufferedSocketReader();

  int read(char* buffer, std::size_t buffer_size, std::size_t offset = 0);

  std::size_t available();
  int skip(std::size_t size);

 private:
  std::size_t nblock_recv();

  int sock_;
  std::size_t available_;
  char* buffer_;
  std::size_t buffer_size_;
};

}  // namespace bamboo

#endif
