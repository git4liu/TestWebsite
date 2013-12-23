#pragma once

class CDlgDTO
{
public:
	CDlgDTO();
	CDlgDTO(CDlgDTO & cdd);
	CDlgDTO(int _time_out , BOOL _is_IP_CHECK , BOOL _is_CODE_CHECK) 
		: time_out(_time_out) , is_IP_Check(_is_IP_CHECK) , is_CODE_check(_is_CODE_CHECK) {}
	~CDlgDTO(void);

public:
	unsigned int time_out;
	BOOL is_IP_Check;
	BOOL is_CODE_check;

	void operator=(CDlgDTO cdd)
	{
		this->is_CODE_check = cdd.is_CODE_check;
		this->is_IP_Check = cdd.is_IP_Check;
		this->time_out = cdd.time_out;
	}
};

