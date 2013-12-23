
// UtilDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Util.h"
#include "UtilDlg.h"
#include "afxdialogex.h"
#include "Model.h"
#include <sstream>
//#include "InternetService.h"
#include <boost/thread/thread.hpp>
#include "DlgDTO.h"
#include "HttpRequest.hpp"
#include "HttpRequest2.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::vector<CModel> websites;


struct function_thread
{
	function_thread(CUtilDlg &_dlg , CDlgDTO _cdd)
		:dlg(_dlg) , cdd(_cdd)
	{}
	void operator()()
	{
		std::vector<CModel>::iterator it = websites.begin();
		while(it != websites.end())
		{
			/*HttpRequest hr(cdd);
			hr.request(it);*/
			boost::asio::io_service ios;
			client c(ios , it , cdd);
			ios.run();
			dlg.UpdateListSingle(*it);
			++ it;
		}
		dlg.notify("���");
		//CInternetService::access_thread(websites , cdd , dlg);
	}
	CUtilDlg &dlg;
	CDlgDTO cdd;
};

// CUtilDlg �Ի���

//ȥ��string���ߵĿհ�
std::string trimBlank(std::string & str)
{
	str.erase(0, str.find_first_not_of(" \n"));
	str.erase(str.find_last_not_of(' ') + 1);
	return str;
}

//�ַ����ָ��CString
std::vector<std::string> split(CString str,std::string pattern)
{
	std::vector<std::string> result;
	CStringArray sa;

	while(1){
		int pos = str.Find("\n");
		if(pos>=0){
			result.push_back(str.Left(pos).Trim().GetBuffer());
			str = str.Mid(pos+1);
		} else {
			result.push_back(str.Trim().GetBuffer());
			break;
		}
	}

	return result;

}

//�ַ����ָ��string
std::vector<std::string> split(std::string str,std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str+=pattern;
	unsigned int size=str.size();

	for(unsigned int i=0; i<size; i++)
	{
		pos=str.find(pattern,i);
		if(pos<size)
		{
			std::string s=str.substr(i,pos-i);
			result.push_back(s);
			i=pos+pattern.size()-1;
		}
	}
	return result;
}

CUtilDlg::CUtilDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUtilDlg::IDD, pParent)
	, m_thread(0)
	, m_timeout(0)
	, m_editUrls(_T(""))
{
	//�޸Ľ���ͼ��
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CUtilDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_GETTHREAD, m_thread);
	DDX_Text(pDX, IDC_GETTIMEOUT, m_timeout);
	DDX_Text(pDX, IDC_EditUrls, m_editUrls);
	DDX_Control(pDX, IDC_IP_CHECK, m_ip_check);
	DDX_Control(pDX, IDC_CODE_CHECK, m_code_check);
	DDX_Control(pDX, IDC_START, m_c_btn_start);
}


//�ڴ˴�����Զ������Ϣ
BEGIN_MESSAGE_MAP(CUtilDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_IMPORT, &CUtilDlg::OnBnClickedImport)
	ON_BN_CLICKED(IDC_EXPORT, &CUtilDlg::OnBnClickedExport)
	ON_BN_CLICKED(IDC_START, &CUtilDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_COPY, &CUtilDlg::OnBnClickedCopy)
	ON_EN_CHANGE(IDC_EditUrls, &CUtilDlg::OnEnChangeEditurls)
END_MESSAGE_MAP()


// CUtilDlg ��Ϣ�������

BOOL CUtilDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	//ListControl��ӱ���
	m_list.InsertColumn(0,"��ַ",LVCFMT_LEFT,150,-1);
	m_list.InsertColumn(1,"������",LVCFMT_LEFT,60,-1);
	m_list.InsertColumn(2,"����IP",LVCFMT_LEFT,150,-1);
	m_list.InsertColumn(3,"��ע",LVCFMT_LEFT,150,-1 );
	m_list.InsertColumn(4,"���",LVCFMT_LEFT ,90 ,-1);

	//����Ĭ�ϲ���
	m_timeout = 5 ;
	m_thread = 1 ;
	m_ip_check.SetCheck(BST_CHECKED);
	m_code_check.SetCheck(BST_CHECKED);
	UpdateData(FALSE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUtilDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CUtilDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CUtilDlg::PreTranslateMessage(MSG* pMsg)
{


	if(pMsg->message==WM_KEYDOWN)
	{
		if ( pMsg->wParam=='A' && GetKeyState(VK_CONTROL)<0 )
		{
			// User pressed Ctrl-A.  Let's select-all
			CWnd * wnd = GetFocus() ;
			//if ( wnd && IsEdit(wnd) )
				((CEdit *)wnd)->SetSel(0,-1) ;
		}
		char name[256] = "";
		::GetClassName(pMsg->hwnd , name , 256);
		if(strcmp(name , "SysListView32") == 0)
		{
			if(pMsg->wParam == 'C' && GetKeyState(VK_CONTROL) <0 )
			{
				using namespace std;
				string source = "";
				stringstream ss;
				POSITION pos = m_list.GetFirstSelectedItemPosition();
				while(pos)
				{
					int rowNum = m_list.GetNextSelectedItem(pos);
					char buf[256] = "";
					m_list.GetItemText(rowNum , 0 , buf , 256);
					ss << buf << "\n";
					/*ZeroMemory(buf , 256);
					m_list.GetItemText(rowNum , 1 , buf , 256);
					ss << buf << "		";
					ZeroMemory(buf , 256);
					m_list.GetItemText(rowNum , 2 , buf , 256);
					ss << buf << "		";
					ZeroMemory(buf , 256);
					m_list.GetItemText(rowNum , 3 , buf , 256);
					ss << buf << "\n";
					ZeroMemory(buf , 256);*/
				}

				source = ss.str();
				if(OpenClipboard())
				{
					HGLOBAL  clipbuffer;  
					char   *buffer;  
					EmptyClipboard();  
					clipbuffer=GlobalAlloc(GMEM_DDESHARE,source.length()+1);  
					buffer=(char   *)GlobalLock(clipbuffer);  
					strcpy(buffer,source.c_str());  
					::GlobalUnlock(clipbuffer); 
					SetClipboardData(CF_TEXT,clipbuffer);  
					CloseClipboard(); 
				}

			}
		}
	}   
	
	//���˻س��Զ��˳��¼�
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return false;
	return CDialogEx::PreTranslateMessage(pMsg);
}

//���밴ť���¼�����
void CUtilDlg::OnBnClickedImport()
{
	m_list.DeleteAllItems();
	websites.clear();
	CString szFilter = "text|*.txt|*.*|*.*||";
	CFileDialog dlg(TRUE , CString(".txt") , CString("website.txt") ,OFN_HIDEREADONLY , szFilter , NULL );
	if(dlg.DoModal()==IDOK)
	{
		CString FilePathName=dlg.GetPathName();
		using namespace std;
		ifstream  fin(FilePathName);
		char c[80];
		while(!fin.eof())
		{
			fin.getline(c , 80);
			m_editUrls.Append(c);
			m_editUrls.Append("\r\n");
		}
		fin.close();
	}
	UpdateData(FALSE);
}

//������ť���¼�����
void CUtilDlg::OnBnClickedExport()
{
	CString szFilter = "text|*.txt|*.*|*.*||";
	CFileDialog dlg(FALSE,  CString(".txt"), CString("websiteDone.txt"), OFN_HIDEREADONLY, szFilter, NULL);
	if(dlg.DoModal() == IDOK)
	{
		CString pathName=dlg.GetPathName();
		using namespace std;
		ofstream fout(pathName);
		std::vector<CModel>::iterator it= websites.begin();
		while(it != websites.end())
		{
			stringstream ss;
			ss << it->url;
			ss << "		";
			if(it->status_code <= 0)
				ss << "---" << "		";
			else 
				ss << it->status_code << "		";
			if(it->remoteIP == "")
				ss << "---.---.---.---" << "		";
			else
				ss << it->remoteIP << "		";
			ss << trimBlank(it->addition_msg);
			ss << "\n";
			string temp = ss.str();
			fout.write(temp.c_str() , temp.length());
			it ++;
		}
		fout.close();
	}
}

//��ʼ��ť���¼�������
void CUtilDlg::OnBnClickedStart()
{

	m_c_btn_start.EnableWindow(FALSE);

	//���±���������
	UpdateData(TRUE);

	//�½����ݴ������
	CDlgDTO cdd(m_timeout , m_ip_check.GetCheck() , m_code_check.GetCheck());

	//��ȡ�ı��༭����ı�������string�б�
	std::vector<std::string> urlSplit = split(m_editUrls.Trim() , "\n");
	std::vector<std::string>::iterator it = urlSplit.begin();
	
	//�������������б��
	websites.clear(); 
	m_list.DeleteAllItems();
	int i = 0;
	while(it != urlSplit.end())
	{
		if((*it) == "")
		{
			it ++ ;
			i ++ ;
			continue;
		}
		
		CModel cm ;
		cm.visited = FALSE;
		cm.url = *it ;
		cm.item_id = i;
		cm.status_code = 0 ;

		//����listControl
		m_list.InsertItem(i , cm.url.c_str());
		m_list.SetItemText(cm.item_id , 1 , "");
		m_list.SetItemText(cm.item_id , 2 , "");
		m_list.SetItemText(cm.item_id , 3 , "δ����");
		m_list.Update(i);

		//��������
		websites.push_back(cm);
		it ++ ;
		i ++ ;
	}

	//��ʼ��ַ�����߳�
	boost::thread thrd1(function_thread(*this , cdd));
}

void CUtilDlg::UpdateListSingle( CModel &website)
{
	CString code;
	code.Format("%d" , website.status_code);
	
	if(website.status_code <= 0)
		m_list.SetItemText(website.item_id , 1 , "---");
	else	
		m_list.SetItemText(website.item_id , 1 , code);
	
	if(website.remoteIP == "")
		m_list.SetItemText(website.item_id , 2 , "---.---.---.---");
	else
		m_list.SetItemText(website.item_id , 2 , website.remoteIP.c_str());
	
	m_list.SetItemText(website.item_id , 3 , website.addition_msg.c_str() );

	CString quence;
	quence.Format("%d" , website.item_id);
	m_list.SetItemText(website.item_id , 4 ,quence );
	m_list.Update(website.item_id);
}

void CUtilDlg::OnBnClickedCopy()
{
	using namespace std;
	string source = "";
	std::vector<CModel>::iterator it = websites.begin();
	while(it != websites.end())
	{
		stringstream ss;
		ss << it->url;
		ss << "		";
		if(it->status_code <= 0)
			ss << "---" << "		";
		else 
			ss << it->status_code << "		";
		if(it->remoteIP == "")
			ss << "---.---.---.---" << "		";
		else
			ss << it->remoteIP << "		";
		ss << it->addition_msg;
		ss << "\n";
		string temp = ss.str();
		source += temp ;
		it ++;
	}
	if(OpenClipboard())
	{
		HGLOBAL  clipbuffer;  
		char   *buffer;  
		EmptyClipboard();  
		clipbuffer=GlobalAlloc(GMEM_DDESHARE,source.length()+1);  
		buffer=(char   *)GlobalLock(clipbuffer);  
		strcpy(buffer,source.c_str());  
		::GlobalUnlock(clipbuffer); 
		SetClipboardData(CF_TEXT,clipbuffer);  
		CloseClipboard(); 
	}
}






void CUtilDlg::OnEnChangeEditurls()
{
	
}

void CUtilDlg::notify( CString msg )
{
	AfxMessageBox(msg);
	m_c_btn_start.EnableWindow(TRUE);
}
