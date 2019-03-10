#pragma once
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
//#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/photo/photo.hpp"

#include <vector>
#include <map>

#define _C1_WNUM 60
#define _C1_HNUM 60
#define _UNIT_RESOLOTION_W 32
#define _C1_CODE_LEN 4

//#define _FIRSTLAYER_WNUM 200
//#define _FIRSTLAYER_CELL 16
#define _FIRSTLAYER_WNUM 100
#define _FIRSTLAYER_CELL 32

typedef struct {
//	cv::Mat hogFeature;
	std::vector<unsigned short> vecCodes;
	std::vector<int> vecPositionId;
//	wchar_t strcode;
}_stLayerInfo;

#define _MAX_CANDIDATE 40
typedef struct {
	float accur[_MAX_CANDIDATE];
	wchar_t code[_MAX_CANDIDATE];
}_stMatcResTop5;

typedef struct {
	float accur;
	wchar_t code;
	int firstlayerIdx;
}_recognitionResult;

typedef struct {
	cv::Mat img;
	bool bNedUpdate;
	std::string strPath;
}_dbMat;

class CMNDBLayerImage
{
public:
	CMNDBLayerImage();
	~CMNDBLayerImage();

	void ReleaseLayerImage();
	void InitLayer(int clsId, unsigned short _wnum, unsigned short _hnum, unsigned short _codelen, int cellSizeW, int cellSizeH, CString strPath, bool IsNew);
	void GenerateFirstLayerByCode(int minstrcode, int first_cellSize, int first_wnum);
	void GenerateFirstLayerByShape(int minstrcode, int first_cellSize, int first_wnum);
	cv::Mat GetCutImageByWordInx(int wordIdx, wchar_t& strcode, int _layertype);

	cv::Mat FitBoundingBox(cv::Mat img);
	_recognitionResult GetMatchResultByPixel(cv::Mat& cutImg, _stMatcResTop5& res);
//	void GetMatchResultByHOG(cv::Mat cutImg, _stMatcResTop5& res);
	int GetClassImgNum() { return (int)m_imageDb.size(); }
	cv::Mat& GetMasterImage() { return m_firstlayerImage; }
	cv::Mat& GetLayerImageByID(int id) { return m_imageDb[id].img; }

	void UpdateStrCode(int wordIdx, wchar_t* _code);
	void AddNewTrainingData(int clsid, cv::Mat addImg, wchar_t* _code);
	void UpdateLayer(CString strPath);

	_recognitionResult DeepMatching(cv::Mat cutImg, unsigned short charcode);
	_recognitionResult DeepMatching2(cv::Mat cutImg, int idx);
	int GetCodeNum() { return (int)m_vecStrCode.size(); }
	std::vector<_stLayerInfo>* GetLayerPositionInfo() { return &m_vecLayerInfo; }

	void UpdateDBCode(int id, wchar_t code);
	void SaveUserChanges();
	void FillNullCutImageByWordIdx(int wordIdx);
private:

	// For the first Layer //
	int m_firstLayerCellSize;
	int m_firstLayer_wNum;

	int m_cellResolution;
	int m_classId;
	int m_imgcnt;
	int m_hogResolution;
	int m_totalCodeNum;
	bool m_bNeedToSaveJp3;

	int m_cellSizeW, m_cellSizeH;
	unsigned short m_wnum, m_hnum, m_strcodeLen;	
//	std::vector<cv::Mat> m_imageDb;
	std::vector<_dbMat> m_imageDb;
	std::vector<wchar_t*> m_vecStrCode;
	bool m_bNeedToUpdateCode;
	CString m_strPatJp3;

	cv::Mat m_firstlayerImage;
	int m_firstlayerImageWordNum;
	std::vector<_stLayerInfo> m_vecLayerInfo;
	std::map<unsigned long, std::vector<unsigned long>> m_mapStrCode;

	
	void LoadJP3File(CString str);
	void WriteJP3File(CString str);

	void LoadLayerImageFile(CString str);	
	cv::Mat getWordCutImg(int wordId, int cellSize, cv::Mat& clsImg, int clsId, int wNum, int hNum);
	cv::Rect getPositionByIndex(int wordId, int cellSize, int clsId, int wNum, int hNum);
	void accumulateImg(cv::Mat& accur, cv::Mat& img);
	void averageImg(cv::Mat& accur, cv::Mat& img, int addcnt);
//	cv::Mat getHOGFeature(cv::HOGDescriptor d1, cv::Mat& img);
	float templateMatching(cv::Mat& src, cv::Mat& tmpl);
	float hog_matching(cv::Mat img1, cv::Mat img2);
	cv::Mat deskew(cv::Mat& img);
	void balance_white(cv::Mat& mat);
};

