#include "bamboo/net/net_controller.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/format.hpp>
#include <csignal>
#include <cstring>
#include <sstream>
#include <stdexcept>  // std::runtime_error
#include <string>

#include "bamboo/log.h"

namespace bamboo {

NetController::NetController(std::shared_ptr<Config> conf)
    : conf_(conf),
      listener_(conf, insocks_),
      outsocks_(conf->get_num_of_processes()) {
  listener_.Start();
}

NetController::Messages NetController::get_recv_messages() {
  std::lock_guard<std::mutex> lock(mtx_);
  Messages total_msgs;

  for (IncomingSocketSptrs::iterator iter = insocks_.begin();
       iter != insocks_.end(); ++iter) {
    if (iter->get() == NULL) {
      // Dead socket
      continue;
    } else {
      std::list<std::string> strmsgs = (*iter)->get_messages();
      std::copy(strmsgs.begin(), strmsgs.end(), std::back_inserter(total_msgs));
    }
  }

  return total_msgs;
}

bool NetController::get_recv_message(Message& out) {
  std::lock_guard<std::mutex> lock(mtx_);

  std::string strmsg;

  for (IncomingSocketSptrs::iterator iter = insocks_.begin();
       iter != insocks_.end(); ++iter) {
    // Not dead socket and succeed to get a message
    if (iter->get() != NULL && (*iter)->get_message(out)) {
      return true;
    }
  }

  return false;
}

bool NetController::send_message(int procid, const Message& msg) {
  std::string msg_str;
  {
    std::ostringstream oss;
    boost::archive::text_oarchive oa(oss);
    oa << msg;
    msg_str = oss.str();
  }

  std::lock_guard<std::mutex> lock(mtx_);

  if (outsocks_[procid].get() == NULL) {
    if (init_outgoing_conn(procid) == false) {
      return false;
    }
  }

  try {
    outsocks_[procid]->send_message(msg_str);
  } catch (const SocketError& e) {
    if (outsocks_[procid].get() != NULL) {
      outsocks_[procid]->cleanshutdown();
      outsocks_[procid].reset();
      // Try again
      if (init_outgoing_conn(procid) == false) {
        return false;
      }
      try {
        outsocks_[procid]->send_message(msg_str);
      } catch (const SocketError& e) {
        if (outsocks_[procid].get() != NULL) {
          outsocks_[procid]->cleanshutdown();
          outsocks_[procid].reset();
        }
        LOG(WARNING) << "Failed to send message to " << procid << "["
                     << e.what() << "]";
        return false;
      }
      return true;
    }
    LOG(WARNING) << "Failed to send message to " << procid << "[" << e.what()
                 << "]";
    return false;
  }
  return true;
}

void NetController::shutdown() {
  std::lock_guard<std::mutex> lock(mtx_);
  listener_.cleanshutdown();
  for (IncomingSocketSptrs::iterator iter = insocks_.begin();
       iter != insocks_.end(); ++iter) {
    if (iter->get() != NULL) {
      (*iter)->cleanshutdown();
    }
  }

  for (OutgoingSocketSptrs::iterator iter = outsocks_.begin();
       iter != outsocks_.end(); ++iter) {
    if (iter->get() != NULL) {
      (*iter)->cleanshutdown();
    }
  }
}

bool NetController::is_listening() const { return (!listener_.is_stopped()); }

unsigned short NetController::get_listening_port() const {
  assert(is_listening());
  return listener_.get_port();
}

bool NetController::init_outgoing_conn(int procid) {
  // Not using lock_guard intentionally to avoid deadlock
  // Expects that the mutex has already been locked

  if (outsocks_[procid].get() != NULL) {
    throw std::runtime_error(
        (boost::format("outgoing socket of proc %d is not null")).str());
  }

  // create and connect outgoing socket to procid
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(conf_->get_addresses()[procid].c_str());
  addr.sin_port = htons(conf_->get_ports()[procid]);

  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0) {
    LOG(WARNING) << "Failed to create socket [" << strerror(errno) << "]";
    return false;
  }

  if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    // conf_->get_logger()->Log(Logger::WARNING, (boost::format("Failed to
    // connect to %d [%s]") % procid % strerror(errno)).str());
    close(sock);
    return false;
  }

  // succeed to create and connect socket
  outsocks_[procid] = std::make_shared<OutgoingSocket>(sock);

  LOG(INFO) << "Server[" << conf_->get_id() << "]: socket to " << procid
            << " established";
  return true;
}

}  // namespace bamboo
