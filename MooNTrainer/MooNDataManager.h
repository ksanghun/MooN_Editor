#pragma once
#include "MNSingleton.h"
#include "MooNTree.h"
class CMooNDataManager
{
public:
	CMooNDataManager();
	~CMooNDataManager();

	void InitManager();
	void LoadTextFileForTree(CString strPath);
	void PredictNext(wchar_t* input, std::vector<_stMooNProb>& vecRes);

private:
	CMooNTree m_moonTree;
};
typedef CMNSingleton<CMooNDataManager> SINGLETON_DataMng;
