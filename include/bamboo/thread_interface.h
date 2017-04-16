#ifndef BAMBOO_THREAD_INTERFACE_H
#define BAMBOO_THREAD_INTERFACE_H

#include <thread>

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
  std::thread thread_;
};

}

#endif