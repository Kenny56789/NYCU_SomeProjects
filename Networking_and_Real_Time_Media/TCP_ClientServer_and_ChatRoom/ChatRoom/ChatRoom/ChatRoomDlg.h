
// ChatRoomDlg.h: 標頭檔
//

#pragma once


// CChatRoomDlg 對話方塊
class CChatRoomDlg : public CDialogEx
{
// 建構
public:
	CChatRoomDlg(CWnd* pParent = nullptr);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATROOM_DIALOG };
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
	CString m_Display;
	CString m_Msg;
	afx_msg void OnBnClickedButton1();
	CEdit m_Display2;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	CString m_IP;
	afx_msg void OnBnClickedButton2();
};
