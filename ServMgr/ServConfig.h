#pragma once

#include <Winsvc.h>

class CServItem {
public:
	CString m_strServName;
	CString m_strServDispName;
	DWORD m_dwServState;	//服务当前状态
	CString m_strBinPath;
	DWORD m_dwStartType;
	CString m_strDescription;
	CServItem *m_pNext;
	CServItem() {
		m_dwServState = 0;
		m_pNext = NULL;
	}
};

class CServConfig
{
public:
	CServConfig();
	~CServConfig();

public:
	CServItem *EnumServList();
	CString GetStateString(DWORD dwCurrState);
	BOOL GetServPathAndStartType(LPCTSTR lpszServName, CServItem &tItem);
	CString GetStartTypeString(DWORD dwStartType);
	CString GetServDescription(LPCTSTR lpszServName);
};

