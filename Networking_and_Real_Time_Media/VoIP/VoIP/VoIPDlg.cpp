
// VoIPDlg.cpp: 實作檔案
//

#include "pch.h"
#include "framework.h"
#include "VoIP.h"
#include "VoIPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ==========================================
// =============== VoIP Design ==============
// ==========================================
// 1. 引入 TCPIP 與 CAudio 物件
#include "TCPIP.h"
#include "CAudio.h"
TCPIP A;
CAudio B;

// 2. 網路變數
#define CEVENT   (WM_USER + 100)          // 收到聲音封包事件
//#define RELAY_IP (char *)"140.113.170.78"  // Relay Server(RTP聲音封包交換伺服器) IP
//#define RELAY_IP (char *)"172.18.18.101"  // Relay Server(RTP聲音封包交換伺服器) IP
#define RELAY_IP (char *)"127.0.0.1"  // Relay Server(RTP聲音封包交換伺服器) IP
#define RELAY_PORT 9000                   // UDP Server Port
#define CHANNEL_ID 11                     // Relay_ID
SOCKET      CSock, SSock;
sockaddr_in CAddr;

// 3. RTP變數
struct _RTPHeader // 12 Bytes
{
	// 以 Bit 為單位之參數宣告，必須湊滿 32 bits，否則會被自動塞進空Bytes
	// 因此 unsigned shot seq_num; 要配合前面宣告成 unsigned int seq_num :16;
	unsigned int         CC : 4;  // CC=0~15      // CC field 
	unsigned int          X : 1;  // X=0~1       // X field 
	unsigned int          P : 1;         // padding flag 
	unsigned int    version : 2;  // 0~3

	unsigned int          PT : 7;  // 0~127    // PT field, Payload(聲音) Type
	unsigned int          MM : 1;  // 0~1     // M field 

	unsigned int        seq_num : 16; // 0~65535 length of the recovery 
	unsigned int          TS;        // +- 21億 Timestamp 
	unsigned int          ssrc : 16;
	unsigned int          Relay_ID : 16;
}; //12 bytes

// 3.5. 傳輸狀態
int Tx = 0, Rx = 0;

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


// CVoIPDlg 對話方塊



CVoIPDlg::CVoIPDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VOIP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVoIPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVoIPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CVoIPDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CVoIPDlg 訊息處理常式

BOOL CVoIPDlg::OnInitDialog()
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

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CVoIPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CVoIPDlg::OnPaint()
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
HCURSOR CVoIPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CVoIPDlg::OnBnClickedButton1() // Call-Out
{
	// 4. 啟動網路電話
	A.Start_UDP_Server(&SSock, RELAY_PORT, CEVENT, m_hWnd);
	A.Start_UDP_Client(&CSock, &CAddr, RELAY_PORT, RELAY_IP, CEVENT, m_hWnd); // UDP Client啟動
	B.Open_Record((DWORD)m_hWnd, CALLBACK_WINDOW); B.Start_Record();          // 啟動錄音, 觸發錄音事件MM_WIM_DATA
	B.Open_Play((DWORD)m_hWnd, CALLBACK_WINDOW);   B.Start_Play();            // 啟動放音
}


LRESULT CVoIPDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	char          S1[200];
	short         Speech[500]; // 聲音資料
	unsigned char RTP[1000];   // RTP封包 = RTP Header(12) + Speech
	_RTPHeader    *lp = (_RTPHeader *)RTP; // (和上一行是一體的兩面, 指向同一塊記憶體)
	int           i, Len = sizeof(sockaddr), Len1;
	
	// 5. 處理錄音事件
	if (message == MM_WIM_DATA)
	{
		// 5.1. 取得錄音資料
		B.Record_Speech(Speech, &Len1);
		// 5.2. 補上RTP Header(12 Byte)
		lp->Relay_ID = CHANNEL_ID; // 設定兩兩互通通道代碼
		memcpy(RTP + 12, Speech, Len1 * 2);
		// 5.3. RTP封包送給對方
		sendto(CSock, (char*)RTP, 12 + Len1 * 2, 0, (sockaddr*)&CAddr, Len);
		Tx += Len1 * 2 + 12;
		sprintf_s(S1, sizeof(S1), "Tx = %d  Rx = %d", Tx, Rx); SetWindowText(S1);
	}

	// 5.5 不處理放音事件
	if (message == MM_WOM_DONE) // 聲音放完了也不能怎樣, 不處理放音事件
	{
	}
	// 6. 處理接收聲音封包
	else if (message == CEVENT)
	{
		// 6.1. 取得聲音RTP封包
		i = recvfrom(wParam, (char*)RTP, sizeof(RTP), 0, (sockaddr*)&CAddr, &Len);
		// 6.2. 去除RTP Header(12)
		// 6.3. 放音
		if (i > 12)
		{
			B.Play_Speech((short*)&RTP[12], (i - 12) / 2);  // "/ 2"才是short的長度
			Rx += i;
			sprintf_s(S1, sizeof(S1), "Tx = %d  Rx = %d", Tx, Rx); SetWindowText(S1);
		}
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}
