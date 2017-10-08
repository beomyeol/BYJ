#include "bamboo/net/incoming_socket.h"

#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstdint>
#include <vector>

namespace bamboo {

IncomingSocket::IncomingSocket(int sock)
    : sock_(sock), reader_(sock), stop_flag_(false), bytes_last_checked_(0) {
  ::shutdown(sock, SHUT_WR);
}

IncomingSocket::~IncomingSocket() {}

std::list<std::string> IncomingSocket::get_messages() {
  std::list<std::string> retval;
  std::string temp;
  while (queue_.try_pop(temp) == true) {
    retval.push_back(temp);
  }
  return retval;
}

bool IncomingSocket::get_message(std::string& out) {
  return queue_.try_pop(out);
}

void IncomingSocket::Run() {
  while (!stop_flag_) {
    try {
      std::size_t available = reader_.BytesAvailable();
      if (available < sizeof(int32_t) || available == bytes_last_checked_) {
        usleep(10000);  // 10 ms
      } else {
        // Read size
        int32_t len;
        reader_.Read(reinterpret_cast<char*>(std::addressof(len)),
                     sizeof(int32_t));
        std::size_t msg_bytes = sizeof(int32_t) + len;
        if (available < msg_bytes) {
          continue;
        }
        // Read content
        std::vector<char> buffer;
        buffer.resize(len);
        reader_.Read(&buffer[0], buffer.size(), sizeof(int32_t));
        queue_.push(std::string(buffer.begin(), buffer.end()));
        reader_.Skip(msg_bytes);
        bytes_last_checked_ = available - msg_bytes;
      }
    } catch (SocketError const& e) {
      cleanshutdown();
    }
  }
  Shutdown();
}

void IncomingSocket::cleanshutdown() { stop_flag_ = true; }

void IncomingSocket::Shutdown() {
  ::shutdown(sock_, SHUT_RD);
  close(sock_);
}

bool IncomingSocket::is_stopped() const { return stop_flag_; }

}  // namespace bamboo
