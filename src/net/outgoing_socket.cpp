#include "byj/net/outgoing_socket.h"
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <algorithm>
#include <iterator>
#include <vector>
#include "boost/thread/lock_guard.hpp" //lock_guard

using namespace byj;

const char OutgoingSocket::MSG_SEP;

OutgoingSocket::OutgoingSocket(int sock)
: sock_(sock) {
    ::shutdown(sock, SHUT_RD);
}

void OutgoingSocket::send_message(const std::string& msg) {
    boost::lock_guard<boost::mutex> lock(mtx_);

    std::vector<char> send_buffer(msg.begin(), msg.end());
    send_buffer.push_back(MSG_SEP);

    int written_bytes = 0;
    int current_written_bytes = 0;

    while (written_bytes != send_buffer.size()) {
        current_written_bytes = send(sock_, &send_buffer[written_bytes], send_buffer.size() - written_bytes, 0);
        int err = errno;
        if (current_written_bytes < 0) {
            throw SocketError("Failed to send", err);
        } else if (current_written_bytes == 0) {
            throw SocketError("Connection closed", 0);
        } else {
            written_bytes += current_written_bytes;
        }
    }
}

void OutgoingSocket::cleanshutdown() {
    boost::lock_guard<boost::mutex> lock(mtx_);

    ::shutdown(sock_, SHUT_WR);
    close(sock_);
}
