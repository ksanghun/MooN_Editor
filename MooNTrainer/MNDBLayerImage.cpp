#include "stdafx.h"
#include "MNDBLayerImage.h"
#include <ctime>
#include "MainFrm.h"

CMNDBLayerImage::CMNDBLayerImage()
{
	m_imgcnt = 0;
	m_classId = 0;
	m_cellSizeW = 0;
	m_cellSizeH = 0;
	m_hogResolution = 32;
	m_totalCodeNum = 0;
	m_bNeedToSaveJp3 = false;
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
//		m_vecLayerInfo[i].hogFeature.release();
	//	delete [] m_vecLayerInfo[i].strcode;
		m_vecLayerInfo[i].vecPositionId.clear();
	}
	m_vecLayerInfo.swap(std::vector<_stLayerInfo>());

	m_mapStrCode.clear();
}

void CMNDBLayerImage::InitLayer(int clsId, unsigned short _wnum, unsigned short _hnum, unsigned short _codelen, int cellSizeW, int cellSizeH, CString strPath)
{
	ReleaseLayerImage();

	m_imgcnt = 0;
	m_classId = clsId;
	m_wnum = _wnum;
	m_hnum = _hnum;
	m_strcodeLen = _codelen;
	m_cellSizeW = cellSizeW;	
	m_cellSizeH = cellSizeH;

	// Load string info file //
	CString strJp3;
	strJp3.Format(L"%s/class%02d.jp3", strPath, m_classId);
	LoadJP3File(strJp3);

	pMain->AddOutputString(L"Load Jp3 file...done.", false);

	// load class images //
	LoadLayerImageFile(strPath);
}

cv::Mat CMNDBLayerImage::getHOGFeature(cv::HOGDescriptor d1, cv::Mat& img)
{
	//	cv::HOGDescriptor d1(cv::Size(resolution, resolution), cv::Size(8, 8), cv::Size(4, 4), cv::Size(4, 4), 9);
	std::vector<float> descriptors1;
	d1.compute(img, descriptors1);

	//copy vector to mat  
	cv::Mat A(descriptors1.size(), 1, CV_32FC1);
	//copy vector to mat  
	memcpy(A.data, descriptors1.data(), descriptors1.size() * sizeof(float));

	return A;
}

cv::Mat CMNDBLayerImage::getWordCutImg(int wordId, int cellSize, cv::Mat& clsImg, int clsId, int wNum, int hNum)
{
	cv::Rect r1 = getPositionByIndex(wordId, cellSize, clsId, wNum, hNum);
	cv::Mat img = cv::Mat(cellSize, cellSize, CV_8UC1, cv::Scalar(255));
	clsImg(r1).copyTo(img(cv::Rect(0, 0, cellSize, cellSize)));
	return img;
}

cv::Mat CMNDBLayerImage::GetCutImageByWordInx(int wordIdx, wchar_t& strcode)
{
	int imgIdx = wordIdx / (m_wnum * m_hnum);
	strcode = m_vecStrCode[wordIdx][0];
	return getWordCutImg(wordIdx, m_cellSizeW, m_imageDb[imgIdx].img, m_classId, m_wnum, m_hnum);
}

void CMNDBLayerImage::FillNullCutImageByWordIdx(int wordIdx)
{
	int imgIdx = wordIdx / (m_wnum * m_hnum);
	cv::Rect r1 = getPositionByIndex(wordIdx, m_cellSizeW, m_classId, m_wnum, m_hnum);
	m_imageDb[imgIdx].img(r1).setTo(cv::Scalar(255));
	m_imageDb[imgIdx].bNedUpdate = true;
	
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

void CMNDBLayerImage::GenerateFirstLayer(int minstrcode, int resolution)
{
	clock_t begin = clock();

	m_mapStrCode.clear();
	m_layerResolution = resolution;
	int misscnt = 0;
	//	std::map<unsigned long, std::vector<unsigned long>> mapStrCode;
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
	//	cv::Mat layerImg = cv::Mat::zeros(cv::Size(m_wnum * m_cellSizeW, m_hnum * m_cellSizeH), CV_8U);
	m_firstlayerImage.release();
//	m_firstlayerImage = cv::Mat::zeros(cv::Size(m_wnum * m_cellSizeW, m_hnum * m_cellSizeH), CV_8U);
	m_firstlayerImage = cv::Mat(cv::Size(m_wnum * m_cellSizeW, m_hnum * m_cellSizeH), CV_8U);
//	m_firstlayerImage.setTo(cv::Scalar(255));
	std::map<unsigned long, std::vector<unsigned long>>::iterator iter = m_mapStrCode.begin();

	m_firstlayerImageWordNum = 0;
	m_vecLayerInfo.swap(std::vector<_stLayerInfo>());
	for (; iter != m_mapStrCode.end(); iter++) {
		cv::Mat accur = cv::Mat::zeros(cv::Size(m_cellSizeW, m_cellSizeH), CV_8U);

		_stLayerInfo layerInfo;
		int numsize = iter->second.size();

		// if numsize is small, augmentation //

		for (auto i = 0; i < numsize; i++) {

			int imgIdx = iter->second[i] / (m_wnum * m_hnum);

			//	if (imgIdx > 13) continue;
			//	int cellsize = m_imageDb[imgIdx].cols / m_wnum;
			cv::Mat img = getWordCutImg(iter->second[i], m_cellSizeW, m_imageDb[imgIdx].img, m_classId, m_wnum, m_hnum);

			//img = deskew(img);
			cv::bitwise_not(img, img);
			if (iter->first > 10000) {  // in case of symbols //
				img = FitBoundingBox(img);
			}
			accumulateImg(accur, img);

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

		cv::Rect r1 = getPositionByIndex(m_firstlayerImageWordNum, m_cellSizeW, m_classId, m_wnum, m_hnum);
		accur.copyTo(m_firstlayerImage(r1));
		m_firstlayerImageWordNum++;
		//		TRACE("unicode in map:  %d\n", iter->first);

		cv::resize(accur, accur, cv::Size(m_hogResolution, m_hogResolution));
		//		layerInfo.hogFeature = getHOGFeature(d, accur);

		//layerInfo.strcode = new wchar_t[m_strcodeLen];
		//memset(layerInfo.strcode, 0x00, sizeof(wchar_t)*m_strcodeLen);
		layerInfo.strcode = (wchar_t)(unsigned short)iter->first;
		m_vecLayerInfo.push_back(layerInfo);
	}

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	TRACE("Elapsed Time (GenerateFirstLayer) : %3.2f\n", (float)elapsed_secs);





//	clock_t begin = clock();
//
//	m_mapStrCode.clear();
//	m_layerResolution = resolution;
//	int misscnt = 0;
////	std::map<unsigned long, std::vector<unsigned long>> mapStrCode;
//	for (auto i = 0; i < m_vecStrCode.size(); i++) {
//		unsigned short w1 = (unsigned short)m_vecStrCode[i][0];
//		if ((w1 > minstrcode)) {
//			m_mapStrCode[w1].push_back(i);
//		}
//		else {
//			misscnt++;
//		}
//	}
//
//	cv::HOGDescriptor d(cv::Size(m_hogResolution, m_hogResolution), cv::Size(8, 8), cv::Size(4, 4), cv::Size(4, 4), 9);
//	cv::Mat layerImg = cv::Mat::zeros(cv::Size(m_wnum * m_cellSizeW, m_hnum * m_cellSizeH), CV_8U);
//	std::map<unsigned long, std::vector<unsigned long>>::iterator iter = m_mapStrCode.begin();
//
//	m_firstlayerImageWordNum = 0;
//	for (; iter != m_mapStrCode.end(); iter++) {
//		cv::Mat accur = cv::Mat::zeros(cv::Size(m_cellSizeW, m_cellSizeH), CV_8U);
//
//		_stLayerInfo layerInfo;
//		int numsize = iter->second.size();
//
//		// if numsize is small, augmentation //
//
//		for (auto i = 0; i < numsize; i++) {
//
//			int imgIdx = iter->second[i] / (m_wnum * m_hnum);
//
//		//	if (imgIdx > 13) continue;
//		//	int cellsize = m_imageDb[imgIdx].cols / m_wnum;
//			cv::Mat img = getWordCutImg(iter->second[i], m_cellSizeW, m_imageDb[imgIdx], m_classId, m_wnum, m_hnum);
//			
//			//img = deskew(img);
//			cv::bitwise_not(img, img);
//			img = FitBoundingBox(img);
//			accumulateImg(accur, img);
//
//			layerInfo.vecPositionId.push_back(iter->second[i]);
//		}
//		
//		// Resize====== //
//		int cnt = iter->second.size();
//		if (cnt > 255)		cnt = 255;
//		int th = (cnt / 2);
//
//
//	//	balance_white(accur);
//		//if (iter->first >= 44032) {
//		cv::equalizeHist(accur, accur);
//		cv::threshold(accur, accur, 220, 255, cv::THRESH_OTSU);
//		//}
//		//else {
//		//	cv::threshold(accur, accur, 0, 255, cv::THRESH_BINARY);
//		//}
//
//		cv::bitwise_not(accur, accur);
//	//	cv::equalizeHist(accur, accur);
//		
//		cv::Rect r1 = getPositionByIndex(m_firstlayerImageWordNum, m_cellSizeW, m_classId, m_wnum, m_hnum);
//		accur.copyTo(layerImg(r1));
//		m_firstlayerImageWordNum++;
////		TRACE("unicode in map:  %d\n", iter->first);
//
//		cv::resize(accur, accur, cv::Size(m_hogResolution, m_hogResolution));
//		layerInfo.hogFeature = getHOGFeature(d, accur);
//		//layerInfo.strcode = new wchar_t[m_strcodeLen];
//		//memset(layerInfo.strcode, 0x00, sizeof(wchar_t)*m_strcodeLen);
//		layerInfo.strcode = (wchar_t)(unsigned short)iter->first;		
//		m_vecLayerInfo.push_back(layerInfo);
//	}
//	
//	clock_t end = clock();
//	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//	TRACE("Elapsed Time (GenerateFirstLayer) : %3.2f\n", (float)elapsed_secs);
//
//	cv::resize(layerImg, layerImg, cv::Size(m_wnum * resolution, m_hnum * resolution));
//	m_firstlayerImage = layerImg.clone();
//	//layerImg.release();
//	//cv::imshow("Layer Image", m_firstlayerImage);

}

cv::Mat& CMNDBLayerImage::GetLayerImageByID(int id)
{ 
	return m_imageDb[id].img; 
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
		clsImg.strPath = sz;
		m_imgcnt++;

		if (clsImg.img.ptr() != nullptr) {
			//cv::imshow("class image", clsImg);
			//cv::waitKey(10);

			m_imageDb.push_back(std::move(clsImg));
			CString strLog;
			strLog.Format(L"Load %s...done.", strFile);
			pMain->AddOutputString(strLog, false);
		}
		else
			IsFile = false;		

	} while (IsFile);

	m_imgcnt = m_imageDb.size();
	pMain->SetLayerImgCnt(0, m_imgcnt);

	//clock_t end = clock();
	//double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	//TRACE("Elapsed Time (LoadLayerImageFile) : %3.2f\n", (float)elapsed_secs);
}

void CMNDBLayerImage::UpdateDBCode(int id, wchar_t code)
{
	if ((id>=0) && (id < m_vecStrCode.size())) {
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

void CMNDBLayerImage::LoadJP3File(CString str)
{
//	clock_t begin = clock();
//	m_vecStrCode.swap(std::vector<wchar_t*>());
	for (auto i = 0; i < m_vecStrCode.size(); i++) {
		delete[] m_vecStrCode[i];
	}
	m_vecStrCode.swap(std::vector<wchar_t*>());

	m_strPatJp3 = str;
	USES_CONVERSION;
	char* sz = 0;
	sz = T2A(str);

	FILE* fp = 0;
	fopen_s(&fp, sz, "rb");
	if (fp) {
		int num = 0, cLen = 0;
		fread(&num, sizeof(int), 1, fp);
		fread(&cLen, sizeof(int), 1, fp);

		m_totalCodeNum = num;
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
//	clock_t end = clock();
//	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//	TRACE("Elapsed Time (LoadJP3File) : %3.2f\n", (float)elapsed_secs);
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

		threshold(img, img, 100, 255, cv::THRESH_BINARY);
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
	cv::bitwise_not(imgforMaster, imgforMaster);
	//	cutImg = deskew(cutImg);
	imgforMaster = FitBoundingBox(imgforMaster);
	cv::bitwise_not(imgforMaster, imgforMaster);
	//=============================//


	float accur = 0.0f;
	int matchIdx = 0;
	cv::resize(imgforMaster, imgforMaster, cv::Size(m_layerResolution, m_layerResolution));

	std::map<unsigned long, unsigned long> mapMatch;
	for (int i = 0; i < m_firstlayerImageWordNum; i++) {

		cv::Rect r1 = getPositionByIndex(i, m_layerResolution, m_classId, m_wnum, m_hnum);
		cv::Mat layercut = cv::Mat(m_layerResolution + 4, m_layerResolution + 4, CV_8UC1, cv::Scalar(255));
		m_firstlayerImage(r1).copyTo(layercut(cv::Rect(2, 2, m_layerResolution, m_layerResolution)));

		float res = templateMatching(imgforMaster, layercut);
		float res2 = templateMatching(cutImg, layercut);

		if (res2 > res)	res = res2;


		//cv::resize(imgforMaster, imgforMaster, cv::Size(30, 30));
		//float res2 = templateMatching(imgforMaster, layercut);
		//if (res2 > res) {
		//	res = res2;
		//}

		//if (res > accur) {
		//	accur = res;
		//	matchIdx = i;
		//}
		//if (res > 0.05f) {
			int idx = res * 10000;
			while (mapMatch.find(idx) != mapMatch.end()) {
				idx++;
			}

			mapMatch[idx] = i;
		//}
	}

	//if (matchIdx < m_vecLayerInfo.size()) {
	//	strcode = m_vecLayerInfo[matchIdx].strcode;
	//}


	std::map<unsigned long, unsigned long>::iterator iter = mapMatch.end();
	iter--;
	int cnt = 0;	
	_recognitionResult result;
	result.accur = 0;
	result.code = 0;

	for (; iter != mapMatch.end(); iter--) {
		res.accur[cnt] = (float)iter->first / 10000.0f;
		res.code[cnt] = m_vecLayerInfo[iter->second].strcode;

		// Get Top 1 //
		_recognitionResult temp = DeepMatching(cutImg, (unsigned short)res.code[cnt]);

		// To avoid 1 trained sample //
		if (temp.accur == 1.0) {
			temp.accur -= 0.2f;
		}


		if (temp.accur > result.accur) {
			result = temp;
			result.firstlayerIdx = iter->second;
		}

		cnt++;
		if (cnt > _MAX_CANDIDATE-1)
			break;
	}

	return result;
//	cv::imshow("layercut", layercut);
//	accur.copyTo(layerImg(r1));
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
			cv::Mat layercut = cv::Mat(m_layerResolution + 4, m_layerResolution + 4, CV_8UC1, cv::Scalar(255));
			GetCutImageByWordInx(iter->second[i], result.code).copyTo(layercut(cv::Rect(2, 2, m_layerResolution, m_layerResolution)));

			float accur = templateMatching(cutImg, layercut);
			//if (accur > 0.95f) {
			//	result.accur = accur;
			//	break;
			//}
			//else {
				if (accur > result.accur) {
					result.accur = accur;
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



void CMNDBLayerImage::GetMatchResultByHOG(cv::Mat cutImg, _stMatcResTop5& res)
{
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

	//if (matchIdx < m_vecLayerInfo.size()) {
	//	strcode = m_vecLayerInfo[matchIdx].strcode;
	//}
}

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