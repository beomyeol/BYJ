#ifndef __BAMBOO_THREAD_INTERFACE__
#define __BAMBOO_THREAD_INTERFACE__

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

}  // namespace bamboo

#endif
