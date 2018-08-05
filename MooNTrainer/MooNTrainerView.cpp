
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
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMooNTrainerView

IMPLEMENT_DYNCREATE(CMooNTrainerView, CView)

BEGIN_MESSAGE_MAP(CMooNTrainerView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CMooNTrainerView construction/destruction

static UINT ThreadInitDBLayer(LPVOID lpParam)
{
	CMooNTrainerView* pClass = (CMooNTrainerView*)lpParam;
	pClass->ProcInitDBLayer();
	return 0L;
}




CMooNTrainerView* pView = NULL;
CMooNTrainerView::CMooNTrainerView()
{
	pView = this;
	// TODO: add construction code here
	m_imgMove = CPoint(0, 0);
	m_mousedown = CPoint(0, 0);


	m_leftTop = CPoint(0, 0);
	m_widthStep = _UNIT_RESOLOTION_W;

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
	//CMooNTrainerDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
	//if (!pDoc)
	//	return;

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

//CMooNTrainerDoc* CMooNTrainerView::GetDocument() const // non-debug version is inline
//{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMooNTrainerDoc)));
//	return (CMooNTrainerDoc*)m_pDocument;
//}
#endif //_DEBUG


// CMooNTrainerView message handlers


void CMooNTrainerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class


//	cv::Mat img = cv::imread("D:\\01_Project2018\\FGS_DB\\qisha canon\\class00_16.jp2");
//	cv::imshow("test", img);
////	cv::imwrite("D:\\01_Project2018\\FGS_DB\\qisha canon\\class00_16.jp2", img);


	
}

void CMooNTrainerView::LoadDBLayer(CString strPath)
{
	m_strClassPath = strPath;
	CWinThread* pl;
	pl = AfxBeginThread(ThreadInitDBLayer, this);
}

void CMooNTrainerView::ProcInitDBLayer()
{
	BeginWaitCursor();
	m_dbLayer.Init(m_strClassPath);
	m_dbLayerImg = m_dbLayer.GetMasterImage(0);
	EndWaitCursor();

	InvalidateRect(FALSE);
}

void CMooNTrainerView::DrawCvMat(CDC* pDC, cv::Mat& origin, CRect rect)
{
	CImage mfcImg;
	cv::Mat outImg;
	cv::flip(origin, outImg, 0);
	//흑백이면 채널을3으로
	if (outImg.channels() == 1) {
		cv::cvtColor(outImg, outImg, CV_GRAY2BGR);
	}
	//이미지의 크기를 rect에 들어가게 수정합니다.
	if (outImg.cols != rect.Width() || outImg.rows != rect.Height()) {
		int newWidth = outImg.cols*(rect.Height() / (double)outImg.rows);
		int newHeight = rect.Height();
		if (newWidth > rect.Width()) {
			newWidth = rect.Width();
			newHeight = outImg.rows*(rect.Width() / (double)outImg.cols);
		}
		cv::resize(outImg, outImg, cv::Size(newWidth, newHeight), 0, 0, CV_INTER_NN);
	}
	mfcImg.Create(outImg.cols, outImg.rows, 24);
	BITMAPINFO bitInfo = { { sizeof(BITMAPINFOHEADER),outImg.cols,outImg.rows,1,24 },0 };

	void* vp = outImg.data;
	IplImage* iplimage = nullptr;
	if (outImg.cols % 4 != 0 && outImg.isContinuous()) {
		iplimage = cvCreateImage(outImg.size(), 8, outImg.channels());
		outImg.copyTo(cv::cvarrToMat(iplimage));
		vp = iplimage->imageData;
	}
	pDC->SetStretchBltMode(HALFTONE);
	StretchDIBits(mfcImg.GetDC(), 0, 0, outImg.cols, outImg.rows, 0, 0, outImg.cols, outImg.rows, vp, &bitInfo, DIB_RGB_COLORS, SRCCOPY);
	if (iplimage != nullptr) {
		cvReleaseImage(&iplimage);
	}



	//m_leftTop.x = (rect.Width() - outImg.cols) / 2 + rect.left;
	//m_leftTop.y = (rect.Height() - outImg.rows) / 2 + rect.top;
	m_leftTop.x = 0;
	m_leftTop.y = 0;
	m_widthStep = (float)_UNIT_RESOLOTION_W * ((float)rect.Width() / (float)outImg.cols);


	mfcImg.BitBlt(*pDC, m_leftTop.x, m_leftTop.y);
	mfcImg.ReleaseDC();
	outImg.release();
}

void CMooNTrainerView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CView::OnPaint() for painting messages


	Render();
}

void CMooNTrainerView::Render()
{
	CDC* pDC = GetDC();
	HDC hDC = pDC->GetSafeHdc();
	//CreateCompatibleDC(hDC);
	//m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());

	CRect rect;
	this->GetClientRect(&rect);
	//rect.SetRect(-m_imgMove.x, -m_imgMove.y, m_dbLayerImg.cols - m_imgMove.x, m_dbLayerImg.rows - m_imgMove.y);	
	if(m_dbLayerImg.ptr() != nullptr)
		DrawCvMat(pDC, m_dbLayerImg, rect);
}

void CMooNTrainerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetCapture();
	m_mousedown = point;

	CView::OnLButtonDown(nFlags, point);
}


void CMooNTrainerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
}


void CMooNTrainerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetCapture()) {
		m_imgMove.x += (m_mousedown.x - point.x);
		m_imgMove.y += (m_mousedown.y - point.y);
		m_mousedown = point;
		
	//	InvalidateRect(FALSE);
	}

	CView::OnMouseMove(nFlags, point);
}


BOOL CMooNTrainerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return CView::OnEraseBkgnd(pDC);
//	return TRUE;
}


void CMooNTrainerView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 100) {		
		
	}

	CView::OnTimer(nIDEvent);
}


void CMooNTrainerView::ValidateDBLayer()
{
	pMain->ResetListCtrl();

	_stMatcResTop5 res;

	clock_t begin = clock();
	int matchcnt = 0;
	int num = m_dbLayer.GetTotalCodeNum(0);
	int start = 0;
	int totalnum = 0;
	int dberrcnt = 0;
	CString strLog;
	for (int i = start; i < start + num; i++) {

		wchar_t strgt;
		cv::Mat cut = m_dbLayer.GetCutImagebyWorkPos(0, i, strgt);
		cv::Mat cutsrc = cut.clone();

		//cv::bitwise_not(cut, cut);
		//cut = deskew(cut);
		//cut = FitBoundingBox(cut);
		//cv::bitwise_not(cut, cut);

		if (strgt < 1) {
			dberrcnt++;
			continue;
		}

		totalnum++;
		float faccur = 0.0f;
		wchar_t strcode;
		_recognitionResult topRes = m_dbLayer.GetMatchResult(0, cutsrc, res);
		//if (res.accur[0] > 0.7f) {  // if the result of deep matching is less than master image's results, take master matching result.
		//	topRes.accur = res.accur[0];
		//	topRes.code = res.code[0];
		//}


		bool ismatch = false;
		if (topRes.code == strgt) {
			ismatch = true;
			matchcnt++;
		}

		//for (int i = 0; i < _MAX_CANDIDATE; i++) {
		//	if (strgt == res.code[i]) {
		//		matchcnt++;
		//		ismatch = true;
		//		break;
		//	}
		//}

		if (ismatch == false) {
			strLog.Format(L"%d: Not matched... %s-%s - %3.2f\n", i, (CString)strgt, (CString)topRes.code, topRes.accur);
			pMain->AddOutputString(strLog, false);
			pMain->AddRecord(cut, strgt, topRes.code, topRes.accur);
			
			//for (int j = 0; j < _MAX_CANDIDATE; j++) {
			//	strLog.Format(L"%d: Top-10... %s-%s - %3.2f\n", j, (CString)strgt, (CString)res.code[j], res.accur[j]);
			//	pMain->AddOutputString(strLog, false);
			//}

			//	cv::imshow("cut", cut);			
			//CString strFile;
			//strFile.Format(L"%s/errors/%d.jpg", m_strClassPath, i);

			//USES_CONVERSION;
			//char* sz = 0;
			//sz = T2A(strFile);
			//cv::imwrite(sz, cut);

			//strFile.Format(L"%s/errors/%d_a.jpg",m_strClassPath, i);
			//sz = T2A(strFile);
			//cv::imwrite(sz, cutsrc);
		}
		else {
			pMain->AddRecord(cut, strgt, topRes.code, topRes.accur);
			//strLog.Format(L"%d:Recognition result: %s-%s - %3.2f\n", i, (CString)strgt, (CString)topRes.code, topRes.accur);
			//pMain->AddOutputString(strLog, false);
			//for (int j = 0; j < _MAX_CANDIDATE; j++) {
			//	strLog.Format(L"%d: Top-10... %s-%s - %3.2f\n", j, (CString)strgt, (CString)res.code[j], res.accur[j]);
			//	pMain->AddOutputString(strLog, false);
			//}
		}
	}

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	//strLog.Format(L"%d cut image(s) , Elapsed Time (GetMatchResultByPixel) : %3.2f\n", num, (float)elapsed_secs);


	float precision = (float)matchcnt*100.0f / (float)totalnum;
	//strLog.Format(L"Match precision %3.6f\n", precision);

	//	TRACE(L"Match Result %s(%s) - %3.2f\n", (CString)strgt, (CString)strcode, faccur);
	strLog.Format(L"Elapsed Time: %3.2f, Precison(Top-10): %3.2f, DB error: %d", (float)elapsed_secs, precision, dberrcnt);
	strLog += L"%";
	pMain->AddOutputString(strLog, false);

}