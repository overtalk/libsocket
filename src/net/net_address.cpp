/**
 * Created by Jian Chen
 * @since  2016.09.12
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_address.hpp>
#include <chen/base/num.hpp>

// -----------------------------------------------------------------------------
// address
chen::net::address::address(std::nullptr_t) : _addr(nullptr)
{
}

chen::net::address::address(const ip::address &addr, std::uint16_t port) : _addr(addr), _port(port)
{
}

// todo
//chen::net::address::address(const std::string &mixed) : address(resolver::split(mixed))
//{
//}

chen::net::address::address(const char *mixed) : address(std::string(mixed))
{
}

// property
std::string chen::net::address::str(bool cidr, bool scope) const
{
    switch (this->_addr.type())
    {
        case ip::address::Type::IPv4:
            return this->_addr.v4().str(cidr) + ":" + num::str(this->_port);

        case ip::address::Type::IPv6:
            return "[" + this->_addr.v6().str(cidr, scope) + "]:" + num::str(this->_port);

        default:
            return "";
    }
}

bool chen::net::address::empty() const
{
    return this->_addr.empty();
}

chen::net::address::operator bool() const
{
    return !this->empty();
}

const chen::ip::address& chen::net::address::addr() const
{
    return this->_addr;
}

void chen::net::address::addr(const ip::address &value)
{
    this->_addr = value;
}

std::uint16_t chen::net::address::port() const
{
    return this->_port;
}

void chen::net::address::port(std::uint16_t value)
{
    this->_port = value;
}

// special
bool chen::net::address::isWellKnownPort() const
{
    // from 0 through 1023
    return this->_port <= 1023;
}

bool chen::net::address::isRegisteredPort() const
{
    // from 1024 through 49151
    return (this->_port >= 1024) && (this->_port <= 49151);
}

bool chen::net::address::isDynamicPort() const
{
    // from 49152 through 65535
    return this->_port >= 49152;
}

// comparison
bool chen::net::address::operator==(const address &o) const
{
    return (this->_addr == o._addr) && (this->_port == o._port);
}

bool chen::net::address::operator!=(const address &o) const
{
    return !(*this == o);
}

bool chen::net::address::operator<(const address &o) const
{
    return (this->_addr == o._addr) ? this->_port < o._port : this->_addr < o._addr;
}

bool chen::net::address::operator>(const address &o) const
{
    return o < *this;
}

bool chen::net::address::operator<=(const address &o) const
{
    return (this->_addr == o._addr) ? this->_port <= o._port : this->_addr <= o._addr;
}

bool chen::net::address::operator>=(const address &o) const
{
    return o <= *this;
}

// override
struct ::sockaddr_storage chen::net::address::get() const
{
    struct ::sockaddr_storage out{};

    switch (this->_addr.type())
    {
        case ip::address::Type::IPv4:
        {
            auto in = (struct ::sockaddr_in*)&out;

            in->sin_family      = AF_INET;
            in->sin_port        = chen::num::swap(this->_port);
            in->sin_addr.s_addr = chen::num::swap(this->_addr.v4().addr());
        }
            break;

        case ip::address::Type::IPv6:
        {
            auto in = (struct ::sockaddr_in6*)&out;

            in->sin6_family   = AF_INET6;
            in->sin6_port     = chen::num::swap(this->_port);
            in->sin6_scope_id = this->_addr.v6().scope();

            ::memcpy(in->sin6_addr.s6_addr, this->_addr.v6().addr().data(), 16);
        }
            break;

        default:
            break;
    }

    return out;
}

void chen::net::address::set(const struct ::sockaddr_storage &val)
{
    switch (val.ss_family)
    {
        case AF_INET:
        {
            auto in = (struct ::sockaddr_in*)&val;
            this->_addr = num::swap(in->sin_addr.s_addr);
            this->_port = num::swap(in->sin_port);
        }
            break;

        case AF_INET6:
        {
            auto in = (struct ::sockaddr_in6*)&val;
            this->_addr = ip::version6(in->sin6_addr.s6_addr, 128, in->sin6_scope_id);
            this->_port = num::swap(in->sin6_port);
        }
            break;

        default:
            throw std::runtime_error("address: unknown sockaddr type");
    }
}

socklen_t chen::net::address::len() const
{
    switch (this->_addr.type())
    {
        case ip::address::Type::IPv4:
            return sizeof(struct ::sockaddr_in);

        case ip::address::Type::IPv6:
            return sizeof(struct ::sockaddr_in6);

        default:
            return 0;
    }
}