#pragma once

#include <atomic>
#include <list>
#include <condition_variable>
#include <mutex>
#include "common/uncopyable.h"
#include "math/util.h"

template<typename T>
class BlockingQueue: private Uncopyable {
public:
    BlockingQueue(int capacity);

    size_t size();

    bool TryPush(T&& v);
    bool Push(T&& v);

    bool TryPop(T& v);
    bool WaitPop(T& v, int32_t wait_us=-1);

    void NotifyAll();
    void NotifyOne();

    void Stop();

private: 
    std::mutex mutex_;
    std::condition_variable ready_;

    std::list<T> items_;
    size_t capacity_;
    bool stop_;
};

template<typename T>
BlockingQueue<T>::BlockingQueue(int capacity) : stop_(false) {
    if (capacity >= 0) {
        capacity_ = math::Max(capacity, 128);
    }
}

template<typename T>
void BlockingQueue<T>::Stop() {
    std::lock_guard<std::mutex> lk(mutex_);
    stop_ = true;
}

template<typename T>
size_t BlockingQueue<T>::size() {
    std::lock_guard<std::mutex> lk(mutex_);
    return items_.size();
}

template<typename T>
void BlockingQueue<T>::NotifyAll() {
    //std::lock_guard<std::mutex> lk(mutex_);
    ready_.notify_all();
}

template<typename T>
void BlockingQueue<T>::NotifyOne() {
    //std::lock_guard<std::mutex> lk(mutex_);
    ready_.notify_one();
}

template<typename T>
bool BlockingQueue<T>::TryPush(T&& v) {
    {
        std::lock_guard<std::mutex> lk(mutex_);
        if (capacity_ > 0 && items_.size() >= capacity_) {
            return false;
        }
        items_.push_back(std::move(v));
    }
    ready_.notify_one();
    return true;
}

template<typename T>
bool BlockingQueue<T>::Push(T&& v) {
    {
        std::lock_guard<std::mutex> lk(mutex_);
        items_.push_back(std::move(v));
    }
    ready_.notify_one();
    return true;
}

template<typename T>
bool BlockingQueue<T>::TryPop(T& v) {
    std::unique_lock<std::mutex> lk(mutex_);
    if (items_.empty()) {
        return false;
    }
    v = std::move(items_.front());
    items_.pop_front();
    return true;
}

template<typename T>
bool BlockingQueue<T>::WaitPop(T& v, int32_t wait_us) {
    std::unique_lock<std::mutex> lk(mutex_);
    if (items_.empty()) {
        if (wait_us < 0) {
            ready_.wait(lk, [this]{ return stop_ || !items_.empty(); });
        } else if (wait_us > 0){
            auto tp = std::chrono::steady_clock::now() + std::chrono::microseconds(wait_us);
            while (ready_.wait_until(lk, tp) != std::cv_status::timeout 
                && (!stop_ && items_.empty())) {
            }
        }
    }

    if (items_.empty()) {
        return false;
    }

    v = std::move(items_.front());
    items_.pop_front();
    return true;
}
