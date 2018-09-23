#pragma once
#include "afxcmn.h"


// CDlgProbilityTest dialog

class CDlgProbilityTest : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProbilityTest)

public:
	CDlgProbilityTest(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProbilityTest();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnFile();
	afx_msg void OnBnClickedButton2();
	CString m_editInput;
	CListCtrl m_ctrlList;
};
