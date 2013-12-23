
// UtilDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "Model.h"
#include "afxwin.h"



// CUtilDlg �Ի���
class CUtilDlg : public CDialogEx
{
// ����
public:
	CUtilDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_UTIL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
