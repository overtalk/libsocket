/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/inet/inet_endpoint.hpp>
#include <socket/bsd/basic_socket.hpp>
#include <socket/sys/runloop.hpp>

namespace chen
{
    class inet_socket
    {
    public:
        /**
         * Peer & Local endpoint
         */
        inet_endpoint peer() const;
        inet_endpoint sock() const;

        /**
         * Check socket is valid
         */
        bool valid() const;
        operator bool() const;

    protected:
        /**
         * Non-blocking mode
         */
        std::error_code nonblocking(bool enable);

        /**
         * Reset socket
         */
        void reset();
        void reset(basic_socket &&s);
        void reset(ip_address::Type family, int type);

    protected:
        basic_socket _socket;
    };
}