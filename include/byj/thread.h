#ifndef __UTIL_THREAD__
#define __UTIL_THREAD__

#include <thread>

namespace byj {
    class thread {
    public:
        virtual ~thread();

        virtual void run() = 0;

        void start();
        void join();
        void detach();
        bool joinable() const;

    private:
        std::thread thread_;
    };
}

#endif