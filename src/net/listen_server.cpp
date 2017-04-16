#include "bamboo/net/listen_server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

#include <boost/format.hpp>

#include "bamboo/logger.h"

using namespace bamboo;

ListenServer::ListenServer(std::shared_ptr<Config> conf,
                           IncomingSocketSptrs& insocks)
    : stop_flag_ (true),
      conf_ (conf),
      insocks_ (insocks),
      port_ (conf->get_ports()[conf->get_id()]) {}

ListenServer::~ListenServer() {
  cleanshutdown();
}

void ListenServer::run() {
  sockaddr_in server_addr;

  sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock_ < 0) {
    conf_->get_logger()->log(Logger::FATAL,
        (boost::format("Failed to open socket [%s]") % strerror(errno)).str());
    return;
  }

  int optval = 1;
  if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR,
                 &optval, sizeof(optval)) < 0) {
    conf_->get_logger()->log(Logger::FATAL,
        (boost::format("Failed to set SO_REUSEADDR option [%s]") %
            strerror(errno)).str());
    close(sock_);
    return;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port_);

  if (bind(sock_, (const struct sockaddr*) &server_addr,
           sizeof(server_addr)) < 0) {
    conf_->get_logger()->log(Logger::FATAL,
        (boost::format("Failed to bind socket [%s]") % strerror(errno)).str());
    close(sock_);
    return;
  }

  if (listen(sock_, 10) < 0) {
    conf_->get_logger()->log(Logger::FATAL,
        (boost::format("Failed to listen [%s]") % strerror(errno)).str());
    close(sock_);
    return;
  }

  if (port_ == 0) {
    sockaddr_in sin;
    socklen_t sin_len;
    if (getsockname(sock_, (struct sockaddr*)&sin, &sin_len) < 0) {
      int err = errno;
      conf_->get_logger()->log(Logger::FATAL, (boost::format(
          "Failed to get a randomly assigned port err=%s [%s:%d]")
              % strerror(err) % __FILE__ % __LINE__).str());
      throw SocketError("Failed to get a randomly assigned port", err);
    } else {
      port_ = ntohs(sin.sin_port);
    }
  }

  stop_flag_ = false;

  sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int connfd;

  while (!stop_flag_) {
    connfd = accept(sock_, (struct sockaddr*) &client_addr, &client_len);
    if (connfd < 0) {
      if (!is_stopped()) {
        conf_->get_logger()->log(Logger::WARNING,
            (boost::format("Failed to accept [%s]") % strerror(errno)).str());
      }
      break;
    } else {
      IncomingSocketSptr insock = std::make_shared<IncomingSocket>(connfd);
      insocks_.push_back(insock);
      insock->start();
      conf_->get_logger()->log(Logger::INFO, "Recv connection");
    }
  }

  cleanshutdown();
}

void ListenServer::cleanshutdown() {
  if (!stop_flag_) {
    stop_flag_ = true;
    close(sock_);
  }
}

bool ListenServer::is_stopped() const {
  return stop_flag_;
}

unsigned short ListenServer::get_port() const {
  return port_;
}
