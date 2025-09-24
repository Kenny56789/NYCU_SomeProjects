
// BMPDlg.cpp: 實作檔案
//

#include "pch.h"
#include "framework.h"
#include "BMP.h"
#include "BMPDlg.h"
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


// CBMPDlg 對話方塊



CBMPDlg::CBMPDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BMP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBMPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
}

BEGIN_MESSAGE_MAP(CBMPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CBMPDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CBMPDlg::OnBnClickedButton2)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CBMPDlg 訊息處理常式

BOOL CBMPDlg::OnInitDialog()
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
	m_Slider.SetRange(0, 180); // 設定滑動範圍 0 ~ 180
	m_Slider.SetTicFreq(1);    // 滑動刻度
	m_Slider.SetPos(0);        // 設定滑塊控件的初始位置

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CBMPDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBMPDlg::OnPaint()
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
HCURSOR CBMPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#define PI    3.14159
// 讀取 *.BMP(RGB 24bits)檔案, 並繪出
void CBMPDlg::OnBnClickedButton1()
{
	// 1. 變數設定
	char File[100] = { "Canyon.bmp" };
	HDC hdc = ::GetDC(m_hWnd);

	// 2. 開啟檔案
	errno_t err;
	FILE* In1;
	if ((err = fopen_s(&In1, File, "rb")) != 0)
	{
		SetWindowText("Failed"); return;
	}
	else
		SetWindowText("Success");

	// 3. 讀取 BMP 檔案 Header
	// 4. 分析圖片 Width, Height, Bits, ...
	unsigned char H[54]; // Hile(14) + Info(40) = 54 Bytes
	char S1[100];
	int Width, Height, Bits;
	fread(H, 1, 54, In1);
	Width  = ((int)H[18] << 0) + ((int)H[19] << 8) + ((int)H[20] << 16) + ((int)H[21] << 24);
	Height = ((int)H[22] << 0) + ((int)H[23] << 8) + ((int)H[24] << 16) + ((int)H[25] << 24);
	Bits   = ((int)H[28] << 0) + ((int)H[29] << 8);
	sprintf_s(S1, 100, "Width = %d Height = %d Bits = %d", Width, Height, Bits);
	SetWindowText(S1);

	// 6. 繪出圖形
	unsigned char UCBuf[100];
	int x, y, d = (4 - (Width * 3) % 4) % 4;
	for (y = Height; y >= 0; y--)
	{
		for (x = 0; x < Width; x++)
		{
			// 6.1 讀取(x, y)點Pixel之BGR畫素
			fread(UCBuf, 1, 3, In1);
			// 6.1.5 修改圖形
			//if ((UCBuf[2] > 128) && (UCBuf[1] < 128) && (UCBuf[0] < 128))
			//{
			//	UCBuf[3] = UCBuf[2];
			//	UCBuf[2] = UCBuf[0];
			//	UCBuf[0] = UCBuf[3];
			//}
			// 6.2 設定顏色
			CPen P1(PS_SOLID, 1, RGB(UCBuf[2], UCBuf[1], UCBuf[0]));
			SelectObject(hdc, P1);
			// 6.3 畫出點
			MoveToEx(hdc, x, y, 0); LineTo(hdc, x + 1, y);
		}
		fread(UCBuf, 1, d, In1);
	}
	fclose(In1);
}

int angle;
void CBMPDlg::OnBnClickedButton2()
{
	// 1. 變數設定
	char File[100] = { "Mask.bmp" };
	HDC hdc = ::GetDC(m_hWnd);

	// 2. 開啟檔案
	errno_t err;
	FILE* In1;
	if ((err = fopen_s(&In1, File, "rb")) != 0)
	{
		SetWindowText("Failed"); return;
	}
	else
		SetWindowText("Success");

	// 3. 讀取 BMP 檔案 Header
	// 4. 分析圖片 Width, Height, Bits, ...
	unsigned char H[54]; // Hile(14) + Info(40) = 54 Bytes
	char S1[100];
	int Width, Height, Bits;
	fread(H, 1, 54, In1);
	Width = ((int)H[18] << 0) + ((int)H[19] << 8) + ((int)H[20] << 16) + ((int)H[21] << 24);
	Height = ((int)H[22] << 0) + ((int)H[23] << 8) + ((int)H[24] << 16) + ((int)H[25] << 24);
	Bits = ((int)H[28] << 0) + ((int)H[29] << 8);
	sprintf_s(S1, 100, "Width = %d Height = %d Bits = %d", Width, Height, Bits);
	SetWindowText(S1);

	// 6. 繪出圖形
	float a = (float)angle * PI / 180;
	unsigned char UCBuf[100];
	int x, y, xp, yp, d = (4 - (Width * 3) % 4) % 4;
	int dx = 800, dy = 80;
	for (y = Height; y >= 0; y--)
	{
		for (x = 0; x < Width; x++)
		{
			// 6.1 讀取(x, y)點Pixel之BGR畫素
			fread(UCBuf, 1, 3, In1);
			// 6.1.5 修改圖形
			if ((UCBuf[1] > 160) && (UCBuf[0] < 128) && (UCBuf[2] < 128))
			{
				// 不畫
			}
			else
			{
				// 6.2 設定顏色
				CPen P1(PS_SOLID, 1, RGB(UCBuf[2], UCBuf[1], UCBuf[0]));
				SelectObject(hdc, P1);

				// 6.2.5 旋轉
				//(x, y) => (xp, yp)
				xp = (int)((float)x * cos(a) - (float)y * sin(a));
				yp = (int)((float)x * sin(a) + (float)y * cos(a));

				// 6.3 畫出點
				MoveToEx(hdc, xp + dx, yp + dy, 0); LineTo(hdc, xp + dx + 1, yp + dy);
			}
		}
		fread(UCBuf, 1, d, In1);
	}
	fclose(In1);
}

void CBMPDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CWnd* pSlidCtrl1 = GetDlgItem(IDC_SLIDER1);
	if (pScrollBar == pSlidCtrl1)
	{
		angle = 1 * ((CSliderCtrl*)pSlidCtrl1)->GetPos();
	}
	char S1[100];
	sprintf_s(S1, sizeof(S1), "Rotate angle = %d", angle);
	SetWindowText(S1);
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
