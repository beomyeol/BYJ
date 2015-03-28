#ifndef __BYJ_NET_LISTEN_SERVER__
#define __BYJ_NET_LISTEN_SERVER__

#include "../thread_interface.h"
#include "config.h"
#include "incoming_socket.h"
#include <tbb/concurrent_vector.h>
#include <vector>
#include <memory>

namespace byj {

class ListenServer : public ThreadInterface {
public:
    typedef boost::shared_ptr<IncomingSocket> insocket_sptr_type;
    typedef tbb::concurrent_vector<insocket_sptr_type> insocket_sptrs_type;

    ListenServer(boost::shared_ptr<Config> conf, insocket_sptrs_type& insocks);
    ~ListenServer();

    virtual void run();
    bool is_stopped() const;
    void cleanshutdown();

    unsigned short get_port() const;

private:
    volatile bool stop_flag_;
    boost::shared_ptr<Config> conf_;
    insocket_sptrs_type& insocks_;
    int sock_;
    unsigned short port_;
};

}

#endif
