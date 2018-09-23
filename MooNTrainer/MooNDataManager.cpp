#include "stdafx.h"
#include "MooNDataManager.h"
#include "MainFrm.h"

CMooNDataManager::CMooNDataManager()
{
}


CMooNDataManager::~CMooNDataManager()
{
}

void CMooNDataManager::InitManager()
{

}

#define _MAX_BUFF_SIZE 1024
void CMooNDataManager::LoadTextFileForTree(CString strPath)
{
	m_moonTree.ReleaseDataSet();
	wchar_t* sz = strPath.GetBuffer();
	FILE* fp;// = _wfopen_s(sz, L"rt,ccs=UTF-8");
	_wfopen_s(&fp, sz, L"rt,ccs=UTF-8");

	if (fp) {
		WCHAR rbuf[_MAX_BUFF_SIZE];
		memset(rbuf, '\0', sizeof(WCHAR) * _MAX_BUFF_SIZE);

		fseek(fp, 0, SEEK_SET);
		while (fgetws(rbuf, sizeof(WCHAR) * _MAX_BUFF_SIZE, fp) != NULL)
		{			
			int len = wcslen(rbuf);
			m_moonTree.ProcessString(rbuf, len);
			memset(rbuf, '\0', sizeof(WCHAR) * _MAX_BUFF_SIZE);
		}
		fclose(fp);
	}
}

void CMooNDataManager::PredictNext(wchar_t* input, std::vector<_stMooNProb>& vecRes)
{
	m_moonTree.GetNextChar(input, vecRes);
}