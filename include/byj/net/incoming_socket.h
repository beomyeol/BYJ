#ifndef __BYJ_NET_INCOMING_SOCKET__
#define __BYJ_NET_INCOMING_SOCKET__

#include "tbb/concurrent_queue.h"
#include "buffered_socket_reader.h"
#include "../thread_interface.h"
#include <string>
#include <list>

namespace byj {

class IncomingSocket : public ThreadInterface {
public:
    IncomingSocket(int sock);
    virtual ~IncomingSocket();

    std::list<std::string> get_messages();
    bool get_message(std::string& out);
    void cleanshutdown();
    bool is_stopped() const;

protected:
    void shutdown();
    virtual void run();

private:
    const static char MSG_SEP = '&';
    int sock_;
    BufferedSocketReader reader_;
    volatile bool stop_flag_;
    tbb::concurrent_queue<std::string> queue_;
    std::size_t bytes_last_checked;
};

}

#endif
