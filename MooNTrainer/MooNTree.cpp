#include "stdafx.h"
#include "MooNTree.h"



CMooNTree::CMooNTree()
{
}


CMooNTree::~CMooNTree()
{
	ReleaseDataSet();
}

void CMooNTree::ReleaseDataSet()
{
	std::map<unsigned short, CMooNTreeItem*>::iterator iter = m_treeItem.begin();
	for (; iter != m_treeItem.end(); ++iter) {
		delete iter->second;
		iter->second = nullptr;
	}
	m_treeItem.swap(std::map<unsigned short, CMooNTreeItem*>());
}

bool CMooNTree::isVaildCode(unsigned short scode)
{
	if ((scode == 0xFEFF) || (scode <= 0x000A)) // Skip Enter, NULL Line, Tab, Etc
		return false;
	return true;
}

void CMooNTree::ProcessString(wchar_t* _string, int len)
{
	for (int i = 0; i < len; ++i) {
		unsigned short rootCode = _string[i];
		if (isVaildCode(rootCode)) {
			if (m_treeItem.find(rootCode) == m_treeItem.end()) {
				CMooNTreeItem* tItem = new CMooNTreeItem;
				m_treeItem[rootCode] = tItem;
			}

			for (int level = 0; level < _CHARTREE_DEPTH; level++) {
				int id = i + 1 + level;
				if (id >= len)
					break;

				unsigned short childCode = _string[id];
				if (isVaildCode(childCode)) {
					m_treeItem[rootCode]->AddChEntity(level, childCode);
				}
			}
		}
	}
}

void CMooNTree::GetNextChar(wchar_t* input, std::vector<_stMooNProb>& vecRes)
{
	if (m_treeItem.find(input[0]) != m_treeItem.end()) {
		m_treeItem[input[0]]->GetNextChar(input, vecRes);
	}
}