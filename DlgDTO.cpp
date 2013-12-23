#include "StdAfx.h"
#include "DlgDTO.h"

CDlgDTO::~CDlgDTO(void)
{
}

CDlgDTO::CDlgDTO()
{
	this->time_out = 10;
	this->is_CODE_check = 1;
	this->is_IP_Check = 1;
}

CDlgDTO::CDlgDTO( CDlgDTO  &cdd )
{
	this->is_CODE_check = cdd.is_CODE_check;
	this->is_IP_Check = cdd.is_IP_Check;
	this->time_out = cdd.time_out;
}
