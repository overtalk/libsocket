/**
 * Created by Jian Chen
 * @since  2016.09.02
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/ip/ip_option.hpp>

// todo
//// -----------------------------------------------------------------------------
//// ttl
//chen::ip::option::ttl::ttl(const chen::socket &sock)
//{
//    this->val = chen::option::basic::optionInt(sock.native(), IPPROTO_IP, IP_TTL);
//}
//
//chen::ip::option::ttl::ttl(int val) : val(val)
//{
//}
//
//bool chen::ip::option::ttl::apply(chen::socket &sock)
//{
//    return !::setsockopt(sock.native(), IPPROTO_IP, IP_TTL, &this->val, sizeof(this->val));
//}