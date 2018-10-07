#pragma once
#include "afxcmn.h"

#include "yolo_rnn_class.hpp"
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
	CString m_strTrainingText;
	afx_msg void OnBnClickedBnConvert();
	afx_msg void OnBnClickedBnTraining();


private:
	Detector* m_pPredictor;

	void convertUnicodeToAscii(wchar_t* buf, int len, unsigned char* dstBuff);
	void decompsiteWchar(wchar_t* src, unsigned char* dst, int wlen);
	void setRNNModel(CString strModel);
public:
	afx_msg void OnBnClickedBnLoadRnn();
	CString m_strRnnWeight;
	virtual BOOL OnInitDialog();
};
