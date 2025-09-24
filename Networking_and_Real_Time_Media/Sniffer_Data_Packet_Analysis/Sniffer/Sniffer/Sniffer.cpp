// Sniffer.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
#include <iostream>

// ===== 1. 引入檔設定 =====
#include "process.h"
#include "ip2location.h" // IP轉國家
#include "Hsf_Pcap.h"    // NPF Function

// 攔截封包
void Fun(PVOID p)
{
	// A.NPF變數宣告
	int  i, res, Data_Len;
	struct pcap_pkthdr* header;
	const u_char* pkt_data;

	// B.分析TCPIP封包變數指標宣告
	char* lpApp; // 4. 應用層(??B)
	TCP* lpTcp;  // 3-1. TCP層(20B)
	UDP* lpUdp;  // 3-2. UDP層(8B)
	IP* lpIp;    // 2. IP層(20B)
	MAC* lpMac;  // 1. MAC層(14B)

	while ((res = pcap_next_ex((pcap_t*)p, &header, &pkt_data)) >= 0) // 抓封包
	{
		Add_Pcap((char*)pkt_data, header->caplen); // 分析&塞進Queue Buffer
		
		//// C.過濾FTP(TCP:21)封包
		//if (header->caplen > 54) // MAC(14)+IP(20)+TCP(20)=54(TCP), 14+20+8=42(UDP)
		//{
		//	lpMac = (MAC  *)&pkt_data[ 0];
		//	lpIp  = (IP   *)&pkt_data[14]; // Protocol=TCP
		//	lpTcp = (TCP  *)&pkt_data[34];
		//	lpApp = (char *)&pkt_data[54];
		//	if (lpIp->Protocol == 0x06) // TCP:0x06, UDP=0x11;
		//	{
		//		if ((lpTcp->DPort[1] == 21) || (lpTcp->SPort[1] == 21))
		//		{
		//			lpApp[header->caplen - 54] = 0; // 字串結束
		//			printf("FTP> %s\n", lpApp);
		//		}
		//	}
		//}
	}
}

void PrintOut()
{
	// 定時列印封包
	// 1. 清除螢幕
	system("cls"); 

	// 2. Bubble Sorting
	PCAP_DATA* lpi, * lpj, A;
	int i, j, k;
	for (i = 0; i < MAX - 1; i++)
	{
		lpi = &Pcap_Data[i];
		if (lpi->Flag) // if (lpi->Flag == 1)
		{
			for (j = i + 1; j < MAX; j++)
			{
				lpj = &Pcap_Data[j];
				if (lpj->Flag)
				{
					if ((lpi->Rx_Len + lpi->Tx_Len) < (lpj->Rx_Len + lpj->Tx_Len))
					{
						A = *lpi; *lpi = *lpj; *lpj = A; // lpi, lpj Exchange
					}
				}
			}
		}
	}

	// 3. Print Out 35 Items
	PCAP_DATA* lp;
	for (int i = 0; i < 35; i++)
	{
		lp = &Pcap_Data[i];
		if (lp->Flag)
		{
			printf("%2d ", i);
			if (lp->Protocol == 0x06) printf("TCP "); else printf("UDP ");  // 協定
			printf("[%3d.%3d.%3d.%3d]:%5d ", lp->SIP[0], lp->SIP[0], lp->SIP[0], lp->SIP[0], lp->SPort); // 來源端IP/Port
			printf("[%3d.%3d.%3d.%3d]:%5d ", lp->DIP[0], lp->DIP[0], lp->DIP[0], lp->DIP[0], lp->DPort); // 目的端IP/Port
			printf("[%6d/%6d] ", lp->Tx_No, lp->Rx_No);    // 封包數量)
			printf("[%10d/%10d] ", lp->Tx_Len, lp->Rx_Len);// 封包長度(BYTE)
			printf("[%s -> %s]\n", lp->SAddr, lp->DAddr);  // 國家
		}
	}
}

int main()
{
    // 2. 啟動NPF攔截封包
    pcap_t* p;
    Start_PCAP(2, &p); // 第一個參數為網路卡的編號, 從0開始往上改
    _beginthread(Fun, 0, (void*)p);

    // 3. 定時列印連結封包統計資料
    while (1)
    {
        Sleep(5000); // 休息5秒
        PrintOut();
    }
}

