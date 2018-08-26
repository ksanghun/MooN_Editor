#pragma once

#include "MNDBLayerImage.h"




typedef struct _stLayerImgInfo{
	int wnum, hnum;
	int unitRes;
	int codelen;
}_stLayerImgInfo;


class CMNDBLayer
{
public:
	CMNDBLayer();
	~CMNDBLayer();

	void Init(CString strDbPath);

	cv::Mat GetCutImagebyWorkPos(int clsId, int pos, wchar_t& strcode);
	_recognitionResult GetMatchResult(int clsid, cv::Mat& cutImg, _stMatcResTop5& res);
	int GetClassImgNum(int clsid) { return m_layerImage[clsid].GetClassImgNum(); }
	cv::Mat GetMasterImage(int clsid) { return m_layerImage[clsid].GetMasterImage(); }
	cv::Mat GetLayerImageByID(int clsid, int id) { return m_layerImage[clsid].GetLayerImageByID(id); }
	cv::Mat FitBoundingBox(cv::Mat img);
	cv::Mat& GetClassImage(int clsid, int wordIdx);
	int GetTotalCodeNum(int clsid);
	std::vector<_stLayerInfo>* GetMasterImageInfo(int clsid); 
	_stLayerImgInfo GetLayerImgInfo(int clsid);

	void UpdateDBCode(int clsid, int id, wchar_t code);
private:

	CMNDBLayerImage m_layerImage[8];	
	int layerAddCnt;
	cv::Mat deskew(cv::Mat& img);
};

