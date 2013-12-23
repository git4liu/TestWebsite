
// UtilDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "Model.h"
#include "afxwin.h"



// CUtilDlg 对话框
class CUtilDlg : public CDialogEx
{
// 构造
public:
	CUtilDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_UTIL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
	
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;
	int m_thread;
	int m_timeout;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedImport();
	void UpdateListAll();
	void UpdateListSingle( CModel &website);
	afx_msg void OnBnClickedExport();
	afx_msg void OnBnClickedStart();
	CString m_editUrls;
	afx_msg void OnBnClickedCopy();
	CButton m_ip_check;
	CButton m_code_check;
	afx_msg void OnEnChangeEditurls();
	void notify(CString msg);
	CButton m_c_btn_start;
};
