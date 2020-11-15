#include "thread_pool.h"
#include <assert.h>
#include <utility>
#include <iostream>

ThreadPool::ThreadPool(int threads): 
    exit_(false)
{
    for (int i = 0; i < threads; ++i) {
        threads_.emplace_back([this] {
            while (true) {
                Task task;
                {
                    std::unique_lock<std::mutex> lk(mutex_);
                    ready_.wait(lk, [this]{ return exit_ || !tasks_.empty(); });
                    if (exit_ && tasks_.empty()) {
                        return;
                    }
                    
                    task = std::move(tasks_.front());
                    tasks_.pop_front();
                }
                
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    Join();
}

void ThreadPool::Join() {
    {
        std::lock_guard<std::mutex> lk(mutex_);
        if (exit_) return;
        exit_ = true;
    }
    
    ready_.notify_all();

    for (auto& t: threads_) {
        t.join();
    }
}

void ThreadPool::Enqueue(Task&& task) {
    {
        std::lock_guard<std::mutex> lk(mutex_);
        if (exit_) return;

        tasks_.push_back(std::move(task));
    }
    ready_.notify_one();
}
