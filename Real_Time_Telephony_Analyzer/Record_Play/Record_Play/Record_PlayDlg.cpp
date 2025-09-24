
// Record_PlayDlg.cpp: 實作檔案
//

#include "pch.h"
#include "framework.h"
#include "Record_Play.h"
#include "Record_PlayDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CRecordPlayDlg 對話方塊



CRecordPlayDlg::CRecordPlayDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RECORD_PLAY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRecordPlayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRecordPlayDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CRecordPlayDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CRecordPlayDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CRecordPlayDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CRecordPlayDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CRecordPlayDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// CRecordPlayDlg 訊息處理常式

BOOL CRecordPlayDlg::OnInitDialog()
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

void CRecordPlayDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRecordPlayDlg::OnPaint()
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
HCURSOR CRecordPlayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include "CAudio.h"
CAudio A;
#define SAMPLE_RATE 8000 // >= 8KHz
#define FFT_SIZE123  512 // 2的次方
#define BUF_LEN123   480 // 80整數倍

// 啟動錄音
void CRecordPlayDlg::OnBnClickedButton1()
{
	// m_hWnd: 接收事件用之視窗代碼
	// CALLBACK_WINDOW: 使用事件通知已錄滿聲音, 事件:MM_WIN_DATA
	// CALLBACK_FUNCTION: 使用呼叫Callback Function通知已錄滿聲音
	system("del Record.dat"); // 刪除錄音檔案
	A.Init_Audio(BUF_LEN123, -1, -1, SAMPLE_RATE); 
	A.Init_FFT(FFT_SIZE123);
	A.Open_Record((DWORD)m_hWnd, CALLBACK_WINDOW);
	A.Start_Record(); // 系統發出錄滿聲音事件MM_WIN_DATA
}

// 停止錄音
void CRecordPlayDlg::OnBnClickedButton2()
{
	A.Stop_Record();
	A.Close_Record();
	A.Add_Wav("Record.dat", "Record.wav", SAMPLE_RATE); // 轉檔(加上header)
}

// 啟動放音
FILE* In123 = 0;
void CRecordPlayDlg::OnBnClickedButton3()
{
	errno_t err;
	// 1. 開啟放音裝置
	A.Open_Play((DWORD)m_hWnd, CALLBACK_WINDOW);
	A.Start_Play();

	// 2. 開啟檔案
	if ((err = fopen_s(&In123, "Record.dat", "rb")) != 0)
	{
		fclose(In123); 
		In123 = 0;
	}
}

// 停止放音
void CRecordPlayDlg::OnBnClickedButton4()
{
	// 1. 關閉放音裝置
	A.Stop_Play();
	A.Close_Play();

	// 2. 關閉放音檔案指標 In123
	fclose(In123);
	In123 = 0;
}


// (開啟WindowProc處理MM_WIM_DATA)
// 處理錄放音事件
LRESULT CRecordPlayDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	short Speech[1400]; // Time-Domain Waveform
	float PSD[1400];    // Frequency-Domain Waveform
	int   i, x, y, Len;
	HDC   hdc = ::GetDC(m_hWnd);
	FILE* In, * Out;
	errno_t err;
	if (message == MM_WIM_DATA) // 錄音事件
	{
		// 1. 讀取聲音資料
		A.Record_Speech(Speech, &Len); // 取得錄音資料、音效卡聲音
		if (Len == BUF_LEN123)
		{
			// 1.5 存檔
			if ((err = fopen_s(&Out, "Record.dat", "ab+")) == 0) 
			{
				fwrite(Speech, Len, sizeof(short), Out); fclose(Out);
			}
			// 2. 清除螢幕
			CPen P(PS_SOLID, 1, RGB(240, 240, 240)); // 灰色
			SelectObject(hdc, P);
			for (y = 0; y < 700; y++)
			{
				MoveToEx(hdc, 0, y, 0); LineTo(hdc, 3 * Len, y); // (0, y)畫到(Len*3, y)
				//LineTo(hdc, Len, y);
			}
			// 3. 畫出時域波形
			CPen P1(PS_SOLID, 2, RGB(0, 0, 0)); // 黑色
			SelectObject(hdc, P1);
			for (i = 0; i < Len; i++)
			{
				x = 3 * i;
				y = 200 + Speech[i] / 200;
				if (i == 0) MoveToEx(hdc, x, y, 0); // (測試不加入的後果) 清除右下角畫到右上角的線
				LineTo(hdc, x, y);
			}
			// 4. FFT 轉換
			for (i = BUF_LEN123; i < FFT_SIZE123; i++) Speech[i] = 0; // 480~511 set to zero
			A.REALFFT(Speech, PSD, FFT_SIZE123, BUF_LEN123);

			// 4.1. 搜尋最大值位置 k1
			float max1 = 0;
			int k1, freq1;
			for (i = 1; i < FFT_SIZE123 / 2; i++)
			{
				if (
					((PSD[i] > PSD[i - 1]) && (PSD[i] > PSD[i + 1])) &&// 頻譜凸出來(山頂)
					(PSD[i] > max1))
				{
					max1 = PSD[i];
					k1 = i; 
					freq1 = k1 * SAMPLE_RATE / FFT_SIZE123;
				}
			}
			// 4.2. 搜尋第二大值位置 k2
			float max2 = 0;
			int k2, freq2;
			for (i = 1; i < FFT_SIZE123 / 2 - 1; i++)
			{
				if (
					((PSD[i] > PSD[i - 1]) && (PSD[i] > PSD[i + 1])) &&// 頻譜凸出來(山頂)
					(PSD[i] > max2) && (i != k1))
				{
					max2 = PSD[i];
					k2 = i;
					freq2 = k2 * SAMPLE_RATE / FFT_SIZE123;
				}
			}
			// 4.3. freq1 < freq2
			if (freq1 > freq2) { i = freq1; freq1 = freq2; freq2 = i; }

			// 4.4. 比對 k1[697, 770, 852, 941]
			int j, k, k3, k4, m = 10000;
			int T1[4] = { 697, 770, 852, 941 };
			for (i = 0; i < 4; i++)
			{
				j = abs(freq1 - T1[i]);
				if ((i == 0) || ((m > j) && (j < 10))) { k3 = i; m = j; }
			}

			// 4.5. 比對 k2[1209, 1336, 1477, 1633]
			int T2[4] = { 1209, 1336, 1477, 1633 };
			m = 10000;
			for (i = 0; i < 4; i++)
			{
				j = abs(freq2 - T2[i]);
				if ((i == 0) || ((m > j) && (j < 10))) { k4 = i; m = j; }
			}

			// 4.6. 確認按鍵
			char T3[4][4] = {
				{'1', '2', '3', 'A'},
				{'4', '5', '6', 'B'},
				{'7', '8', '9', 'C'},
				{'*', '0', '#', 'D'}
			};
			char S1[100];
			sprintf_s(S1, sizeof(S1), "freq1 = %d  freq2 = %d  dig = %c", freq1, freq2, T3[k3][k4]);
			SetWindowText(S1);

			// 5. 畫出頻譜波形
			CPen P2(PS_SOLID, 2, RGB(255, 0, 0)); // 黑色
			SelectObject(hdc, P2);
			for (i = 0; i < Len; i++)
			{
				x = 3 * i;
				y = 700 - 10 * PSD[i] / 1000000000;
				if (i == 0) MoveToEx(hdc, x, y, 0); // (測試不加入的後果) 清除右下角畫到右上角的線
				LineTo(hdc, x, y);
			}
		}
	}
	else if (message == MM_WOM_DONE) // 放音事件
	{
		// 1. 讀取檔案
		if ((In123 != 0) && (!feof(In123)))  // 不是空指標 且 不是檔案尾
		{
			fread(Speech, BUF_LEN123, sizeof(short), In123);

			// 2. 放音
			A.Play_Speech(Speech, BUF_LEN123);
		}
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}


void CRecordPlayDlg::OnBnClickedButton5()
{
	A.USB_PlayDisconnectTone(m_hWnd);
}
