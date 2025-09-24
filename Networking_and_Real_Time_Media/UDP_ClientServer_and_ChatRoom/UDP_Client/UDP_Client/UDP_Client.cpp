// UDP_Client.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
#include <iostream>

// 1. 設定相關參數與檔案
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "wsock32.lib") // 連結需用到之Lib檔案
#include <winsock2.h>               // 引入 定義檔

// 1.5 UDP Client相關變數
SOCKET      CSock; // 代表己端通訊埠Port與IP
sockaddr_in CAddr; // 代表遠端通訊埠Port與IP

int main()
{
	WSADATA Wsa;
	
	// 2. 開啟 UDP Client Socket
	WSAStartup(0x202, &Wsa); // Windows特有
	CSock = socket(AF_INET, SOCK_DGRAM, 0); // 開啟UDP通道
	
	// 2.4 Client 不需設定自己端IP與Port
	// 2.5 設定遠端IP與Port
	CAddr.sin_family = AF_INET;
	CAddr.sin_port = htons(6000);
	//CAddr.sin_addr.s_addr = inet_addr("127.0.0.1");   // Loopback(自己IP, for自己玩)
	CAddr.sin_addr.s_addr = inet_addr("192.168.56.1"); // 實體單一IP(由ipconfig取得)
	//CAddr.sin_addr.s_addr = inet_addr("192.168.13.255"); // 廣播IP

	// 3. 讀取鍵盤, 送出封包
	char S1[2000];
	int Len = sizeof(sockaddr);
	while (1)
	{
		printf("Input = "); scanf_s("%s", S1, sizeof(S1)); // 讀鍵盤
		sendto(CSock, S1, strlen(S1), 0, (sockaddr *)&CAddr, Len); // 送出封包
	}
}

