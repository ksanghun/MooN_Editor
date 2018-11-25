// DlgProbilityTest.cpp : implementation file
//

#include "stdafx.h"
#include "MooNTrainer.h"
#include "DlgProbilityTest.h"
#include "afxdialogex.h"
#include "MooNDataManager.h"
#include "MainFrm.h"

// CDlgProbilityTest dialog

IMPLEMENT_DYNAMIC(CDlgProbilityTest, CDialogEx)

CDlgProbilityTest::CDlgProbilityTest(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_editInput(_T(""))
	, m_strTrainingText(_T(""))
	, m_strRnnWeight(_T(""))
{
	m_pPredictor = NULL;
}

CDlgProbilityTest::~CDlgProbilityTest()
{
}

void CDlgProbilityTest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INPUT, m_editInput);
	DDX_Control(pDX, IDC_LIST_RESULT, m_ctrlList);
	DDX_Text(pDX, IDC_EDIT_INPUT_TEXT, m_strTrainingText);
	DDX_Text(pDX, IDC_EDIT_RNN_WEIGHT, m_strRnnWeight);
}


BEGIN_MESSAGE_MAP(CDlgProbilityTest, CDialogEx)
	ON_BN_CLICKED(IDC_BN_FILE, &CDlgProbilityTest::OnBnClickedBnFile)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgProbilityTest::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BN_CONVERT, &CDlgProbilityTest::OnBnClickedBnConvert)
	ON_BN_CLICKED(IDC_BN_TRAINING, &CDlgProbilityTest::OnBnClickedBnTraining)
	ON_BN_CLICKED(IDC_BN_LOAD_RNN, &CDlgProbilityTest::OnBnClickedBnLoadRnn)
	ON_BN_CLICKED(IDC_BN_STYLE_TRANSFER, &CDlgProbilityTest::OnBnClickedBnStyleTransfer)
END_MESSAGE_MAP()


// CDlgProbilityTest message handlers


void CDlgProbilityTest::OnBnClickedBnFile()
{
	// TODO: Add your control notification handler code here
	static TCHAR BASED_CODE szFilter[] = _T("Text file(*.TXT, *.txt) | *.TXT;*.txt; |葛电颇老(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.txt"), _T("Text file"), OFN_HIDEREADONLY, szFilter);

	if (IDOK == dlg.DoModal()) {
	//	SINGLETON_DataMng::GetInstance()->LoadTextFileForTree(dlg.GetPathName());
	//	MessageBox(L"A Probability map has been generated.");
		m_strTrainingText = dlg.GetPathName();
		UpdateData(FALSE);
	}
}


void CDlgProbilityTest::OnBnClickedButton2()		// Predict Next Characters
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_editInput == L"") {
		MessageBox(L"Input a keyword");
		return;
	}

	if (m_pPredictor == NULL) {
		MessageBox(L"Model file is not loaded");
		return;
	}

	//if (m_editInput.GetLength() > 10) {
	//	MessageBox(L"Input a keyword less than 10.");
	//	return;
	//}

	//wchar_t* input = m_editInput.GetBuffer();
	//std::vector<_stMooNProb> vecRes;
	//SINGLETON_DataMng::GetInstance()->PredictNext(input, vecRes);

	//for (auto i = 0; i < vecRes.size(); i++) {
	//	CString stroutput;
	//	CString strCode;
	//	if (vecRes[i].code == 32)
	//		strCode = L"space";
	//	else
	//		strCode = (wchar_t)vecRes[i].code;
	//	stroutput.Format(L"%s-%s(%3.2f)", input, strCode, vecRes[i].prob);
	//}

	wchar_t* input = m_editInput.GetBuffer();
	int wlen = wcslen(input);
	int len = wlen * 2;
	unsigned char* cInput = new unsigned char[len];

	decompsiteWchar(input, cInput, wlen);
	std::vector<next_char> res = m_pPredictor->predict_next(cInput, len, 0.5f);


	int listid = 0;
	CString strItem;
	m_ctrlList.DeleteAllItems();

	wchar_t* next = new wchar_t[2];
	for (auto i = 0; i < res.size(); i++) {
		if (res[i].prob > 0.0) {

			unsigned char* secInput = 0;
			secInput = new unsigned char[len + 1];
			memcpy(secInput, cInput, sizeof(char)*len);
			secInput[len] = res[i].next;

			std::vector<next_char> res2 = m_pPredictor->predict_next(secInput, len + 1, 0.5f);
			delete[] secInput;

			for (auto j = 0; j < res2.size(); j++) {
				if (res2[j].prob > 0.0) {
					next[0] = ((res[i].next - 1) * 255) + (res2[j].next - 1);
					next[1] = 0;
					int nCode = ((res[i].next - 1) * 255) + (res2[j].next - 1);

					float prob1 = res[i].prob * 100;
					float prob2 = res2[j].prob * 100;

					//CString stroutput;
					//stroutput.Format(L"Prediction: %s - %s (%3.2f-%3.2f)", m_editInput, next, prob1, prob2);
					//pMain->AddOutputString(stroutput, false);

					m_ctrlList.InsertItem(listid, next, 0);

					strItem.Format(L"%3.2f", prob1);	strItem += L"%";
					m_ctrlList.SetItem(listid, 1, LVIF_TEXT, strItem, 0, 0, 0, NULL);	

					strItem.Format(L"%3.2f", prob2);	strItem += L"%";
					m_ctrlList.SetItem(listid, 2, LVIF_TEXT, strItem, 0, 0, 0, NULL);

					listid++;
				}
			}
		}
	}

	UpdateData(FALSE);
	delete[] cInput;
	delete next;

}

void CDlgProbilityTest::convertUnicodeToAscii(wchar_t* buf, int len, unsigned char* dstBuff)
{
	int min = 99999, max = 0;
	for (int i = 0; i < len; i++) {
		unsigned short ucode = buf[i];
		if (ucode != 10) {

			unsigned short code1 = (ucode / 255) + 1;
			unsigned short code2 = (ucode % 255) + 1;

			if ((code1 >= 255) || (code2 >= 255)) {
				break;
			}

			dstBuff[i * 2] = code1;
			dstBuff[i * 2 + 1] = code2;

			if (min > ucode)
				min = ucode;
			if (max < ucode)
				max = ucode;
		}
	}
}

void CDlgProbilityTest::decompsiteWchar(wchar_t* src, unsigned char* dst, int wlen)
{
	for (int i = 0; i < wlen; i++) {
		dst[i * 2] = (unsigned short(src[i]) / 255) + 1;
		dst[i * 2 + 1] = (unsigned short(src[i]) % 255) + 1;
	}
}

#define _MAX_BUFF_SIZE 1024
void CDlgProbilityTest::OnBnClickedBnConvert()
{
	// TODO: Add your control notification handler code here
	CString sPath;
	GetModuleFileName(nullptr, sPath.GetBuffer(_MAX_PATH + 1), _MAX_PATH);
	sPath.ReleaseBuffer();
	CString path = sPath.Left(sPath.ReverseFind(_T('\\')));
	CString strFile = path + "\\rnn\\train.txt";
	CString strFile2 = path + "\\rnn\\uni_tbl.txt";


	if (m_strTrainingText != L"") {
		FILE* fp;
		_wfopen_s(&fp, m_strTrainingText, L"rt,ccs=UTF-8");

		FILE* fpw = 0;
		const char* file = (CStringA)strFile;
		fopen_s(&fpw, file, "wb");

		bool unicodetbl[65536];
		memset(unicodetbl, 0x00, sizeof(bool) * 65536);

		if (fp) {
			wchar_t rbuf[_MAX_BUFF_SIZE];
			memset(rbuf, '\0', sizeof(wchar_t) * _MAX_BUFF_SIZE);

			// Skip first !! //
			//fseek(fp, 2, SEEK_SET);
			while (fgetws(rbuf, sizeof(wchar_t) * _MAX_BUFF_SIZE, fp) != NULL)
			{
				int len = wcslen(rbuf);
				unsigned char* chBuff = new unsigned char[len * 2];
				convertUnicodeToAscii(rbuf, len, chBuff);
				fwrite(chBuff, len*2, 1, fpw);
				delete[] chBuff;

				for (int i = 0; i < len; i++) {
					unsigned short sid = rbuf[i];
					unicodetbl[sid] = true;
				}
			}
			fclose(fpw);
			fclose(fp);


			FILE* fp2 = 0;
			const char* file2 = (CStringA)strFile2;
			fopen_s(&fp2, file2, "wb");
			if (fp2) {
				fwrite(unicodetbl, sizeof(bool) * 65536, 1, fp2);
				fclose(fp2);
			}
		}
		MessageBox(L"Training file is generated.");
	}
}


void CDlgProbilityTest::OnBnClickedBnTraining()
{
	// TODO: Add your control notification handler code here
	CString sPath;
	GetModuleFileName(nullptr, sPath.GetBuffer(_MAX_PATH + 1), _MAX_PATH);
	sPath.ReleaseBuffer();
	CString path = sPath.Left(sPath.ReverseFind(_T('\\')));
	CString strFile = path + "\\darknet.exe";
	CString strrnn =  path + "\\rnn";

	ShellExecute(
		NULL,
		_T("open"),
		strFile,
		strrnn, // params                            
		_T(" C:\ "),
		SW_SHOW);


}


void CDlgProbilityTest::OnBnClickedBnLoadRnn()
{
	// TODO: Add your control notification handler code here
	static TCHAR BASED_CODE szFilter[] = _T("Weight file(*.weights, *.weights) | *.weights;*.weights; |葛电颇老(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.weights"), _T("Weight file"), OFN_HIDEREADONLY, szFilter);

	if (IDOK == dlg.DoModal()) {
		//	SINGLETON_DataMng::GetInstance()->LoadTextFileForTree(dlg.GetPathName());
		//	MessageBox(L"A Probability map has been generated.");
		m_strRnnWeight = dlg.GetPathName();
		setRNNModel(m_strRnnWeight);
		UpdateData(FALSE);
	}
}

void CDlgProbilityTest::setRNNModel(CString strModel)
{
	CString sPath;
	GetModuleFileName(nullptr, sPath.GetBuffer(_MAX_PATH + 1), _MAX_PATH);
	sPath.ReleaseBuffer();
	CString path = sPath.Left(sPath.ReverseFind(_T('\\')));
	CString strFile = path + "\\rnn\\rnn.cfg";

	if (m_pPredictor != NULL) {
		delete m_pPredictor;
		m_pPredictor = NULL;
	}

	const char* cfgFile = (CStringA)strFile;
	const char* weightFile = (CStringA)m_strRnnWeight;
	m_pPredictor = new Detector(cfgFile, weightFile);

	MessageBox(L"RNN Model is loaded");



	
}


BOOL CDlgProbilityTest::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_ctrlList.EnableScrollBarCtrl(SB_HORZ);
	m_ctrlList.ShowScrollBar(SB_HORZ);
	m_ctrlList.SetScrollRange(SB_HORZ, 0, 2000);
	m_ctrlList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	m_ctrlList.InsertColumn(0, L"Prediction", LVCFMT_LEFT, 100);
	m_ctrlList.InsertColumn(1, L"Prob_1", LVCFMT_LEFT, 100);
	m_ctrlList.InsertColumn(2, L"Prob_1", LVCFMT_LEFT, 100);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgProbilityTest::OnBnClickedBnStyleTransfer()
{
	// TODO: Add your control notification handler code here
	FILE* fp;
	_wfopen_s(&fp, L"D:\\StyleTransfer\\testimg\\ucode.txt", L"rt,ccs=UTF-8");
	if (fp) {
		wchar_t rbuf[_MAX_BUFF_SIZE];
		memset(rbuf, '\0', sizeof(wchar_t) * _MAX_BUFF_SIZE);

		// Skip first !! //
		//fseek(fp, 2, SEEK_SET);
		std::vector<wchar_t*>vecWchar;
		while (fgetws(rbuf, sizeof(wchar_t) * _MAX_BUFF_SIZE, fp) != NULL)
		{
			int len = wcslen(rbuf)-1;
			for (int i = 0; i < len; i++) {
				wchar_t* ch = new wchar_t[4];
				memset(ch, 0x00, sizeof(wchar_t) * 4);
				ch[0] = rbuf[i];
				vecWchar.push_back(ch);
			}
		}
		fclose(fp);

		// Write //

		FILE* fp2 = 0;
		fopen_s(&fp2, "D:\\StyleTransfer\\testimg\\class00.jp3", "wb");		
		if (fp2) {
			int num = vecWchar.size();
			int clen = 4;
			fwrite(&num, sizeof(int), 1, fp);
			fwrite(&clen, sizeof(int), 1, fp);
			for (int i = 0; i < num; i++) {
				fwrite(vecWchar[i], sizeof(wchar_t)*4, 1, fp);
			}
			fclose(fp2);
		}
	}




// generate image //
	USES_CONVERSION;

	cv::Mat clsimg = cv::Mat::zeros(cv::Size(60 * 32, 60 * 32), CV_8U);

	int imgid = 9;
	int addidx = 0;
	for (int i = 0; i <20; i++) {
		CString strPng;
		strPng.Format(L"D:\\StyleTransfer\\testimg\\inferred_%04d.png", imgid);

		char* sz = 0;
		sz = T2A(strPng);
		cv::Mat srcImg = cv::imread(sz, CV_LOAD_IMAGE_GRAYSCALE);
	//	cv::imshow("read", srcImg);

		for (int j = 0; j < 160; j++) {
			cv::Mat img = getWordCutImg(j, 256, srcImg, 0, 10, 16);
			cv::resize(img, img, cv::Size(32, 32));

			cv::Rect r1 = getPositionByIndex(addidx, 32, 0, 60, 60);
			img.copyTo(clsimg(r1));
			addidx++;
		}
		imgid += 10;		
	}

//	cv::imshow("read", clsimg);
	cv::imwrite("D:\\StyleTransfer\\testimg\\class00_00.jp2", clsimg);
	cv::imwrite("D:\\StyleTransfer\\testimg\\class00_00.jpg", clsimg);


}


cv::Mat CDlgProbilityTest::getWordCutImg(int wordId, int cellSize, cv::Mat& clsImg, int clsId, int wNum, int hNum)
{
	cv::Rect r1;
	
	int wNumInClass = wNum*hNum;
	wordId = wordId - (wordId / wNumInClass)*wNumInClass;

	int w = (clsId + 1) * cellSize;
	int h = cellSize;

//	cv::Rect r1;
	r1.y = (wordId % hNum)*h;
	r1.x = (wordId / hNum)*w;
	r1.width = cellSize;
	r1.height = cellSize;
	
	
//	= getPositionByIndex(wordId, cellSize, clsId, wNum, hNum);
	cv::Mat img = cv::Mat(cellSize, cellSize, CV_8UC1, cv::Scalar(255));
	clsImg(r1).copyTo(img(cv::Rect(0, 0, cellSize, cellSize)));
	return img;
}

cv::Rect CDlgProbilityTest::getPositionByIndex(int wordId, int cellSize, int clsId, int wNum, int hNum)
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