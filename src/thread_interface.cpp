#include "byj/thread_interface.h"

using namespace byj;

thread_interface::~thread_interface() {
}

void thread_interface::start() {
    thread_ = std::thread(&thread_interface::run, this);
}

void thread_interface::join() {
    thread_.join();
}

void thread_interface::detach() {
    thread_.detach();
}

bool thread_interface::joinable() const {
    return thread_.joinable();
}