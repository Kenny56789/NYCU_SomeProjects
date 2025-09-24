
// ChatRoomDlg.cpp: 實作檔案
//

#include "pch.h"
#include "framework.h"
#include "ChatRoom.h"
#include "ChatRoomDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ##################################################################
// ##### 1.系統初始化 #####
#define CHAT_ROOM_EVENT (WM_USER + 100)
#include "TCPIP.h"
TCPIP    A;
SOCKET   CSock; // Server使用
SOCKET   SSock; // Client使用
SOCKET   Sock;  // Server & Client Mode共用
// ##################################################################

// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatRoomDlg 對話方塊



CChatRoomDlg::CChatRoomDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATROOM_DIALOG, pParent)
	, m_Display(_T("Welcome to ChatRoom\r\n"))
	, m_Msg(_T("Hello"))
	, m_IP(_T("127.0.0.1"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatRoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Display);
	DDX_Text(pDX, IDC_EDIT2, m_Msg);
	DDX_Control(pDX, IDC_EDIT1, m_Display2);
	DDX_Text(pDX, IDC_EDIT3, m_IP);
}

BEGIN_MESSAGE_MAP(CChatRoomDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CChatRoomDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CChatRoomDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CChatRoomDlg 訊息處理常式

BOOL CChatRoomDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	// ##################################################################
	// ##### 2.啟動伺服器 #####
	A.Start_TCP_Server(&SSock, 6000, CHAT_ROOM_EVENT, m_hWnd); // non-blocking Mode, Event-Driven Mode
	UpdateData(1); m_Display += "Start Server Ok\r\n"; UpdateData(0);

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CChatRoomDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CChatRoomDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CChatRoomDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 送出訊息
void CChatRoomDlg::OnBnClickedButton1()
{
	char S1[2000];
	// 1.取得輸入訊息
	UpdateData(1); sprintf_s(S1, sizeof(S1), "%s", m_Msg); UpdateData(0);
	// 2.送出訊息
	send(Sock, S1, strlen(S1), 0);
	// 3.顯示自己訊息
	UpdateData(1); m_Display += "You:\r\n   "; m_Display += S1; m_Display += "\r\n\r\n"; UpdateData(0);
	// 4.捲動軸
	m_Display2.LineScroll(m_Display2.GetLineCount());
}


LRESULT CChatRoomDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	sockaddr Addr;
	int i, Len = sizeof(sockaddr);
	char S1[2000], S2[2000];

	// Server Event: 觸發 (FD_ACCEPT,  FD_WRITE, FD_READ, FD_CLOSE)
	// Client Event: 觸發 (FD_CONNECT, FD_WRITE, FD_READ, FD_CLOSE)

	if (message == CHAT_ROOM_EVENT)
	{
		if (lParam == FD_ACCEPT) // 遠端要求連線
		{
			// 接收遠端連線
			Sock = accept(wParam, &Addr, &Len);
			UpdateData(1);
			m_Display += "Accept remote connect\r\n";
			UpdateData(0);
			// 資料捲動
			m_Display2.LineScroll(m_Display2.GetLineCount());

		}
		else if (LOWORD(lParam) == FD_CONNECT) 
		{
			if (HIWORD(lParam)) // 連線遠端成功
			{
				UpdateData(1);
				m_Display += "Connect Remote OK\r\n";
				UpdateData(0);
				// 資料捲動
				m_Display2.LineScroll(m_Display2.GetLineCount());
			}
			else // 連線遠端失敗
			{
				UpdateData(1);
				m_Display += "Connect Remote Fail\r\n";
				UpdateData(0);
				// 資料捲動
				m_Display2.LineScroll(m_Display2.GetLineCount());
			}
		}
		else if (LOWORD(lParam) == FD_READ) // 遠端送來資料
		{
			i = recv(wParam, S1, sizeof(S1), 0); // 接收資料 & 顯示
			if (i > 0)
			{
				// 接收資料 & 顯示
				S1[i] = 0;
				UpdateData(1);
				m_Display += "Remote: \r\n  "; m_Display += S1; m_Display += "\r\n\r\n";
				UpdateData(0);
				// 資料捲動
				m_Display2.LineScroll(m_Display2.GetLineCount());
			}
		}
		else if (LOWORD(lParam) == FD_CLOSE) // 遠端斷線
		{
			closesocket(wParam);
			UpdateData(1);
			m_Display += "Remote: \r\n  Disconnect\r\n"; 
			UpdateData(0);
			// 資料捲動
			m_Display2.LineScroll(m_Display2.GetLineCount());
		}
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}


void CChatRoomDlg::OnBnClickedButton2()
{
	char IP[20];
	// 1.取得IP
	UpdateData(1); sprintf_s(IP, sizeof(IP), "%s", m_IP); UpdateData(0);
	// 2.啟動連線遠端(FD_CONNECT, FD_READ, FD_CLOSE)
	A.Start_TCP_Client(&CSock, 6000, IP, CHAT_ROOM_EVENT, m_hWnd);
	Sock = CSock;
	// 3.顯示
	UpdateData(1);
	m_Display += "Connect to Remote\r\n";
	UpdateData(0);
	// 資料捲動
	m_Display2.LineScroll(m_Display2.GetLineCount());
}
