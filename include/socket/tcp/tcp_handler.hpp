/**
 * Created by Jian Chen
 * @since  2016.11.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <socket/tcp/tcp_conn.hpp>

namespace chen
{
    namespace tcp
    {
        class handler
        {
        public:
            virtual ~handler() = default;

        public:
            virtual void onAccepted(chen::tcp::conn &c) {}
            virtual void onDisconnect(chen::tcp::conn &c) {}
            virtual void onRead(chen::tcp::conn &c) {}
            virtual void onWrite(chen::tcp::conn &c) {}
        };
    }
}