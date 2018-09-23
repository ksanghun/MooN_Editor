#pragma once


typedef struct _stMooNTreeEntity {
	std::map<unsigned short, unsigned short> nextEntity;
}_stMooNTreeEntity;

typedef struct _stMooNProb {
	unsigned short code;
	float prob;
}_stMooNProb;

#define _CHARTREE_DEPTH 10
class CMooNTreeItem
{
public:
	CMooNTreeItem();
	~CMooNTreeItem();
	void AddChEntity(int level, unsigned short code);
	void GetNextChar(wchar_t* input, std::vector<_stMooNProb>& vecRes);
private:
	_stMooNTreeEntity m_vecForward[_CHARTREE_DEPTH];
	void SearchNext(wchar_t* _str, int& level, int length, std::vector<_stMooNProb>& vecRes);
};

