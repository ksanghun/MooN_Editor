#include "stdafx.h"
#include "MNDBLayerImage.h"
#include <ctime>
#include "MainFrm.h"

#define _GEN_DB_TH 0.4f

CMNDBLayerImage::CMNDBLayerImage()
{
	m_imgcnt = 0;
	m_classId = 0;
	m_cellSizeW = 0;
	m_cellSizeH = 0;
	m_hogResolution = 32;

	m_bNeedToUpdateCode = false;
}

CMNDBLayerImage::~CMNDBLayerImage()
{
	ReleaseLayerImage();
}

void CMNDBLayerImage::ReleaseLayerImage()
{
	for (auto i = 0; i<m_imageDb.size(); i++) {
		m_imageDb[i].img.release();
	}
	m_imageDb.swap(std::vector<_dbMat>());

	for (auto i = 0; i < m_vecStrCode.size(); i++) {
		delete[] m_vecStrCode[i];
	}
	m_vecStrCode.swap(std::vector<wchar_t*>());

	for (auto i = 0; i < m_vecLayerInfo.size(); i++) {
	//	m_vecLayerInfo[i].hogFeature.release();
	//	delete [] m_vecLayerInfo[i].strcode;
		m_vecLayerInfo[i].vecPositionId.clear();
	}
	m_vecLayerInfo.swap(std::vector<_stLayerInfo>());

	m_mapStrCode.clear();
}

void CMNDBLayerImage::InitLayer(int clsId, unsigned short _wnum, unsigned short _hnum, unsigned short _codelen, int cellSizeW, int cellSizeH, CString strPath, bool IsNew)
{
	ReleaseLayerImage();

	m_imgcnt = 0;
	m_classId = clsId;
	m_wnum = _wnum;
	m_hnum = _hnum;
	m_strcodeLen = _codelen;
	m_cellSizeW = cellSizeW;
	m_cellSizeH = cellSizeH;

	m_cellResolution = cellSizeW;
	// Load string info file //

	if (IsNew == false) {
		CString strJp3;
		strJp3.Format(L"%s/class%02d.jp3", strPath, m_classId);
		LoadJP3File(strJp3);
		pMain->AddOutputString(L"Load Jp3 file...done.", false);
		// load class images //
		LoadLayerImageFile(strPath);
	}
}

//cv::Mat CMNDBLayerImage::getHOGFeature(cv::HOGDescriptor d1, cv::Mat& img)
//{
//	//	cv::HOGDescriptor d1(cv::Size(resolution, resolution), cv::Size(8, 8), cv::Size(4, 4), cv::Size(4, 4), 9);
//	std::vector<float> descriptors1;
//	d1.compute(img, descriptors1);
//
//	//copy vector to mat  
//	cv::Mat A(descriptors1.size(), 1, CV_32FC1);
//	//copy vector to mat  
//	memcpy(A.data, descriptors1.data(), descriptors1.size() * sizeof(float));
//
//	return A;
//}

cv::Mat CMNDBLayerImage::getWordCutImg(int wordId, int cellSize, cv::Mat& clsImg, int clsId, int wNum, int hNum)
{
	cv::Rect r1 = getPositionByIndex(wordId, cellSize, clsId, wNum, hNum);
	cv::Mat img = cv::Mat(cellSize, cellSize, CV_8UC1, cv::Scalar(255));
	clsImg(r1).copyTo(img(cv::Rect(0, 0, cellSize, cellSize)));
	return img;
}

cv::Mat CMNDBLayerImage::GetCutImageByWordInx(int wordIdx, wchar_t& strcode, int _layertype)
{
	if (_layertype == 0) {
		int imgIdx = wordIdx / (_FIRSTLAYER_WNUM * _FIRSTLAYER_WNUM);
		strcode = m_vecStrCode[wordIdx][0];
		return getWordCutImg(wordIdx, _FIRSTLAYER_CELL, m_imageDb[imgIdx].img, m_classId, _FIRSTLAYER_WNUM, _FIRSTLAYER_WNUM);
	}
	else {
		int imgIdx = wordIdx / (m_wnum * m_hnum);
		strcode = m_vecStrCode[wordIdx][0];
		return getWordCutImg(wordIdx, m_cellSizeW, m_imageDb[imgIdx].img, m_classId, m_wnum, m_hnum);
	}
}

cv::Rect CMNDBLayerImage::getPositionByIndex(int wordId, int cellSize, int clsId, int wNum, int hNum)
{
	int wNumInClass = wNum*hNum;
	wordId = wordId - (wordId / wNumInClass)*wNumInClass;

	int w = (clsId + 1) * cellSize;
	int h = cellSize;

	cv::Rect r1;
	r1.x = (wordId % wNum)*w;
	r1.y = (wordId / wNum)*h;
	r1.width = cellSize;
	r1.height = cellSize;

	return r1;
}


void CMNDBLayerImage::accumulateImg(cv::Mat& accur, cv::Mat& img)
{
	for (int x = 0; x < img.cols; x++) {
		for (int y = 0; y < img.rows; y++) {
			if (img.at<unsigned char>(y, x) == 255) {
				accur.at<unsigned char>(y, x)++;
				if (accur.at<unsigned char>(y, x) > 254)
					accur.at<unsigned char>(y, x) = 254;
			}
		}
	}
}

void CMNDBLayerImage::averageImg(cv::Mat& accur, cv::Mat& img, int addcnt)
{
	for (int x = 0; x < img.cols; x++) {
		for (int y = 0; y < img.rows; y++) {
		//	int averPixel = (img.at<unsigned char>(y, x)*addcnt + accur.at<unsigned char>(y, x)) / (addcnt);
			int averPixel = (img.at<unsigned char>(y, x) + accur.at<unsigned char>(y, x));
			if (averPixel > 255) averPixel = 255;
			accur.at<unsigned char>(y, x) = averPixel;
		}
	}
}



void CMNDBLayerImage::GenerateFirstLayerByShape(int minstrcode, int first_cellSize, int first_wnum)
{
	clock_t begin = clock();

	m_mapStrCode.clear();
	//	m_cellResolution = resolution;
	m_firstLayerCellSize = first_cellSize;
	m_firstLayer_wNum = first_wnum;
	
	m_vecLayerInfo.swap(std::vector<_stLayerInfo>());
	m_firstlayerImage = cv::Mat(cv::Size(first_wnum * first_cellSize, first_wnum * first_cellSize), CV_8U, cv::Scalar(255));
	m_firstlayerImageWordNum = 0;
	for (auto i = 0; i < m_vecStrCode.size(); i++) {
		int imgIdx = i / (m_wnum * m_hnum);
		cv::Mat img = getWordCutImg(i, m_cellSizeW, m_imageDb[imgIdx].img, m_classId, m_wnum, m_hnum);
		cv::resize(img, img, cv::Size(first_cellSize, first_cellSize));
//		cv::bitwise_not(img, img);

		bool IsMatched = false;
		for (auto j = 0; j < m_vecLayerInfo.size(); j++) {
			cv::Mat cellimg = getWordCutImg(j, first_cellSize, m_firstlayerImage, 0, m_firstLayer_wNum, m_firstLayer_wNum);
//			cv::bitwise_not(cellimg, cellimg);

			// Templete Matching //
	//		float shapeSimilarity = templateMatching(img, cellimg);
			float shapeSimilarity = templateMatching(cellimg, img);
			if (shapeSimilarity > _GEN_DB_TH) {

				//cv::imshow("cellimg", cellimg);
				//cv::imshow("img", img);
		//		averageImg(cellimg, img, (int)m_vecLayerInfo[j].vecCodes.size());
//				averageImg(img, cellimg, (int)m_vecLayerInfo[j].vecCodes.size());
				//cv::imshow("accur", cellimg);

		//		cv::waitKey(10);
		//		cv::equalizeHist(cellimg, cellimg);
		//		cv::threshold(cellimg, cellimg, 220, 255, cv::THRESH_OTSU);
		//		cv::bitwise_not(cellimg, cellimg);		

				cv::Rect r1 = getPositionByIndex(j, first_cellSize, 0, first_wnum, first_wnum);
				//cv::bitwise_not(cellimg, cellimg);
				//cellimg.copyTo(m_firstlayerImage(r1));
	
				m_vecLayerInfo[j].vecCodes.push_back((unsigned short)m_vecStrCode[i][0]);
				m_vecLayerInfo[j].vecPositionId.push_back(i);
				IsMatched = true;
				break;
			}
		}
		
		// If not matched from first image //	
		if (IsMatched == false) {
		//	cv::equalizeHist(img, img);
		//	cv::threshold(img, img, 220, 255, cv::THRESH_OTSU);
		//	cv::bitwise_not(img, img);

			cv::Rect r1 = getPositionByIndex((int)m_vecLayerInfo.size(), first_cellSize, 0, first_wnum, first_wnum);
//			cv::bitwise_not(img, img);
			img.copyTo(m_firstlayerImage(r1));
			_stLayerInfo layerInfo;
			layerInfo.vecCodes.push_back((unsigned short)m_vecStrCode[i][0]);
			layerInfo.vecPositionId.push_back(i);
			m_vecLayerInfo.push_back(layerInfo);
			m_firstlayerImageWordNum++;
		}


		//cv::imshow("first layer", m_firstlayerImage);
		//cv::waitKey(1);

//		m_firstlayerImage = cv::Mat(cv::Size(first_wnum * first_cellSize, first_wnum * first_cellSize), CV_8U, cv::Scalar(255));
		//unsigned short w1 = (unsigned short)m_vecStrCode[i][0];
		//if ((w1 > minstrcode)) {
		//	m_mapStrCode[w1].push_back(i);
		//}
		//else {
		//	misscnt++;
		//}
	}








	//m_firstlayerImage = cv::Mat::zeros(cv::Size(first_wnum * first_cellSize, first_wnum * first_cellSize), CV_8U);
	//m_firstlayerImageWordNum = 0;
	//for (auto i = 0; i < m_vecLayerInfo.size(); i++) {
	//	cv::Mat accur = cv::Mat::zeros(cv::Size(m_cellSizeW, m_cellSizeH), CV_8U);
	//	for (auto j = 0; j < m_vecLayerInfo[i].vecPositionId.size(); j++) {

	//		int idx = m_vecLayerInfo[i].vecPositionId[j];
	//		int imgIdx = idx / (m_wnum * m_hnum);

	//		cv::Mat img = getWordCutImg(idx, m_cellSizeW, m_imageDb[imgIdx].img, m_classId, m_wnum, m_hnum);
	//		cv::resize(img, img, cv::Size(first_cellSize, first_cellSize));
	//		cv::bitwise_not(img, img);
	//		accumulateImg(accur, img);
	//	}

	//	int cnt = m_vecLayerInfo[i].vecPositionId.size();
	//	if (cnt > 255)		cnt = 255;
	//	int th = (cnt / 2);
	//	cv::equalizeHist(accur, accur);
	//	cv::threshold(accur, accur, 220, 255, cv::THRESH_OTSU);
	//	cv::bitwise_not(accur, accur);
	//	cv::Rect r1 = getPositionByIndex(m_firstlayerImageWordNum, first_cellSize, 0, first_wnum, first_wnum);
	//	accur.copyTo(m_firstlayerImage(r1));
	//	m_firstlayerImageWordNum++;

	//	cv::imshow("first layer", m_firstlayerImage);
	//	cv::waitKey(1);
	//}








	////	cv::HOGDescriptor d(cv::Size(m_hogResolution, m_hogResolution), cv::Size(8, 8), cv::Size(4, 4), cv::Size(4, 4), 9);
	//m_firstlayerImage.release();
	////	m_firstlayerImage = cv::Mat::zeros(cv::Size(m_wnum * m_cellSizeW, m_hnum * m_cellSizeH), CV_8U);
	//m_firstlayerImage = cv::Mat::zeros(cv::Size(first_wnum * first_cellSize, first_wnum * first_cellSize), CV_8U);
	//std::map<unsigned long, std::vector<unsigned long>>::iterator iter = m_mapStrCode.begin();

	//m_firstlayerImageWordNum = 0;
	//m_vecLayerInfo.swap(std::vector<_stLayerInfo>());
	//for (; iter != m_mapStrCode.end(); iter++) {
	//	cv::Mat accur = cv::Mat::zeros(cv::Size(m_cellSizeW, m_cellSizeH), CV_8U);
	//	_stLayerInfo layerInfo;
	//	int numsize = iter->second.size();

	//	for (auto i = 0; i < numsize; i++) {
	//		int imgIdx = iter->second[i] / (m_wnum * m_hnum);
	//		//	if (imgIdx > 13) continue;
	//		//	int cellsize = m_imageDb[imgIdx].cols / m_wnum;
	//		cv::Mat img = getWordCutImg(iter->second[i], m_cellSizeW, m_imageDb[imgIdx].img, m_classId, m_wnum, m_hnum);

	//		//img = deskew(img);
	//		cv::bitwise_not(img, img);
	//		if (iter->first > 10000) {  // in case of symbols //
	//			img = FitBoundingBox(img);
	//		}
	//		accumulateImg(accur, img);
	//		layerInfo.vecPositionId.push_back(iter->second[i]);
	//	}

	//	// Resize====== //
	//	int cnt = iter->second.size();
	//	if (cnt > 255)		cnt = 255;
	//	int th = (cnt / 2);
	//	//	balance_white(accur);
	//	//if (iter->first >= 44032) {
	//	cv::equalizeHist(accur, accur);
	//	cv::threshold(accur, accur, 220, 255, cv::THRESH_OTSU);

	//	//}
	//	//else {
	//	//	cv::threshold(accur, accur, 0, 255, cv::THRESH_BINARY);
	//	//}

	//	cv::bitwise_not(accur, accur);
	//	//	cv::equalizeHist(accur, accur);

	//	//	cv::Rect r1 = getPositionByIndex(m_firstlayerImageWordNum, m_cellSizeW, m_classId, m_wnum, m_hnum);		
	//	cv::Rect r1 = getPositionByIndex(m_firstlayerImageWordNum, first_cellSize, 0, first_wnum, first_wnum);
	//	cv::resize(accur, accur, cv::Size(first_cellSize, first_cellSize));
	//	accur.copyTo(m_firstlayerImage(r1));
	//	m_firstlayerImageWordNum++;
	//	//		TRACE("unicode in map:  %d\n", iter->first);

	//	//		cv::resize(accur, accur, cv::Size(m_hogResolution, m_hogResolution));
	//	//		layerInfo.hogFeature = getHOGFeature(d, accur);

	//	//layerInfo.strcode = new wchar_t[m_strcodeLen];
	//	//memset(layerInfo.strcode, 0x00, sizeof(wchar_t)*m_strcodeLen);
	//	layerInfo.strcode = (wchar_t)(unsigned short)iter->first;
	//	m_vecLayerInfo.push_back(layerInfo);
	//}

	//clock_t end = clock();
	//double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	//TRACE("Elapsed Time (GenerateFirstLayer) : %3.2f\n", (float)elapsed_secs);

	//cv::resize(layerImg, layerImg, cv::Size(m_wnum * resolution, m_hnum * resolution));

	//m_firstlayerImage.release();
	//m_firstlayerImage = layerImg.clone();
	//layerImg.release();
	//cv::imshow("Layer Image", m_firstlayerImage);

}

void CMNDBLayerImage::GenerateFirstLayerByCode(int minstrcode, int first_cellSize, int first_wnum)
{
	clock_t begin = clock();

	m_mapStrCode.clear();
//	m_cellResolution = resolution;
	m_firstLayerCellSize = first_cellSize;
	m_firstLayer_wNum = first_wnum;

	int misscnt = 0;
	for (auto i = 0; i < m_vecStrCode.size(); i++) {
		unsigned short w1 = (unsigned short)m_vecStrCode[i][0];
		if ((w1 > minstrcode)) {
			m_mapStrCode[w1].push_back(i);
		}
		else {
			misscnt++;
		}
	}

//	cv::HOGDescriptor d(cv::Size(m_hogResolution, m_hogResolution), cv::Size(8, 8), cv::Size(4, 4), cv::Size(4, 4), 9);
	m_firstlayerImage.release();
//	m_firstlayerImage = cv::Mat::zeros(cv::Size(m_wnum * m_cellSizeW, m_hnum * m_cellSizeH), CV_8U);
	m_firstlayerImage = cv::Mat::zeros(cv::Size(first_wnum * first_cellSize, first_wnum * first_cellSize), CV_8U);
	std::map<unsigned long, std::vector<unsigned long>>::iterator iter = m_mapStrCode.begin();

	m_firstlayerImageWordNum = 0;
	m_vecLayerInfo.swap(std::vector<_stLayerInfo>());
	for (; iter != m_mapStrCode.end(); iter++) {
		cv::Mat accur = cv::Mat::zeros(cv::Size(m_cellSizeW, m_cellSizeH), CV_8U);
		_stLayerInfo layerInfo;
		int numsize = iter->second.size();
		
		for (auto i = 0; i < numsize; i++) {
			int imgIdx = iter->second[i] / (m_wnum * m_hnum);
		//	if (imgIdx > 13) continue;
		//	int cellsize = m_imageDb[imgIdx].cols / m_wnum;
			cv::Mat img = getWordCutImg(iter->second[i], m_cellSizeW, m_imageDb[imgIdx].img, m_classId, m_wnum, m_hnum);
			
			//img = deskew(img);
			cv::bitwise_not(img, img);
			if (iter->first < 10000) {  // in case of symbols //
				img = FitBoundingBox(img);
			}
			accumulateImg(accur, img);
		//	averageImg(accur, img, 0);
			layerInfo.vecPositionId.push_back(iter->second[i]);
		}
		
		// Resize====== //
		int cnt = iter->second.size();
		if (cnt > 255)		cnt = 255;
		int th = (cnt / 2);
	//	balance_white(accur);
		//if (iter->first >= 44032) {
		cv::equalizeHist(accur, accur);
		cv::threshold(accur, accur, 220, 255, cv::THRESH_OTSU);

		//}
		//else {
		//	cv::threshold(accur, accur, 0, 255, cv::THRESH_BINARY);
		//}

		cv::bitwise_not(accur, accur);
	//	cv::equalizeHist(accur, accur);
		
	//	cv::Rect r1 = getPositionByIndex(m_firstlayerImageWordNum, m_cellSizeW, m_classId, m_wnum, m_hnum);		
		cv::Rect r1 = getPositionByIndex(m_firstlayerImageWordNum, first_cellSize, 0, first_wnum, first_wnum);
		cv::resize(accur, accur, cv::Size(first_cellSize, first_cellSize));
		accur.copyTo(m_firstlayerImage(r1));
		m_firstlayerImageWordNum++;
//		TRACE("unicode in map:  %d\n", iter->first);

//		cv::resize(accur, accur, cv::Size(m_hogResolution, m_hogResolution));
//		layerInfo.hogFeature = getHOGFeature(d, accur);

		//layerInfo.strcode = new wchar_t[m_strcodeLen];
		//memset(layerInfo.strcode, 0x00, sizeof(wchar_t)*m_strcodeLen);
//		layerInfo.strcode = (wchar_t)(unsigned short)iter->first;		
		layerInfo.vecCodes.push_back((unsigned short)iter->first);
		m_vecLayerInfo.push_back(layerInfo);
	}
	
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	TRACE("Elapsed Time (GenerateFirstLayer) : %3.2f\n", (float)elapsed_secs);

	//cv::resize(layerImg, layerImg, cv::Size(m_wnum * resolution, m_hnum * resolution));

	//m_firstlayerImage.release();
	//m_firstlayerImage = layerImg.clone();
	//layerImg.release();
	//cv::imshow("Layer Image", m_firstlayerImage);

}

void CMNDBLayerImage::LoadLayerImageFile(CString str)
{
//	clock_t begin = clock();

	USES_CONVERSION;
	CString strFile;
	std::vector<cv::Mat> vecDBImg;

	bool IsFile = true;
	do {
		strFile.Format(L"%s/class%02d_%02d.jp2", str, m_classId, m_imgcnt);
		char* sz = 0;
		sz = T2A(strFile);
		
		_dbMat clsImg;
		clsImg.img = cv::imread(sz, CV_LOAD_IMAGE_GRAYSCALE);
		clsImg.bNedUpdate = false;

		m_imgcnt++;

		if (clsImg.img.ptr() != nullptr) {
			//cv::imshow("class image", clsImg);
			//cv::waitKey(10);
			
			m_imageDb.push_back(std::move(clsImg));
		}
		else
			IsFile = false;



		CString strLog;
		strLog.Format(L"Load %s...done.", strFile);
		pMain->AddOutputString(strLog, false);

	} while (IsFile);

	//clock_t end = clock();
	//double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	//TRACE("Elapsed Time (LoadLayerImageFile) : %3.2f\n", (float)elapsed_secs);
}

void CMNDBLayerImage::LoadJP3File(CString str)
{
	clock_t begin = clock();
//	m_vecStrCode.swap(std::vector<wchar_t*>());
	for (auto i = 0; i < m_vecStrCode.size(); i++) {
		delete[] m_vecStrCode[i];
	}
	m_vecStrCode.swap(std::vector<wchar_t*>());


	USES_CONVERSION;
	char* sz = 0;
	sz = T2A(str);

	FILE* fp = 0;
	fopen_s(&fp, sz, "rb");
	if (fp) {
		int num = 0, cLen = 0;
		fread(&num, sizeof(int), 1, fp);
		fread(&cLen, sizeof(int), 1, fp);

		// For test //
//		num = 9600;

		for (int j = 0; j < num; j++) {
			wchar_t* code = new wchar_t[cLen];
			memset(code, 0x00, sizeof(wchar_t)*cLen);
			fread(code, sizeof(wchar_t)*cLen, 1, fp);
			m_vecStrCode.push_back(code);
		}
		fclose(fp);
	}

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	TRACE("Elapsed Time (LoadJP3File) : %3.2f\n", (float)elapsed_secs);
}


cv::Mat CMNDBLayerImage::FitBoundingBox(cv::Mat img)
{
	std::vector<std::vector<cv::Point> > contours_poly;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	contours_poly.resize(contours.size());

	cv::Rect rect(0, 0, 0, 0);
	for (int i = 0; i < contours.size(); i++) {
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 1, true);
		cv::Rect r = cv::boundingRect(cv::Mat(contours_poly[i]));
		if (r.area() > 20) {
			rect = rect | r;
		}
	}

	if (rect.width == 0)
		return img;
	else {

		cv::threshold(img, img, 100, 255, cv::THRESH_BINARY);
		cv::Mat res = cv::Mat(rect.width, rect.height, CV_8UC1, cv::Scalar(255));
		img(rect).copyTo(res);
		cv::resize(res, res, cv::Size(img.cols, img.rows));
	//	res = deskew(res);
		return res;
	}
}


_recognitionResult CMNDBLayerImage::GetMatchResultByPixel(cv::Mat& cutImg, _stMatcResTop5& res)
{
	// Normalization //
	cv::Mat imgforMaster = cutImg.clone();
	cv::threshold(imgforMaster, imgforMaster, 220, 255, cv::THRESH_OTSU);
	//cv::bitwise_not(imgforMaster, imgforMaster);
	////	cutImg = deskew(cutImg);
	//imgforMaster = FitBoundingBox(imgforMaster);
	//cv::bitwise_not(imgforMaster, imgforMaster);
	//=============================//

	float th = _GEN_DB_TH - 0.1f;
	float accur = 0.0f;
	int matchIdx = 0;
//	cv::resize(imgforMaster, imgforMaster, cv::Size(m_cellResolution, m_cellResolution));

	std::map<unsigned long, unsigned long> mapMatch;
	for (int i = 0; i < m_firstlayerImageWordNum; i++) {

		cv::Rect r1 = getPositionByIndex(i, m_firstLayerCellSize, 0, m_firstLayer_wNum, m_firstLayer_wNum);
		//cv::Mat layercut = cv::Mat(m_layerResolution + 4, m_layerResolution + 4, CV_8UC1, cv::Scalar(255));
		//m_firstlayerImage(r1).copyTo(layercut(cv::Rect(2, 2, m_layerResolution, m_layerResolution)));
		//cv::Mat layercut = cv::Mat(m_firstLayerCellSize + 2, m_firstLayerCellSize + 2, CV_8UC1, cv::Scalar(255));
		//m_firstlayerImage(r1).copyTo(layercut(cv::Rect(1, 1, m_firstLayerCellSize, m_firstLayerCellSize)));

//		cv::resize(imgforMaster, imgforMaster, cv::Size(m_firstLayerCellSize, m_firstLayerCellSize));


		cv::Mat layercut = cv::Mat(m_firstLayerCellSize, m_firstLayerCellSize, CV_8UC1, cv::Scalar(255));
		m_firstlayerImage(r1).copyTo(layercut(cv::Rect(0, 0, m_firstLayerCellSize, m_firstLayerCellSize)));
		

		float prob = templateMatching(imgforMaster, layercut);
	//	float prob = templateMatching(layercut, imgforMaster);

		
	//	float res = templateMatching(cutImg, layercut);

	//	if (res2 > res)	res = res2;


		//cv::resize(imgforMaster, imgforMaster, cv::Size(30, 30));
		//float res2 = templateMatching(imgforMaster, layercut);
		//if (res2 > res) {
		//	res = res2;
		//}

		//if (res > accur) {
		//	accur = res;
		//	matchIdx = i;
		//}
		if (prob > th) {
			//cv::imshow("cut", imgforMaster);
			//cv::imshow("layer", layercut);
			//cv::waitKey(1);
			int idx = prob * 10000;
			while (mapMatch.find(idx) != mapMatch.end()) {
				idx++;
			}
			mapMatch[idx] = i;
		}
	}

	//if (matchIdx < m_vecLayerInfo.size()) {
	//	strcode = m_vecLayerInfo[matchIdx].strcode;
	//}


	
	int cnt = 0;	
	_recognitionResult result;
	result.accur = 0;
	result.code = 0;

	if (mapMatch.size() > 0) {
		std::map<unsigned long, unsigned long>::iterator iter = mapMatch.end();
		iter--;
		for (; iter != mapMatch.end(); iter--) {
			res.accur[cnt] = (float)iter->first / 10000.0f;
		//	res.code[cnt] = m_vecLayerInfo[iter->second].vecCodes[0];
			int firstLayerIndex = iter->second;

			// Get Top 1 //
			//_recognitionResult temp = DeepMatching(cutImg, (unsigned short)res.code[cnt]);
			_recognitionResult temp = DeepMatching2(cutImg, firstLayerIndex);
			if (temp.accur > result.accur) {
				result = temp;
			}

			cnt++;
			if (cnt > _MAX_CANDIDATE - 1)
				break;
		}
	}

	return result;
//	cv::imshow("layercut", layercut);
//	accur.copyTo(layerImg(r1));
}

void CMNDBLayerImage::UpdateStrCode(int wordIdx, wchar_t* _code)
{
	if (wordIdx < m_vecStrCode.size()) {
		memset(m_vecStrCode[wordIdx], 0x00, sizeof(m_vecStrCode[wordIdx]));
		memcpy(m_vecStrCode[wordIdx], _code, sizeof(m_vecStrCode[wordIdx]));
	//	m_vecStrCode[wordIdx][0] = _code;
		m_bNeedToUpdateCode = true;
	}
}

void CMNDBLayerImage::AddNewTrainingData(int clsid, cv::Mat addImg, wchar_t* _code)
{
	wchar_t* code = new wchar_t[m_strcodeLen];
	memset(code, 0x00, sizeof(wchar_t)*m_strcodeLen);
	memcpy(code, _code, sizeof(wchar_t)*m_strcodeLen);
	m_vecStrCode.push_back(code);
	m_bNeedToUpdateCode = true;

	int wordIdx = m_vecStrCode.size() - 1;
	int imgIdx = wordIdx / (m_wnum * m_hnum);
	cv::Rect r1 = getPositionByIndex(wordIdx, m_cellSizeH, 0, m_wnum, m_hnum);


	if (imgIdx >= m_imageDb.size()) {
		int imgw = m_wnum * m_cellResolution;
		int imgh = m_hnum * m_cellResolution;

		_dbMat newImg;
		newImg.img = cv::Mat(imgw, imgh, CV_8UC1, cv::Scalar(255));
		newImg.bNedUpdate = true;
		m_imageDb.push_back(newImg);
	}
	addImg.copyTo(m_imageDb[imgIdx].img(r1));
	m_imageDb[imgIdx].bNedUpdate = true;
}


_recognitionResult CMNDBLayerImage::DeepMatching2(cv::Mat cutImg, int idx)
{
	_recognitionResult result;
	result.accur = 0;
//	result.code = (wchar_t)charcode;
	result.firstlayerIdx = 0;

	float accurate = 0;
	int cnt = 0;
	wchar_t code=0;

	for(auto i=0; i<m_vecLayerInfo[idx].vecPositionId.size(); i++){
	
			//	cv::Mat infImg = GetCutImageByWordInx(iter->second[i], result.code);
			cv::Mat layercut = cv::Mat(m_cellResolution + 2, m_cellResolution + 2, CV_8UC1, cv::Scalar(255));
			GetCutImageByWordInx(m_vecLayerInfo[idx].vecPositionId[i], code, 1).copyTo(layercut(cv::Rect(1, 1, m_cellResolution, m_cellResolution)));

			float accur = templateMatching(cutImg, layercut);
			//if (accur > 0.95f) {
			//	result.accur = accur;
			//	break;
			//}
			//else {
			if (accur > result.accur) {
				result.accur = accur;
				result.code = code;
			//	result.firstlayerIdx = iter->second[i];
			}
			//}

			//	cv::imshow("cut", infImg);
			//	cv::waitKey(10);
			accurate += accur;
			cnt++;
	//	}
	}

	//if (cnt > 0) {
	//	result.accur = accurate / (float)cnt;
	//}
	return result;
}

_recognitionResult CMNDBLayerImage::DeepMatching(cv::Mat cutImg, unsigned short charcode)
{
	_recognitionResult result;
	result.accur = 0;
	result.code = (wchar_t)charcode;
	result.firstlayerIdx = 0;

	float accurate = 0;
	int cnt = 0;
	std::map<unsigned long, std::vector<unsigned long>>::iterator iter =  m_mapStrCode.find((unsigned long)charcode);
	if (iter != m_mapStrCode.end()) {
		for (auto i = 0; i < iter->second.size(); i++) {

		//	cv::Mat infImg = GetCutImageByWordInx(iter->second[i], result.code);
			cv::Mat layercut = cv::Mat(m_cellResolution + 4, m_cellResolution + 4, CV_8UC1, cv::Scalar(255));
			GetCutImageByWordInx(iter->second[i], result.code, 1).copyTo(layercut(cv::Rect(2, 2, m_cellResolution, m_cellResolution)));

			float accur = templateMatching(cutImg, layercut);
			//if (accur > 0.95f) {
			//	result.accur = accur;
			//	break;
			//}
			//else {
				if (accur > result.accur) {
					result.accur = accur;
					result.firstlayerIdx = iter->second[i];
				}
			//}

		//	cv::imshow("cut", infImg);
		//	cv::waitKey(10);
			accurate += accur;
			cnt++;
		}
	}

	//if (cnt > 0) {
	//	result.accur = accurate / (float)cnt;
	//}
	return result;
}



//void CMNDBLayerImage::GetMatchResultByHOG(cv::Mat cutImg, _stMatcResTop5& res)
//{
//	cv::resize(cutImg, cutImg, cv::Size(m_hogResolution, m_hogResolution));
//	cv::HOGDescriptor d(cv::Size(m_hogResolution, m_hogResolution), cv::Size(8, 8), cv::Size(4, 4), cv::Size(4, 4), 9);
//	cv::Mat A = getHOGFeature(d, cutImg);
//
//	float accur = 4096;
////	int matchIdx = 0;
//	std::map<unsigned long, unsigned long> mapMatch;
//	for (auto i = 0; i < m_vecLayerInfo.size(); i++) {
//		float res = hog_matching(A, m_vecLayerInfo[i].hogFeature);
//		//if (res < accur) {
//		//	accur = res;
//		//	matchIdx = i;
//		//}
//
//		if (res < 2000) {
//			int idx = res * 100;
//			mapMatch[idx] = i;
//		}
//	}
//
//	std::map<unsigned long, unsigned long>::iterator iter = mapMatch.begin();
//	int cnt = 0;
//	for (; iter != mapMatch.end(); iter++) {
//		res.accur[cnt] = (float)iter->first * 0.01f;
//		res.code[cnt] = m_vecLayerInfo[iter->second].strcode;
//		cnt++;
//		if (cnt > 19)
//			break;
//	}
//
//	//if (matchIdx < m_vecLayerInfo.size()) {
//	//	strcode = m_vecLayerInfo[matchIdx].strcode;
//	//}
//}

float CMNDBLayerImage::templateMatching(cv::Mat& src, cv::Mat& tmpl)
{

	int result_cols = tmpl.cols - src.cols + 1;
	int result_rows = tmpl.rows - src.rows + 1;
	cv::Mat result(result_rows, result_cols, CV_32FC1);
	cv::matchTemplate(src, tmpl, result, CV_TM_CCOEFF_NORMED);

	float th = 0.0f;
	float fD = 0.0f;
	//	th = result.at<float>(0, 0);
	for (int y = 0; y < result.rows; y++) {
		for (int x = 0; x < result.cols; x++) {
			fD = result.at<float>(y, x);
			if (fD > th) {
				//	TRACE("%3.2f\n", fD);
				th = fD;
			}
		}
	}
	result.release();
	return th;
}

float CMNDBLayerImage::hog_matching(cv::Mat A, cv::Mat B)
{
	cv::Mat C = A - B;
	C = C.mul(C);
	cv::sqrt(C, C);
	cv::Scalar rr = cv::sum(C);
	return rr(0);
}

cv::Mat CMNDBLayerImage::deskew(cv::Mat& img)
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
	//	cv::Mat imgOut = cv::Mat(img.rows, img.cols, img.type(), cv::Scalar(255));
	warpAffine(img, imgOut, warpMat, imgOut.size(), affineFlags);

	return imgOut;
}


void CMNDBLayerImage::balance_white(cv::Mat& mat)
{
	double discard_ratio = 0.05;
	int hists[3][256];
	memset(hists, 0, 3 * 256 * sizeof(int));

	for (int y = 0; y < mat.rows; ++y) {
		uchar* ptr = mat.ptr<uchar>(y);
		for (int x = 0; x < mat.cols; ++x) {
			for (int j = 0; j < 3; ++j) {
				hists[j][ptr[x * 3 + j]] += 1;
			}
		}
	}

	// cumulative hist
	int total = mat.cols*mat.rows;
	int vmin[3], vmax[3];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 255; ++j) {
			hists[i][j + 1] += hists[i][j];
		}
		vmin[i] = 0;
		vmax[i] = 255;
		while (hists[i][vmin[i]] < discard_ratio * total)
			vmin[i] += 1;
		while (hists[i][vmax[i]] > (1 - discard_ratio) * total)
			vmax[i] -= 1;
		if (vmax[i] < 255 - 1)
			vmax[i] += 1;
	}


	for (int y = 0; y < mat.rows; ++y) {
		uchar* ptr = mat.ptr<uchar>(y);
		for (int x = 0; x < mat.cols; ++x) {
			for (int j = 0; j < 3; ++j) {
				int val = ptr[x * 3 + j];
				if (val < vmin[j])
					val = vmin[j];
				if (val > vmax[j])
					val = vmax[j];
				ptr[x * 3 + j] = static_cast<uchar>((val - vmin[j]) * 255.0 / (vmax[j] - vmin[j]));
			}
		}
	}
}


void CMNDBLayerImage::UpdateLayer(CString strPath)
{
	// Update jp3 file //
	CString strJp3;
	strJp3.Format(L"%s/class%02d.jp3", strPath, m_classId);
	USES_CONVERSION;
	char* sz = 0;
	sz = T2A(strJp3);

	if (m_bNeedToUpdateCode) {
		GenerateFirstLayerByShape(0, _FIRSTLAYER_CELL, _FIRSTLAYER_WNUM);
		FILE* fp = 0;
		fopen_s(&fp, sz, "wb");
		if (fp) {
			int num = m_vecStrCode.size();
			int clen = m_strcodeLen;
			fwrite(&num, sizeof(int), 1, fp);
			fwrite(&clen, sizeof(int), 1, fp);
			for (int i = 0; i < num; i++) {
				fwrite(m_vecStrCode[i], sizeof(wchar_t)*m_strcodeLen, 1, fp);
			}
			fclose(fp);
		}
		m_bNeedToUpdateCode = false;
	}


	// Update Layers which are changed
	


	CString strFile;
	for (int i = 0; i < m_imageDb.size(); i++) {
		if (m_imageDb[i].bNedUpdate) {
			strFile.Format(L"%s\\class%02d_%02d.jp2", strPath, m_classId, i);
			sz = T2A(strFile);
			cv::imwrite(sz, m_imageDb[i].img);

			strFile.Format(L"%s\\class%02d_%02d.jpg", strPath, m_classId, i);
			sz = T2A(strFile);
			cv::imwrite(sz, m_imageDb[i].img);


			//strFile.Format(L"%s\\class_master.jpg", strPath);
			//sz = T2A(strFile);
			//cv::imwrite(sz,m_firstlayerImage);

			m_imageDb[i].bNedUpdate = false;
		}
	}
}

void CMNDBLayerImage::UpdateDBCode(int id, wchar_t code)
{
	if ((id >= 0) && (id < m_vecStrCode.size())) {
		if (code == 0) {		// Delete //
			delete[] m_vecStrCode[id];
			wchar_t* ncode = new wchar_t[_C1_CODE_LEN];
			memset(ncode, 0x00, sizeof(wchar_t)*_C1_CODE_LEN);
			m_vecStrCode[id] = ncode;
			m_bNeedToSaveJp3 = true;

			FillNullCutImageByWordIdx(id);
		}
		else {
			delete[] m_vecStrCode[id];
			wchar_t* ncode = new wchar_t[_C1_CODE_LEN];
			memset(ncode, 0x00, sizeof(wchar_t)*_C1_CODE_LEN);
			memcpy(ncode, &code, sizeof(wchar_t));
			m_vecStrCode[id] = ncode;
			m_bNeedToSaveJp3 = true;
		}
	}

	//if (id == -1) { // Save File
	//	WriteJP3File(m_strPatJp3);
	//}
}

void CMNDBLayerImage::SaveUserChanges()
{
	WriteJP3File(m_strPatJp3);
	m_bNeedToSaveJp3 = false;

	for (int i = 0; i < m_imgcnt; i++) {
		if (m_imageDb[i].bNedUpdate) {
			cv::imwrite(m_imageDb[i].strPath, m_imageDb[i].img);
		}
	}

}

void CMNDBLayerImage::FillNullCutImageByWordIdx(int wordIdx)
{
	int imgIdx = wordIdx / (m_wnum * m_hnum);
	cv::Rect r1 = getPositionByIndex(wordIdx, m_cellSizeW, m_classId, m_wnum, m_hnum);
	m_imageDb[imgIdx].img(r1).setTo(cv::Scalar(255));
	m_imageDb[imgIdx].bNedUpdate = true;

}

void CMNDBLayerImage::WriteJP3File(CString str)
{
	USES_CONVERSION;
	char* sz = 0;
	sz = T2A(str);

	FILE* fp = 0;
	fopen_s(&fp, sz, "wb");
	if (fp) {
		int num = m_vecStrCode.size();
		int clen = m_strcodeLen;
		fwrite(&num, sizeof(int), 1, fp);
		fwrite(&clen, sizeof(int), 1, fp);
		for (int i = 0; i < num; i++) {
			fwrite(m_vecStrCode[i], sizeof(wchar_t)*m_strcodeLen, 1, fp);
		}
		fclose(fp);
	}
}
