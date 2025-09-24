
// PPTDlg.cpp: 實作檔案
//

#include "pch.h"
#include "framework.h"
#include "PPT.h"
#include "PPTDlg.h"
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


// CPPTDlg 對話方塊



CPPTDlg::CPPTDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PPT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPPTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPPTDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON1, &CPPTDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPPTDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CPPTDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CPPTDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CPPTDlg 訊息處理常式

BOOL CPPTDlg::OnInitDialog()
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
	hdc = ::GetDC(m_hWnd); // 取得繪圖視窗代碼(hdc), m_hWnd=視窗代碼(接收事件用)

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CPPTDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPPTDlg::OnPaint()
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
HCURSOR CPPTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPPTDlg::Line(int x1, int y1, int x2, int y2, int w, int Color)
{
	// 設定顏色
	unsigned char r, g, b;
	r = (Color >> 16) & 0xFF; // 取得高位元
	g = (Color >>  8) & 0xFF; // 取得中位元
	b = (Color >>  0) & 0xFF; // 取得低位元
	CPen P1(PS_SOLID, w, RGB(r, g, b));
	SelectObject(hdc, P1); // 選色筆

	// 畫出線條
	MoveToEx(hdc, x1, y1, 0); LineTo(hdc, x2, y2);
}
void CPPTDlg::Rect(int x1, int y1, int x2, int y2, int w, int Color, int Mode) // Mode = 0中空, Mode=1填滿
{
	// 設定顏色
	unsigned char r, g, b;
	r = (Color >> 16) & 0xFF; // 取得高位元
	g = (Color >> 8) & 0xFF; // 取得中位元
	b = (Color >> 0) & 0xFF; // 取得低位元
	CPen P1(PS_SOLID, w, RGB(r, g, b));
	SelectObject(hdc, P1); // 選色筆

	// 畫出四邊線條
	if (Mode == 0)
	{
		MoveToEx(hdc, x1, y1, 0); 
		LineTo(hdc, x1, y2);
		LineTo(hdc, x2, y2);
		LineTo(hdc, x2, y1);
		LineTo(hdc, x1, y1);
	}
	else if (Mode == 1) // 填滿
	{
		/*if (y1 < y2)
			for (int y = y1; y <= y2; y++)
			{
				MoveToEx(hdc, x1, y, 0); LineTo(hdc, x2, y);
			}
		else
			for (int y = y2; y <= y1; y++)
			{
				MoveToEx(hdc, x1, y, 0); LineTo(hdc, x2, y);
			}*/
		for (int y = min(y1, y2); y <= max(y1, y2); y++)
		{
			MoveToEx(hdc, x1, y, 0); LineTo(hdc, x2, y);
		}
	}
}
# define PI 3.14159
void CPPTDlg::Circle(int cx, int cy, int rx, int ry, int w, int Color, int Mode) // (cx, cy)圓心, (rx, ry)半徑, Mode = 0中空, Mode=1填滿
{
	// 設定顏色
	unsigned char r, g, b;
	r = (Color >> 16) & 0xFF; // 取得高位元
	g = (Color >> 8) & 0xFF; // 取得中位元
	b = (Color >> 0) & 0xFF; // 取得低位元
	CPen P1(PS_SOLID, w, RGB(r, g, b));
	SelectObject(hdc, P1); // 選色筆

	// 畫出線條
	int Angle;
	float Theta;
	int x, y;
	if (Mode == 0) // 中空
	{
		for (Angle = 0; Angle <= 360; Angle++)
		{
			Theta = (float)Angle * PI / 180;
			x = cx + (int)((float)rx * cos(Theta) + 0.5);
			y = cy + (int)((float)ry * sin(Theta) + 0.5);
			if (Angle == 0) MoveToEx(hdc, x, y, 0);
			LineTo(hdc, x, y);
		}
	}
	else if (Mode == 1) // 填滿
	{
		int y, x1, x2;
		float r = sqrt((float)(rx * rx + ry * ry));
		for (y = -ry; y <= ry; y++)
		{
			x1 = (int)((float)rx * sqrt(1 - (float)y * y / ry / ry) + 0.5);
			x2 = 0 - x1;
			MoveToEx(hdc, cx + x1, cy + y, 0); LineTo(hdc, cx + x2, cy + y);
		}
	}
}

// 紀錄已繪出物件
#define MAX 1000
struct OBJECT
{
	int Type; // 0:Line, 1:Rect, 2:Circle
	int x1, y1, x2, y2; // Line, Rect,
	int cx, cy, rx, ry; // Circle
	int w;
	int Color;
	int Mode; // Rect, Circle
}Object[MAX];
int Obj_No = 0;

void CPPTDlg::ReDraw() // 重繪物件
{
	OBJECT* lp;
	for (int i = 0; i < Obj_No; i++)
	{
		lp = &Object[i];
		if      (lp->Type == 0) Line(lp->x1, lp->y1, lp->x2, lp->y2, lp->w, lp->Color);
		else if (lp->Type == 1) Rect(lp->x1, lp->y1, lp->x2, lp->y2, lp->w, lp->Color, lp->Mode);
		else if (lp->Type == 2) Circle(lp->cx, lp->cy, lp->rx, lp->ry, lp->w, lp->Color, lp->Mode);
	}
}

void CPPTDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	// 紀錄滑鼠被按下與按下位置
	Mouse_Down_Flag = 1;
	Mouse_Down_x = point.x;
	Mouse_Down_y = point.y;
	// 設定滑鼠移動起始位址
	Mouse_Move_x = point.x;
	Mouse_Move_y = point.y;
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CPPTDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	Mouse_Down_Flag = 0;

	// 2.5 新增物件
	OBJECT* lp = &Object[Obj_No];
	if (Type == 0)
		lp->Type = 0;
	else if (Type == 1)
		lp->Type = 1;
	else if (Type = 2)
		lp->Type = 2;
	lp->cx = cx;
	lp->cy = cy;
	lp->rx = rx;
	lp->ry = ry;
	lp->x1 = Mouse_Down_x;
	lp->y1 = Mouse_Down_y;
	lp->x2 = Mouse_Move_x;
	lp->y2 = Mouse_Move_y;
	lp->w = 5;
	lp->Color = 0x000000;
	if (mode)
		lp->Mode = 1;
	else 
		lp->Mode = 0;
	Obj_No++;

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CPPTDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	//int cx, cy, rx, ry;
	if (Mouse_Down_Flag)
	{
		if (Type == 0)
		{
			Line(Mouse_Down_x, Mouse_Down_y, Mouse_Move_x, Mouse_Move_y, 5, 0xF0F0F0); // 0xF0F0F0 = 灰色
			ReDraw();
			Mouse_Move_x = point.x, Mouse_Move_y = point.y; // 更新滑鼠移動位址
			Line(Mouse_Down_x, Mouse_Down_y, Mouse_Move_x, Mouse_Move_y, 5, 0x000000); // 0x000000 = 黑色
		}
		// mode = 0:hollow, 1: Solid
		else if (Type == 1)
		{
			Rect(Mouse_Down_x, Mouse_Down_y, Mouse_Move_x, Mouse_Move_y, 5, 0xF0F0F0, mode); // 0xF0F0F0 = 灰色
			ReDraw();
			Mouse_Move_x = point.x, Mouse_Move_y = point.y; // 更新滑鼠移動位址
			Rect(Mouse_Down_x, Mouse_Down_y, Mouse_Move_x, Mouse_Move_y, 5, 0x000000, mode); // 0xF0F0F0 = 灰色
		}
		else if (Type == 2)
		{
			cx = (Mouse_Down_x + Mouse_Move_x) / 2;
			cy = (Mouse_Down_y + Mouse_Move_y) / 2;
			rx = abs(Mouse_Down_x - Mouse_Move_x) / 2;
			ry = abs(Mouse_Down_y - Mouse_Move_y) / 2;
			Circle(cx, cy, rx, ry, 5, 0xF0F0F0, mode);

			ReDraw();
			Mouse_Move_x = point.x, Mouse_Move_y = point.y; // 更新滑鼠移動位址

			cx = (Mouse_Down_x + Mouse_Move_x) / 2;
			cy = (Mouse_Down_y + Mouse_Move_y) / 2;
			rx = abs(Mouse_Down_x - Mouse_Move_x) / 2;
			ry = abs(Mouse_Down_y - Mouse_Move_y) / 2;
			Circle(cx, cy, rx, ry, 5, 0x00000, mode);
		}
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

void CPPTDlg::OnBnClickedButton1() // Line
{
	Type = 0;
	SetDlgItemText(IDC_BUTTON1, "<<Line>>");
	SetDlgItemText(IDC_BUTTON2, "Rect");
	SetDlgItemText(IDC_BUTTON3, "Circle");
}


void CPPTDlg::OnBnClickedButton2() // Rect
{
	Type = 1;
	SetDlgItemText(IDC_BUTTON1, "Line");
	SetDlgItemText(IDC_BUTTON2, "<<Rect>>");
	SetDlgItemText(IDC_BUTTON3, "Circle");
}


void CPPTDlg::OnBnClickedButton3() // Circle
{
	Type = 2;
	SetDlgItemText(IDC_BUTTON1, "Line");
	SetDlgItemText(IDC_BUTTON2, "Rect");
	SetDlgItemText(IDC_BUTTON3, "<<Circle>>");
}

// mode: Solid / Hollow
void CPPTDlg::OnBnClickedButton4()
{
	mode = (mode == 1) ? 0 : 1;
	/*CString str;
	if (GetDlgItemText(IDC_BUTTON4, str), str == "Mode <<Hollow>>")
		SetDlgItemText(IDC_BUTTON4, "Mode <<Solid>>");
	else
		SetDlgItemText(IDC_BUTTON4, "Mode <<Hollow>>");*/
	if (mode == 1)
		SetDlgItemText(IDC_BUTTON4, "Mode <<Solid>>");
	else
		SetDlgItemText(IDC_BUTTON4, "Mode <<Hollow>>");
}
