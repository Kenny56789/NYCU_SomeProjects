
// PPTDlg.h: 標頭檔
//

#pragma once


// CPPTDlg 對話方塊
class CPPTDlg : public CDialogEx
{
// 自訂成員變數與函式(定義 *.h)(內容 *.cpp)
public:
	HDC hdc;
	int Mouse_Down_Flag = 0; // 紀錄滑鼠是否按下旗標
	int Mouse_Down_x, Mouse_Down_y; // 紀錄滑鼠按下起始位址
	int Mouse_Move_x, Mouse_Move_y; // 紀錄滑鼠移動前位址
	int cx, cy, rx, ry;
	int Type; // 形狀
	int mode = 0; // 填滿

	void Line(int x1, int y1, int x2, int y2, int w, int Color);
	void Rect(int x1, int y1, int x2, int y2, int w, int Color, int Mode);
	void Circle(int cx, int cy, int rx, int ry, int w, int Color, int Mode);
	void ReDraw(); // 重繪物件

// 建構
public:
	CPPTDlg(CWnd* pParent = nullptr);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PPT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
