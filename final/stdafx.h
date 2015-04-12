
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持


#include <afxsock.h>            // MFC 套接字扩展

#include<stdio.h>
#include<Windows.h>
#include<MMSystem.h>
#include<cmath>
#include<inttypes.h>
#include<math.h>
#include<limits.h>
#include<signal.h>
#include<assert.h>
#include<stdint.h>
#include"config.h"
extern "C"
{
#include"libavcodec/avcodec.h"
#include "libavutil/avstring.h"
#include "libavutil/colorspace.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"
#include"faad_realtime/audio.h"
#include "SDL.h"
#include "SDL_thread.h"
};
#include "CTChart.h"
#include "CSeries.h" 
#include "CAxis.h"
#include "CAxes.h"
#include "CLegend.h"
#include "CPage.h"
#include"CMarks.h"
//最多存储的帧信息
#define MAX_FRAME_NUM 10000
//最多存储的Packet信息（一般以此为准，关键）
#define MAX_PACKET_NUM 10000
//最多存储的监测输入输出状态的信息
#define MAX_IOCHECK_NUM 10000

#define INT64_MIN  (-9223372036854775807LL - 1)
#define INT64_MAX 9223372036854775807LL

static DWORD time_pause_start=0;//初次暂停发生的时间
static DWORD time_pause_last=0;//暂停持续时间
static int list_selected_dlg=-1;

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif



