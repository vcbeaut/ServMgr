#include "stdafx.h"
#include "ServConfig.h"


CServConfig::CServConfig()
{
}


CServConfig::~CServConfig()
{
}

CServItem *CServConfig::EnumServList()
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCM) {
		return NULL;
	}
	CServItem *pServHeader = NULL, *pServPre = NULL, *pServNext = NULL;
	LPENUM_SERVICE_STATUS pServStatus = NULL;
	DWORD dwBytesNeeded = 0, dwServCound = 0, dwResume = 0, dwRealBytes = 0;
	BOOL bRet = EnumServicesStatus(hSCM, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &dwBytesNeeded, &dwServCound, &dwResume);
	if (!bRet && GetLastError() == ERROR_MORE_DATA) {
		dwRealBytes = dwBytesNeeded;
		pServStatus = new ENUM_SERVICE_STATUS[dwRealBytes + 1];
		ZeroMemory(pServStatus, dwRealBytes + 1);
		bRet = EnumServicesStatus(hSCM, SERVICE_WIN32, SERVICE_STATE_ALL, pServStatus, dwRealBytes, &dwBytesNeeded, &dwServCound, &dwResume);
		if (!bRet) {
			CloseServiceHandle(hSCM);
			return NULL;
		}
	}
	else{
		CloseServiceHandle(hSCM);
		return NULL;
	}
	pServPre = pServNext;
	for (DWORD dwIdx = 0; dwIdx < dwServCound; dwIdx++) {
		pServNext = new CServItem;
		pServNext->m_strServName = pServStatus[dwIdx].lpServiceName;
		pServNext->m_strServDispName = pServStatus[dwIdx].lpDisplayName;
		pServNext->m_dwServState = pServStatus[dwIdx].ServiceStatus.dwCurrentState;
		GetServPathAndStartType(pServNext->m_strServName, *pServNext);
		pServNext->m_strDescription = GetServDescription(pServNext->m_strServName);
		(pServHeader == NULL) ? (pServHeader = pServNext) : pServHeader;
		(pServPre == NULL) ? (pServPre = pServNext) : (pServPre->m_pNext = pServNext, pServPre = pServNext);
	}
	CloseServiceHandle(hSCM);
	delete[] pServStatus;
	return pServHeader;
}


CString CServConfig::GetStateString(DWORD dwCurrState)
{
	CString strResult;
	switch (dwCurrState)
	{
	case SERVICE_START_PENDING:
		strResult = _T("正在启动");
		break;
	case SERVICE_STOP_PENDING:
		strResult = _T("正在停止");
		break;
	case SERVICE_RUNNING:
		strResult = _T("已启动");
		break;
	case SERVICE_CONTINUE_PENDING:
		strResult = _T("继续中");
		break;
	case SERVICE_PAUSE_PENDING:
		strResult = _T("暂停中");
		break;
	case SERVICE_PAUSED:
		strResult = _T("暂停");
		break;
	case SERVICE_STOPPED:
	default:break;
	}
	return strResult;
}


BOOL CServConfig::GetServPathAndStartType(LPCTSTR lpszServName, CServItem &tItem)
{
	BOOL bRet = FALSE;
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL){
		return FALSE;
	}
	SC_HANDLE hSvc = OpenService(hSCM, lpszServName, SERVICE_QUERY_CONFIG);
	if (hSvc == NULL){
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	// for the QueryServiceConfig;
	QUERY_SERVICE_CONFIG *pServCfg = NULL;
	DWORD cbBytesNeeded = 0, cbBufferSize = 0;
	bRet = QueryServiceConfig(hSvc, NULL, 0, &cbBytesNeeded);
	if (bRet == FALSE){
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER){
			pServCfg = new QUERY_SERVICE_CONFIG[cbBytesNeeded + 1];
			ZeroMemory(pServCfg, cbBytesNeeded + 1);
			cbBufferSize = cbBytesNeeded;
			cbBytesNeeded = 0;
		}
		else{
			goto __Error_End;
		}
	}
	bRet = QueryServiceConfig(hSvc, pServCfg, cbBufferSize, &cbBytesNeeded);
	if (bRet == FALSE){
		goto __Error_End;
	}
	tItem.m_strBinPath = pServCfg->lpBinaryPathName;
	tItem.m_dwStartType = pServCfg->dwStartType;
	bRet = TRUE;
__Error_End:
	CloseServiceHandle(hSvc);
	CloseServiceHandle(hSCM);
	return bRet;
}

CString CServConfig::GetStartTypeString(DWORD dwStartType)
{
	CString strResult;
	switch (dwStartType)
	{
	case SERVICE_AUTO_START:
		strResult = _T("自动");
		break;
	case SERVICE_DEMAND_START:
		strResult = _T("手动");
		break;
	case SERVICE_DISABLED:
		strResult = _T("已禁用");
		break;
	default:
		strResult = _T("未知");
		break;
	}
	return strResult;
}

CString CServConfig::GetServDescription(LPCTSTR lpszServName)
{
	CString strResult;
	BOOL bRet = FALSE;
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL){
		return strResult;
	}
	SC_HANDLE hSvc = OpenService(hSCM, lpszServName, SERVICE_QUERY_CONFIG);
	if (hSvc == NULL){
		CloseServiceHandle(hSCM);
		return strResult;
	}
	//for the QueryServiceConfig2;
	DWORD dwNeeded = 0, dwLen = 0;
	LPSERVICE_DESCRIPTION pDescripTion = NULL;
	bRet = QueryServiceConfig2(hSvc, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &dwNeeded);
	if (!bRet && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		dwLen = dwNeeded + 1;
		pDescripTion = new SERVICE_DESCRIPTION[dwLen];
		ZeroMemory(pDescripTion, dwLen);
		if (QueryServiceConfig2(hSvc, SERVICE_CONFIG_DESCRIPTION, (LPBYTE)pDescripTion, dwLen, &dwNeeded)) {
			strResult = pDescripTion->lpDescription;
		}
	}
	CloseServiceHandle(hSvc);
	CloseServiceHandle(hSCM);
	return strResult;
}