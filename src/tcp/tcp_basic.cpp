/**
 * Created by Jian Chen
 * @since  2016.09.03
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/tcp/tcp_basic.hpp>

// -----------------------------------------------------------------------------
// basic
chen::tcp::basic::basic(socket_t fd) : socket(fd)
{
}

chen::tcp::basic::basic(ip::address::Type family) : socket(family, SOCK_STREAM)
{
}

chen::tcp::option chen::tcp::basic::option()
{
    return tcp::option(this->native());
}