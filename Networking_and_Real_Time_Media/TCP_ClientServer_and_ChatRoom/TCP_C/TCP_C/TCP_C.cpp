// TCP_C.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
#include <iostream>
#include <process.h> // for _beginthread

// 1. 引入檔+基本設定
#include "TCPIP.h"
//#define  SERVER_IP   (char *)"142.250.204.36" //Google(80, 443)
//#define  SERVER_IP   (char *)"134.208.10.240" // 東華BBS
//#define  SERVER_IP   (char *)"210.61.132.2"   // 中華電信FTP
#define  SERVER_IP   (char *)"127.0.0.1"      // Localhost(loopback)
#define  SERVER_PORT       6000//, 21:FTP, 23:TELNET(BBS, PPT), 80:http

// 2. Global Variable
SOCKET CSock; // 1個socket就是1個連線
TCPIP  A;

// 執行續
void Fun(PVOID p)
{
	char S1[2000];
	int  i;
	SOCKET Sock = (SOCKET)p;
	// 4. 接收資料
	while (1)
	{
		i = recv(Sock, S1, sizeof(S1) - 1, 0); // 卡住(recv)
		if (i > 0) // 收到資料
		{ 
			S1[i] = 0; 
			printf("recv(%d) = %s\n", i, S1); 
		}
		else // 斷線
		{
			printf("Server Disconnect\n");
			break;
		}
	}
}

int main()
{
	int i, Len = sizeof(sockaddr);
	char S1[2000];

	// 3. 連線TCP伺服器
	A.Start_TCP_Client(&CSock, SERVER_IP, 0, SERVER_PORT); // 卡住(connect)
	_beginthread(Fun, 0, (void *)CSock);

	while (1)
	{
		// 5. 送出資料
		printf("Send>"); // 提示
		scanf_s("%s", S1, sizeof(S1));  // 卡住(讀鍵盤)
		send(CSock, S1, strlen(S1), 0); // 送資料
	}
}
