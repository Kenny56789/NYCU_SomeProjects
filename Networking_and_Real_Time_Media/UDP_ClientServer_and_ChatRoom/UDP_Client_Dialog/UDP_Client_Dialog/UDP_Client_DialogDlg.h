
// UDP_Client_DialogDlg.h: 標頭檔
//

#pragma once


// CUDPClientDialogDlg 對話方塊
class CUDPClientDialogDlg : public CDialogEx
{
// 建構
public:
	CUDPClientDialogDlg(CWnd* pParent = nullptr);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UDP_CLIENT_DIALOG_DIALOG };
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
	afx_msg void OnBnClickedButton1();
	CString m_Msg;
	CString m_Display;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
