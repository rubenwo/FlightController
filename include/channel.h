#include <future>
#include <iostream>
#include <thread>
#include <queue>

template <typename T>
class concurrent_queue
{
private:
    std::queue<T> _queue;
    std::mutex _mutex;

public:
    bool empty()
    {
        std::lock_guard<std::mutex> l(_mutex);
        return _queue.empty();
    }

    size_t size()
    {
        std::lock_guard<std::mutex> l(_mutex);
        return _queue.size();
    }

    bool try_pop(T &out)
    {
        std::lock_guard<std::mutex> l(_mutex);

        if (_queue.empty())
        {
            return false;
        }

        out = std::move(_queue.front());

        _queue.pop();

        return true;
    }

    void push(T val)
    {
        std::lock_guard<std::mutex> l(_mutex);
        _queue.push(std::move(val));
    }
};

class channel_closed : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

template <typename T>
class buffered_channel
{
private:
    concurrent_queue<T> _queue;

    std::atomic_bool _open;

public:
    buffered_channel()
    {
        _open.store(true);
    }

    ~buffered_channel()
    {
        drain();
    }

    void drain()
    {
        _open.store(false);
    }

    bool is_open()
    {
        return _open || !_queue.empty();
    }

    void send(T val)
    {
        if (!_open)
        {
            throw channel_closed("send attempt while closed");
        }
        _queue.push(std::move(val));
    }

    bool recv(T &val)
    {
        while (is_open())
        {
            if (_queue.try_pop(val))
            {
                return true;
            }
            std::this_thread::yield();
        }
        return false;
    }
};
