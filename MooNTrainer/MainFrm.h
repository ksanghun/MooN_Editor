
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "OutputWnd.h"
#include "DockProperties.h"


class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	void AddOutputString(CString str, bool IsReplace);
// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	//CMFCMenuBar       m_wndMenuBar;
	//CMFCToolBar       m_wndToolBar;
	//CMFCStatusBar     m_wndStatusBar;
	//CMFCToolBarImages m_UserImages;
	//CFileView         m_wndFileView;
	//CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CDockProperties	  m_wndProperties;
	CString			  m_strInitPath;
	bool				m_bIsCreated;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	void InitUserData();
	void SaveUserData();
public:

	afx_msg void OnFileOpen();
	afx_msg void OnToolValiDb();

	void AddRecord(cv::Mat& srcImg, wchar_t strTrained, wchar_t strRecognized, float fAccuracy, int _id1, int _id2, int _listType);
	void ResetListCtrl(int listid);
	void SetPreviewImg(cv::Mat& pimg, CString strInfo, int _type);
	void SetLayerImgCnt(int clsid, int imgnum);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPopDelete();
	afx_msg void OnPopAugmentation();
	afx_msg void OnToolProbilitytest();
};


