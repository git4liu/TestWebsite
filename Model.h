#pragma once
#include "stdafx.h"

class CModel
{
public:
	CModel(void);
	~CModel(void);
public:
	std::string url;
	BOOL visited;
	int status_code;
	std::string addition_msg;
	int item_id;
	std::string remoteIP;
};

