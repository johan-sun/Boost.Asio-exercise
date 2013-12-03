#include<iostream>
#include<boost/asio.hpp>
#include<boost/bind.hpp>
#include<boost/format.hpp>
namespace bsys = boost::system;
namespace basio = boost::asio;
namespace baip = basio::ip;
size_t read_complete(char* buf, bsys::error_code const& err, size_t bytes)
{
	if ( err ) return 0;
	bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
	return found ? 0:1;
}

void handle_connections(basio::io_service& io_service)
{
	baip::tcp::acceptor acceptor(io_service, baip::tcp::endpoint(baip::tcp::v4(), 8001));
	char buf[1024];
	for(;;)
	{
		baip::tcp::socket sock(io_service);
		acceptor.accept(sock);
		int bytes = basio::read(sock, basio::buffer(buf), boost::bind(read_complete, buf, _1, _2));
		std::string msg(buf, bytes);
		basio::write(sock, basio::buffer(msg));
		sock.close();
	}
}

int main(int argc, char *argv[])
{
	basio::io_service io_service;
	handle_connections(io_service);
	return 0;
}
