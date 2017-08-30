#include "bamboo/net/buffered_socket_reader.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace bamboo {

BufferedSocketReader::BufferedSocketReader(int sock, std::size_t buffer_size)
    : sock_(sock), available_(0), buffer_size_(buffer_size) {
  buffer_ = new char[buffer_size];
  // set non-blocking flag
  fcntl(sock_, F_SETFL, fcntl(sock_, F_GETFL) | O_NONBLOCK);
}

BufferedSocketReader::~BufferedSocketReader() { delete[] buffer_; }

int BufferedSocketReader::read(char* buffer, std::size_t buffer_size,
                               std::size_t offset) {
  std::size_t i;
  for (i = 0; (i < buffer_size) && (offset + i) < available_; ++i) {
    buffer[i] = buffer_[offset + i];
  }
  return i;
}

std::size_t BufferedSocketReader::available() {
  nblock_recv();
  return available_;
}

// skip n bytes.
// return: actual skipped bytes
int BufferedSocketReader::skip(std::size_t size) {
  int actual_skipped_size;
  actual_skipped_size = (size > buffer_size_) ? buffer_size_ : size;
  memmove(buffer_, buffer_ + actual_skipped_size,
          buffer_size_ - actual_skipped_size);
  available_ -= actual_skipped_size;
  return actual_skipped_size;
}

std::size_t BufferedSocketReader::nblock_recv() {
  int len = recv(sock_, buffer_ + available_, buffer_size_ - available_, 0);
  int err = errno;
  if (len < 0) {
    if (!(err == EAGAIN || err == EWOULDBLOCK)) {
      throw SocketError("Failed to recv", err);
    }
  } else if (len == 0) {
    throw SocketError("Connection closed", 0);
  } else {
    available_ += len;
  }
  return len;
}

}  // namespace bamboo
