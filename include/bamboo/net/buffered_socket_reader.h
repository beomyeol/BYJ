#ifndef BAMBOO_NET_BUFFERED_SOCKET_READER_H_
#define BAMBOO_NET_BUFFERED_SOCKET_READER_H_

#include "socket_error.h"

namespace bamboo {

class BufferedSocketReader {
 public:
  BufferedSocketReader(int sock, std::size_t buffer_size = 8192);
  ~BufferedSocketReader();

  int Read(char* buffer, std::size_t buffer_size, std::size_t offset = 0);

  std::size_t BytesAvailable();
  int Skip(std::size_t size);

 private:
  std::size_t NonBlockRecv();

  const int sock_;
  std::size_t bytes_available_;
  char* buffer_;
  std::size_t buffer_size_;
};

}  // namespace bamboo

#endif  // BAMBOO_NET_BUFFERED_SOCKET_READER_H_
