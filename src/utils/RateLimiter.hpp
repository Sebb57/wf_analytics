/*
** EPITECH PROJECT, 2026
** wf_analytics
** File description:
** RateLimiter.hpp
*/

#ifndef RATELIMITER_HPP_
    #define RATELIMITER_HPP_

#include <chrono>
#include <thread>

namespace wf::utils {

class RateLimiter {
    private:
        using Clock = std::chrono::steady_clock;

        Clock::duration _interval;
        Clock::time_point _next = Clock::now();

    public:
        RateLimiter(double perSecond) : _interval(std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(1.0 / perSecond))) {}

        void wait()
        {
            std::this_thread::sleep_until(this->_next);
            this->_next = Clock::now() + this->_interval;
        }
};

}

#endif /* RATELIMITER_HPP_ */
