#ifndef __BYJ_NET_NETCONTROLLER__
#define __BYJ_NET_NETCONTROLLER__

#include "config.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread/mutex.hpp"
#include "tbb/concurrent_vector.h"
#include "incoming_socket.h"
#include "outgoing_socket.h"
#include "listen_server.h"
#include <list>
#include <vector>

namespace byj {

class NetController {
public:
    NetController(boost::shared_ptr<Config> conf);

    typedef std::string Message;
    typedef std::list<Message> Messages;

    Messages get_recv_messages();
    bool get_recv_message(Message& out);
    bool send_message(int procid, const Message& msg);
    void shutdown();

    bool is_listening() const;
    unsigned short get_listening_port() const;

private:
    bool init_outgoing_conn(int procid);

    boost::shared_ptr<Config> conf_;
    boost::mutex mtx_;
    ListenServer listener_;

    typedef boost::shared_ptr<IncomingSocket> InsocketSptr;
    typedef tbb::concurrent_vector<InsocketSptr> InsocketSptrs;
    InsocketSptrs insocks_;

    typedef boost::shared_ptr<OutgoingSocket> OutsocketSptr;
    typedef std::vector<OutsocketSptr> OutsocketSptrs;
    OutsocketSptrs outsocks_;
};

}

#endif
