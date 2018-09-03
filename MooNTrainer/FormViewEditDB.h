#pragma once
#include "afxcmn.h"
#include "ZListCtrlLog.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "afxwin.h"

// CFormViewEditDB form view
#define _LIST_ICON_SIZE 32

struct _stListItemForEditing {
	int mid;
	int wid;
	wchar_t code;
};

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

	CZListCtrlLog m_listCtrlVali;
	CImageList m_imgListVali;
	int m_nRecordNumVali;
	int m_imgListIdVali;



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnBnClickedBnDbValidation();	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();



	void ResetLogList(int listid);
	void AddRecord(cv::Mat& srcImg, wchar_t strTrained, wchar_t strRecognized, float fAccuracy, int _masterId, int _imgWid, int _listType);
	CBitmap* GetLogCBitmap(cv::Mat& pimg);
	void SetPreviewImg(cv::Mat& pimg, CString strInfo, int _type);
	void SetLayerImgCnt(int clsid, int imgnum);
	void DeleteSelListItem();

	CString m_staticPreviewInfo;
	CTabCtrl m_ctrlTab;
	afx_msg void OnTcnSelchangeTabList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
};


