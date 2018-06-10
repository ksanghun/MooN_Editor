
// MooNTrainerView.cpp : implementation of the CMooNTrainerView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MooNTrainer.h"
#endif

#include "MooNTrainerDoc.h"
#include "MooNTrainerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMooNTrainerView

IMPLEMENT_DYNCREATE(CMooNTrainerView, CView)

BEGIN_MESSAGE_MAP(CMooNTrainerView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CMooNTrainerView construction/destruction

CMooNTrainerView::CMooNTrainerView()
{
	// TODO: add construction code here

}

CMooNTrainerView::~CMooNTrainerView()
{
}

BOOL CMooNTrainerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMooNTrainerView drawing

void CMooNTrainerView::OnDraw(CDC* /*pDC*/)
{
	CMooNTrainerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void CMooNTrainerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMooNTrainerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMooNTrainerView diagnostics

#ifdef _DEBUG
void CMooNTrainerView::AssertValid() const
{
	CView::AssertValid();
}

void CMooNTrainerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMooNTrainerDoc* CMooNTrainerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMooNTrainerDoc)));
	return (CMooNTrainerDoc*)m_pDocument;
}
#endif //_DEBUG


// CMooNTrainerView message handlers
