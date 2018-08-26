
// MooNTrainerView.h : interface of the CMooNTrainerView class
//

#pragma once

#include "GLVIEW.h"
#include "MNDBLayer.h"

static UINT ThreadInitDBLayer(LPVOID lpParam);
static UINT ThreadDBValidation(LPVOID lpParam);


class CMooNTrainerView : public CView
{
protected: // create from serialization only
	CMooNTrainerView();
	DECLARE_DYNCREATE(CMooNTrainerView)

// Attributes
public:
//	CMooNTrainerDoc* GetDocument() const;

// Operations
	void EndThread();

	void ProcInitDBLayer();
	void LoadDBLayer(CString strPath);

	void ProcValidate();
	void ChangeDisplayImage(int clsid, int id);

public:

private:
	CMNDBLayer m_dbLayer;
	cv::Mat m_dbLayerMasterImg;
//	cv::Mat m_dbLayerImg;

	std::vector<_stLayerInfo>* m_pVecLayerInfo;

	CPoint m_imgMove;
	CPoint m_mousedown;

	CString m_strClassPath;

	CPoint m_leftTop;
	float m_widthStep;
	float m_dispScale;
	int m_selCellId;
	cv::Rect m_selRect;

	int m_dispImgId;

	CGLVIEW* m_pViewImage;

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CMooNTrainerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
//	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	void DrawCvMat(CDC* pDC, cv::Mat& origin, CRect rect);
	void Render();
	void ValidateDBLayer();
	void SelectCellMaster(CPoint point, bool IsAddList);
	void UpdateDBCode(int clsid, int id, wchar_t code);

	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // debug version in MooNTrainerView.cpp
//inline CMooNTrainerDoc* CMooNTrainerView::GetDocument() const
//   { return reinterpret_cast<CMooNTrainerDoc*>(m_pDocument); }
#endif

