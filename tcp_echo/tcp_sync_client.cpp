#include<iostream>
#include<algorithm>
#include<boost/asio.hpp>
#include<boost/bind.hpp>
#include<boost/format.hpp>
#include<boost/thread.hpp>
namespace bsys = boost::system;
namespace basio = boost::asio;
namespace baip = boost::asio::ip;
size_t read_complete(char* buf, bsys::error_code const& err, size_t bytes)
{
	if( err ) return 0;
	bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
	return found? 0 : 1;
}

void sync_echo(std::string msg, basio::io_service & service, baip::tcp::endpoint const& end)
{
	msg += "\n";
	baip::tcp::socket sock(service);
	sock.connect(end);
	basio::write(sock, basio::buffer(msg), basio::transfer_all());
	char buf[1024];
	int bytes = basio::read(sock, basio::buffer(buf), bind(read_complete, buf, _1, _2));
	std::string copy(buf, bytes - 1);
	msg = msg.substr(0, msg.size() - 1);
	std::cout << boost::format("server echoed our %s : %s") % msg % (copy == msg? "OK" : "FAIL") << std::endl;
	sock.close();
}
int main(int argc, char *argv[])
{
	basio::io_service service;
	basio::io_service s2;
	basio::ip::tcp::endpoint ep(baip::address::from_string("127.0.0.1"), 8001);
	char const * messages[] = {
		"John says hi",
		"so does James",
		"Lucy just got home",
		"Boost.Asio is Fun!"
	};
	boost::thread_group threads;
	for(auto p : messages)
	{
		threads.create_thread(boost::bind(sync_echo, p, boost::ref(service), boost::cref(ep)));
		boost::this_thread::sleep(boost::posix_time::millisec(100));
	}
	threads.join_all();
	return 0;
}
