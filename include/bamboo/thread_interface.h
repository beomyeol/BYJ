#ifndef BAMBOO_THREAD_INTERFACE_H_
#define BAMBOO_THREAD_INTERFACE_H_

#include <thread>

namespace bamboo {

class ThreadInterface {
 public:
  virtual ~ThreadInterface();

  void Start();
  void Join();
  void Detach();
  bool Joinable() const;

 protected:
  virtual void Run() = 0;

 private:
  std::thread thread_;
};

}  // namespace bamboo

#endif  // BAMBOO_THREAD_INTERFACE_H_
