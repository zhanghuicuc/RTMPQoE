// Score.cpp : 实现文件
//

#include "stdafx.h"
#include "final.h"
#include "Score.h"
#include "afxdialogex.h"

// Score 对话框

IMPLEMENT_DYNAMIC(Score, CDialogEx)

Score::Score(CWnd* pParent /*=NULL*/)
	: CDialogEx(Score::IDD, pParent)
{
	//  m_video_score_value = 0.0f;
	m_access_score_value = 0.0f;
	m_audio_score_value = 0.0f;
	m_buf_score_value = 0.0f;
	m_sync_score_value = 0.0f;
	m_video_score_value = 0.0f;
}

Score::~Score()
{
}
BOOL Score::OnInitDialog()
{
	CDialogEx::OnInitDialog();//这一句很重要不能丢！
	lineSeries2= (CSeries)m_chart3.Series(0);
	lineSeries3= (CSeries)m_chart3.Series(1);
	lineSeries4= (CSeries)m_chart3.Series(2);
	lineSeries5= (CSeries)m_chart3.Series(3);
	page3=m_chart3.get_Page();
	SeriesMarks2=(CMarks)lineSeries2.get_Marks();
	SeriesMarks3=(CMarks)lineSeries3.get_Marks();
	SeriesMarks4=(CMarks)lineSeries4.get_Marks();
	SeriesMarks5=(CMarks)lineSeries5.get_Marks();
	//SeriesMarks2.put_Visible(TRUE);
	//SeriesMarks3.put_Visible(TRUE);
	//SeriesMarks4.put_Visible(TRUE);
	//SeriesMarks5.put_Visible(TRUE);
	ClearAllSeries3();
	
	/*_CRT_FLOAT fltval;
	CString weight;
	_CRT_FLOAT sum;sum.f=0;
	for(int m=0,n=1035;m<12;m++,n++)
	{
		GetDlgItemText(n,weight);
		_atoflt(&fltval,weight.GetBuffer(weight.GetLength()));
		sum.f+=fltval.f;
	}
	if(sum.f!=1)
		MessageBox("权重之和不为1，请重新输入权重值","ERROR",MB_OK);*///float加着加着后面的精度就全乱了

	return TRUE;
}
void Score::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART4, m_chart3);
	DDX_Control(pDX, IDC_SCORE_ACCESS, m_score_access);
	DDX_Control(pDX, IDC_SCORE_VIDEO, m_score_video);
	DDX_Control(pDX, IDC_SCORE_AUDIO, m_score_audio);
	DDX_Control(pDX, IDC_SCORE_BUF, m_score_buf);
	DDX_Control(pDX, IDC_SCORE_SYNC, m_score_sync);
	//  DDX_Text(pDX, IDC_SCORE_ACCESS, m_video_score_value);
	DDX_Text(pDX, IDC_SCORE_ACCESS, m_access_score_value);
	DDX_Text(pDX, IDC_SCORE_AUDIO, m_audio_score_value);
	DDX_Text(pDX, IDC_SCORE_BUF, m_buf_score_value);
	DDX_Text(pDX, IDC_SCORE_SYNC, m_sync_score_value);
	DDX_Text(pDX, IDC_SCORE_VIDEO, m_video_score_value);
}


BEGIN_MESSAGE_MAP(Score, CDialogEx)
	ON_BN_CLICKED(IDOK, &Score::OnBnClickedOk)
END_MESSAGE_MAP()


// Score 消息处理程序


void Score::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
	ShowWindow(SW_HIDE);
}

void Score::ClearAllSeries3(void)
{	
	for(long l=0;l<m_chart3.get_SeriesCount();l++)
	{
		((CSeries)m_chart3.Series(l)).Clear();
	}
}