// Telnet.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。

#include <iostream>

// 0. Include & Variable Define
#include <conio.h>   // _getch()
#include <process.h> // _beginthread
#include "TCPIP.h"
SOCKET CSock;
TCPIP  A;

// 0.5.執行續, 讀網路訊息並列印
// 執行時，會將p轉成socket，接著進入無窮迴圈收資料，若i > 0為真的有收到資料, 否則為斷線
void Fun(PVOID p) 
{
    int    i, Len = sizeof(sockaddr);
    char   S1[2000];
    SOCKET Sock = (SOCKET)p;
    while (1)
    {
        i = recv(Sock, S1, sizeof(S1) - 1, 0); // 卡住(Blocking Mode)
        if (i > 0) { S1[i] = 0; printf("%s", S1); }
        else       { printf("Telnet Server Disconnect\n"); break; }
    }
}


int main()
{
    /*
    unsigned char d;
    while (1)
    {
        d = _getch();
        printf("Key = %c ID = %x\n", d, d);
    }
    */

    // 1. 變數宣告
    //char Server_IP[100] = { "64.13.139.230" }; // telhack.com:23
    char Server_IP[100] = { "134.208.10.240" }; // bbs.ndhu.edu.tw:23
    int  Server_Port = 23; // 23: Telnet
    // 2. 啟動 TCP Client & 啟動 thread 接收資料
    A.Start_TCP_Client(&CSock, Server_IP, 0, Server_Port);
    _beginthread(Fun, 0, (PVOID)CSock); // 啟動 thread 獨立接收資料

    // 3. 讀鍵盤, 送出按鍵資料
    unsigned char c, c1; // char: -128 ~ 127(0x80~0x7F), unsigned char: 0~255(0x00~0xFF)
    int F = 0;
    while ((c = _getch()) != 'q') // 讀鍵盤一個按鍵(不顯示)(若要顯示:_getche())
    {
        // 修正上下左右按鍵碼(e.g. 按下鍵盤會讀到0xE048, 但要轉為1B.5B.41送出)
        //if      (c == 0xE048) // 上 => 1B.5B.41
        //else if (c == 0xE050) // 下 => 1B.5B.42
        //else if (c == 0xE04B) // 左 => 1B.5B.44
        //else if (c == 0xE04D) // 右 => 1B.5B.43
        if (F == 0) 
        {
            if   (c == 0xE0) F = 1;
            else send(CSock, (char *)&c, 1, 0);
        }
        else // 已經按了上, 下, 左, 右
        {
            c1 = 0x1B; send(CSock, (char*)&c1, 1, 0);
            c1 = 0x5B; send(CSock, (char*)&c1, 1, 0);
            if      (c == 0x48) c1 = 0x41; // send 1B.5B.41
            else if (c == 0x50) c1 = 0x42; // send 1B.5B.42
            else if (c == 0x4B) c1 = 0x44; // send 1B.5B.44
            else if (c == 0x4D) c1 = 0x43; // send 1B.5B.43
            send(CSock, (char*)&c1, 1, 0);
            F = 0;
        }
        
    }
    closesocket(CSock);
}
