//#include <iostream> 
//#include <boost/asio.hpp> 
//#include <boost/date_time/posix_time/posix_time.hpp> 
//
//using namespace boost;
//void print(boost::system::error_code err) 
//{ 
//	std::cout << "Hello, world! "; 
//} 
//int main() 
//{ 
//	boost::asio::io_service io; 
//	boost::asio::deadline_timer t(io, boost::posix_time::seconds(5)); 
//	t.async_wait(print); 
//	io.run(); 
//	return 0; 
//} 

#include <iostream> 
#include <boost/asio.hpp> 
#include <boost/bind.hpp> 
#include <boost/date_time/posix_time/posix_time.hpp> 
using namespace boost;
void print(boost::system::error_code err , boost::asio::deadline_timer* t, int* count) 
{ 
	if (*count < 5) 
	{ 
		std::cout << *count << " "; 
		++(*count); 
		t->expires_at(t->expires_at() + boost::posix_time::seconds(1)); 
		t->async_wait(boost::bind(print, 
			asio::placeholders::error, t, count)); 
	} 
} 
int main() 
{ 
	asio::io_service io; 
	int count = 0; 
	asio::deadline_timer t(io, boost::posix_time::seconds(1)); 
	t.async_wait(boost::bind(print, 
		asio::placeholders::error, &t, &count)); 
	io.run(); 
	std::cout << "Final count is " << count << " "; 
	return 0; 
} 
