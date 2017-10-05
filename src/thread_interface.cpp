#include "bamboo/thread_interface.h"

namespace bamboo {

ThreadInterface::~ThreadInterface() {}

void ThreadInterface::Start() {
  thread_ = std::thread(&ThreadInterface::Run, this);
}

void ThreadInterface::Join() { thread_.join(); }

void ThreadInterface::Detach() { thread_.detach(); }

bool ThreadInterface::Joinable() const { return thread_.joinable(); }

}  // namespace bamboo
