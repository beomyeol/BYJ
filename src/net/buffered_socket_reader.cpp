#include "bamboo/net/buffered_socket_reader.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace bamboo {

BufferedSocketReader::BufferedSocketReader(int sock, std::size_t buffer_size)
    : sock_(sock), bytes_available_(0), buffer_size_(buffer_size) {
  buffer_ = new char[buffer_size];
  // set non-blocking flag
  fcntl(sock_, F_SETFL, fcntl(sock_, F_GETFL) | O_NONBLOCK);
}

BufferedSocketReader::~BufferedSocketReader() { delete[] buffer_; }

int BufferedSocketReader::Read(char* buffer, std::size_t buffer_size,
                               std::size_t offset) {
  std::size_t i;
  for (i = 0; (i < buffer_size) && (offset + i) < bytes_available_; ++i) {
    buffer[i] = buffer_[offset + i];
  }
  return i;
}

std::size_t BufferedSocketReader::BytesAvailable() {
  NonBlockRecv();
  return bytes_available_;
}

// skip n bytes.
// return: actual skipped bytes
int BufferedSocketReader::Skip(std::size_t size) {
  int actual_skipped_size;
  actual_skipped_size = (size > buffer_size_) ? buffer_size_ : size;
  memmove(buffer_, buffer_ + actual_skipped_size,
          buffer_size_ - actual_skipped_size);
  bytes_available_ -= actual_skipped_size;
  return actual_skipped_size;
}

std::size_t BufferedSocketReader::NonBlockRecv() {
  int len = recv(sock_, buffer_ + bytes_available_,
                 buffer_size_ - bytes_available_, 0);
  int err = errno;
  if (len < 0) {
    if (!(err == EAGAIN || err == EWOULDBLOCK)) {
      throw SocketError("Failed to recv", err);
    }
  } else if (len == 0) {
    throw SocketError("Connection closed", 0);
  } else {
    bytes_available_ += len;
  }
  return len;
}

}  // namespace bamboo
