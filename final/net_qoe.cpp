#include"stdafx.h"
#include"net_qoe.h"

typedef struct delay_param
{
	u_int dns_delay;
	u_int tcp_delay;
	u_int rtmp_delay;
}delay_param;

//四字节ip地址
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ipaddress;

//ipv4首部
typedef struct ip_header
{
	u_char ver_ihl;	//版本+首部长度共8bit
	u_char tos;		//服务类型
	u_short tlen;	//总长
	u_short identification;	//标识
	u_short flags_fo;	//标志位3bit，段偏移量13bit
	u_char ttl;		//存活时间
	u_char proto;	//协议
	u_short crc;	//首部校验和
	ip_address saddr;	//源地址
	ip_address daddr;	//目标地址
	u_int op_pad;	//选项与填充
}ip_header;

//udp首部
typedef struct udp_header
{
	u_short sport;	//源端口
	u_short dport;	//目标端口
	u_short len;	//udp数据包长度
	u_short crc;	//校验和
}udp_header;

//dns首部
typedef struct dns_header
{
	u_short id;//标识
	u_short flag;//标志
}dns_header;

 //定义TCP首部
typedef struct tcp_header            
{ 
    u_short sport;               //16位源端口 
    u_short dport;               //16位目的端口 
    unsigned int seq;         //32位序列号 
    unsigned int ack;         //32位确认号
    unsigned char lenres;        //4位首部长度/6位保留字
    unsigned char flag;            //8位标志位 
    u_short win;                 //16位窗口大小 
    u_short sum;                 //16位校验和 
    u_short urp;                 //16位紧急数据偏移量 
}tcp_header;

/*rtmp首部（类型0） 结构体长度的计算很变态哒！
typedef struct rtmp_header
{
	u_char fmt_csid;	//块类型及块流id共8bit
	u_short ts_bs1;		//时间戳及bodysize共48位
	u_int ts_bs2;
	u_char msg_tid;		//8位type id
	u_int sid;			//stream id共32位
}rtmp_header;*/

//播放命令中的rtmp body的string部分
typedef struct rtmp_body_string
{
	//u_char amf_type;	//8bit amf0 类型
	//u_short str_len;	//16bit string长度
	u_int str;	//都是被结构体长度的变态内容搞成这样的！
}rtmp_body_string;

CfinalDlg* final_dlg;
AdapterList* adplist_dlg;
void get_devlist(LPVOID lpParam)
{
	adplist_dlg=(AdapterList*)lpParam;
    pcap_if_t *alldevs;
    pcap_if_t *d;
    int i=0;
    char errbuf[PCAP_ERRBUF_SIZE];
    CString index,dev_name,dev_description;
	

    /*获取本地机器设备列表 */
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL , &alldevs, errbuf) == -1)
    {
        fprintf(stderr,"Error in pcap_findalldevs_ex: %s\n", errbuf);
        exit(1);
    }
    
    /* 打印列表 */
    for(d= alldevs; d != NULL; d= d->next)
    {
		//------------------------------
		index.Format("%d",++i);
		//获取当前记录条数
		int nIndex=adplist_dlg->m_devlist.GetItemCount();
		//“行”数据结构
		LV_ITEM lvitem;
		lvitem.mask=LVIF_TEXT;
		lvitem.iItem=nIndex;
		lvitem.iSubItem=0;
		//注：vframe_index不可以直接赋值！
		//务必使用f_index执行Format!再赋值！
		lvitem.pszText=(char *)(LPCTSTR)index;
		//------------------------

		adplist_dlg->m_devlist.InsertItem(&lvitem);
		adplist_dlg->m_devlist.SetItemText(nIndex,1,d->name);
		if (d->description)
			adplist_dlg->m_devlist.SetItemText(nIndex,2, d->description);
		else
			adplist_dlg->m_devlist.SetItemText(nIndex,2, "No description available");
    }
    
    if (i == 0)
    {
        printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
        return;
    }

    /* 不再需要设备列表了，释放它 */
    pcap_freealldevs(alldevs);
}

delay_param* delay_qoe()
{
	//memset(&dp,0,sizeof(delay_param));
	//dp->dns_delay=0;
	//dp->rtmp_delay=0;
	//dp->tcp_delay=0;
	pcap_if *alldevs;
	pcap_if *d;
	int inum;
	static u_int i=0;
	static u_int j=0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];

	u_int netmask;
	char packet_filter[]="port 53 or tcp";
	struct bpf_program fcode;

	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	int res;

	struct tm *ltime;
	char timestr[16];
	time_t local_tv_sec;
	time_t local_tv_usec;
	int result=0;

	ip_header *ih;
	udp_header *uh;
	dns_header *dnsh;
	tcp_header *th;
	delay_param *dp=(delay_param*)malloc(sizeof(delay_param));
	u_char *rh;
	rtmp_body_string* rtmp_body_str;

	u_int ip_len;
	u_int tcp_header_len;
	u_int tcp_body_len;

	u_short sport;
	u_short dport;

	time_t time_dnsr[10];		//dns请求
	time_t time_dnsq[10];		//dns回应
	memset(time_dnsr,0,sizeof(time_dnsr));
	memset(time_dnsq,0,sizeof(time_dnsq));
	time_t time_rtmp_play=0;
	time_t time_tcp_1=0;
	time_t time_tcp_2=0;
	time_t time_tcp_3=0;

	//获得设备列表
	if(pcap_findalldevs_ex(PCAP_SRC_IF_STRING,NULL,&alldevs,errbuf)== -1)
	{
		fprintf(stderr,"wocao,err",errbuf);
		system("pause");
	}

	//跳转到已选设备
	//for(d=alldevs,i=0;i<list_selected_dlg;d=d->next,i++);
	d=alldevs->next;

	//打开适配器
	if((adhandle=pcap_open(d->name,65536,PCAP_OPENFLAG_PROMISCUOUS,1000,NULL,errbuf))==NULL)
	{
		fprintf(stderr,"\nwocao, da bu kai adapter!\n");
		pcap_freealldevs(alldevs);system("pause");
			return NULL;
	}

	//检查数据链路层
	if(pcap_datalink(adhandle)!=DLT_EN10MB)
	{
		fprintf(stderr,"\nwocao, i need yi tai wang!\n");
		pcap_freealldevs(alldevs);system("pause");
			return NULL;
	}

	if(d->addresses!=NULL)
		//获取接口第一个地址的掩码
		netmask=((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.s_addr;
	else
		//如果接口没有地址就假设一个c类的掩码
		netmask= 0xffffff;

	//编译过滤器
	if(pcap_compile(adhandle,&fcode,packet_filter,1,netmask)<0)
	{
		fprintf(stderr,"\nwocao,cant compile the filter1 !\n");
		pcap_freealldevs(alldevs);system("pause");
			return NULL;
	}

	//设置过滤器
	if(pcap_setfilter(adhandle,&fcode)<0)
	{
		fprintf(stderr,"\nwocao,cant set the filter!\n");
		pcap_freealldevs(alldevs);system("pause");
			return NULL;
	}

	printf("\nlistening on %s....\n",d->description);

	while((res = pcap_next_ex( adhandle, &header, &pkt_data)) >= 0)
	{
		if(res == 0)
			/* Timeout elapsed */
			continue;
		
		//将时间戳转换为可识别的格式
		local_tv_sec=header->ts.tv_sec;
		local_tv_usec=header->ts.tv_usec;

		//获取ip数据包头部的位置
		ih=(ip_header*)(pkt_data+14);//都是按字节算的
		//获取udp首部的位置
		ip_len=(ih->ver_ihl & 0xf)*4;

		if(ih->proto==0x11)
		{
			uh=(udp_header*)((u_char*)ih+ip_len);
			//if(uh->dport==0x3500 || uh->sport==0x3500)
			//{
				//获取dns首部的位置
				dnsh=(dns_header*)((u_char*)uh+8);

				if(dnsh->flag==0x8081)
					{
						time_dnsr[i]=local_tv_sec*1000000+local_tv_usec;	//单位是微妙
						if(time_dnsq[i]!=0 && time_dnsr[i]!=0)
						{	
							dp->dns_delay=time_dnsr[i]-time_dnsq[i];
							result=dp->dns_delay;
							memset(time_dnsr,0,sizeof(time_dnsr));
							memset(time_dnsq,0,sizeof(time_dnsq));
						}
						i++;
					}
				else
					{
						time_dnsq[j]=local_tv_sec*1000000+local_tv_usec;
						j++;
					}
			//}
		}
		else if(result)
		{
			th=(tcp_header*)((u_char*)ih+ip_len);//unsigned char占一个字节
			//获取rtmp内容开始的位置
			//tcp数据包头部长度
			tcp_header_len=(((th->lenres<<4)|(th->lenres>>4)) & 0xf)*4;
			//tcp_body_len=ih->tlen-ip_len-tcp_header_len;
			//rh=(u_char*)((u_char*)th+tcp_header_len);//tcp除了包头还有“身体”的长度,只要不是在奇葩的握手阶段，身体部分就是后续协议的body
			rtmp_body_str=(rtmp_body_string*)((u_char*)th+12+tcp_header_len);

			local_tv_sec = header->ts.tv_sec;
			local_tv_usec= header->ts.tv_usec;
		
			if(th->flag==0x02)
			{
				time_tcp_1=(local_tv_sec*1000000+local_tv_usec);
			}
			if(th->flag==0x12 && time_tcp_1!=0)
			{
				time_tcp_2=(local_tv_sec*1000000+local_tv_usec);
			}
			if(th->flag==0x10 && time_tcp_1!=0 && time_tcp_2!=0 && time_tcp_3==0)
			{
				time_tcp_3=(local_tv_sec*1000000+local_tv_usec);
				dp->tcp_delay=time_tcp_3-time_tcp_1;
			}
			if(time_tcp_3!=0)
			{
				if(rtmp_body_str->str==0x70040002)	//显示play的时候的值，其实这种方法不甚严谨
				{
					time_rtmp_play=local_tv_sec*1000000+local_tv_usec;
					dp->rtmp_delay=time_rtmp_play-time_tcp_3;
					time_rtmp_play=0;
					time_tcp_1=0;
					time_tcp_2=0;
					time_tcp_3=0;
					return dp;
				}
				//printf("%.2x",rtmp_body_str->str_len);
			}
		}
	}
	if(res == -1)
	{
		//printf("Error reading the packets: %s\n", pcap_geterr(adhandle));
		system("pause");
		return NULL;
	}

	pcap_close(adhandle);  	
	free(dp);
}
//--------------------------------------------------------------------------------------------
__int64 download_speed()
{
	pcap_if *alldevs;
	pcap_if *d;
	int inum=1;
	int i=0;

	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct timeval st_ts;
	u_int netmask;
	struct bpf_program fcode;

	struct timeval *old_ts=&st_ts;
	u_int delay;
	_int64 KBps,KPps;
	struct tm* ltime;
	//char timestr[16];
	time_t local_tv_sec;

	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	
	//获得设备列表
	if(pcap_findalldevs_ex(PCAP_SRC_IF_STRING,NULL,&alldevs,errbuf)== -1)
	{
		fprintf(stderr,"wocao,err",errbuf);
		printf("wocao,err");
		system("pause");
	}

	//跳转到已选设备
	//for(d=alldevs,i=0;i<list_selected_dlg;d=d->next,i++);
	d=alldevs->next;

	//打开适配器
	if((fp=pcap_open(d->name,100,PCAP_OPENFLAG_PROMISCUOUS,1000,NULL,errbuf))==NULL)
	{
		fprintf(stderr,"wocao cant open adapter\n",errbuf);
		printf("wocao cant open adapter\n");
		system("pause");
		return -1;
	}

	if(d->addresses!=NULL)
		//获取接口第一个地址的掩码
		netmask=((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.s_addr;
	else
		//如果接口没有地址就假设一个c类的掩码
		netmask= 0xffffff;

	//编译过滤器
	if(pcap_compile(fp,&fcode,"tcp",1,netmask)<0)
	{
		fprintf(stderr,"wocao cant compile the filter\n",errbuf);
		printf("wocao cant compile the filter\n");
		return -1;
	}

	//设置过滤器
	if(pcap_setfilter(fp,&fcode)<0)
	{
		fprintf(stderr,"wocao cant set the filter\n");
		pcap_close(fp);
		return -1;
	}

	//将接口设置为统计模式
	if(pcap_setmode(fp,MODE_STAT)<0)
	{
		fprintf(stderr,"wocao cant set the mode\n");
		printf("wocao cant set the mode\n");
		pcap_close(fp);
		system("pause");
		return -1;
	}

	//printf("tcp traffic summary:\n");

	while((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
	{
		if(res == 0)
			/* Timeout elapsed */
			continue;

		//以毫秒计算上一次采样的延迟时间，这个值通过采样到的时间戳获得
		delay=(header->ts.tv_sec-old_ts->tv_sec)*1000000-old_ts->tv_usec+header->ts.tv_usec;
		//获取每秒的比特数
		KBps=((*(LONGLONG*)(pkt_data+8))*8*1000000)/(delay);
		//得到每秒的数据包数量
		//KPps.QuadPart=((*(LONGLONG*)(pkt_data))*8*1000000)/(delay);

		
		return KBps;

		//将时间戳转换为可识别的格式
		local_tv_sec=header->ts.tv_sec;
		ltime=localtime(&local_tv_sec);
		//strftime(timestr,sizeof(timestr),"%H:%M:%S",ltime);

		//打印时间戳
		//printf("%s",timestr);

		//打印采样结果
		//printf("\tBPS=%I64u",KBps.QuadPart);
		//printf("\tPPS=%I64u\n",KPps.QuadPart);

		//存储当前的时间戳
		old_ts->tv_sec=header->ts.tv_sec;
		old_ts->tv_usec=header->ts.tv_usec;
	}


	if(res == -1)
	{
		printf("Error reading the packets: %s\n", pcap_geterr(fp));
		system("pause");
		return -1;
	}
	pcap_close(fp);
}

