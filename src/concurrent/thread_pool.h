#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <list>
#include <functional>
#include <limits>
#include <condition_variable>
#include <mutex>
#include "common/uncopyable.h"
#include "blocking_queue.h"

class ThreadPool: private Uncopyable {
public:
    using Task = std::function<void()>;

    ThreadPool(int threads);
    ~ThreadPool();

    size_t size() { return threads_.size(); }
    size_t task_size() { return tasks_.size(); }

    void Join();

    void Enqueue(Task&& task);

    template<typename F, typename... Args>
    void Enqueue(F&& f, Args&&... args) {
        {
            std::lock_guard<std::mutex> lk(mutex_);
            if (exit_) return;
            
            auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
            tasks_.push_back(std::move(task));
        }
        ready_.notify_one();
    }

private:
    std::list<Task> tasks_;
    std::vector<std::thread> threads_;
    std::mutex mutex_;
    std::condition_variable ready_;

    bool exit_;
};
