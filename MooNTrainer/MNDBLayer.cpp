#include "stdafx.h"
#include "MNDBLayer.h"
#include <ctime>
#include "MainFrm.h"
#include "MooNTrainerView.h"



CMNDBLayer::CMNDBLayer()
{
}


CMNDBLayer::~CMNDBLayer()
{
}

void CMNDBLayer::Init(CString strDbPath)
{
	clock_t begin = clock();
	
	int classNum = 1;  // For the first class now
	for (int i = 0; i < classNum; i++) {
		m_layerImage[0].InitLayer(0, _C1_WNUM, _C1_HNUM, _C1_CODE_LEN, _UNIT_RESOLOTION_W, _UNIT_RESOLOTION_W, strDbPath);
		m_layerImage[0].GenerateFirstLayer(1, 32);
	}

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	
	pMain->AddOutputString(L"Layer Images are loaded", false);
	pMain->AddOutputString(L"Master Image is generated", false);

	CString strLog;
	strLog.Format(L"Elapsed Time: %3.2f seconds\n", (float)elapsed_secs);
	pMain->AddOutputString(strLog, false);
	
	pView->EndThread();
}


cv::Mat CMNDBLayer::deskew(cv::Mat& img)
{
	int SZ = 20;
	float affineFlags = cv::WARP_INVERSE_MAP | cv::INTER_LINEAR;

	cv::Moments m = moments(img);
	if (abs(m.mu02) < 1e-2) {
		return img.clone();
	}

	float skew = m.mu11 / m.mu02;
	cv::Mat warpMat = (cv::Mat_<float>(2, 3) << 1, skew, -0.5*SZ*skew, 0, 1, 0);
	cv::Mat imgOut = cv::Mat::zeros(img.rows, img.cols, img.type());
	warpAffine(img, imgOut, warpMat, imgOut.size(), affineFlags);

	return imgOut;
}

cv::Mat CMNDBLayer::GetCutImagebyWorkPos(int clsId, int pos, wchar_t& strcode)
{
	return m_layerImage[clsId].GetCutImageByWordInx(pos, strcode);
}

_recognitionResult CMNDBLayer::GetMatchResult(int clsid, cv::Mat& cutImg, _stMatcResTop5& res)
{
//	cv::imshow("before", cutImg);
//	cv::imshow("after", cutImg);
	return m_layerImage[clsid].GetMatchResultByPixel(cutImg, res);
//	m_layerImage[clsid].GetMatchResultByHOG(cutImg, res);
}

cv::Mat CMNDBLayer::FitBoundingBox(cv::Mat img)
{
	std::vector<std::vector<cv::Point> > contours_poly;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	contours_poly.resize(contours.size());

	cv::Rect rect(0,0,0,0);
	for (int i = 0; i < contours.size(); i++) {
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
		cv::Rect r = cv::boundingRect(cv::Mat(contours_poly[i]));
		if (r.area() > 4) {
			rect = rect | r;
		}
	}
	threshold(img, img, 100, 255, cv::THRESH_BINARY);
	cv::Mat res = cv::Mat(rect.width, rect.height, CV_8UC1, cv::Scalar(255));
	img(rect).copyTo(res);
	cv::resize(res, res, cv::Size(img.cols, img.rows));
	return res;
}

cv::Mat& CMNDBLayer::GetClassImage(int clsid, int wordIdx)
{
	return m_layerImage[clsid].GetLayerImageByID(wordIdx);
}

int CMNDBLayer::GetTotalCodeNum(int clsid)
{
	return m_layerImage[clsid].GetCodeNum();
}

std::vector<_stLayerInfo>* CMNDBLayer::GetMasterImageInfo(int clsid)
{ 
	return m_layerImage[clsid].GetFirstLayerInfo();
}

_stLayerImgInfo CMNDBLayer::GetLayerImgInfo(int clsid)
{
	_stLayerImgInfo info;
	info.wnum = _C1_WNUM;
	info.hnum = _C1_HNUM;
	info.unitRes = _UNIT_RESOLOTION_W;
	info.codelen = _C1_CODE_LEN;
	return info;
}

void CMNDBLayer::UpdateDBCode(int clsid, int id, wchar_t code)
{
	m_layerImage[clsid].UpdateDBCode(id, code);
}

void CMNDBLayer::SaveUserChanges(int clsid)
{
	m_layerImage[clsid].SaveUserChanges();
}