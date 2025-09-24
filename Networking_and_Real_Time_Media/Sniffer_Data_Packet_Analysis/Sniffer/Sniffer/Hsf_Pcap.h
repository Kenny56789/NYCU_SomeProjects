//#pragma once
// For WinPcap
#define HAVE_REMOTE
#pragma comment(lib,"WinPcap/lib/wpcap.lib")
#pragma comment(lib,"WinPcap/lib/packet.lib")

#include "WinPcap/include/pcap.h"

// For Pcap/TCP File
#define B_ENDIAN_4(A) (((unsigned int)A[0]<<24)+((unsigned int)A[1]<<16)+((unsigned int)A[2]<<8)+((unsigned int)A[3]<<0))
#define L_ENDIAN_4(A) (((unsigned int)A[3]<<24)+((unsigned int)A[2]<<16)+((unsigned int)A[1]<<8)+((unsigned int)A[0]<<0))



// @@@@@@@@@@@@@@@@@@@@@@@@@ TCPIP @@@@@@@@@@@@@@@@@@@@@@@@@@@@@
struct MAC // MAC[14]
{
	unsigned char  DMAC[6];
	unsigned char  SMAC[6];
	unsigned char  Type[2];
};
struct IP // IP(20)
{
	unsigned char  Ver[2], Len[2];
	unsigned char  ID[2], Frag[2];
	unsigned char  Time, Protocol, Check[2];
	unsigned char  SIP[4], DIP[4];
};
struct TCP // TCP(20)
{
	unsigned char  SPort[2], DPort[2];
	unsigned char  Seq[4], Ack[4];
	unsigned char  Offset, Flag11, Window[2];
	unsigned char  Check[2], Urgent[2];
};
struct UDP // UDP(8)
{
	unsigned char  SPort[2], DPort[2];
	unsigned char  Len[2], Check[2];
};

void Start_PCAP(int No, pcap_t **fp)
{
	pcap_if_t *alldevs, *d;
	//pcap_t *fp;
	u_int i, inum = No;
	char errbuf[PCAP_ERRBUF_SIZE];
	int res, Len;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	char  S1[2000];
	/* The user didn't provide a packet source: Retrieve the local device list */
	pcap_findalldevs_ex((char *)PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf);
	
	d = alldevs;
	while (d != 0)
	{	sprintf_s(S1,2000, "Name=%s  Description=%s Next=%lx\r\n", d->name, d->description, d->next);
		printf("%s\n", S1);
		d = d->next;
	}
	
	//inum = 2;
	// Jump to the selected adapter 
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	// Open the device 
	*fp = pcap_open(d->name, 100, PCAP_OPENFLAG_PROMISCUOUS, 20, NULL, errbuf);
	//pcap_freealldevs(alldevs);
}


// ############################################################

#define   MAX  10000
struct PCAP_DATA
{
	int            Flag;
	unsigned char  SIP[4], DIP[4]; // 雙方IP
	unsigned short SPort, DPort;   // 雙方Port
	int            Protocol;       // TCP(0x06),UDP(0x11)
	int            Tx_No, Rx_No;   // Packets:數量
	int            Tx_Len, Rx_Len; // Bytes
	char           SAddr[100], DAddr[100]; // IP對應之國家
} Pcap_Data[MAX];

int Init_Flag = 0;
void Init_Pcap()
{
	memset(Pcap_Data, 0, MAX * sizeof(PCAP_DATA));
}

int Search_Pcap(unsigned char *SIP, unsigned char *DIP, unsigned short SPort, unsigned short DPort, int Protocol)
{
	int i;
	PCAP_DATA *p;
	for (i = 0; i < MAX; i++)
	{
		p = &Pcap_Data[i];
		if (p->Flag)
		{
			if      ((!memcmp(p->SIP, SIP, 4)) && (!memcmp(p->DIP, DIP, 4)) && (p->SPort == SPort) && (p->DPort == DPort) && (p->Protocol == Protocol)) return i;
			else if ((!memcmp(p->SIP, DIP, 4)) && (!memcmp(p->DIP, SIP, 4)) && (p->SPort == DPort) && (p->DPort == SPort) && (p->Protocol == Protocol)) return i + MAX;
		}
	}
	return -1; // not found
}

void Add_Pcap(char *lp, int Len) // 新增連線封包
{
	int   i, Data_Len;
	unsigned short SPort, DPort;
	MAC   *lpMac;
	IP    *lpIp;
	TCP   *lpTcp;
	UDP   *lpUdp;
	char  *lpData; // for FTP

	// 1.初始化
	if (Init_Flag == 0) { Init_Flag = 1; Init_Pcap(); }

	// 2.解析封包MAC,IP,TCP(54)/UDP(42)
	if (Len > 42)
	{
		lpMac = (MAC *)&lp[0];  // 14
		lpIp = (IP  *)&lp[14];  // 20
		if (lpIp->Protocol == 0x06) // TCP
		{
			lpTcp = (TCP *)&lp[34]; // 20
			  lpData = (char *)&lp[54]; // for FTP
			Data_Len = Len - 54;
			  lpData[Data_Len] = 0;  // for FTP
			SPort = lpTcp->SPort[1] + lpTcp->SPort[0] * 256;
			DPort = lpTcp->DPort[1] + lpTcp->DPort[0] * 256;
			//if ((SPort == 21) || (DPort == 21)) printf("%s", lpData); // for FTP

		}
		else if (lpIp->Protocol == 0x11) // UDP
		{
			lpUdp = (UDP *)&lp[34]; // 20
			Data_Len = Len - 42;
			SPort = lpUdp->SPort[1] + lpUdp->SPort[0] * 256;
			DPort = lpUdp->DPort[1] + lpUdp->DPort[0] * 256;
		}
		else // 其他非TCP,UDP
		{
			Data_Len = 0;
		}

		if (Data_Len > 0)
		{
			// 2.1.檢查是否已經存在
			i = Search_Pcap(lpIp->SIP, lpIp->DIP, SPort, DPort, (int)lpIp->Protocol);
			if (i >= 0) // 已存在
			{
				if (i < MAX)
				{
					Pcap_Data[i].Tx_No++; Pcap_Data[i].Tx_Len += Data_Len;
				}
				else
				{
					i -= MAX;
					Pcap_Data[i].Rx_No++; Pcap_Data[i].Rx_Len += Data_Len;
				}
			}
			else // 不存在，就要新增
			{
				for (i = 0; i < MAX; i++) if (!Pcap_Data[i].Flag) break; // 找空格
				if (i < MAX)
				{
					Pcap_Data[i].Flag = 1;
					memcpy(Pcap_Data[i].SIP, lpIp->SIP, 4);
					memcpy(Pcap_Data[i].DIP, lpIp->DIP, 4);
					Pcap_Data[i].SPort = SPort;
					Pcap_Data[i].DPort = DPort;
					Pcap_Data[i].Protocol = (int)lpIp->Protocol;
					Pcap_Data[i].Tx_No = 1; Pcap_Data[i].Tx_Len = Data_Len;
					Pcap_Data[i].Rx_No = 0; Pcap_Data[i].Rx_Len = 0;
					// 新增國家(IP->國家)
					char S1[100],Addr[100];
                    // 找來源端國家
					sprintf_s(S1, 99, "%d.%d.%d.%d", Pcap_Data[i].SIP[0], Pcap_Data[i].SIP[1], Pcap_Data[i].SIP[2], Pcap_Data[i].SIP[3]);
					IP2Location(S1, Addr);
					strcpy_s(Pcap_Data[i].SAddr, Addr);
					// 找目的端國家
					sprintf_s(S1, 99, "%d.%d.%d.%d", Pcap_Data[i].DIP[0], Pcap_Data[i].DIP[1], Pcap_Data[i].DIP[2], Pcap_Data[i].DIP[3]);
					IP2Location(S1, Addr);
					strcpy_s(Pcap_Data[i].DAddr, Addr);
				}
			}
		}

	}
}
