#ifndef __BYJ_THREAD__
#define __BYJ_THREAD__

#include <thread>

namespace byj {
    class thread_interface {
    public:
        virtual ~thread_interface();

        void start();
        void join();
        void detach();
        bool joinable() const;

    protected:
        virtual void run() = 0;

    private:
        std::thread thread_;
    };
}

#endif