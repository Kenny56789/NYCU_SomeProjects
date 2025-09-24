// UDP_Server.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
#include <iostream>

// 1. 設定相關參數與檔案
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "wsock32.lib") // 連結需用到之Lib檔案
#include <winsock2.h>               // 引入 定義檔

// 1.5 UDP Server相關變數
#define     MAX 100
SOCKET      CSock;      // 代表己端通訊埠Port與IP
sockaddr_in CAddr[MAX]; // 代表遠端通訊埠Port與IP
int         C_No = 0;   // 紀錄遠端連線數

int main()
{
	WSADATA Wsa;
	int     i, Len = sizeof(sockaddr);
	
	// 2. 開啟 UDP Server Socket
	WSAStartup(0x202, &Wsa); // Windows特有
	CSock = socket(AF_INET, SOCK_DGRAM, 0); // 開啟UDP通道

	// 2.5 Server端不需設定遠端IP與Port
	// 3. 設定Local(Server)端Port服務
	sockaddr_in Addr;
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(6000);
	Addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// 綁定Local 6000 Port成為伺服器
	bind(CSock, (sockaddr *)&Addr, Len); 

	// 4. 讀取網路封包, 列印之
	char S1[2000], IP[100];
	int Port;
	while (1)
	{
		i = recvfrom(CSock, S1, sizeof(S1), 0, (sockaddr *)&Addr, &Len); // 收資料, 對應Client端的sendto
		strcpy_s(IP, sizeof(IP), inet_ntoa(Addr.sin_addr));
		Port = Addr.sin_port; Port = ((Port >> 8) & 0xFF) + ((Port & 0xFF) << 8);
		S1[i] = 0; // 字串結尾, E.g."Hi": S1[0] = 'H', S1[1] = 'i', S1[2] = 0x00(字串結束)
		printf("recv(%s:%d) = %s\n", IP, Port, S1);
	}
}

