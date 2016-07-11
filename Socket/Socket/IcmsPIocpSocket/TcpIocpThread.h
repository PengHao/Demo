#pragma once
#include "iCMS_PIocpsocket.h"
#include "InnerStruct.h"

class XhTcpConnect;
class CTcpIocpThread
{
public:
	static CTcpIocpThread* GetTcpIocp();
	int  InitWinSocketEvnt();
	int  StopThread();
	int  PushIoContext(HANDLE hSocket,ULONG_PTR pIoContext);
	int  RemoveIoContext(HANDLE hSocket,ULONG_PTR pIoContext,BOOL bNeedDelete = FALSE);
	XhTcpConnect*	GetConnectInstance();
	int  RemoveConnectInstance(XhTcpConnect*);
	int	 PostQueuedCompletionMsg(XhTcpConnect* pCon,int nMessageID);

private:
	CTcpIocpThread(void);
	~CTcpIocpThread(void);
	static DWORD __stdcall IocpThread_(void* pParam);
	DWORD IocpThread();

	static UINT __stdcall CheckConnectionThread_(void* pParam);
	UINT CheckConnectionThread();


	HANDLE							m_hCheckThreadHandle;
	HANDLE							m_iocpHandle;
	HANDLE*							m_pThreadHandle;
	DWORD							m_dwIocpThreadCnt;
	//HANDLE							m_iocpThread;
	BOOL							m_bQuitThread;
	iCMSP_SimpleCS					m_simpleCS;
	map<ULONG_PTR,HANDLE>			m_contextMap;
	list<XhTcpConnect*>				m_cachedConnect;
	DWORD							m_dwLastCheckConnect;
	iCMSP_SimpleCS					m_visitLastCheckConnect;
};
