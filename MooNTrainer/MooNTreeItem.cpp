#include "stdafx.h"
#include "MooNTreeItem.h"

CMooNTreeItem::CMooNTreeItem()
{
}


CMooNTreeItem::~CMooNTreeItem()
{
}

void CMooNTreeItem::AddChEntity(int level, unsigned short code)
{
	if (m_vecForward[level].nextEntity.find(code) == m_vecForward[level].nextEntity.end()) {
		m_vecForward[level].nextEntity[code] = 1;
	}
	else {
		m_vecForward[level].nextEntity[code]++;
	}
}


void CMooNTreeItem::GetNextChar(wchar_t* input, std::vector<_stMooNProb>& vecRes)
{
	int len = wcslen(input);
	int level = 0;

	std::vector<_stMooNProb> tmpRes;
	SearchNext(input, level, len, tmpRes);

	// sorting //
	if (tmpRes.size() > 1) {
		float totalCnt = 0;
		auto i = 0;
		for (i = 0; i < tmpRes.size() - 1; i++) {
			totalCnt += tmpRes[i].prob;
			for (auto j = i + 1; j < tmpRes.size(); j++) {
				if (tmpRes[i].prob < tmpRes[j].prob) {
					_stMooNProb tmp = tmpRes[i];
					tmpRes[i] = tmpRes[j];
					tmpRes[j] = tmp;
				}
			}
		}
		totalCnt += tmpRes[i].prob;

		// Add Result with percentage
		i = 0;
		for (i = 0; i < tmpRes.size(); i++) {
			_stMooNProb res = tmpRes[i];
			res.prob /= totalCnt;
			vecRes.push_back(res);
		}
	}

}

void CMooNTreeItem::SearchNext(wchar_t* _str, int& level, int length, std::vector<_stMooNProb>& vecRes)
{
	if (level < length-1) {
		unsigned short code = _str[level+1];
		if (m_vecForward[level].nextEntity.find(code) != m_vecForward[level].nextEntity.end()) {
			level++;
			SearchNext(_str, level, length, vecRes);
		}
	}
	else {		// End of recursive call //
	//	unsigned short code = _str[level];
	//	if (m_vecForward[level].nextEntity.find(code) != m_vecForward[level].nextEntity.end()) {						
			std::map<unsigned short, unsigned short>::iterator iter = m_vecForward[level].nextEntity.begin();
			for (; iter != m_vecForward[level].nextEntity.end(); ++iter) {
				_stMooNProb nextChar;
				nextChar.code = iter->first;
				nextChar.prob = iter->second;
				vecRes.push_back(nextChar);
			}			
	//	}
	}
}