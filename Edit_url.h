#pragma once


// CEdit_url

class CEdit_url : public CEdit
{
	DECLARE_DYNAMIC(CEdit_url)

public:
	CEdit_url();
	virtual ~CEdit_url();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


