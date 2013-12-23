#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "DlgDTO.h"
#include <vector>
#include "Model.h"

using boost::asio::ip::tcp;
using namespace boost::asio;

class client
{
public:
	~client()
	{
		//std::cout << "析构函数" << std::endl;
		
	}
	client(boost::asio::io_service& io_service, std::vector<CModel>::iterator _it , CDlgDTO & _cdd)
		:
		ios(io_service),
		resolver_(io_service),
		socket_(io_service), 
		timer_(io_service),
		it(_it),
		cdd(_cdd)
	{
		std::string server = it->url;
		std::string path = "http://" + server + "/";
		std::ostream request_stream(&request_);
		request_stream << "GET " << path << " HTTP/1.0\r\n";
		request_stream << "Host: " << server << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";

		tcp::resolver::query query(server, "http");
		resolver_.async_resolve(query,
			boost::bind(&client::handle_resolve, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator));

		/*===add for test timer =======*/
		timer_.expires_from_now(boost::posix_time::seconds(cdd.time_out));
		timer_.async_wait(boost::bind(&client::close_socket, this));
		/*===add for test timer =======*/
	}

private:
	void handle_resolve(const boost::system::error_code& err,
		tcp::resolver::iterator endpoint_iterator)
	{
		if (!err)
		{
			boost::asio::async_connect(socket_, endpoint_iterator,
				boost::bind(&client::handle_connect, this,
				boost::asio::placeholders::error));
		}
		else
		{
			it->addition_msg =  err.message();
		}
	}

	void handle_connect(const boost::system::error_code& err)
	{
		
		if (!err)
		{
			// The connection was successful. Send the request.
			boost::asio::async_write(socket_, request_,
				boost::bind(&client::handle_write_request, this,
				boost::asio::placeholders::error));
		}
		else
		{
			it->addition_msg = err.message();
		}
	}

	void handle_write_request(const boost::system::error_code& err)
	{
		if(cdd.is_IP_Check)
			it->remoteIP = socket_.remote_endpoint().address().to_string();
		if (!err)
		{
			boost::asio::async_read_until(socket_, response_, "\r\n",
				boost::bind(&client::handle_read_status_line, this,
				boost::asio::placeholders::error));
		}
		else
		{
			it->addition_msg =  err.message();
		}
	}

	void handle_read_status_line(const boost::system::error_code& err)
	{
		if(!cdd.is_CODE_check)
		{
			socket_.close();
			ios.stop();
			return;
		}
		if (!err)
		{
			std::istream response_stream(&response_);
			std::string http_version;
			response_stream >> http_version;
			unsigned int status_code;
			response_stream >> status_code;
			it->status_code = status_code;
			std::string status_message;
			std::getline(response_stream, status_message);
			it->addition_msg = status_message;
			if (!response_stream || http_version.substr(0, 5) != "HTTP/")
			{
				it->addition_msg = "Invalid response";
				return;
			}
			if (status_code != 200)
			{
				it->addition_msg = "Response returned with status code ";
				return;
			}
			ios.stop();
		}
		else
		{
			it->addition_msg =  err.message();
		}
	}

	
	boost::asio::io_service & ios;
	tcp::resolver resolver_;
	tcp::socket socket_;
	boost::asio::streambuf request_;
	boost::asio::streambuf response_;

	CDlgDTO cdd;
	std::vector<CModel>::iterator it;

	/*===add for test timer===*/
	deadline_timer timer_;  
	void close_socket()  
	{  
		socket_.close();  
		it->addition_msg = "链接超时";
		ios.stop();
	}
	/*==add for test timer=====*/
};

//int main(int argc, char* argv[])
//{
//
//	argc = 3;
//	argv[1] = "www.baidu.com";
//	argv[2] = "http://www.baidu.com/";
//	try
//	{
//		if (argc != 3)
//		{
//			std::cout << "Usage: async_client <server> <path>\n";
//			std::cout << "Example:\n";
//			std::cout << "  async_client www.boost.org /LICENSE_1_0.txt\n";
//			return 1;
//		}
//
//		boost::asio::io_service io_service;
//		client c(io_service, argv[1], argv[2]);
//		io_service.run();
//	}
//	catch (std::exception& e)
//	{
//		std::cout << "Exception: " << e.what() << "\n";
//	}
//
//
//
//	getchar();
//	return 0;
//}