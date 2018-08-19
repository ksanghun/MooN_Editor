#pragma once

#include "FormViewEditDB.h"
//#include "data_type.h"
// CDockProperties

class CDockProperties : public CDockablePane
{
	DECLARE_DYNAMIC(CDockProperties)

private:
	CFormViewEditDB* m_pProperties;

public:
	CDockProperties();
	virtual ~CDockProperties();

	float GetThreshold();// { return m_pProperties->GetThreshold(); }
	COLORREF GetMatchingColor();// { return m_pProperties->GetMatchingColor(); }
	void SetParagraphInfo(float fskew, CString strName, bool IsVerti);
	void GetCurrSetting();

	void SetOCRResInfo(wchar_t* strCode, float fConfi, int lang);
	void SetEncodedFilePath(CString strPath);
	//void DoKeywordSearch() {		m_pProperties->DoKeywordSearch();	};
	//void RotateSelLinebox() {		m_pProperties->RotateSelLinebox();	 };

	void AddRecord(cv::Mat& srcImg, wchar_t strTrained, wchar_t strRecognized, float fAccuracy, int _id1, int _id2);
	void ResetListCtrl();
	void SetPreviewImg(cv::Mat& pimg, CString strInfo);
	void SetLayerImgCnt(int clsid, int imgnum);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


