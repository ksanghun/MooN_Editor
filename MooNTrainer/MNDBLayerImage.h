#pragma once

#include <vector>
#include <map>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"

typedef struct {
	cv::Mat hogFeature;
	std::vector<int> vecPositionId;
	wchar_t strcode;
}_stLayerInfo;

#define _MAX_CANDIDATE 5
typedef struct {
	float accur[_MAX_CANDIDATE];
	wchar_t code[_MAX_CANDIDATE];
}_stMatcResTop5;

typedef struct {
	float accur;
	wchar_t code;
}_recognitionResult;

typedef struct {
	cv::Mat img;
	bool bNedUpdate;
}_dbMat;


class CMNDBLayerImage
{
public:
	CMNDBLayerImage();
	~CMNDBLayerImage();

	void InitLayer(int clsId, unsigned short _wnum, unsigned short _hnum, unsigned short _codelen, int cellSizeW, int cellSizeH, CString strPath);
	void GenerateFirstLayer(int minstrcode, int resolution);
	cv::Mat GetCutImageByWordInx(int wordIdx, wchar_t& strcode);

	cv::Mat FitBoundingBox(cv::Mat img);
	_recognitionResult GetMatchResultByPixel(cv::Mat& cutImg, _stMatcResTop5& res);
	void GetMatchResultByHOG(cv::Mat cutImg, _stMatcResTop5& res);
	int GetClassImgNum() { (int)m_imageDb.size(); }
	cv::Mat& GetMasterImage() { return m_firstlayerImage; }
	cv::Mat& GetLayerImageByID(int id) { return m_imageDb[id].img; }

	_recognitionResult DeepMatching(cv::Mat cutImg, unsigned short charcode);
	int GetCodeNum() { return m_totalCodeNum; }
private:

	int m_classId;
	int m_imgcnt;
	int m_layerResolution;
	int m_hogResolution;
	int m_totalCodeNum;

	int m_cellSizeW, m_cellSizeH;
	unsigned short m_wnum, m_hnum, m_strcodeLen;	
	std::vector<_dbMat> m_imageDb;
	std::vector<wchar_t*> m_vecStrCode;
	

	cv::Mat m_firstlayerImage;
	int m_firstlayerImageWordNum;
	std::vector<_stLayerInfo> m_vecLayerInfo;
	std::map<unsigned long, std::vector<unsigned long>> m_mapStrCode;

	void ReleaseLayerImage();
	void LoadJP3File(CString str);
	void LoadLayerImageFile(CString str);	
	cv::Mat getWordCutImg(int wordId, int cellSize, cv::Mat& clsImg, int clsId, int wNum, int hNum);
	cv::Rect getPositionByIndex(int wordId, int cellSize, int clsId, int wNum, int hNum);
	void accumulateImg(cv::Mat& accur, cv::Mat& img);
	cv::Mat getHOGFeature(cv::HOGDescriptor d1, cv::Mat& img);
	float templateMatching(cv::Mat& src, cv::Mat& tmpl);
	float hog_matching(cv::Mat img1, cv::Mat img2);
	cv::Mat deskew(cv::Mat& img);
	void balance_white(cv::Mat& mat);
};

