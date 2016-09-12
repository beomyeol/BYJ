#include "bamboo/net/net_controller.h"
#include "boost/thread/lock_guard.hpp"
#include <stdexcept> // std::runtime_error
#include "boost/format.hpp"
#include "boost/make_shared.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <sstream>
#include <string>
#include <csignal>
#include "bamboo/logger.h"

using namespace bamboo;

NetController::NetController(boost::shared_ptr<Config> conf)
: conf_(conf)
, listener_(conf, insocks_)
, outsocks_(conf->get_num_of_processes()) {
  listener_.start();
}

NetController::Messages NetController::get_recv_messages() {
  boost::lock_guard<boost::mutex> lock(mtx_);
  Messages total_msgs;

  for(InsocketSptrs::iterator iter = insocks_.begin();
    iter != insocks_.end();
    ++iter) {
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
  boost::lock_guard<boost::mutex> lock(mtx_);

  std::string strmsg;

  for(InsocketSptrs::iterator iter = insocks_.begin();
    iter != insocks_.end();
    ++iter) {
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

  boost::lock_guard<boost::mutex> lock(mtx_);

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
        if(outsocks_[procid].get() != NULL) {
          outsocks_[procid]->cleanshutdown();
          outsocks_[procid].reset();
        }
        conf_->get_logger()->log(Logger::WARNING, (boost::format("Failed to send message to %d [%s]") % procid % e.what()).str());
        return false;
      }
      return true;
    }
    conf_->get_logger()->log(Logger::WARNING, (boost::format("Failed to send message to %d [%s]") % procid % e.what()).str());
    return false;
  }
  return true;
}

void NetController::shutdown() {
  boost::lock_guard<boost::mutex> lock(mtx_);
  listener_.cleanshutdown();
  for (InsocketSptrs::iterator iter = insocks_.begin();
    iter != insocks_.end();
    ++iter) {
    if (iter->get() != NULL) {
      (*iter)->cleanshutdown();
    }
  }

  for (OutsocketSptrs::iterator iter = outsocks_.begin();
    iter != outsocks_.end();
    ++iter) {
    if (iter->get() != NULL) {
      (*iter)->cleanshutdown();
    }
  }
}

bool NetController::is_listening() const {
  return (!listener_.is_stopped());
}

unsigned short NetController::get_listening_port() const {
  assert(is_listening());
  return listener_.get_port();
}

bool NetController::init_outgoing_conn(int procid) {
  // Not using lock_guard intentionally to avoid deadlock
  // Expects that the mutex has already been locked

  if (outsocks_[procid].get() != NULL) {
    throw std::runtime_error((boost::format("outgoing socket of proc %d is not null")).str());
  }

  // create and connect outgoing socket to procid
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(conf_->get_addresses()[procid].c_str());
  addr.sin_port = htons(conf_->get_ports()[procid]);

  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0) {
    conf_->get_logger()->log(Logger::WARNING, std::string("Failed to create socket [") + strerror(errno) + "]");
    return false;
  }

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    //conf_->get_logger()->log(Logger::WARNING, (boost::format("Failed to connect to %d [%s]") % procid % strerror(errno)).str());
    close(sock);
    return false;
  }

  // succeed to create and connect socket
  outsocks_[procid] = boost::make_shared<OutgoingSocket>(sock);

  conf_->get_logger()->log(Logger::INFO, (boost::format("Server[%d]: socket to %d established") % conf_->get_id() % procid).str());

  return true;
}
