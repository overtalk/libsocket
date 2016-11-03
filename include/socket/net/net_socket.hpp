/**
 * Created by Jian Chen
 * @since  2016.09.19
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/net/net_endpoint.hpp>
#include <socket/net/net_runloop.hpp>
#include <socket/bsd/bsd_socket.hpp>

namespace chen
{
    namespace net
    {
        class socket
        {
        public:
            /**
             * Peer & Local endpoint
             */
            net::endpoint peer() const;
            net::endpoint sock() const;

            /**
             * Non-blocking mode
             */
            std::error_code nonblocking(bool enable);

            /**
             * Check socket is valid
             */
            bool valid() const;
            operator bool() const;

        protected:
            /**
             * Reset socket
             */
            void reset();
            void reset(bsd::socket &&s);
            void reset(ip::address::Type family, int type);

        protected:
            bsd::socket _socket;
        };
    }
}