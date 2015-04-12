#pragma once
#include "afxwin.h"
#include "resource.h"
#include "stdafx.h"
#include "afxcmn.h"
//#include "net_qoe.h"这里不能包含，否则有错
//#include "finalDlg.h"

// AdapterList 对话框

class AdapterList : public CDialogEx
{
	DECLARE_DYNAMIC(AdapterList)

public:
	AdapterList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~AdapterList();

// 对话框数据
	enum { IDD = IDD_DEVLIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL AdapterList::OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedOk();
	CListCtrl m_devlist;
};
