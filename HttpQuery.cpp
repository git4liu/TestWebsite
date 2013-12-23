#include "StdAfx.h"
#include "HttpQuery.h"
#include <boost/bind.hpp> 
#include <boost/date_time/posix_time/posix_time_types.hpp> 


HttpQuery::HttpQuery(void)
{

}


HttpQuery::~HttpQuery(void)
{
}

void HttpQuery::start(CModel &cm  , CDlgDTO & cdd)
{
	io_service ios;
	deadline_timer timer(ios);

	unsigned int time_out = cdd.time_out ;
	BOOL is_ip_check = cdd.is_IP_Check;
	BOOL is_code_check = cdd.is_CODE_check;

	using namespace std;
	using boost::asio::ip::tcp;
	string url = cm.url;
	string hostname = url;
	string requestname = "http://" + url + "/";

	timer.expires_from_now(boost::posix_time::seconds(time_out));
	timer.async_wait(boost::bind(&HttpQuery::close , this));

	try
	{
		//prepare
		boost::asio::io_service io_service;
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(hostname, "http");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		socket = tcp::socket(io_service);
		boost::asio::connect(socket, endpoint_iterator);


		//getIP
		string remoteIP = socket.remote_endpoint().address().to_string();
		if(is_ip_check)
			cm.remoteIP = remoteIP;


		if(!is_code_check)
			return ;

		//send http request
		boost::asio::streambuf request;
		ostream request_stream(&request);
		request_stream << "GET " << requestname << " HTTP/1.0\r\n";
		request_stream << "Host: " << hostname << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";
		boost::asio::write(socket, request);
		
		//receive http response
		boost::asio::streambuf response;
		boost::asio::read_until(socket, response, "\r\n" );
		istream response_stream(&response);
		
		//manage return message
		string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		cm.status_code = status_code;
		string status_message;
		getline(response_stream, status_message);
		cm.addition_msg = status_message;

		
		
		//error response
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			cm.addition_msg = "Invalid response";
		}
		if (status_code != 200)
		{
			cm.addition_msg = "返回了不正常的状态码" ;
		}

		socket.close();

	}
	catch (std::exception& e)
	{
		cm.addition_msg = e.what();
		
	}
}
