#pragma once
#include "afxcmn.h"
#include "ZListCtrlLog.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"

// CFormViewEditDB form view
#define _LIST_ICON_SIZE 32

class CFormViewEditDB : public CFormView
{
	DECLARE_DYNCREATE(CFormViewEditDB)

protected:
public:
	CFormViewEditDB();           // protected constructor used by dynamic creation
	virtual ~CFormViewEditDB();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMVIEWEDITDB };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	bool m_bIsCreated;
	CZListCtrlLog m_listCtrl;
	CImageList m_imgList;
	int m_nRecordNum;
	int m_imgListId;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnBnClickedBnDbValidation();	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();



	void ResetLogList();
	void AddRecord(cv::Mat& srcImg, wchar_t strTrained, wchar_t strRecognized, float fAccuracy);
	CBitmap* GetLogCBitmap(cv::Mat& pimg);
};

