/**
 * Created by Jian Chen
 * @since  2015.07.30
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <chen/mt/semaphore.hpp>

using namespace chen;

// ------------------------------------------------------------------
// semaphore
semaphore::semaphore(std::size_t count) : _count(count)
{

}

void semaphore::post()
{
    std::unique_lock<std::mutex> lock(this->_mutex);
    ++this->_count;
    this->_cond.notify_one();
}

void semaphore::wait()
{
    std::unique_lock<std::mutex> lock(this->_mutex);

    this->_cond.wait(lock, [&] {
        return this->_count;
    });

    --this->_count;
}