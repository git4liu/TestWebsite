#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <string>
#include "Model.h"
#include "DlgDTO.h"


using namespace std;


void writeToLog(char * str)
{
	FILE * pfile;
	fopen_s( &pfile , "httprequest.log" , "a+");
	if (pfile == NULL)
	{
		return ;
	}
	fprintf_s( pfile , "%s\n" , str);
	fclose( pfile );
}

class HttpRequest
{
private:
	CDlgDTO  cdd;
	boost::asio::ip::tcp::socket *_socket;
public:
	HttpRequest(CDlgDTO  _cdd): cdd( _cdd){}

	void request(std::vector<CModel>::iterator it)
	{
		boost::asio::io_service ios;
		boost::asio::deadline_timer t_reqeust(ios);
		boost::asio::deadline_timer t_timer(ios);


		t_timer.expires_from_now(boost::posix_time::seconds(cdd.time_out));
		t_reqeust.expires_from_now(boost::posix_time::seconds(0));


		t_timer.async_wait(boost::bind(&HttpRequest::timer_handler , this , &ios , it));
		t_reqeust.async_wait(boost::bind(&HttpRequest::request_handler , this , &ios , it ));

		ios.run();
	}

	void request_handler(boost::asio::io_service * ios , std::vector<CModel>::iterator it )
	{
		//boost::this_thread::sleep(boost::posix_time::seconds(10));
		BOOL is_ip_check = cdd.is_IP_Check;
		BOOL is_code_check =  cdd.is_CODE_check;

		using namespace std;
		using boost::asio::ip::tcp;
		string url = it->url;
		string hostname = url;
		string requestname = "http://" + url + "/";

		try
		{
			//prepare
			boost::asio::io_service io_service;
			tcp::resolver resolver(io_service);
			tcp::resolver::query query(hostname, "http");
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			tcp::socket socket(io_service);
			_socket = &socket;
			boost::asio::connect(socket, endpoint_iterator);


			//getIP
			string remoteIP = socket.remote_endpoint().address().to_string();
			if(is_ip_check)
				it->remoteIP = remoteIP;


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
			it->status_code = status_code;
			string status_message;
			getline(response_stream, status_message);
			it->addition_msg = status_message;

			//error response
			if (!response_stream || http_version.substr(0, 5) != "HTTP/")
			{
				it->addition_msg = "Invalid response";
			}
			if (status_code != 200)
			{
				it->addition_msg = "返回了不正常的状态码" ;
			}

			socket.close();
		}
		catch (std::exception& e)
		{
			it->addition_msg = e.what();

		}
		ios->stop();
	}

	void timer_handler(boost::asio::io_service * ios , std::vector<CModel>::iterator it)
	{
		
		it->addition_msg = "已经超时";
		_socket->close();
		ios->stop();
	}

};
