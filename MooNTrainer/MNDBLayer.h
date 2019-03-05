#pragma once

#include "MNDBLayerImage.h"

typedef struct _stLayerImgInfo {
	int wnum, hnum;
	int unitRes;
	int codelen;
}_stLayerImgInfo;

class CMNDBLayer
{
public:
	CMNDBLayer();
	~CMNDBLayer();

	void Init(CString strDbPath, bool IsNew);

	cv::Mat GetCutImagebyWorkPos(int clsId, int pos, wchar_t& strcode, int _layertype);
	_recognitionResult GetMatchResult(int clsid, cv::Mat& cutImg, _stMatcResTop5& res);
	int GetClassImgNum(int clsid) { return m_layerImage[clsid].GetClassImgNum(); }
	cv::Mat GetMasterImage(int clsid) { return m_layerImage[clsid].GetMasterImage(); }
	cv::Mat GetLayerImageByID(int clsid, int id) { return m_layerImage[clsid].GetLayerImageByID(id); }
	cv::Mat FitBoundingBox(cv::Mat img);
	cv::Mat& GetClassImage(int clsid, int wordIdx);

	void UpdateStrCode(int clsid, int wordIdx, wchar_t* _code);
	void AddNewTrainingData(int clsid, cv::Mat addImg, wchar_t* _code);
	void UpdateLayers(int clsid, CString strDbPath);
	_stLayerImgInfo CMNDBLayer::GetLayerImgInfo(int clsid);
	_stLayerImgInfo CMNDBLayer::GetLayerMasterInfo(int clsid);
	std::vector<_stLayerInfo>* GetLayerPositionInfo(int clsid);
	int GetTotalCodeNum(int clsid);

	void UpdateDBCode(int clsid, int id, wchar_t code);
	void SaveUserChanges(int clsid);
private:
	bool m_IsInitDB;
	CMNDBLayerImage m_layerImage[8];	
	int layerAddCnt;
	cv::Mat deskew(cv::Mat& img);
};

