#include"ffplay.h"
#include"stdafx.h"
#include"pause_detect.h"

int qoe_ptr_location_pre=0;
char qoe_ptr_location_pre_val=NULL;
int is_rebuff=0;
//以下变量只记录出现缓冲的情况
//缓冲次数
static int qoe_rebuff_num=0;
//记录每次缓冲的时间
static int qoe_rebuff_time[MAX_IOCHECK_NUM]={0};
//以下变量记录所有时间段的情况
//检测次数
static int qoe_checknum=0;

typedef struct PacketQueue{
  AVPacketList *first_pkt, *last_pkt;
  int nb_packets;
  int size;
  SDL_mutex *mutex;
  SDL_cond *cond;
};

typedef struct VideoPicture 
{
  SDL_Overlay *bmp;
  int width, height; /* source height & width */
  int allocated;
  double pts;
};

typedef struct VideoState
{
	AVFormatContext *pFormatCtx;
	int videostream,audiostream;
	AVDictionary *avdic;

	AVStream *audio_st;
	PacketQueue audioq;
	uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE *3)/2];
	unsigned int audio_buf_size;
	unsigned int audio_buf_index;
	AVPacket audiopkt;
	uint8_t *audio_pkt_data;
	int		audio_pkt_size;
	int     audio_hw_buf_size; 
	double	audio_clock;
	double  audio_diff_cum; /* used for AV difference average computation */
	double  audio_diff_avg_coef;
	double  audio_diff_threshold;
	int     audio_diff_avg_count;
	int av_sync_type;

	double frame_last_pts,frame_last_delay,frame_timer;
	
	AVStream *video_st;
	PacketQueue videoq;
	double video_clock;
	double video_current_pts;
	int64_t video_current_pts_time;

	VideoPicture pictq[VIDEO_PICTURE_QUEUE_SIZE];
	int pictq_size,pictq_rindex,pictq_windex;
	SDL_mutex *pictq_mutex;
	SDL_cond *pictq_cond;
	
	SDL_Thread *parse_tid;
	SDL_Thread *video_tid;
	
	char filename[1024];
	int quit;
	
	double  external_clock; /* external clock base */
	int64_t external_clock_time;
};

void pause_detect(VideoState* is,LPVOID wnd)
{
	HWND WND=(HWND)wnd;
	AVFormatContext *pFormatCtx = is->pFormatCtx;
	CString rebuff_num;
	int buffer_ptr_location=0;
	//while(1)
	//{
		//到达上限后自动回0-------------------------
		if(qoe_checknum>=MAX_IOCHECK_NUM){
			qoe_checknum=0;
			qoe_rebuff_num=0;
			//continue;
		}
		//表明当前状态是正常（0）还是暂停（1）
		if(pFormatCtx->pb!=NULL)
		{
			//获取buffer中指针的位置
			buffer_ptr_location=pFormatCtx->pb->buf_ptr-pFormatCtx->pb->buffer;
			//如果和前一次检查的位置相等,而且所指向的值也相等，则说明指针没有移动，视频出现暂停
			if((qoe_ptr_location_pre==buffer_ptr_location)&&(qoe_ptr_location_pre_val==*(pFormatCtx->pb->buf_ptr))){
				if(is_rebuff==0){
					//首次出现暂停,则增加一次暂停次数
					qoe_rebuff_num++;
					//同时改编为暂停状态
					is_rebuff=1;
				}else{
					//不是首次暂停，记录暂停的时间（增加一个单位时间）
					qoe_rebuff_time[qoe_rebuff_num]++;
				}
			}else{
				//视频不再暂停
				is_rebuff=0;
			}
		
			//缓冲次数-------------------------------
			rebuff_num.Format("%d",qoe_rebuff_num);
			SetDlgItemText(WND,IDC_PAUSE_FREQ,rebuff_num);
			//自增----------
			qoe_checknum++;
			//存储本次，以用于下一次比较
			qoe_ptr_location_pre=buffer_ptr_location;
			qoe_ptr_location_pre_val=*(pFormatCtx->pb->buf_ptr);
			//continue;
		}
		//else
			//continue;
	//}
}