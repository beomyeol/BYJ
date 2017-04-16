#include "bamboo/net/incoming_socket.h"

#include <sys/socket.h>
#include <unistd.h>

#include <vector>
#include <algorithm>

using namespace bamboo;

const char IncomingSocket::MSG_SEP;

IncomingSocket::IncomingSocket(int sock)
: sock_ (sock)
, reader_ (sock)
, stop_flag_ (false)
, bytes_last_checked (0) {
  ::shutdown(sock, SHUT_WR);
}

IncomingSocket::~IncomingSocket() {

}

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

void IncomingSocket::run() {
  while (!stop_flag_) {
    try {
      std::size_t available = reader_.available();
      if (available == bytes_last_checked) {
        usleep(10000); // 10 ms
      } else {
        std::vector<char> buffer;
        buffer.resize(available);
        reader_.read(&buffer[0], buffer.size(), 0);
        std::vector<char>::iterator msg_begin = buffer.begin();
        std::vector<char>::iterator msg_end = std::find(msg_begin, buffer.end(), MSG_SEP);
        while (msg_end != buffer.end()) {
          queue_.push(std::string(msg_begin, msg_end));
          msg_begin = ++msg_end;
          msg_end = std::find(msg_begin, buffer.end(), MSG_SEP);
        }
        std::size_t readbytes = std::distance(buffer.begin(), msg_begin);
        reader_.skip(readbytes);
        bytes_last_checked = available - readbytes;
      }
    } catch (SocketError const& e) {
      cleanshutdown();
    }

  }
  shutdown();
}

void IncomingSocket::cleanshutdown() {
  stop_flag_ = true;
}

void IncomingSocket::shutdown() {
  ::shutdown(sock_, SHUT_RD);
  close(sock_);
}

bool IncomingSocket::is_stopped() const {
  return stop_flag_;
}
