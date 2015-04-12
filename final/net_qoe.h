#define WPCAP
#define HAVE_REMOTE
#include<pcap.h>
#include<stdlib.h>


#include"stdafx.h"
#include"final.h"
#include"finalDlg.h"
#include"afxdialogex.h"
#include"resource.h"//直接关系到能否调用对话框中的各种资源
#include"AdapterList.h"

typedef struct delay_param;
delay_param* delay_qoe();
__int64 download_speed();
void get_devlist(LPVOID lpParam);