#ifndef WEBSOCKET_ASYNC_ECHO_SERVER_HPP
#define WEBSOCKET_ASYNC_ECHO_SERVER_HPP

#include <beast/core.hpp>
#include <beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

/// Block until SIGINT or SIGTERM is received.
inline
void
sig_wait()
{
    boost::asio::io_service ios;
    boost::asio::signal_set signals(
        ios, SIGINT, SIGTERM);
    signals.async_wait(
        [&](boost::system::error_code const&, int)
        {
        });
    ios.run();
}

//------------------------------------------------------------------------------
//
// Example: WebSocket echo server, asynchronous
//
//------------------------------------------------------------------------------

namespace http = beast::http;           // from <beast/http.hpp>
namespace websocket = beast::websocket; // from <beast/websocket.hpp>
namespace ip = boost::asio::ip;         // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio.hpp>

class server
{
    using error_code = beast::error_code;

    std::ostream* log_;
    boost::asio::io_service ios_;
    tcp::socket sock_;
    tcp::endpoint ep_;
    std::vector<std::thread> thread_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::function<void(beast::websocket::stream<tcp::socket>&)> mod_;
    boost::optional<boost::asio::io_service::work> work_;

    static
    void
    print_1(std::ostream&)
    {
    }

    template<class T1, class... TN>
    static
    void
    print_1(std::ostream& os, T1 const& t1, TN const&... tn)
    {
        os << t1;
        print_1(os, tn...);
    }

    // compose a string to std::cout or std::cerr atomically
    //
    template<class...Args>
    static
    void
    print(std::ostream& os, Args const&... args)
    {
        std::stringstream ss;
        print_1(ss, args...);
        os << ss.str();
    }

    //--------------------------------------------------------------------------

    class connection : public std::enable_shared_from_this<connection>
    {
        std::ostream* log_;
        tcp::endpoint ep_;
        beast::websocket::stream<tcp::socket> ws_;
        boost::asio::io_service::strand strand_;
        beast::multi_buffer buffer_;
        std::size_t id_;

    public:
        connection(connection&&) = default;
        connection(connection const&) = default;

        connection(server& parent, tcp::endpoint const& ep,
                tcp::socket&& sock)
            : log_(parent.log_)
            , ep_(ep)
            , ws_(std::move(sock))
            , strand_(ws_.get_io_service())
            , id_([]
                {
                    static std::atomic<std::size_t> n{0};
                    return ++n;
                }())
        {
            parent.mod_(ws_);
        }

        void run()
        {
            ws_.async_accept_ex(
                [](beast::websocket::response_type& res)
                {
                    res.insert(http::field::server, BEAST_VERSION_STRING);
                },
                strand_.wrap(std::bind(
                    &connection::on_accept,
                    shared_from_this(),
                    std::placeholders::_1)));
        }

    private:
        void on_accept(error_code ec)
        {
            if(ec)
                return fail("accept", ec);
            do_read();
        }

        void do_read()
        {
            ws_.async_read(buffer_,
                strand_.wrap(std::bind(
                    &connection::on_read,
                    shared_from_this(),
                    std::placeholders::_1)));
        }

        void on_read(error_code ec)
        {
            if(ec)
                return fail("read", ec);

            ws_.binary(ws_.got_binary());
            ws_.async_write(buffer_.data(),
                strand_.wrap(std::bind(
                    &connection::on_write,
                    shared_from_this(),
                    std::placeholders::_1)));
        }

        void on_write(error_code ec)
        {
            if(ec)
                return fail("write", ec);
            
            buffer_.consume(buffer_.size());

            do_read();
        }

        void
        fail(std::string what, error_code ec)
        {
            if(log_)
                if(ec != beast::websocket::error::closed)
                    print(*log_, "[#", id_, " ", ep_, "] ", what, ": ", ec.message());
        }
    };

    void
    fail(std::string what, error_code ec)
    {
        if(log_)
            print(*log_, what, ": ", ec.message());
    }

    void
    on_accept(error_code ec)
    {
        if(! acceptor_.is_open())
            return;
        if(ec == boost::asio::error::operation_aborted)
            return;
        if(ec)
            fail("accept", ec);
        std::make_shared<connection>(*this, ep_, std::move(sock_))->run();
        acceptor_.async_accept(sock_, ep_,
            std::bind(&server::on_accept, this,
                std::placeholders::_1));
    }

public:
    /** Constructor.

        @param log A pointer to a stream to log to, or `nullptr`
        to disable logging.
        
        @param threads The number of threads in the io_service.
    */
    server(std::ostream* log, std::size_t threads)
        : log_(log)
        , sock_(ios_)
        , acceptor_(ios_)
        , work_(ios_)
    {
        thread_.reserve(threads);
        for(std::size_t i = 0; i < threads; ++i)
            thread_.emplace_back(
                [&]{ ios_.run(); });
    }

    /// Destructor.
    ~server()
    {
        work_ = boost::none;
        ios_.dispatch([&]
            {
                error_code ec;
                acceptor_.close(ec);
            });
        for(auto& t : thread_)
            t.join();
    }

    /// Return the listening endpoint.
    tcp::endpoint
    local_endpoint() const
    {
        return acceptor_.local_endpoint();
    }

    /** Set a handler called for new streams.
        This function is called for each new stream.
        It is used to set options for every connection.
    */
    template<class F>
    void
    on_new_stream(F const& f)
    {
        mod_ = f;
    }

    /** Open a listening port.
        @param ep The address and port to bind to.
        @param ec Set to the error, if any occurred.
    */
    void
    open(tcp::endpoint const& ep, error_code& ec)
    {
        acceptor_.open(ep.protocol(), ec);
        if(ec)
            return fail("open", ec);
        acceptor_.set_option(
            boost::asio::socket_base::reuse_address{true});
        acceptor_.bind(ep, ec);
        if(ec)
            return fail("bind", ec);
        acceptor_.listen(
            boost::asio::socket_base::max_connections, ec);
        if(ec)
            return fail("listen", ec);
        acceptor_.async_accept(sock_, ep_,
            std::bind(&server::on_accept, this,
                std::placeholders::_1));
    }
};

class set_stream_options
{
    beast::websocket::permessage_deflate pmd_;

public:
    set_stream_options(set_stream_options const&) = default;

    set_stream_options(
            beast::websocket::permessage_deflate const& pmd)
        : pmd_(pmd)
    {
    }

    template<class NextLayer>
    void
    operator()(beast::websocket::stream<NextLayer>& ws) const
    {
        ws.auto_fragment(false);
        ws.set_option(pmd_);
        ws.read_message_max(64 * 1024 * 1024);
    }
};

int main(int argc, char* argv[])
{
    // Check command line arguments.
    if(argc != 4)
    {
        std::cerr <<
            "Usage: " << argv[0] << " <address> <port> <threads>\n"
            "  For IPv4, try: " << argv[0] << " 0.0.0.0 8080 1\n"
            "  For IPv6, try: " << argv[0] << " 0::0 8080 1\n"
            ;
        return EXIT_FAILURE;
    }

    auto address = ip::address::from_string(argv[1]);
    unsigned short port = static_cast<unsigned short>(std::atoi(argv[2]));
    unsigned short threads = static_cast<unsigned short>(std::atoi(argv[3]));

    websocket::permessage_deflate pmd;
    pmd.client_enable = true;
    pmd.server_enable = true;
    pmd.compLevel = 3;

    server s{&std::cout, threads};
    s.on_new_stream(set_stream_options{pmd});

    beast::error_code ec;
    s.open(tcp::endpoint{address, port}, ec);
    if(ec)
    {
        std::cerr << "Error: " << ec.message();
        return EXIT_FAILURE;
    }

    sig_wait();

    return EXIT_SUCCESS;
}

#endif
