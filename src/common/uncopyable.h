#pragma once

class Uncopyable {
protected:
    Uncopyable() = default;
    ~Uncopyable() = default;
    Uncopyable(const Uncopyable&) = delete;
    Uncopyable& operator=(const Uncopyable&) = delete;
};
