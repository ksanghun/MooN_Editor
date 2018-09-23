// DlgProbilityTest.cpp : implementation file
//

#include "stdafx.h"
#include "MooNTrainer.h"
#include "DlgProbilityTest.h"
#include "afxdialogex.h"
#include "MooNDataManager.h"


// CDlgProbilityTest dialog

IMPLEMENT_DYNAMIC(CDlgProbilityTest, CDialogEx)

CDlgProbilityTest::CDlgProbilityTest(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_editInput(_T(""))
{

}

CDlgProbilityTest::~CDlgProbilityTest()
{
}

void CDlgProbilityTest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INPUT, m_editInput);
	DDX_Control(pDX, IDC_LIST_RESULT, m_ctrlList);
}


BEGIN_MESSAGE_MAP(CDlgProbilityTest, CDialogEx)
	ON_BN_CLICKED(IDC_BN_FILE, &CDlgProbilityTest::OnBnClickedBnFile)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgProbilityTest::OnBnClickedButton2)
END_MESSAGE_MAP()


// CDlgProbilityTest message handlers


void CDlgProbilityTest::OnBnClickedBnFile()
{
	// TODO: Add your control notification handler code here
	static TCHAR BASED_CODE szFilter[] = _T("Text file(*.TXT, *.txt) | *.TXT;*.txt; |모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.txt"), _T("Text file"), OFN_HIDEREADONLY, szFilter);

	if (IDOK == dlg.DoModal()) {
		SINGLETON_DataMng::GetInstance()->LoadTextFileForTree(dlg.GetPathName());
		MessageBox(L"A Probability map has been generated.");
	}
}


void CDlgProbilityTest::OnBnClickedButton2()		// Predict Next Characters
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_editInput == L"") {
		MessageBox(L"Input a keyword");
		return;
	}
	if (m_editInput.GetLength() > 10) {
		MessageBox(L"Input a keyword less than 10.");
		return;
	}

	wchar_t* input = m_editInput.GetBuffer();
	std::vector<_stMooNProb> vecRes;
	SINGLETON_DataMng::GetInstance()->PredictNext(input, vecRes);

	for (auto i = 0; i < vecRes.size(); i++) {
		CString stroutput;
		CString strCode;
		if (vecRes[i].code == 32)
			strCode = L"space";
		else
			strCode = (wchar_t)vecRes[i].code;
		stroutput.Format(L"%s-%s(%3.2f)", input, strCode, vecRes[i].prob);
	}

}
