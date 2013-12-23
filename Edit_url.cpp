// Edit_url.cpp : 实现文件
//

#include "stdafx.h"
#include "Util.h"
#include "Edit_url.h"


// CEdit_url

IMPLEMENT_DYNAMIC(CEdit_url, CEdit)

CEdit_url::CEdit_url()
{

}

CEdit_url::~CEdit_url()
{
}


BEGIN_MESSAGE_MAP(CEdit_url, CEdit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CEdit_url 消息处理程序




BOOL CEdit_url::PreTranslateMessage(MSG* pMsg)
{
	AfxMessageBox("d");
	if(pMsg->message == WM_KEYDOWN)
	{
		BOOL b = GetKeyState(VK_CONTROL) & 0x80;
		if(b && (pMsg->wParam=='a'||pMsg->wParam=='A'))
		{
			SetSel(0,-1);
			return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}


void CEdit_url::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	AfxMessageBox("dbclick");
	//SetSel(0,-1);
}
