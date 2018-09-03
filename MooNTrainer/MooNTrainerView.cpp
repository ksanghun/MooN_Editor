
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

enum TIMEREVNT { _ENDTHREAD = 100 };
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
	ON_WM_MBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CMooNTrainerView construction/destruction

static UINT ThreadInitDBLayer(LPVOID lpParam)
{
	CMooNTrainerView* pClass = (CMooNTrainerView*)lpParam;
	pClass->ProcInitDBLayer();
	return 0L;
}

static UINT ThreadDBValidation(LPVOID lpParam)
{
	CMooNTrainerView* pClass = (CMooNTrainerView*)lpParam;
	pClass->ValidateDBLayer();
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
	m_dispScale = 1.0f;
	m_selCellId = 0;
	m_selRect = cv::Rect(0, 0, 0, 0);

	m_pViewImage = NULL;
}

CMooNTrainerView::~CMooNTrainerView()
{
	if (m_pViewImage) {
		delete m_pViewImage;
	}
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
//	ProcInitDBLayer();
}

void CMooNTrainerView::ProcInitDBLayer()
{
	BeginWaitCursor();
	m_dbLayer.Init(m_strClassPath);	
	EndWaitCursor();
//	InvalidateRect(FALSE);

	//m_dbLayerMasterImg = m_dbLayer.GetMasterImage(0);
	//m_pVecLayerInfo = m_dbLayer.GetMasterImageInfo(0);
	//m_dispImgId = 0;
	//if (m_pViewImage) {
	//	cv::cvtColor(m_dbLayerMasterImg, m_dbLayerMasterImg, CV_GRAY2RGB);
	//	m_pViewImage->SetMasterImage(m_dbLayerMasterImg);
	//}
}


void CMooNTrainerView::EndThread()
{
	SetTimer(_ENDTHREAD, 500, NULL);
}


void CMooNTrainerView::ProcValidate()
{
	CWinThread* pl;
	pl = AfxBeginThread(ThreadDBValidation, this);
}

void CMooNTrainerView::ChangeDisplayImage(int clsid, int id)
{
	m_dispImgId = id;
	//if (id == 0) {
	//	m_dbLayerImg = m_dbLayer.GetMasterImage(clsid);
	//}
	//else {
	//	m_dbLayerImg = m_dbLayer.GetLayerImageByID(clsid, id-1);
	//}
	
	m_selCellId = 0;
	m_selRect = cv::Rect(0, 0, 0, 0);

	

	
	Render();
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
	m_dispScale = (float)origin.cols / (float)outImg.cols;

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
	//CDC* pDC = GetDC();
	//HDC hDC = pDC->GetSafeHdc();
	////CreateCompatibleDC(hDC);
	////m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());

	//CRect rect;
	//this->GetClientRect(&rect);
	////rect.SetRect(-m_imgMove.x, -m_imgMove.y, m_dbLayerImg.cols - m_imgMove.x, m_dbLayerImg.rows - m_imgMove.y);	
	//if (m_dbLayerMasterImg.ptr() != nullptr) {
	//	cv::Mat dispImg = m_dbLayerMasterImg.clone();
	//	cv::cvtColor(dispImg, dispImg, CV_GRAY2BGR);
	//	cv::rectangle(dispImg, m_selRect, cv::Scalar(0, 255, 0), 2);

	//	rect.right = rect.left + rect.Width() - 240;
	//	DrawCvMat(pDC, dispImg, rect);
	//}
}

void CMooNTrainerView::SelectCellMaster(CPoint point, bool IsAddList)
{
	int imgPosx = point.x * m_dispScale;
	int imgPosy = point.y * m_dispScale;

	if ((imgPosx < m_dbLayerMasterImg.cols) && (imgPosy < m_dbLayerMasterImg.rows)) {

		_stLayerImgInfo info = m_dbLayer.GetLayerImgInfo(0);
		int xid = imgPosx / info.unitRes;
		int yid = imgPosy / info.unitRes;

		m_selCellId = yid*info.wnum + xid;
		m_selRect.x = xid*info.unitRes;
		m_selRect.y = yid*info.unitRes;
		m_selRect.width = info.unitRes;
		m_selRect.height = info.unitRes;
		Render();

		CString strLog;
		//strLog.Format(L"Mouse Down: %d: %d, %d", m_selCellId, (int)(point.x*m_dispScale), (int)(point.y*m_dispScale));
		//pMain->AddOutputString(strLog, false);

//		if (m_dispImgId == 0) {  // In case of Master Image
			if (m_selCellId < m_pVecLayerInfo->size()) {
				wchar_t strcode = m_pVecLayerInfo->at(m_selCellId).strcode;
				unsigned short uCode = strcode;
				int trainNum = m_pVecLayerInfo->at(m_selCellId).vecPositionId.size();
				strLog.Format(L"Code: %d (%s) \nTrained number: %d", uCode, CString(strcode), trainNum);
				//pMain->AddOutputString(strLog, false);
				cv::Mat cutimg = m_dbLayerMasterImg(m_selRect).clone();
				//	cv::imshow("cut", cutimg);
				pMain->SetPreviewImg(cutimg, strLog, 0);
				
				if (m_pViewImage) {
				//	m_pViewImage->SetMasterImageSelection(m_selRect);
					m_pViewImage->SetMasterImageSelection(m_selCellId, _C1_WNUM, _C1_HNUM);
				}

				if (IsAddList) {
					pMain->ResetListCtrl(0);
					for (int k = 0; k < m_pVecLayerInfo->at(m_selCellId).vecPositionId.size(); k++) {
						int charid = m_pVecLayerInfo->at(m_selCellId).vecPositionId[k];
						wchar_t strgt;
						cv::Mat cut = m_dbLayer.GetCutImagebyWorkPos(0, charid, strgt);
						pMain->AddRecord(cut, strgt, '-', 0, m_selCellId, charid, 0);
					}
				}
			}
		//}
		//else {
		//	cv::Mat cutimg = m_dbLayerMasterImg(m_selRect).clone();
		//	pMain->SetPreviewImg(cutimg, strLog);
		//}
	}
}

void CMooNTrainerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetCapture();
	m_mousedown = point;

//	SelectCell(point, true);
	

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
		//m_imgMove.x += (m_mousedown.x - point.x);
		//m_imgMove.y += (m_mousedown.y - point.y);
		//m_mousedown = point;
		
	//	InvalidateRect(FALSE);
	}

	CView::OnMouseMove(nFlags, point);
}


BOOL CMooNTrainerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
//	return CView::OnEraseBkgnd(pDC);
//	return TRUE;
	return 0;
}


void CMooNTrainerView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == _ENDTHREAD) {		
		m_dbLayerMasterImg = m_dbLayer.GetMasterImage(0);
		m_pVecLayerInfo = m_dbLayer.GetMasterImageInfo(0);
		m_dispImgId = 0;
		if (m_pViewImage) {
			cv::cvtColor(m_dbLayerMasterImg, m_dbLayerMasterImg, CV_GRAY2RGB);
			m_pViewImage->SetMasterImage(m_dbLayerMasterImg);

			// Add Sub Layer //
			for (int i = 0; i < m_dbLayer.GetClassImgNum(0); i++) {
				cv::Mat img = m_dbLayer.GetLayerImageByID(0, i);
				cv::cvtColor(img, img, CV_GRAY2RGB);
				m_pViewImage->SetSubLayer(img, i/3, i%3);
				img.release();
			}
		}

		KillTimer(_ENDTHREAD);
	}

	CView::OnTimer(nIDEvent);
}


void CMooNTrainerView::ValidateDBLayer()
{
	pMain->ResetListCtrl(1);

	_stMatcResTop5 res;

	clock_t begin = clock();
	int matchcnt = 0;
	int num = m_dbLayer.GetTotalCodeNum(0);
	int start = 0;
	int totalnum = 0;
	int dberrcnt = 0;
	CString strLog;


	strLog = L"Start DB Validation...";
	pMain->AddOutputString(strLog, false);


	CString strLog2[3];
	strLog2[0] = L"Processing.		";
	strLog2[1] = L"Processing..		";
	strLog2[2] = L"Processing...	";
	int logidx = 0;

	for (int i = start; i < start + num; i++) {

		wchar_t strgt;
		cv::Mat cut = m_dbLayer.GetCutImagebyWorkPos(0, i, strgt);
		cv::Mat cutsrc = cut.clone();

		m_pViewImage->SetSubImageSelection(i, _C1_WNUM, _C1_HNUM);

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
		m_pViewImage->SetMasterImageSelection(topRes.firstlayerIdx, _C1_WNUM, _C1_HNUM);

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
			pMain->AddRecord(cut, strgt, topRes.code, topRes.accur, topRes.firstlayerIdx, i, 1);
			
			strLog.Format(L"Processing...	(%d/%d)", i, num - 1);
			pMain->AddOutputString(strLog, false);

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
			//pMain->AddRecord(cut, strgt, topRes.code, topRes.accur);
			//strLog.Format(L"%d:Recognition result: %s-%s - %3.2f\n", i, (CString)strgt, (CString)topRes.code, topRes.accur);
			//pMain->AddOutputString(strLog, false);
			//for (int j = 0; j < _MAX_CANDIDATE; j++) {
			//	strLog.Format(L"%d: Top-10... %s-%s - %3.2f\n", j, (CString)strgt, (CString)res.code[j], res.accur[j]);
			//	pMain->AddOutputString(strLog, false);
			//}

			if (i % 10 == 0) {
				strLog.Format(L"%s	(%d/%d)", strLog2[logidx], i, num - 1);
				pMain->AddOutputString(strLog, true);
				logidx++;
				if (logidx > 2) logidx = 0;

			}
		}



		

				
		

	}

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	//strLog.Format(L"%d cut image(s) , Elapsed Time (GetMatchResultByPixel) : %3.2f\n", num, (float)elapsed_secs);


	float precision = (float)matchcnt*100.0f / (float)totalnum;
	//strLog.Format(L"Match precision %3.6f\n", precision);

	//	TRACE(L"Match Result %s(%s) - %3.2f\n", (CString)strgt, (CString)strcode, faccur);
	strLog.Format(L"End DB Validation, Elapsed Time: %3.2f, Precison(Top-10): %3.2f, DB error: %d", (float)elapsed_secs, precision, dberrcnt);
	strLog += L"%";
	pMain->AddOutputString(strLog, false);



	int mismatch = totalnum - matchcnt;
	strLog.Format(L"***Validation result***\nMismatched: %d \nPrecison: %3.2f%s (%d/%d)", mismatch, precision, L"%", mismatch, totalnum);
	cv::Mat img;
	pMain->SetPreviewImg(img, strLog, 1);


	m_pViewImage->ReleaseSelections();
}


void CMooNTrainerView::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnMButtonDblClk(nFlags, point);
}


void CMooNTrainerView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//m_mousedown = point;
	//SelectCell(point, true);

	CView::OnLButtonDblClk(nFlags, point);
}


int CMooNTrainerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect cRect;
	GetWindowRect(&cRect);
	if (m_pViewImage == NULL) {
		m_pViewImage = new CGLVIEW;
		//	m_pImageView->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, this, 0x01);
		m_pViewImage->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, cRect, this, 0x01);
		
		//cv::Mat bgimg = cv::imread("./img/bg.png");
		//cv::resize(bgimg, bgimg, cv::Size(1024, 1024));		
		//cv::cvtColor(bgimg, bgimg, CV_BGR2RGB);

		m_pViewImage->InitGLview(0, 0);
		//bgimg.release();

	}

	return 0;
}


void CMooNTrainerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_pViewImage) {
		m_pViewImage->MoveWindow(0, 0, cx, cy);
	}
}


BOOL CMooNTrainerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pViewImage) {
		m_pViewImage->MouseWheel(zDelta);
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMooNTrainerView::UpdateDBCode(int clsid, int id, wchar_t code)
{
	m_dbLayer.UpdateDBCode(clsid, id, code);
}

void CMooNTrainerView::SaveUserChanges(int clsid)
{
	m_dbLayer.SaveUserChanges(clsid);
}