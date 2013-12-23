#pragma once
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include "Model.h"
#include "DlgDTO.h"


using namespace boost::asio;  
using boost::asio::ip::tcp;  

class HttpQuery
{
	

public:
	HttpQuery(void);
	~HttpQuery(void);

	void start(CModel &cm  , CDlgDTO & cdd);

	void close()
	{
		socket.close();
	}

private:
	tcp::socket socket;

};

