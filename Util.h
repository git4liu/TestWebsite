
// Util.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUtilApp:
// �йش����ʵ�֣������ Util.cpp
//

class CUtilApp : public CWinApp
{
public:
	CUtilApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	DECLARE_MESSAGE_MAP()
};

extern CUtilApp theApp;