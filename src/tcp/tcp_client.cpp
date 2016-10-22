/**
 * Created by Jian Chen
 * @since  2016.08.21
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#include <socket/net/net_resolver.hpp>
#include <socket/tcp/tcp_client.hpp>
#include <chen/base/str.hpp>

// -----------------------------------------------------------------------------
// client
chen::tcp::client::client(net::runloop &runloop) : _runloop(runloop)
{
}

chen::tcp::client::~client()
{
    this->disconnect();
}

// connection
void chen::tcp::client::connect(const char *mixed)
{
    this->connect(mixed, ip::address::Type::None);
}

void chen::tcp::client::connect(const std::string &mixed, ip::address::Type type)
{
    auto ret = net::resolver::resolve(mixed, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: client resolve address '%s' fail", mixed.c_str()));

    this->connect(ret.front());
}

void chen::tcp::client::connect(const std::string &host, std::uint16_t port, ip::address::Type type)
{
    auto ret = net::resolver::resolve(host, port, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: client resolve address '%s' fail", host.c_str()));

    this->connect(ret.front());
}

void chen::tcp::client::connect(const std::string &host, const std::string &service, ip::address::Type type)
{
    auto ret = net::resolver::resolve(host, service, type);
    if (ret.empty())
        throw std::runtime_error(str::format("tcp: client resolve address '%s' or service '%s' fail", host.c_str(), service.c_str()));

    this->connect(ret.front());
}

void chen::tcp::client::connect(const net::endpoint &ep)
{
    // disconnect is if connected
    if (!this->isDisconnect())
        this->disconnect();

    // recreate socket and use nb mode
    this->reset(ep.addr().type());
    this->nonblocking(true);

    // listen events using runloop
    this->_runloop.set(this->_handle.native(),
                       net::runloop::OpcodeRead | net::runloop::OpcodeWrite,
                       net::runloop::FlagEdge,
                       std::bind(&client::onEvent, this, std::placeholders::_1));

    // connect to remote host
    this->_remote = ep;
    this->_state  = State::Connecting;

    auto error = this->_handle.connect(ep);
    if (error != std::errc::operation_in_progress)
        throw std::system_error(error, "tcp: client connect error");
}

void chen::tcp::client::reconnect()
{
    // todo
}

void chen::tcp::client::disconnect()
{
    this->_runloop.del(this->_handle.native());

    this->_handle.shutdown();
    this->_handle.close();

    this->_state = State::Disconnect;
}

// property
bool chen::tcp::client::isConnecting() const
{
    return this->_state == State::Connecting;
}

bool chen::tcp::client::isConnected() const
{
    return this->_state == State::Connected;
}

bool chen::tcp::client::isDisconnect() const
{
    return this->_state == State::Disconnect;
}

chen::net::endpoint chen::tcp::client::remote() const
{
    return this->_remote;
}

// event
void chen::tcp::client::attach(std::function<void (tcp::connected_event event)> callback)
{
    this->_cb_connected = callback;
}

void chen::tcp::client::attach(std::function<void (tcp::disconnect_event event)> callback)
{
    this->_cb_disconnect = callback;
}

void chen::tcp::client::attach(std::function<void (tcp::read_event event)> callback)
{
    this->_cb_read = callback;
}

void chen::tcp::client::attach(std::function<void (tcp::write_event event)> callback)
{
    this->_cb_write = callback;
}

void chen::tcp::client::detach(Event type)
{
    switch (type)
    {
        case Event::Connected:
            this->_cb_connected = nullptr;
            break;

        case Event::Disconnect:
            this->_cb_disconnect = nullptr;
            break;

        case Event::Read:
            this->_cb_read = nullptr;
            break;

        case Event::Write:
            this->_cb_write = nullptr;
            break;
    }
}

// notify
void chen::tcp::client::notify(tcp::connected_event &&event)
{
    this->_state = !event.err ? State::Connected : State::Disconnect;

    if (this->_cb_connected)
        this->_cb_connected(std::move(event));
}

void chen::tcp::client::notify(tcp::disconnect_event &&event)
{
    this->_state = State::Disconnect;

    if (this->_cb_disconnect)
        this->_cb_disconnect(std::move(event));
}

void chen::tcp::client::notify(tcp::read_event &&event)
{
    if (this->_cb_read)
        this->_cb_read(std::move(event));
}

void chen::tcp::client::notify(tcp::write_event &&event)
{
    if (this->_cb_write)
        this->_cb_write(std::move(event));
}

// event
void chen::tcp::client::onReadable()
{
    if (this->isConnected())
    {
//     this->notify(read_event(std::move(data)));
    }
    else
    {
        // todo test not received connected, but remote send a data to me
        throw std::runtime_error("tcp: client in disconnect state but received read event");
    }
}

void chen::tcp::client::onWritable()
{
    if (this->isConnecting())
    {
        this->notify(connected_event(this->_remote, {}));
    }
    else if (this->isConnected())
    {
        // todo define various policy
//      this->notify(write_event(size));
    }
    else
    {
        throw std::runtime_error("tcp: client in disconnect state but received write event");
    }
}

void chen::tcp::client::onEnded()
{
    auto error = this->option().error();

    if (this->isConnecting())
    {
        // connection refused or reset by peer
        // @see http://man7.org/linux/man-pages/man2/connect.2.html about EINPROGRESS
        this->disconnect();
        this->notify(connected_event(this->_remote, error));
    }
    else if (this->isConnected())
    {
        // todo still read the rest of the data until read return error

        // connection broken
        this->disconnect();
        this->notify(disconnect_event(error));
    }
    else
    {
        throw std::runtime_error("tcp: client in disconnect state but received end event");
    }
}

void chen::tcp::client::onEvent(net::runloop::Event type)
{
    switch (type)
    {
        case net::runloop::Event::Read:
            return this->onReadable();

        case net::runloop::Event::Write:
            return this->onWritable();

        case net::runloop::Event::End:
            return this->onEnded();
    }
}