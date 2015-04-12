#include"ffplay.h"
#include"stdafx.h"
#include"final.h"
#include"finalDlg.h"
#include"afxdialogex.h"
#include"resource.h"//直接关系到能否调用对话框中的各种资源

typedef struct PacketQueue;
typedef struct VideoPicture;
typedef struct VideoState;
void pause_detect(VideoState* is,LPVOID wnd);