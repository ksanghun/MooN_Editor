// DockProperties.cpp : implementation file
//

#include "stdafx.h"
//#include "MooN.h"
#include "DockProperties.h"


// CDockProperties

IMPLEMENT_DYNAMIC(CDockProperties, CDockablePane)

CDockProperties::CDockProperties()
{
	m_pProperties = NULL;
}

CDockProperties::~CDockProperties()
{
}


BEGIN_MESSAGE_MAP(CDockProperties, CDockablePane)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CDockProperties message handlers




void CDockProperties::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_pProperties)
		m_pProperties->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);

}


int CDockProperties::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_pProperties = new CFormViewEditDB;
	if (!m_pProperties->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 200, 200), this, 0, NULL))
	{
		AfxMessageBox(_T("Failed in creating CMyFormView"));
	}

	return 0;
}


float CDockProperties::GetThreshold() 
{ 
	//if (m_pProperties) {
	//	return m_pProperties->GetThreshold();
	//}
	//else {
	//	return 0;
	//}
	return 0;
}
COLORREF CDockProperties::GetMatchingColor() 
{ 
	//if (m_pProperties) {
	//	return m_pProperties->GetMatchingColor();
	//}
	//else {
	//	return RGB(0, 0, 0);
	//}
	return RGB(0, 0, 0);
}

void CDockProperties::GetCurrSetting()
{
//	m_pProperties->GetCurrSetting();
}

void CDockProperties::SetParagraphInfo(float fskew, CString strName, bool IsVerti)
{
	//if (m_pProperties) {
	//	m_pProperties->SetParagraphInfo(fskew, strName, IsVerti);
	//}
}

void CDockProperties::SetOCRResInfo(wchar_t* strCode, float fConfi, int lang)
{
	//if (m_pProperties) {
	//	m_pProperties->SetOCRInfo(strCode, fConfi, lang);
	//}
}

void CDockProperties::SetEncodedFilePath(CString strPath)
{
	//if (m_pProperties) {
	//	m_pProperties->SetEncodeFilePath(strPath);
	//}
}

void CDockProperties::AddRecord(cv::Mat& srcImg, wchar_t strTrained, wchar_t strRecognizede, float fAccuracy, int _id1, int _id2, int _listType)
{
	if (m_pProperties) {
		m_pProperties->AddRecord(srcImg, strTrained, strRecognizede, fAccuracy, _id1, _id2, _listType);
	}
}

void CDockProperties::ResetListCtrl(int listid)
{
	if (m_pProperties) {
		m_pProperties->ResetLogList(listid);
	}
}

void CDockProperties::SetPreviewImg(cv::Mat& pimg, CString strInfo, int _type)
{
	if (m_pProperties) {
		m_pProperties->SetPreviewImg(pimg, strInfo, _type);
	}
}

void CDockProperties::SetLayerImgCnt(int clsid, int imgnum)
{
	if (m_pProperties) {
		m_pProperties->SetLayerImgCnt(clsid, imgnum);
	}
}

void CDockProperties::DeleteSelListItem()
{
	if (m_pProperties) {
		m_pProperties->DeleteSelListItem();
	}
}