#include "stdafx.h"
#include "TcpIocpThread.h"
#include "XhTcpConnect.h"

CTcpIocpThread::CTcpIocpThread(void)
:m_iocpHandle(NULL),m_bQuitThread(FALSE),m_pThreadHandle(NULL),m_dwIocpThreadCnt(1),m_dwLastCheckConnect(0),m_hCheckThreadHandle(NULL)
{
	InitWinSocketEvnt();
}

CTcpIocpThread::~CTcpIocpThread(void)
{
	StopThread();
	WSACleanup();
}

CTcpIocpThread*	CTcpIocpThread::GetTcpIocp()
{
	static CTcpIocpThread	g_singleCTcpIocpThread;
	return &g_singleCTcpIocpThread;
}

int  CTcpIocpThread::InitWinSocketEvnt()
{
	//foo function,nothing to do,just init the global variable

	STRY;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);

	TCHAR chFileName[4096] = {0};
	::_stprintf (chFileName,_T("%s\\data\\localSet.ini"),GetWorkingDirectory());
	DWORD dwCpuMultiCast = ::GetPrivateProfileInt (_T("iocp"),_T("thread_cnt"),4,chFileName);

	WSADATA wsaData = {0};
	WSAStartup(0x202,&wsaData);

	if (NULL == m_iocpHandle )
	m_iocpHandle = ::CreateIoCompletionPort (INVALID_HANDLE_VALUE,NULL,0,0);
	if (NULL == m_pThreadHandle)
	{
		SYSTEM_INFO	systemInfo = {0};
		GetSystemInfo(&systemInfo);
		DWORD dwIocpThreadCnt = systemInfo.dwNumberOfProcessors * dwCpuMultiCast;
		m_dwIocpThreadCnt   = dwIocpThreadCnt; 
		//m_dwIocpThreadCnt = 2;
		m_pThreadHandle     = new HANDLE [dwIocpThreadCnt];
		while (m_pThreadHandle && dwIocpThreadCnt)
		{
			DWORD dwThreadId = 0;
			m_pThreadHandle[--dwIocpThreadCnt] = CreateThread(NULL,0,IocpThread_,this,0,&dwThreadId);
		}
	}

	if(!m_hCheckThreadHandle)
	{
		m_hCheckThreadHandle = (HANDLE)_beginthreadex(NULL,0,CheckConnectionThread_,this,0,0);
	}
	return 1;
	SCATCH;
	return -1;
}



DWORD CTcpIocpThread::IocpThread_(void* pParam)
{
	STRY;
	CTcpIocpThread* pSelf = (CTcpIocpThread*)pParam;
	if (NULL == pSelf /*|| ::IsBadReadPtr(pSelf,sizeof(CTcpIocpThread))*/)
		return -1;
	return pSelf ->IocpThread();
	SCATCH;
	return -1;
}

DWORD CTcpIocpThread::IocpThread()
{
	STRY;
	IOCP_OVERLAPPED* pIocpOverlapped	= NULL;
	IIocpHandle*	 lpCompletionKey	= NULL;
	BOOL	bGetQueueStaus				= FALSE;
	DWORD	numberOfBytes				= 0;
	TTRACE("thread[%s:%d] started!\r\n",__FUNCTION__,::GetCurrentThreadId ());
	while (FALSE == m_bQuitThread)
	{
		STRY;
		bGetQueueStaus = GetQueuedCompletionStatus (m_iocpHandle,&numberOfBytes
			,(DWORD*)&lpCompletionKey,(LPOVERLAPPED*)&pIocpOverlapped,INFINITE);
		if ( TRUE == m_bQuitThread )
			break;

		if (NULL == lpCompletionKey || NULL == pIocpOverlapped )
			continue;
		SCATCH;

		STRY;
		/* !!!just copy from msdn !!!
		If a socket handle associated with a completion port is closed, 
		GetQueuedCompletionStatus returns ERROR_SUCCESS, 
		with *lpOverlapped non-NULL and lpNumberOfBytes equal zero.	*/
		DWORD lastErrorCode = WSAGetLastError();
		BOOL bHasDeleted = FALSE;
		STRY;
		iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
		map<ULONG_PTR,HANDLE>::iterator iterFind = m_contextMap.find ((ULONG_PTR)lpCompletionKey);
		if (iterFind == m_contextMap.end ())
			bHasDeleted = TRUE;
		SCATCH;
		if (ERROR_SUCCESS == bGetQueueStaus &&  0 == numberOfBytes && lpCompletionKey && pIocpOverlapped && !bHasDeleted)
		{
			if (ERROR_OPERATION_ABORTED != lastErrorCode && (int)(pIocpOverlapped ->IoDirection) < 4)
				lpCompletionKey ->OnGotError (pIocpOverlapped,lastErrorCode);
		}
		else if (-0x17 == (int)numberOfBytes && lpCompletionKey && !bHasDeleted)
		{
			STRY;
			iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
			map<ULONG_PTR,HANDLE>::iterator iterFind = m_contextMap.find ((ULONG_PTR)lpCompletionKey);
			if (iterFind != m_contextMap.end ())
			{
				XhTcpConnect* pCon = dynamic_cast<XhTcpConnect*>(lpCompletionKey);
				if(pCon)
				{
					if(pCon->m_bIsServerSide)
					{
						if(pCon->GetUseRefCount() == 0)
						{
							STRY;
							m_contextMap.erase(iterFind);
							TCHAR chAddr[32] = {0};
							pCon->GetPeerAddr(chAddr);
							delete pCon;
							//TTRACE("connnect ptr:%p peer addr:%s is stop connected,the connect object has left:%d\r\n",lpCompletionKey,chAddr,m_contextMap.size());
							SCATCH;
						}
					}
					else
						m_contextMap.erase(iterFind);
				}
			}
			SCATCH;
		}
		else if(-0x18 == (int)numberOfBytes && lpCompletionKey && !bHasDeleted)//Need Stop the Connect
		{
			iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
			map<ULONG_PTR,HANDLE>::iterator iterFind = m_contextMap.find ((ULONG_PTR)lpCompletionKey);
			if (iterFind != m_contextMap.end ())
			{
				XhTcpConnect* pCon = dynamic_cast<XhTcpConnect*>(lpCompletionKey);
				if(pCon && pCon->m_bIsServerSide)
				{
					pCon->StopConnect();
				}
			}
		}
		else if(-0x19 == (int)numberOfBytes && lpCompletionKey && !bHasDeleted)//Need Stop the Connect
		{
			iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
			map<ULONG_PTR,HANDLE>::iterator iterFind = m_contextMap.find ((ULONG_PTR)lpCompletionKey);
			if (iterFind != m_contextMap.end ())
			{
				XhTcpConnect* pCon = dynamic_cast<XhTcpConnect*>(lpCompletionKey);
				if(pCon)
				{
					pCon->StopConnect(false);
				}
			}
		}
		else if(-0x20 == (int)numberOfBytes && lpCompletionKey && !bHasDeleted)
		{
			XhTcpConnect* pCon = NULL;
			STRY;
			iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
			map<ULONG_PTR,HANDLE>::iterator iterFind = m_contextMap.find ((ULONG_PTR)lpCompletionKey);
			if (iterFind != m_contextMap.end ())
			{
				pCon = dynamic_cast<XhTcpConnect*>(lpCompletionKey);
				if(pCon)
				{
					pCon->AddUseRef();
					//pCon->CheckConnect(GetTickCount());
				}
			}
			SCATCH;
			STRY;
			if(pCon)
			{
				pCon->CheckConnect(GetTickCount());
				pCon->Release(__FUNCTION__);
			}
			SCATCH;
		}
		else if (lpCompletionKey != NULL && pIocpOverlapped && !bHasDeleted)
		{
			if ( 0 == numberOfBytes)
			{
				if(ERROR_OPERATION_ABORTED != lastErrorCode && ERROR_IO_PENDING != lastErrorCode)
				{
					iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
					map<ULONG_PTR,HANDLE>::iterator iterFind = m_contextMap.find ((ULONG_PTR)lpCompletionKey);
					if (iterFind != m_contextMap.end ())
					{
						lpCompletionKey ->OnGotError (pIocpOverlapped,lastErrorCode);
					}
				}
			}
			else
			{
				XhTcpConnect* pCon = NULL;
				BOOL bIsServerSide = FALSE;
				STRY;
				iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
				map<ULONG_PTR,HANDLE>::iterator iterFind = m_contextMap.find ((ULONG_PTR)lpCompletionKey);
				if(iterFind != m_contextMap.end())
				{
					pCon = dynamic_cast<XhTcpConnect*>(lpCompletionKey);
					if(pCon)
					{
						bIsServerSide = pCon->m_bIsServerSide;
						if(pCon->AddUseRef() < 1)
							pCon = NULL;
					}
				}
				SCATCH;

				if(!bIsServerSide)
				{
					STRY;
					iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
					map<ULONG_PTR,HANDLE>::iterator iterFind = m_contextMap.find ((ULONG_PTR)lpCompletionKey);
					if(iterFind != m_contextMap.end())
					{
						pCon = dynamic_cast<XhTcpConnect*>(lpCompletionKey);
						//TTRACE("%s recv the data want to call pCon the ptr:0x%p\r\n",__FUNCTION__,pCon);
						if(pCon)
							pCon->DealIocpAffair(pIocpOverlapped,numberOfBytes);
					}
					SCATCH;
				}
				else
				{
					if(pCon)
					{
						STRY;
						pCon ->DealIocpAffair(pIocpOverlapped,numberOfBytes);
						SCATCH;
					}
				}
				if(pCon)
				{
					STRY;
					iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
					map<ULONG_PTR,HANDLE>::iterator iterFind = m_contextMap.find ((ULONG_PTR)lpCompletionKey);
					if(iterFind != m_contextMap.end())
					{
						pCon->Release(__FUNCTION__);
					}
					SCATCH;
				}
			}
		}
		SCATCH;
	}
	//TTRACE("thread[%s:%d] exited!\r\n",__FUNCTION__,::GetCurrentThreadId ());
	return 1;
	SCATCH;
	return -1;
}

int  CTcpIocpThread::StopThread()
{
	STRY;
	m_bQuitThread = TRUE;

	DWORD tempThreadCnt = m_dwIocpThreadCnt;
	DWORD status = WAIT_OBJECT_0;

	if (m_pThreadHandle)
	{
		while (tempThreadCnt -- > 0)
		PostQueuedCompletionStatus(m_iocpHandle,0,NULL,NULL);

		iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
		status = ::WaitForMultipleObjects (m_dwIocpThreadCnt,m_pThreadHandle,TRUE,10000);

		SAFE_CLOSE_HANDLE(m_iocpHandle);
		tempThreadCnt = m_dwIocpThreadCnt;
		while (tempThreadCnt -- > 0)
			CloseHandle(m_pThreadHandle[tempThreadCnt]);

		delete []m_pThreadHandle;
		m_pThreadHandle = NULL;
	}

	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);

// 	list<XhTcpConnect*>::iterator iterFirst = m_cachedConnect.begin ();
// 	while (iterFirst != m_cachedConnect.end ())
// 	{
// 		XhTcpConnect* pConnect = *iterFirst;
// 		delete pConnect;
// 		++ iterFirst;
// 	}
// 	m_cachedConnect.clear();

	return WAIT_OBJECT_0 == status ? 1 : -2;
	SCATCH;
	return 0;
}

int  CTcpIocpThread::PushIoContext(HANDLE hSocket,ULONG_PTR pIoContext)
{
	if (INVALID_SOCKET == (SOCKET)hSocket || NULL == pIoContext )
		return -1;

	STRY;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
	m_contextMap[pIoContext] = hSocket;

	m_iocpHandle	 = ::CreateIoCompletionPort (hSocket,m_iocpHandle,pIoContext,0);
	SCATCH;
	return 1;
}

int  CTcpIocpThread::RemoveIoContext(HANDLE hSocket,ULONG_PTR pIoContext,BOOL bNeedDelete)
{
	STRY;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
	if (NULL == hSocket && NULL == pIoContext)
	{
		return StopThread();
	}

	map<ULONG_PTR,HANDLE>::iterator iterFind = m_contextMap.find (pIoContext);
	if (iterFind == m_contextMap.end ())
		return 1;

	XhTcpConnect* pTcp = (XhTcpConnect*)pIoContext;
	if(bNeedDelete)
	{
		TTRACE("%s want delete the ioContext info map item, the obj ptr:0x%p\r\n",__FUNCTION__,pTcp);
		m_contextMap.erase (iterFind);
	}
	else
	{
		TTRACE("%s want post delete the ioContext info the obj ptr:0x%p\r\n",__FUNCTION__,pTcp);
		PostQueuedCompletionStatus(m_iocpHandle,-0x17,pIoContext,(LPOVERLAPPED)&pTcp->m_recvOverlapped);
	}
	SCATCH;
	return 1;
}

XhTcpConnect*	CTcpIocpThread::GetConnectInstance()
{
	STRY;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);

	XhTcpConnect* pResultConnect = NULL;
// 	if (m_cachedConnect.size ())
// 	{
// 		list<XhTcpConnect*>::iterator iterFirst = m_cachedConnect.begin ();
// 		pResultConnect = *iterFirst;
// 		m_cachedConnect.erase (iterFirst);
// 		return pResultConnect;
// 	}

	TTRACE("%s alloc the XhTcpConnect object\r\n",__FUNCTION__);
	pResultConnect = new XhTcpConnect();
	return pResultConnect;
	SCATCH;
	return 0;
}

int  CTcpIocpThread::RemoveConnectInstance(XhTcpConnect* pConnect)
{
	STRY;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);

	if (NULL == pConnect /*|| ::IsBadReadPtr (pConnect,sizeof(XhTcpConnect))*/)
		return 0;

// 	pConnect->StopConnect (false);
// 	m_cachedConnect.push_back(pConnect);
	return m_cachedConnect.size ();
	SCATCH;
	return 0;
}

UINT __stdcall CTcpIocpThread::CheckConnectionThread_( void* pParam )
{
	if(!pParam)
		return -2;
	CTcpIocpThread* pThread = (CTcpIocpThread*)pParam;
	return pThread->CheckConnectionThread();
}

UINT CTcpIocpThread::CheckConnectionThread()
{
	while(m_bQuitThread == FALSE)
	{
		STRY;
		iCMSP_SimpleLock lockLastcheckConnect(&m_visitLastCheckConnect,__FUNCTION__);
		if(m_dwLastCheckConnect == 0)
		{
			m_dwLastCheckConnect = GetTickCount();
		}
		else
		{
			DWORD dwTicket = GetTickCount();
			if((dwTicket - m_dwLastCheckConnect) > CHECK_CONNECTION_TIME_SPAN) //30秒检查一次心跳
			{
				STRY;
				iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
				TTRACE("Now there has %d connect objects!\r\n",m_contextMap.size());
				for(map<ULONG_PTR,HANDLE>::iterator itor = m_contextMap.begin();itor!=m_contextMap.end();itor++)
				{
					PostQueuedCompletionMsg((XhTcpConnect*)itor->first,-0x20);
				}
				SCATCH;
// 				map<ULONG_PTR,HANDLE> contextMap;
// 				STRY;
// 				iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
// 				for(map<ULONG_PTR,HANDLE>::iterator itor = m_contextMap.begin();itor!=m_contextMap.end();itor++)
// 				{
// 					IIocpHandle* pConnect = ((IIocpHandle*)(itor->first));
// 					if(pConnect)
// 					{
// 						DWORD dwTicket1 = pConnect->GetLastRecvDataTS();
// 						if(dwTicket > dwTicket1)
// 						{
// 							if((dwTicket - dwTicket1) > CHECK_CONNECTION_TIME_SPAN)
// 							{
// 								contextMap[itor->first] = itor->second;
// 							}
// 						}
// 					}
// 				}
// 				SCATCH;
// 				for(map<ULONG_PTR,HANDLE>::iterator itor = contextMap.begin();itor != contextMap.end();itor++)
// 				{
// 					iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
// 					map<ULONG_PTR,HANDLE>::iterator itor1 = m_contextMap.find(itor->first);
// 					if(itor1 == m_contextMap.end())
// 						continue;
// 					IIocpHandle* pConnect = ((IIocpHandle*)(itor->first));
// 					if(pConnect)
// 					{
// 						pConnect->CheckConnect(dwTicket);
// 					}
// 				}
				m_dwLastCheckConnect = dwTicket;
			}
		}
		SCATCH;
		Sleep(1000);
	}
	CloseHandle(m_hCheckThreadHandle);
	m_hCheckThreadHandle = NULL;
	return 1;
}

int CTcpIocpThread::PostQueuedCompletionMsg( XhTcpConnect* pCon,int nMessageID )
{
	return PostQueuedCompletionStatus(m_iocpHandle,nMessageID,(ULONG_PTR)pCon,(LPOVERLAPPED)&pCon->m_recvOverlapped);
}
