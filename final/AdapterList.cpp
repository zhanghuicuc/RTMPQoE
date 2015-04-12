// AdapterList.cpp : 实现文件
//

#include "stdafx.h"
#include "final.h"
#include "AdapterList.h"
#include "afxdialogex.h"


// AdapterList 对话框

IMPLEMENT_DYNAMIC(AdapterList, CDialogEx)

AdapterList::AdapterList(CWnd* pParent /*=NULL*/)
	: CDialogEx(AdapterList::IDD, pParent)
{

}

AdapterList::~AdapterList()
{
}

BOOL AdapterList::OnInitDialog()
{
	CDialogEx::OnInitDialog();//这一句很重要不能丢！
	//整行选择；有表格线；表头；单击激活
	DWORD dwExStyle=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE;
	//报表风格；单行选择；高亮显示选择行
	//视频
	m_devlist.ModifyStyle(0,LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
	m_devlist.SetExtendedStyle(dwExStyle);

	m_devlist.InsertColumn(0,"No.",LVCFMT_CENTER,30,0);
	m_devlist.InsertColumn(1,"name",LVCFMT_LEFT,130,0);
	m_devlist.InsertColumn(2,"description",LVCFMT_LEFT,300,0);

	//get_devlist(this);
	return TRUE;
}

void AdapterList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_devlist);
}


BEGIN_MESSAGE_MAP(AdapterList, CDialogEx)
	ON_BN_CLICKED(IDOK, &AdapterList::OnBnClickedOk)
END_MESSAGE_MAP()


// AdapterList 消息处理程序
void AdapterList::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
	for(int i=0; i<m_devlist.GetItemCount(); i++)
	{
		if(m_devlist.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED )
		{
			list_selected_dlg=i;
		}
    }

	ShowWindow(SW_HIDE);
}
