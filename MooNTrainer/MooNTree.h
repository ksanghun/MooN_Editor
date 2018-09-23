#pragma once

#include "MooNTreeItem.h"
class CMooNTree
{
public:
	CMooNTree();
	~CMooNTree();

	void ReleaseDataSet();
	void ProcessString(wchar_t* _string, int len);
	void GetNextChar(wchar_t* input, std::vector<_stMooNProb>& vecRes);

private:
	bool isVaildCode(unsigned short scode);
	std::map<unsigned short, CMooNTreeItem*> m_treeItem;
};

