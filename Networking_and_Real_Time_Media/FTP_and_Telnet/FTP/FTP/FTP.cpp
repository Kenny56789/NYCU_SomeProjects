// FTP.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
#include <iostream>

// ================================
// 設計 FTP 程式(Part-1) ==========
// ================================
// 1. Initialize
#include "TCPIP.h"   // TCP
#include "process.h" // thread
#include "conio.h"   // _getch(), _getche()
TCPIP    A;
SOCKET   CSock;

void Get_IP_Port(char* S1, char* IP, int* Port) // 解析IP, Port
{
    int i, Beg, End, i1, i2, i3, i4, i5, i6;
    // 1.找出(,)位置
    for (i = 0; i < strlen(S1); i++) if (S1[i] == '(') { Beg = i; break; }
    for (i = 0; i < strlen(S1); i++) if (S1[i] == ')') { End = i; break; }
    // 2.擷取6碼數字
    S1[End] = 0;
    sscanf_s(&S1[Beg + 1], "%d,%d,%d,%d,%d,%d", &i1, &i2, &i3, &i4, &i5, &i6);
    // 3.還原IP, Port
    sprintf_s(IP, 100, "%d.%d.%d.%d", i1, i2, i3, i4);
    *Port = i5 * 256 + i6;
}

// ================================
// 1.2. NLST 專用(for 下方(5)(6)中Thread部分)
int    NLST_Flag = 0;
SOCKET NLST_Sock;
void NLST_Fun(PVOID p) // 下載檔案名稱與目錄
{
    int    i, Len = sizeof(sockaddr);
    char   S1[2000];
    SOCKET Sock = (SOCKET)p;
    while (1)
    {
        i = recv(Sock, S1, sizeof(S1) - 1, 0); // 卡住(Blocking Mode)
        if (i > 0) { S1[i] = 0; printf("%s", S1); }
        else 
        {
            printf("NLST Server Disconnect\n");
            closesocket(Sock);
            break; 
        }
    }
}

// ================================
// 1.3. RETR a.c專用(for 下方(5)(6)中Thread部分)
int    RETR_Flag = 0;  // 啟動RETR分析命令
SOCKET RETR_Sock;      // 被動模式連線代碼
char   RETR_File[200]; // 存檔名稱
void RETR_Fun(PVOID p) // 下載檔案名稱與目錄
{
    int    i, Len = sizeof(sockaddr);
    char   S1[2000];
    SOCKET Sock = (SOCKET)p;
    // 1. 開啟檔案
    errno_t err;
    FILE* Out;
    if ((err = fopen_s(&Out, RETR_File, "wb")) != 0)
    {
        printf("File %s can't be create.\n", RETR_File);
        return;
    }
    while (1)
    {
        // 2. 接收網路封包內容, 並存成檔案
        i = recv(Sock, S1, sizeof(S1) - 1, 0); // 卡住(Blocking Mode)
        if (i > 0) 
        {
            //S1[i] = 0; printf("%s", S1); 
            if (Out != 0) fwrite(S1, 1, i, Out); // 存成檔案
        }
        // 3. 關閉檔案
        else 
        {
            fclose(Out); // 關閉檔案
            printf("RETR Server Disconnect\n"); 
            closesocket(Sock); // 關閉網路
            break; 
        }
    }
}

// 1.5 Thread Function
void Fun(PVOID p) // 接收 Command回應
{
    int    i, Len = sizeof(sockaddr), Port;
    char   S1[2000], IP[100];
    SOCKET Sock = (SOCKET)p;
    while (1)
    {
        i = recv(Sock, S1, sizeof(S1) - 1, 0); // 卡住(Blocking Mode)
        if (i > 0) { S1[i] = 0; printf("%s", S1); }
        else       { printf("Telnet Server Disconnect\n"); break; }
        // (3). 攔截過濾分析伺服器傳來的(IP, Port) + NLST_Flag = 0
        if (NLST_Flag)
        {
            // 解析 IP & Port
            Get_IP_Port(S1, IP, &Port);
            A.Start_TCP_Client(&NLST_Sock, IP, 0, Port);
            _beginthread(NLST_Fun, 0, (PVOID)NLST_Sock); // 啟動 thread 獨立接收資料
            strcpy_s(S1, sizeof(S1), "NLST\r\n");
            send(CSock, S1, strlen(S1), 0);
            Sleep(1000); // 暫停1s, 等待系統回應 為了先顯示問候語，再顯示part4.帳號登入部分
            NLST_Flag = 0;
        }
        else if (RETR_Flag)
        {
            // 解析 IP & Port
            Get_IP_Port(S1, IP, &Port);
            A.Start_TCP_Client(&NLST_Sock, IP, 0, Port);
            _beginthread(RETR_Fun, 0, (PVOID)RETR_Sock); // 啟動 thread 獨立接收資料
            sprintf_s(S1, sizeof(S1), "RETR %s\r\n", RETR_File);
            send(CSock, S1, strlen(S1), 0);
            Sleep(1000); // 暫停1s, 等待系統回應 為了先顯示問候語，再顯示part4.帳號登入部分
            RETR_Flag = 0;
        }
    }
}

int main()
{
    /*char S2[2000];
    int i;
    while (1)
    {
        scanf_s("%s", S2, sizeof(S2));
        i = 0;
        do { S2[i++] = _getche(); } while (S2[i - 1] != 0x0d);
        S2[i] = 0; printf("\n");
        printf("=> %s\n", S2);
    }*/
    
    
    // 2. 變數宣告
    //char Server_IP[100] = { "210.61.132.2" }; // ftp.speed.hinet.net:21    ftp/ftp
    char Server_IP[100] = { "194.108.117.16" }; // test.rebex.net, ftp.rebex.com: demo/password
    int  Server_Port = 21; // 21:FTP, 23:TELNET, 80:HTTP, 443:HTTPS, ...
    char User[100], Password[100], Command[100], S1[2000];

    // 3. Start_TCP_Client + 啟動Thread
    A.Start_TCP_Client(&CSock, Server_IP, 0, Server_Port);
    _beginthread(Fun, 0, (PVOID)CSock); // 啟動 thread 獨立接收資料
    Sleep(1000); // 暫停1s, 等待系統回應 為了先顯示問候語，再顯示part4.帳號登入部分

    // 4. 帳號登入處理
    printf("User (%s:(none)): ", Server_IP);        // 輸入帳號提示
    scanf_s("%s", User, sizeof(User));              // 讀取帳號資料
    sprintf_s(S1, sizeof(S1), "USER %s\r\n", User); // 製造帳號指令格式
    send(CSock, S1, strlen(S1), 0);                 // 送出帳號資料
    Sleep(1000);                                    // 暫停1s, 等待系統回應帳號正確否

    // 5. 密碼驗證處理
    printf("Password: ");                               // 輸入密碼提示
    //scanf_s("%s", Password, sizeof(Password));          // 讀取密碼資料
    int i = 0;
    do { Password[i++] = _getch(); printf("*"); } while (Password[i - 1] != 0x0d);
    Password[i - 1] = 0; printf("\n");

    sprintf_s(S1, sizeof(S1), "PASS %s\r\n", Password); // 製造密碼指令格式
    send(CSock, S1, strlen(S1), 0);                     // 送出密碼資料
    Sleep(1000);                                        // 暫停1s, 等待系統回應密碼正確否

    // 6. 進入 Request-Response Mode(送命令+接收回應)
    // case-1(不帶參數): PWD, PASV, ...
    // case-2(帶一參數): USER aaa, PASS bbb, CWD abc, CWD ...
    // case-3(其他)    : NLST(下載目錄), RETR a.c(下載a.c), STOR a.c(上傳a.c)
    do
    {
        printf("ftp> ");                              // 輸入命令提示
        //scanf_s("%s", Command, sizeof(Command));    // 讀取命令資料, 為了帶參數變為以下3行
        int i = 0;
        do { Command[i++] = _getche(); } while (Command[i - 1] != 0x0d);
        Command[i - 1] = 0; printf("\n");
        // 判斷指令: NLST, RETR, STOR
        // case-3-1: NLST(下載目錄)
        if ((!strcmp(Command, "NLST")) || (!strcmp(Command, "nlst")))
        {
            // v(1). 送出PASV指令(被動模式)
            strcpy_s(S1, sizeof(S1), "PASV\r\n");
            // v(2). 設定 NLST Mode
            NLST_Flag = 1;
            // v(3). 攔截過濾分析伺服器傳來的(IP, Port) + NLST_Flag = 0
            // v(4). 送出 NLST指令
            // v(5). 啟動 NLST TCP Client連線 IP, Port + Thread
            // v(6). 啟動 NLST_Thread接收檔案目錄資料
            // x(7). 關閉 NLST TCP Client連線 & NLST_Thread
        }
        // case-3-2: RETR a.c(下載檔案)
        else if ((!strncmp(Command, "RETR", 4)) || (!strncmp(Command, "retr", 4)))
        {
            // (0.5). 取得檔案名稱
            //sscanf_s(Command, "RETR %s", RETR_File, sizeof(RETR_File)); // 避免輸入 "retr" 故障
            strcpy_s(RETR_File, sizeof(RETR_File), &Command[5]);
            // v(1). 送出PASV指令(被動模式)
            strcpy_s(S1, sizeof(S1), "PASV\r\n");
            // v(2). 設定 NLST Mode
            RETR_Flag = 1;
            // v(3). 攔截過濾分析伺服器傳來的(IP, Port) + RETR_Flag = 0
            // v(4). 送出 RETR a.c指令
            // v(5). 啟動 RETR TCP Client連線 IP, Port + Thread
            // v(6). 啟動 RETR_Thread接收檔案內容資料, 並存檔至a.c
            // x(7). 關閉 RETR TCP Client連線 & RETR_Thread
        }
        else if ((!strncmp(Command, "STOR", 4)) || (!strncmp(Command, "stor", 4)))
        {
            // (0.5). 取得檔案名稱
            //sscanf_s(Command, "RETR %s", RETR_File, sizeof(RETR_File));
            //strcpy_s(STOR_File, sizeof(STOR_File), &Command[5]);
            // v(1). 送出PASV指令(被動模式)
            //strcpy_s(S1, sizeof(S1), "PASV\r\n");
            // v(2). 設定 NLST Mode
            RETR_Flag = 1;
            // v(3). 攔截過濾分析伺服器傳來的(IP, Port) + RETR_Flag = 0
            // v(4). 送出 RETR a.c指令
            // v(5). 啟動 RETR TCP Client連線 IP, Port + Thread
            // v(6). 啟動 RETR_Thread接收檔案內容資料, 並存檔至a.c
            // x(7). 關閉 RETR TCP Client連線 & RETR_Thread
        }
        else
        {
            sprintf_s(S1, sizeof(S1), "%s\r\n", Command); // 製造命令指令格式
        }
        send(CSock, S1, strlen(S1), 0);               // 送出命令資料
        Sleep(1000);                                  // 暫停1s, 等待系統回應密碼正確否
    } while (strcmp(Command, "quit"));
    char c = _getch();
    
}
