//
// async_client.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "Model.h"
#include "DlgDTO.h"

using boost::asio::ip::tcp;
using namespace boost::asio;

void writeToLog(char * str)
{


	FILE * pfile;
	fopen_s( &pfile , "log.log" , "a+");
	if (pfile == NULL)
	{
		return ;
	}
	fprintf_s( pfile , "%s\n" , str);
	fclose( pfile );

}


class client
{
public:
	~client()
	{
		writeToLog("析构函数");
	}
	client(boost::asio::io_service& io_service,CModel & _cm , CDlgDTO & _cdd)
		: resolver_(io_service),
		cm(_cm),
		cdd(_cdd),
		socket_(io_service), 
		timer_(io_service)
	{
		writeToLog("进入 client 构造函数");
		std::string server = cm.url;
		std::string path = "http://" + server + "/";
		// Form the request. We specify the "Connection: close" header so that the
		// server will close the socket after transmitting the response. This will
		// allow us to treat all data up until the EOF as the content.
		std::ostream request_stream(&request_);
		request_stream << "GET " << path << " HTTP/1.0\r\n";
		request_stream << "Host: " << server << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";

		// Start an asynchronous resolve to translate the server and service names
		// into a list of endpoints.
		tcp::resolver::query query(server, "http");
		resolver_.async_resolve(query,
			boost::bind(&client::handle_resolve, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator));

		/*===add for test timer =======*/
		//timer_.expires_from_now(boost::posix_time::seconds(cdd.time_out));
		//timer_.async_wait(boost::bind(&client::close, this));
		/*===add for test timer =======*/
		writeToLog("构造结束");
	}

private:
	void handle_resolve(const boost::system::error_code& err,
		tcp::resolver::iterator endpoint_iterator)
	{
		writeToLog("开始执行reslove的回调函数");
		if (!err)
		{
			writeToLog("没有错误");
			// Attempt a connection to each endpoint in the list until we
			// successfully establish a connection.
			boost::asio::async_connect(socket_, endpoint_iterator,
				boost::bind(&client::handle_connect, this,
				boost::asio::placeholders::error));
		}
		else
		{
			writeToLog("有错误");
			cm.addition_msg = err.message();
			cm.addition_msg += "when handle resolve";
		}
	}

	void handle_connect(const boost::system::error_code& err)
	{
		writeToLog("进入connect回调函数");
		if(cdd.is_IP_Check)
		{
			cm.remoteIP = socket_.remote_endpoint().address().to_string();
			writeToLog("IP");
		}
		if (!err)
		{
			writeToLog("没有错误");
			// The connection was successful. Send the request.
			boost::asio::async_write(socket_, request_,
				boost::bind(&client::handle_write_request, this,
				boost::asio::placeholders::error));
			writeToLog("绑定结束");
		}
		else
		{
			writeToLog("有错误");
			//cm.addition_msg =  err.message();
			cm.addition_msg += "when handle connect";
		}
	}

	void handle_write_request(const boost::system::error_code& err)
	{
		writeToLog("进入write request回调函数");
		if(!cdd.is_CODE_check)
			return;
		if (!err)
		{
			writeToLog("没有错误");
			// Read the response status line. The response_ streambuf will
			// automatically grow to accommodate the entire line. The growth may be
			// limited by passing a maximum size to the streambuf constructor.
			boost::asio::async_read_until(socket_, response_, "\r\n",
				boost::bind(&client::handle_read_status_line, this,
				boost::asio::placeholders::error));
		}
		else
		{
			writeToLog("有错误");
			//cm.addition_msg = err.message();
			cm.addition_msg += "when handle write request";
		}
	}

	void handle_read_status_line(const boost::system::error_code& err)
	{
		writeToLog("进入read status line回调函数");
		if (!err)
		{
			writeToLog("没有错误");
			// Check that response is OK.
			std::istream response_stream(&response_);
			std::string http_version;
			response_stream >> http_version;
			unsigned int status_code;
			response_stream >> status_code;
			cm.status_code = status_code;
			writeToLog("status_code");
			std::string status_message;
			std::getline(response_stream, status_message);
			if (!response_stream || http_version.substr(0, 5) != "HTTP/")
			{
				cm.addition_msg += "Invalid response";
				return;
			}
			if (status_code != 200)
			{
				writeToLog("有错误");
				cm.addition_msg += "Response returned with status code " + status_code;
				return;
			}

		}
		else
		{
			//cm.addition_msg =  err.message();
			cm.addition_msg += "when handle read status";
		}
	}
	tcp::resolver resolver_;
	tcp::socket socket_;
	boost::asio::streambuf request_;
	boost::asio::streambuf response_;

	
	/*===add for test timer===*/
	deadline_timer timer_;  
	CModel &cm;
	CDlgDTO &cdd;

	void close()  
	{  
		
		cm.addition_msg += "链接超时";
		socket_.close();  
	}
	/*==add for test timer=====*/
};

//int main(int argc, char* argv[])
//{
//
//	argc = 3;
//	argv[1] = "jdjdjd";
//	argv[2] = "jdjdjd";
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
//	return 0;
//}