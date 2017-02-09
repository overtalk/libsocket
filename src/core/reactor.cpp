/**
 * Created by Jian Chen
 * @since  2017.02.05
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/core/reactor.hpp>

// -----------------------------------------------------------------------------
// reactor
const int chen::reactor::ModeRead  = 1 << 0;
const int chen::reactor::ModeWrite = 1 << 1;
const int chen::reactor::ModeRW    = ModeRead | ModeWrite;

chen::reactor::reactor() : reactor(64)  // 64 is enough
{
}

chen::reactor::~reactor()
{
    // clear objects before destroy backend
#ifdef _WIN32
    auto objects = std::move(this->_objects);
    for (auto &item : objects)
        this->del(item.second);
#else
    auto objects = std::move(this->_objects);
    for (auto &item : objects)
        this->del(item);

    ::close(this->_backend);
#endif

    auto timers = std::move(this->_timers);
    for (auto *item : timers)
        this->del(item);
}

// modify
void chen::reactor::set(ev_timer *ptr, std::chrono::high_resolution_clock::time_point init)
{
    ptr->setup(init);
    this->_timers.insert(ptr);
    ptr->onAttach(this, 0, 0);  // mode & flag are useless

#ifdef _WIN32
    // repoll if in polling
    this->_repoll.set();
#endif
}

void chen::reactor::del(ev_timer *ptr)
{
    ptr->onDetach();
    this->_timers.erase(ptr);

#ifdef _WIN32
    // repoll if in polling
    this->_repoll.set();
#endif
}

// run
void chen::reactor::run()
{
    for (std::error_code code; !code || (code == std::errc::timed_out) || (code == std::errc::interrupted); code = this->poll())
        ;
}

std::error_code chen::reactor::poll()
{
    return this->poll(std::chrono::nanoseconds::min());
}

std::error_code chen::reactor::poll(std::chrono::nanoseconds timeout)
{
    // quickly stop
    if (this->_wakeup.signaled())
    {
        this->_wakeup.reset();
        return std::make_error_code(std::errc::operation_canceled);
    }

    // update timer
    auto zero = std::chrono::nanoseconds::zero();
    auto mini = this->update();

    if ((mini >= zero) && (timeout != zero))
        timeout = (timeout > zero) ? std::min(mini, timeout) : mini;

    // poll events
    auto error = this->gather(timeout);

    // notify user
    this->notify();

    return error;
}

void chen::reactor::post(ev_base *ptr, int type)
{
    this->_queue.emplace(ptr, type);
}

void chen::reactor::post(ev_timer *ptr)
{
    this->_queue.emplace(ptr, 0);
}

void chen::reactor::stop()
{
    // notify wakeup message
    this->_wakeup.set();
}

// phase
std::chrono::nanoseconds chen::reactor::update()
{
    if (this->_timers.empty())
        return std::chrono::nanoseconds::min();

    std::vector<ev_timer*> tmp;

    auto ret = std::chrono::nanoseconds::min();
    auto now = std::chrono::high_resolution_clock::now();

    for (auto *ptr : this->_timers)
    {
        // the front is the nearest timer
        if (ptr->expire(now))
        {
            if (ptr->repeat())
                tmp.emplace_back(ptr);

            // don't wait for the next backend event if we have a callback need to notify
            if (ret != std::chrono::nanoseconds::zero())
                ret = std::chrono::nanoseconds::zero();

            this->post(ptr);
        }
        else
        {
            if (ret != std::chrono::nanoseconds::zero())
                ret = ptr->alarm() - now;

            break;
        }
    }

    for (auto *ptr : tmp)
    {
        this->_timers.erase(ptr);
        ptr->update(now);
        this->_timers.insert(ptr);
    }

    return ret;
}

void chen::reactor::notify()
{
    while (!this->_queue.empty())
    {
        auto item = this->_queue.front();
        this->_queue.pop();

        item.first->onEvent(item.second);
    }
}