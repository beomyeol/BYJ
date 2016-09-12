#ifndef __BAMBOO_THREAD__
#define __BAMBOO_THREAD__

#include "boost/thread.hpp"

namespace bamboo {

class ThreadInterface {
public:
  virtual ~ThreadInterface();

  void start();
  void join();
  void detach();
  bool joinable() const;

protected:
  virtual void run() = 0;

private:
  boost::thread thread_;
};

}

#endif
