// FormViewEditDB.cpp : implementation file
//

#include "stdafx.h"
#include "MooNTrainer.h"
#include "FormViewEditDB.h"
#include "MooNTrainerView.h"


// CFormViewEditDB

IMPLEMENT_DYNCREATE(CFormViewEditDB, CFormView)

CFormViewEditDB::CFormViewEditDB()
	: CFormView(IDD_FORMVIEWEDITDB)
	, m_staticPreviewInfo(_T(""))
{
	m_bIsCreated = false;
}

CFormViewEditDB::~CFormViewEditDB()
{
}

void CFormViewEditDB::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESULT, m_listCtrl);
	DDX_Text(pDX, IDC_STATIC_INFO, m_staticPreviewInfo);
}

BEGIN_MESSAGE_MAP(CFormViewEditDB, CFormView)

	ON_WM_SIZE()
END_MESSAGE_MAP()


// CFormViewEditDB diagnostics

#ifdef _DEBUG
void CFormViewEditDB::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormViewEditDB::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormViewEditDB message handlers


BOOL CFormViewEditDB::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class


	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CFormViewEditDB::OnBnClickedBnDbValidation()
{
	// TODO: Add your control notification handler code here
	pView->ProcValidate();
}



void CFormViewEditDB::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_bIsCreated) {
//		m_listCtrl.MoveWindow(10, 120, cx - 220, cy - 120);
		m_listCtrl.MoveWindow(10, 120, cx - 10, cy - 120);
	}
}


void CFormViewEditDB::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CRect r;
	GetClientRect(&r);

	m_bIsCreated = true;
	int w = r.right - r.left;
	int h = r.bottom - r.top;
//	m_listCtrl.MoveWindow(10, 120, w-220, h-120);
	m_listCtrl.MoveWindow(10, 120, w - 10, h - 120);
	

	m_listCtrl.EnableScrollBarCtrl(SB_HORZ);
	m_listCtrl.ShowScrollBar(SB_HORZ);
	m_listCtrl.SetScrollRange(SB_HORZ, 0, 2000);
	m_listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	m_listCtrl.InitListCtrl();
	m_listCtrl.AddUserColumn(L"Cut Image", _LIST_ICON_SIZE+5);
	m_listCtrl.AddUserColumn(L"Trained", 70);
	m_listCtrl.AddUserColumn(L"Recognized", 70);
	m_listCtrl.AddUserColumn(L"Accuracy", 70);
	m_listCtrl.AddUserColumn(L"ID1", 0);
	m_listCtrl.AddUserColumn(L"ID2", 0);


	UINT nFlags = ILC_MASK;
	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;
	m_imgList.Create(_LIST_ICON_SIZE, _LIST_ICON_SIZE, nFlags, 0, 0);
	m_listCtrl.SetImageList(&m_imgList, LVSIL_SMALL);

	m_nRecordNum = 0;
	m_imgListId = 0;
}

void CFormViewEditDB::ResetLogList()
{
	while (m_imgList.GetImageCount()) 
		m_imgList.Remove(0);

	m_listCtrl.ResetListCtrl();
	m_nRecordNum = 0;
	m_imgListId = 0;
}

void CFormViewEditDB::SetLayerImgCnt(int clsid, int imgnum)
{
	//CString strFile;
	//m_comboImgList.Clear();

	//strFile.Format(L"Trained Layer", clsid);
	//m_comboImgList.AddString(strFile);
	//for (int i = 0; i < imgnum; i++) {
	//	strFile.Format(L"Class_%02d_%02d Sublayer", clsid, i);
	//	m_comboImgList.AddString(strFile);
	//}
	//m_comboImgList.SetCurSel(0);
//	UpdateData(FALSE);
}

void CFormViewEditDB::SetPreviewImg(cv::Mat& pimg, CString strInfo)
{
	IplImage* pImg = new IplImage(pimg);

	if (pImg != nullptr) {
		CWnd* pWnd_ImageTraget = GetDlgItem(IDC_STATIC_PREVIEW);
		CClientDC dcImageTraget(pWnd_ImageTraget);
		RECT rcImageTraget;
		pWnd_ImageTraget->GetClientRect(&rcImageTraget);

		BITMAPINFO bitmapInfo;
		memset(&bitmapInfo, 0, sizeof(bitmapInfo));
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;
		bitmapInfo.bmiHeader.biWidth = pImg->width;
		bitmapInfo.bmiHeader.biHeight = -pImg->height;

		IplImage *tempImage = nullptr;

		if (pImg->nChannels == 1)
		{
			tempImage = cvCreateImage(cvSize(pImg->width, pImg->height), IPL_DEPTH_8U, 3);
			cvCvtColor(pImg, tempImage, CV_GRAY2BGR);
		}
		else if (pImg->nChannels == 3)
		{
			tempImage = cvCloneImage(pImg);
		}

		bitmapInfo.bmiHeader.biBitCount = tempImage->depth * tempImage->nChannels;

		dcImageTraget.SetStretchBltMode(COLORONCOLOR);
		::StretchDIBits(dcImageTraget.GetSafeHdc(), rcImageTraget.left, rcImageTraget.top, rcImageTraget.right, rcImageTraget.bottom,
			0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);



		m_staticPreviewInfo = strInfo;
		UpdateData(FALSE);

	}
}

CBitmap* CFormViewEditDB::GetLogCBitmap(cv::Mat& pimg)
{
	if (pimg.ptr() != NULL) {

		CDC dc;
		CDC memDC;

		CBitmap* bmp = new CBitmap;
		CBitmap* pOldBmp;

		if (!dc.CreateDC(_T("DISPLAY"), NULL, NULL, NULL))
			return NULL;

		if (!memDC.CreateCompatibleDC(&dc))
			return NULL;

		int w, h;
		int nWidth = pimg.cols;
		int nHeight = pimg.rows;
		BYTE* pSrcBits = (BYTE *)pimg.ptr();
		BYTE* pBmpBits = (BYTE *)malloc(sizeof(BYTE)*nWidth*nHeight * 4);

		// IplImage에 저장된 값을 직접 읽어서 
		// 비트맵 데이터를 만듬 
		for (h = 0; h < nHeight; ++h)
		{
			BYTE* pSrc = pSrcBits + pimg.step.p[0] * h;
			BYTE* pDst = pBmpBits + nWidth * 4 * h;
			for (w = 0; w < nWidth; ++w)
			{
				if (pimg.channels() == 1) {
					*(pDst++) = *(pSrc);
					*(pDst++) = *(pSrc);
					*(pDst++) = *(pSrc++);
					*(pDst++) = 0;
				}
				else if (pimg.channels() == 3) {
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = 0;
				}
				else if (pimg.channels() == 4) {
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);
					*(pDst++) = *(pSrc++);;
				}
			}
		}
		//		memDC.CreateCompatibleDC(pDC);
		bmp->CreateCompatibleBitmap(&dc, nWidth, nHeight);
		// 위에서 만들어진 데이터를 가지고 
		// 비트맵을 만듬 
		bmp->SetBitmapBits(nWidth*nHeight * 4, pBmpBits);
		pOldBmp = memDC.SelectObject(bmp);


		memDC.SelectObject(pOldBmp);
		memDC.DeleteDC();
		dc.DeleteDC();

		free(pBmpBits);

		return bmp;
	}
	else {
		return NULL;
	}
}


void CFormViewEditDB::AddRecord(cv::Mat& srcImg, wchar_t strTrained, wchar_t strRecognized, float fAccuracy, int _id1, int _id2)
{
	//	ResetLogList();	
	CString strItem;
	CBitmap* pbmp = GetLogCBitmap(srcImg);

	if (pbmp != NULL) {
		BITMAP bmpObj;
		pbmp->GetBitmap(&bmpObj);
		m_imgList.Add(pbmp, RGB(255, 0, 0));
		delete pbmp;

		// Add resutl information //
		m_listCtrl.InsertItem(m_nRecordNum, L"", m_imgListId);

		m_listCtrl.SetItem(m_nRecordNum, 1, LVIF_TEXT, (CString)strTrained, m_imgListId, 0, 0, NULL);	// CODE //

		m_listCtrl.SetItem(m_nRecordNum, 2, LVIF_TEXT, (CString)strRecognized, m_imgListId, 0, 0, NULL);	// CODE //

		strItem.Format(L"%3.2f", fAccuracy*100);
		m_listCtrl.SetItem(m_nRecordNum, 3, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

		strItem.Format(L"%d", _id1);
		m_listCtrl.SetItem(m_nRecordNum, 4, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

		strItem.Format(L"%d", _id2);
		m_listCtrl.SetItem(m_nRecordNum, 5, LVIF_TEXT, strItem, m_imgListId, 0, 0, NULL);

		m_nRecordNum++;
		m_imgListId++;
	}
}

//void CFormViewEditDB::OnCbnSelchangeComboImglist()
//{
//	// TODO: Add your control notification handler code here
//	UpdateData(TRUE);
//	int id = m_comboImgList.GetCurSel();
//	pView->ChangeDisplayImage(0, id);
//}
