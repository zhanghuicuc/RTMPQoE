
// finalDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "finalDlg.h"
#include"resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HANDLE hNetMutex1;
HANDLE hThread_dl_param;
HANDLE hThread_dl_speed;
HANDLE hThread_adp_sel;
//static int adp_selection=-1;
static int recv_dnsdelay_data=0;
static __int64 dlspeed_total=0;
static __int64 dlspeed_avg=0;
static int dlspeed_checknum=0;
static int ever_stopped=0;

typedef struct delay_param
{
	int dns_delay;
	int tcp_delay;
	int rtmp_delay;
}delay_param;

// CfinalDlg 对话框
CfinalDlg::CfinalDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CfinalDlg::IDD, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	memset(charData_pause_time,0,sizeof(charData_pause_time));
	dnsdelay1=0;
	tcpdelay1=0;
	rtmpdelay1=0;
	m_resolution_value = _T("");
	m_bpp_value = _T("");
	m_bitrate_value = _T("");
	m_pause_freq_value = _T("");
	m_paused_time_value = _T("");
}

void CfinalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIDEOCODEC, m_videocodec);
	DDX_Control(pDX, IDC_BITRATE, m_bitrate);
	DDX_Control(pDX, IDC_FORMAT, m_format);
	DDX_Control(pDX, IDC_INPUT_PROTOCOL, m_input_protocol);
	DDX_Control(pDX, IDC_RESOLUTION, m_resolution);
	DDX_Control(pDX, IDC_FRAMERATE, m_framerate);
	DDX_Control(pDX, IDC_PIXFMT, m_pixfmt);
	DDX_Control(pDX, IDC_AUDIOSAMPLERATE, m_audiosamplerate);
	DDX_Control(pDX, IDC_AUDIOCODEC, m_audiocodec);
	DDX_Control(pDX, IDC_AUDIOCHANNEL, m_audiochannel);
	DDX_Control(pDX, IDC_METADATA, m_metadata);
	DDX_Control(pDX, IDC_PAUSE_FREQ, m_paused_num);
	DDX_Control(pDX, IDC_PAUSE_TIME, m_paused_time);
	DDX_Control(pDX, IDC_TCHART1, m_chart0);
	DDX_Control(pDX, IDC_TCHART3, m_chart1);
	DDX_Control(pDX, IDC_TCHART2, m_chart2);
	DDX_Control(pDX, IDC_DOWNLOAD_SPEED, m_dl_speed);
	DDX_Control(pDX, IDC_AUDIO_BITRATE, m_a_bitrate);
	DDX_Control(pDX, IDC_PAUSE_GAP, m_pause_gap);
	DDX_Control(pDX, IDC_BUFF_TIME, m_buff_time);
	DDX_Control(pDX, IDC_BPP, m_bpp);
	DDX_Control(pDX, IDC_QP, m_qp);
	DDX_Text(pDX, IDC_RESOLUTION, m_resolution_value);
	DDX_Text(pDX, IDC_BPP, m_bpp_value);
	DDX_Text(pDX, IDC_BITRATE, m_bitrate_value);
	DDX_Text(pDX, IDC_PAUSE_FREQ, m_pause_freq_value);
	DDX_Text(pDX, IDC_PAUSE_TIME, m_paused_time_value);
}

BEGIN_MESSAGE_MAP(CfinalDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PLAY, &CfinalDlg::OnBnClickedPlay)
	ON_MESSAGE(WM_DL_PARAM,OnDlParam)//消息映射
	ON_MESSAGE(WM_DL_SPEED,OnDlSpeed)//消息映射
	//ON_MESSAGE(WM_ADP_SEL,OnAdpSel)//消息映射
	ON_BN_CLICKED(IDOK, &CfinalDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SCORE, &CfinalDlg::OnBnClickedScore)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CHOOSE_DEV, &CfinalDlg::OnBnClickedChooseDev)
	ON_BN_CLICKED(IDC_WRITE, &CfinalDlg::OnBnClickedWrite)
	ON_BN_CLICKED(IDCANCEL, &CfinalDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CfinalDlg 消息处理程序

BOOL CfinalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	lineSeries_v_bitrate=(CSeries)m_chart0.Series(0);//一定要放到这里，因为lineseries是在对象构造好之后才有的
	lineSeries_a_bitrate=(CSeries)m_chart0.Series(1);
	barSeries0= (CSeries)m_chart1.Series(0);
	lineSeries1= (CSeries)m_chart2.Series(0);

	SeriesMarks=(CMarks)barSeries0.get_Marks();
	SeriesMarks1=(CMarks)lineSeries1.get_Marks();
	//SeriesMarks1.put_Visible(TRUE);

	page0=m_chart0.get_Page();
	page1=m_chart1.get_Page();
	page2=m_chart2.get_Page();

	//子窗口,要建立非模态对话框需要调用两个函数Create()和ShowWindow()
	score=new Score;
	score->Create(IDD_DIALOG1,NULL);
	//adplist=new AdapterList;
	//adplist->Create(IDD_DEVLIST_DLG,NULL);
	/*get_devlist(adplist);
	adplist->ShowWindow(SW_SHOW);*/

	hNetMutex1=CreateMutex(NULL,FALSE,NULL);
	//hThread_adp_sel=CreateThread(NULL,0,adp_sel,(LPVOID)adplist->m_hWnd,0,NULL);
	hThread_dl_param=CreateThread(NULL,0,qoe_dl_param,(LPVOID)m_hWnd,0,NULL);
	hThread_dl_speed=CreateThread(NULL,0,qoe_dl_speed,(LPVOID)m_hWnd,0,NULL);
	//CloseHandle(hThread_dl_param);
	//CloseHandle(hThread_dl_speed);

	//rtmp下拉地址列表
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("KBS24-800：rtmp://news24kbs-2.gscdn.com/news24_800/news24_800.stream");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("教育1台：rtmp://pub1.guoshi.com/live/newcetv1");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("教育3台：rtmp://pub1.guoshi.com/live/newcetv3");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("香港卫视：rtmp://live.hkstv.hk.lxdns.com/live/hks");
	((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString("中石油：rtmp://wowza.sinopectv.cn:1935/live/sinopec");

	//记录结果的表格
	fopen_s( &result, "result.csv", "a+" );
	//fprintf(result,"测试频道,视频分辨率（尺寸）,bpp,视频码率,缓冲次数,缓冲时长,测试时长,接入性得分,视频质量得分,缓冲情况得分\n");
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CfinalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CfinalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CfinalDlg::OnBnClickedPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	ClearAllSeries0();
	ClearAllSeries1();
	ClearAllSeries2();
	score->ClearAllSeries3();
	
	play_sec_dlg=0;
	//ResumeThread(hThread_adp_sel);
	//ResumeThread(hThread_dl_param);
	//ResumeThread(hThread_dl_speed);

	CString str;
	int index;
	index=((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
	switch(index)
	{
	
	case 0:
		str="rtmp://news24kbs-2.gscdn.com/news24_800/news24_800.stream";
		break;
	case 1:
		str="rtmp://pub1.guoshi.com/live/newcetv1";
		break;
	case 2:
		str="rtmp://pub1.guoshi.com/live/newcetv3";
		break;
	case 3:
		str="rtmp://live.hkstv.hk.lxdns.com/live/hks";
		break;
	case 4:
		str="rtmp://wowza.sinopectv.cn:1935/live/sinopec";
		break;
	case -1:			//此时读取用户输入的内容
		((CComboBox*)GetDlgItem(IDC_COMBO1))->GetWindowTextA(str);
	default:
		break;
	}

	rtmpurl=str.GetBuffer(str.GetLength());	//CString转char*
	fwrite(rtmpurl,sizeof(char),str.GetLength(),result);
	CfinalDlg* pDlg=this;
	ffplay(pDlg);
}

//在画线之前把图形清除一下，否则会覆盖，清除可用CSeries的函数Clear();
//通过CTchart 的get_SeriesCount函数获得所有图像序列，再全部清除，这个函数经常用到，可用定义为类成员函数，
void CfinalDlg::ClearAllSeries0(void)
{	
	for(long i=0;i<m_chart0.get_SeriesCount();i++)
	{
		((CSeries)m_chart0.Series(i)).Clear();
	}
}

void CfinalDlg::ClearAllSeries1(void)
{	
	for(long j=0;j<m_chart1.get_SeriesCount();j++)
	{
		((CSeries)m_chart1.Series(j)).Clear();
	}
}

void CfinalDlg::ClearAllSeries2(void)
{	
	for(long k=0;k<m_chart2.get_SeriesCount();k++)
	{
		((CSeries)m_chart2.Series(k)).Clear();
	}
}
//--------------------------------------------------------------------------------------------------------------

//对应于createthread的线程响应函数，选择适配器
/*DWORD WINAPI CfinalDlg::adp_sel(LPVOID lpParameter)
{
	WaitForSingleObject(hNetMutex1,INFINITE);		
	while(1)
	{
		HWND hwnd=(HWND)lpParameter;	
		::SendMessageA(hwnd,WM_ADP_SEL,NULL,NULL);//将消息传递给对话框,delay不用加&
		//return 0;//线程函数中返回了就是退出线程了，同时mutex也变为有信号状态
		ReleaseMutex(hNetMutex1);
		SuspendThread(hThread_adp_sel);
	}
	return 0;//线程函数中返回了就是退出线程了，同时mutex也变为有信号状态
}

//WM_ADP_SEL消息响应函数的具体实现
LRESULT CfinalDlg::OnAdpSel(WPARAM wParam,LPARAM lParam)
{
	//get_devlist(adplist);
	//return adplist->ShowWindow(SW_SHOW);
	//return 0;
}*/

//对应于createthread的线程响应函数，监听各种时延
DWORD WINAPI CfinalDlg::qoe_dl_param(LPVOID lpParameter)
{
	WaitForSingleObject(hNetMutex1,INFINITE);		
	while(1)
	{
		/*if(list_selected_dlg==-1)
		{	
			//ReleaseMutex(hNetMutex1);
			//SuspendThread(hThread_dl_param);
			continue;
		}*/
		HWND hwnd=(HWND)lpParameter;
		delay_param* dp=delay_qoe();//若设置线程创建就执行，那么顺序是这样的，先oncreate-〉oninitdialog->createthread->到这里-〉delay_qoe->出现主界面
		::SendMessageA(hwnd,WM_DL_PARAM,NULL,(LPARAM)dp);//将消息传递给对话框,delay不用加&
		//return 0;//线程函数中返回了就是退出线程了，同时mutex也变为有信号状态
		ReleaseMutex(hNetMutex1);
		if(ever_stopped)
		{
			::ResumeThread(hThread_dl_speed);
			ever_stopped=0;
		}
		::SuspendThread(hThread_dl_param);
	}
	return 0;//线程函数中返回了就是退出线程了，同时mutex也变为有信号状态
}

//WM_DL_PARAM消息响应函数的具体实现
LRESULT CfinalDlg::OnDlParam(WPARAM wParam,LPARAM lParam)
{
	recv_dnsdelay_data=GetDlgItemInt(IDC_DNS_DELAY);
	if(recv_dnsdelay_data)
	{
		SetDlgItemInt(IDC_DNS_DELAY,0);
	}
	delay_param* dp1=(delay_param*)lParam;//也不用加&
	dnsdelay1=dp1->dns_delay;
	recv_dnsdelay_data=dnsdelay1;
	tcpdelay1=dp1->tcp_delay;
	rtmpdelay1=dp1->rtmp_delay;
	SetDlgItemInt(IDC_DNS_DELAY,dnsdelay1);
	SetDlgItemInt(IDC_TCP_DELAY,tcpdelay1);
	SetDlgItemInt(IDC_RTMP_DELAY,rtmpdelay1);
	return 0;
}

//--------------------------------------------------------------------------------------------------------------
//对应于createthread的线程响应函数，监听下载速率
DWORD WINAPI CfinalDlg::qoe_dl_speed(LPVOID lpParameter)
{
		
	WaitForSingleObject(hNetMutex1,INFINITE);
	while(1)
	{	
		if(!recv_dnsdelay_data)
		{	
		//	ReleaseMutex(hNetMutex1);
			continue;
		}
		HWND hwnd=(HWND)lpParameter;
		__int64 speed=download_speed();
		dlspeed_checknum++;
		dlspeed_total+=speed;
		dlspeed_avg=dlspeed_total/dlspeed_checknum;
		::SendMessageA(hwnd,WM_DL_SPEED,0,(LPARAM)speed);//将消息传递给对话框,不用加&
		ReleaseMutex(hNetMutex1);
	}
	return 0;
}


//WM_DL_SPEED消息响应函数的具体实现
LRESULT CfinalDlg::OnDlSpeed(WPARAM wParam,LPARAM lParam)
{
	UpdateData();

	float qoe_score;
	float access_qoe_score;
	float complete_qoe_score;
	float complete_qoe_score_real;
	float stay_qoe_score;
	float stay_qoe_score_real;

	__int64 speed1=(_int64)lParam;	//也不用加&
	if(play_sec_dlg)
	{
		lineSeries1.AddXY((double)play_sec_dlg,(double)speed1,NULL,0);	
	}
	SetDlgItemInt(IDC_DOWNLOAD_SPEED,dlspeed_avg);
	page2.put_Current(page2.get_Count());
	
	access_qoe_score = 0.195 * score->m_access_score_value;
	complete_qoe_score = score->m_video_score_value;
	complete_qoe_score_real=0.226*(score->m_video_score_value) + 0.052*(score->m_audio_score_value);
	stay_qoe_score = score->m_buf_score_value;
	stay_qoe_score_real=0.272 * score->m_buf_score_value +0.255 * score->m_sync_score_value ;
	
	qoe_score = access_qoe_score + complete_qoe_score_real + stay_qoe_score_real;
		
	score->lineSeries2.AddXY((double)play_sec_dlg,100,NULL,0);
	score->lineSeries3.AddXY((double)play_sec_dlg,100,NULL,0);
	score->lineSeries4.AddXY((double)play_sec_dlg,(double)complete_qoe_score,NULL,0);
	score->lineSeries5.AddXY((double)play_sec_dlg,(double)stay_qoe_score,NULL,0);
	score->page3.put_Current(score->page3.get_Count());
	return 0;
}

void CfinalDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CloseHandle(hThread_dl_param);
	CloseHandle(hThread_dl_speed);
	CDialogEx::OnOK();
}

void CfinalDlg::OnBnClickedScore()
{
	// TODO: 在此添加控件通知处理程序代码
	//UpdateData();
	
	score->ShowWindow(SW_SHOW);

	/*CString weight;
	_CRT_FLOAT fltval;
	for(int p=0,q=1035;p<=12;p++,q++)
	{
		score->GetDlgItemText(q,weight);
		_atoflt(&fltval,weight.GetBuffer(weight.GetLength()));
		every_weight[p]=fltval.f;
	}*/
}

int CfinalDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	Welcome::ShowSplashScreen(this); //显示启动画面 
	/*adplist=new AdapterList;
	adplist->Create(IDD_DEVLIST_DLG,NULL);
	*/
	return 0;
}


void CfinalDlg::OnBnClickedChooseDev()
{
	// TODO: 在此添加控件通知处理程序代码
	get_devlist(adplist);
	adplist->ShowWindow(SW_SHOW);
}

void CfinalDlg::OnBnClickedWrite()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	fprintf(result,",");
	fwrite(m_resolution_value,sizeof(char),m_resolution_value.GetLength(),result);
	fprintf(result,",");
	fwrite(m_bpp_value,sizeof(char),m_bpp_value.GetLength(),result);
	fprintf(result,",");
	fwrite(m_bitrate_value,sizeof(char),m_bitrate_value.GetLength(),result);
	fprintf(result,",");
	fwrite(m_pause_freq_value,sizeof(char),m_pause_freq_value.GetLength(),result);
	fprintf(result,",");
	fwrite(m_paused_time_value,sizeof(char),m_paused_time_value.GetLength(),result);
	fprintf(result,",");
	fprintf(result,"%d",play_sec_dlg);
	fprintf(result,",");
	fprintf(result,"%5.2f",score->m_access_score_value);
	fprintf(result,",");
	fprintf(result,"%5.2f",score->m_video_score_value);
	fprintf(result,",");
	fprintf(result,"%5.2f",score->m_buf_score_value);
	fprintf(result,"\n");
	fflush( result );
}


void CfinalDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	ffplay_quit();
	ClearAllSeries0();
	ClearAllSeries1();
	ClearAllSeries2();
	score->ClearAllSeries3();
	SetDlgItemText(IDC_TCP_DELAY,"");
	SetDlgItemText(IDC_DNS_DELAY,"");
	SetDlgItemText(IDC_RTMP_DELAY,"");
	SetDlgItemText(IDC_BUFF_TIME,"");
	SetDlgItemText(IDC_PAUSE_FREQ,"");
	SetDlgItemText(IDC_PAUSE_TIME,"");
	SetDlgItemText(IDC_PAUSE_GAP,"");
	SetDlgItemText(IDC_DOWNLOAD_SPEED,"");
	SetDlgItemText(IDC_FRAMERATE,"");
	SetDlgItemText(IDC_BPP,"");
	SetDlgItemText(IDC_RESOLUTION,"");
	SetDlgItemText(IDC_PIXFMT,"");
	SetDlgItemText(IDC_FORMAT,"");
	SetDlgItemText(IDC_BITRATE,"");
	SetDlgItemText(IDC_INPUT_PROTOCOL,"");
	SetDlgItemText(IDC_VIDEOCODEC,"");
	SetDlgItemText(IDC_AUDIOCHANNEL,"");
	SetDlgItemText(IDC_AUDIOSAMPLERATE,"");
	SetDlgItemText(IDC_AUDIO_BITRATE,"");
	SetDlgItemText(IDC_AUDIOCODEC,"");
	SetDlgItemText(IDC_METADATA,"");
	score->m_score_access.SetWindowTextA("100");
	score->m_score_video.SetWindowTextA("100");
	score->m_score_audio.SetWindowTextA("100");
	score->m_score_buf.SetWindowTextA("100");

	dlspeed_total=0;
	dlspeed_avg=0;
	dlspeed_checknum=0;
	recv_dnsdelay_data=0;
	play_sec_dlg=0;
	memset(charData_pause_time,0,sizeof(charData_pause_time));

	ever_stopped=1;
	::SuspendThread(hThread_dl_speed);
	::ResumeThread(hThread_dl_param);
}
