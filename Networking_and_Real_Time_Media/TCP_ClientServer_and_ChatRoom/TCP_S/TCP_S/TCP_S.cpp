// TCP_S.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
#include <iostream>
#include <process.h>

// 1. 引入檔+基本設定
#include "TCPIP.h"
#define  SERVER_PORT       6000

#define MAX   1000
struct USER
{
	SOCKET   CSock; // 紀錄每個Client連線
	sockaddr Addr;
	char     IP[20];
	int      Port;
	char     Msg[2000];
	int      PKT_No;
	// ...
} User[MAX];
int     CNo = 0;    // Client端連線數

// 2. Global Variable(被struct USER取代)
//#define MAX 100
SOCKET  SSock;      // 接受連線用 Socket
//SOCKET  CSock[MAX]; // 紀錄每個Client連線
TCPIP   A;
//int     CNo = 0;    // Client端連線數

void Print()
{
	USER* lp;
	system("cls"); // 清除螢幕
	for (int i = 0; i < CNo; i++)
	{
		lp = &User[i];
		printf("CSock = %5d  IP = %15s  Port = %5d  Msg = %s\n",
			lp->CSock, lp->IP, lp->Port, lp->Msg);
	}
}


// 執行續
void Fun0(PVOID p)
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
			//printf("recv(%d) = %s\n", i, S1);
			strcpy_s(User[0].Msg, 1999, S1); Print();
		}
		else // 斷線
		{
			//printf("Client0 Disconnect\n");
			strcpy_s(User[0].Msg, 1999, "Disconnect"); Print();
			break;
		}
		
	}
}

// 執行續
void Fun1(PVOID p)
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
			//printf("recv(%d) = %s\n", i, S1);
			strcpy_s(User[1].Msg, 1999, S1); Print();
		}
		else // 斷線
		{
			//printf("Client1 Disconnect\n");
			strcpy_s(User[1].Msg, 1999, "Disconnect"); Print();
			break;
		}
	}
}

// 執行續
void Fun2(PVOID p)
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
			//printf("recv(%d) = %s\n", i, S1);
			strcpy_s(User[2].Msg, 1999, S1); Print();
		}
		else // 斷線
		{
			//printf("Client2 Disconnect\n");
			strcpy_s(User[2].Msg, 1999, "Disconnect"); Print();
			break;
		}
	}
}

// 執行續
void Fun3(PVOID p)
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
			//printf("recv(%d) = %s\n", i, S1);
			strcpy_s(User[3].Msg, 1999, S1); Print();
		}
		else // 斷線
		{
			//printf("Client3 Disconnect\n");
			strcpy_s(User[3].Msg, 1999, "Disconnect"); Print();
			break;
		}
	}
}
int main()
{
	int i, Len = sizeof(sockaddr);
	char S1[2000];
	sockaddr Addr;
	sockaddr_in* lp = (sockaddr_in*)&Addr;

	// 3. 啟動TCP伺服器
	A.Start_TCP_Server(&SSock, SERVER_PORT); // 卡住(listen)
												
	// 3.5 接受源源不斷的連線要求
	while (1)
	{
		// 4. 接受 Clinet 連線請求
		//CSock[CNo++] = accept(SSock, &Addr, &Len); // 卡住(listen)
		User[CNo++].CSock = accept(SSock, &Addr, &Len); // 卡住(listen)

		// 記錄個資
		User[CNo - 1].Addr = Addr;
		strcpy_s(User[CNo - 1].IP, 20, inet_ntoa(lp->sin_addr));
		User[CNo - 1].Port = ((lp->sin_port >> 8) & 0xFF) + ((lp->sin_port & 0xFF) << 8);
		strcpy_s(User[CNo - 1].Msg, 1999, "Accept");
		Print();

		//if (CNo == 1) _beginthread(Fun0, 0, (void*)CSock[CNo - 1]);
		if      (CNo == 1) _beginthread(Fun0, 0, (void*)User[CNo - 1].CSock);
		else if (CNo == 2) _beginthread(Fun1, 0, (void*)User[CNo - 1].CSock);
		else if (CNo == 3) _beginthread(Fun2, 0, (void*)User[CNo - 1].CSock);
		else if (CNo == 4) _beginthread(Fun3, 0, (void*)User[CNo - 1].CSock);
	}
	//while (1)
	//{
	//	// 5. 送出問候語
	//	strcpy_s(S1, "Welcome to my Server... \r\n");
	//	send(CSock[0], S1, strlen(S1), 0);

	//	// 6. 接收資料
	//	i = recv(CSock[0], S1, sizeof(S1) - 1, 0); // 卡住(recv)
	//	if (i > 0) { S1[i] = 0; printf("recv(%d) = %s\n", i, S1); }
	//}
}
