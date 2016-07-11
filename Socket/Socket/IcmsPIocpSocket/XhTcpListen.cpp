#include "stdafx.h"
#include "XhTcpListen.h"
#include "TcpIocpThread.h"
#include "XhTcpConnect.h"

XhTcpListen::XhTcpListen(void)
:m_pTcpSink(NULL),m_localIp(0),m_localPort(0),m_listenSocket(INVALID_SOCKET)
,m_bUseAcceptEx(FALSE),m_bQuitThread(TRUE),m_bNeedCheckHearder(TRUE)
{
	//CTcpIocpThread::GetTcpIocp () ->InitWinSocketEvnt();
	TCHAR chFileName[4096] = {0};
	::_stprintf (chFileName,_T("%s\\data\\localSet.ini"),GetWorkingDirectory());
	m_bUseAcceptEx  = ::GetPrivateProfileInt (_T("iocp"),_T("use_acceptx"),0,chFileName);
}

XhTcpListen::~XhTcpListen(void)
{
	STRY;
	StopListen();

	XhTcpConnect *pConnector = NULL;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
	map<UINT64,XhTcpConnect*>::iterator itor = m_passiveConnetedMap.begin();
	for(; itor != m_passiveConnetedMap.end(); itor++)
	{
		pConnector = itor->second;
		if( pConnector )
		{
			pConnector->SetCallBackSink(NULL);
			pConnector->Release(false);
		}
	}
	m_passiveConnetedMap.clear();
	SCATCH;
}

int XhTcpListen::SetCallBackSink(ITcpCallbackSink* pSink)
{
	m_pTcpSink = pSink;
	return 1;
}

int XhTcpListen::DoListenOn(long localIp,short localPort)
{
	STRY;
	StopListen();

	m_localIp		= localIp;
	m_localPort		= localPort;
	
	m_listenSocket	= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED); 
	if (INVALID_SOCKET == m_listenSocket)
		return -1;

	struct sockaddr_in localAddr	= {AF_INET};
	localAddr.sin_addr.S_un .S_addr = htonl(m_localIp);
	localAddr.sin_port				= htons(m_localPort);

	static int RE_USE_ADDR = 1;
	setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR,(char *)&RE_USE_ADDR, sizeof(RE_USE_ADDR));

	if (SOCKET_ERROR == bind(m_listenSocket,(const sockaddr*)&localAddr,sizeof(struct sockaddr_in)) )
	{
		TTRACE ("bind to  [%d.%d.%d.%d :%d ] failed [%d]\r\n"
			,localAddr.sin_addr.S_un.S_un_b.s_b1 ,localAddr.sin_addr.S_un.S_un_b.s_b2
			,localAddr.sin_addr.S_un.S_un_b.s_b3,localAddr.sin_addr.S_un.S_un_b.s_b4
			,ntohs(localAddr.sin_port ),::WSAGetLastError ());
		return -2;
	}

	listen(m_listenSocket,5);

	TTRACE ("bind to  [%d.%d.%d.%d :%d ] ok!\r\n"
		,localAddr.sin_addr.S_un.S_un_b.s_b1 ,localAddr.sin_addr.S_un.S_un_b.s_b2
		,localAddr.sin_addr.S_un.S_un_b.s_b3,localAddr.sin_addr.S_un.S_un_b.s_b4,ntohs(localAddr.sin_port ));

	if (TRUE == m_bUseAcceptEx)
	{
		m_recvOverlapped.IoDirection = IOCP_OVERLAPPED::IO_ACCEPT;
		CTcpIocpThread::GetTcpIocp () ->PushIoContext ((HANDLE)m_listenSocket,(ULONG_PTR)this);
		return DoAcceptSocket();
	}
	else
	{
		m_bQuitThread		= FALSE;
		DWORD dwThreadId	= 0;
		m_hListenThread		= CreateThread(NULL,0,Static_ListenThread,this,0,&dwThreadId);
		return INVALID_HANDLE_VALUE != m_hListenThread;
	}
	SCATCH;
	return 0;
}

int XhTcpListen::DoAcceptSocket()
{
	STRY;
	SOCKET sAccept = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == sAccept)
		return -1;

	m_recvOverlapped.userCookie = (DWORD)sAccept;
	DWORD byteReceed = 0;


	LPFN_ACCEPTEX lpfnAcceptEx	= NULL;
	GUID GuidAcceptEx			= WSAID_ACCEPTEX;
	DWORD dwBytes				= 0; 
	WSAIoctl(m_listenSocket,SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx)
		,&lpfnAcceptEx, sizeof(lpfnAcceptEx), &dwBytes,NULL,   NULL);

	int nRet = 0;
	if (lpfnAcceptEx)
	{
		nRet = lpfnAcceptEx(m_listenSocket,(SOCKET)m_recvOverlapped.userCookie
			,(LPVOID)(&m_recvOverlapped.Buffer.buf),0,sizeof(SOCKADDR_IN) + 16
			,sizeof(SOCKADDR_IN) + 16,&byteReceed,(LPOVERLAPPED)&m_recvOverlapped);
	}
	else 
	{
		nRet = AcceptEx(m_listenSocket,(SOCKET)m_recvOverlapped.userCookie
			,(LPVOID)m_recvOverlapped.Buffer.buf,0,sizeof(SOCKADDR_IN) + 16
			,sizeof(SOCKADDR_IN) + 16,&byteReceed,(LPOVERLAPPED)&m_recvOverlapped);
	}

	if (SOCKET_ERROR == nRet && (ERROR_IO_PENDING != WSAGetLastError())) 
	{
		TTRACE("%s fault:%d\r\n",__FUNCTION__,::WSAGetLastError ());
		return -2;
	}
	return 1 ;
	SCATCH;
	return -1;
}

int XhTcpListen::SendData(UINT64 toAddr,BYTE* pData,int dataLen)
{
	STRY;	
	if (-1 == toAddr)
	{
		STRY;
		iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
		for_each(m_passiveConnetedMap.begin (),m_passiveConnetedMap.end ()
			,XhTcpConnect::DoAllSend<UINT64,XhTcpConnect*>(pData,dataLen));
		return dataLen;
		SCATCH;
	}
	else
	{
		XhTcpConnect* pPassiveConneted = NULL;
		int nRet = -2;
		STRY;
		iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
		map<UINT64,XhTcpConnect*>::iterator iterIdx = m_passiveConnetedMap.find (toAddr);
		if (iterIdx != m_passiveConnetedMap.end ())
		{
			pPassiveConneted = (XhTcpConnect*)iterIdx->second ;
			if(pPassiveConneted)
			{
				if(pPassiveConneted->AddUseRef() < 1)
					return -3;
			}
			else
				pPassiveConneted = NULL;
		//	m_passiveConnetedMap.erase(iterIdx);//防止同时访问，在用之前先删除
		}
		SCATCH;
		if (pPassiveConneted /*&& !::IsBadWritePtr (pPassiveConneted,sizeof(XhTcpConnect))*/)
		{
			STRY;
			nRet = pPassiveConneted->SendData(toAddr,pData,dataLen);
			SCATCH;
			STRY;
			iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
			map<UINT64,XhTcpConnect*>::iterator iterIdx = m_passiveConnetedMap.find (toAddr);
			if(iterIdx != m_passiveConnetedMap.end())
			{
				if(iterIdx->second == pPassiveConneted)
					pPassiveConneted->Release(__FUNCTION__);
			}
			SCATCH;
		}
		else
		{
			//TTRACE("%s want to send the data to %I64u ,the data len:%d but not found the connect obj!\r\n",__FUNCTION__,toAddr,dataLen);
		}
		return nRet;
	}
	SCATCH;
	return -1;
}

int XhTcpListen::StopListen()
{
	STRY;
	m_bQuitThread = TRUE;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
	if (INVALID_SOCKET == m_listenSocket )
		return 0;

	if (m_listenSocket)
	{
		shutdown(m_listenSocket,2);
		closesocket(m_listenSocket);
		m_listenSocket = NULL;
	}

	if (m_bUseAcceptEx)
	{
		CTcpIocpThread::GetTcpIocp () ->RemoveIoContext ((HANDLE)m_listenSocket,(ULONG_PTR)this);
	}
	else if (m_hListenThread)
	{
		WaitForSingleObject(m_hListenThread,5000);
//		TerminateThread(m_hListenThread,0);
		SAFE_CLOSE_HANDLE(m_hListenThread);
	}
	
	return 1;
	SCATCH;
	return -1;
}

int XhTcpListen::ClosePeer(UINT64 peerAddr)
{
	STRY;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);

	if (-1 == peerAddr)
	{
		if( m_passiveConnetedMap.size() )
		{
			map<UINT64,XhTcpConnect*>::iterator iterIdx = m_passiveConnetedMap.begin ();
			while (iterIdx != m_passiveConnetedMap.end () && m_passiveConnetedMap.size())
			{
				XhTcpConnect* pPassiveConneted = (XhTcpConnect*)iterIdx->second ;
				//pPassiveConneted->StopConnect();
				//pPassiveConneted->Release();
				iterIdx ++;
			}
			m_passiveConnetedMap.clear ();
		}
	}
	else
	{
		map<UINT64,XhTcpConnect*>::iterator iterIdx = m_passiveConnetedMap.find (peerAddr);
		if (iterIdx != m_passiveConnetedMap.end () && m_passiveConnetedMap.size ())
		{
			XhTcpConnect* pPassiveConneted = (XhTcpConnect*)iterIdx->second ;
			m_passiveConnetedMap.erase(iterIdx);

			if (pPassiveConneted /*&& !::IsBadWritePtr (pPassiveConneted,sizeof(XhTcpConnect))*/)
			{
				//pPassiveConneted->Release();
				//pPassiveConneted->StopConnect();
			}
		}
	}
	return 1;
	SCATCH;
	return -1;
}

long XhTcpListen::DealIocpAffair(IOCP_OVERLAPPED *pIocpOverlapped,DWORD dataCnt)
{
	STRY;
	if (IOCP_OVERLAPPED::IO_ACCEPT == pIocpOverlapped ->IoDirection  )
	{
		SOCKET sAccept = (SOCKET)m_recvOverlapped.userCookie;
		int nRet = setsockopt(sAccept,SOL_SOCKET,SO_UPDATE_ACCEPT_CONTEXT,(char *)&m_listenSocket,sizeof(m_listenSocket) );
		if (SOCKET_ERROR == nRet )
		{
			closesocket(sAccept);
		}
		else
		{
			static DWORD SINGLE_ADDR_LEN = sizeof(SOCKADDR_IN) + 16;
			struct sockaddr_in*	pSelfAddr = NULL;
			struct sockaddr_in*	pPeerAddr = NULL;
			int  selfAddrLen = sizeof(SOCKADDR_IN);
			int  peerAddrLen = sizeof(SOCKADDR_IN);
			GetAcceptExSockaddrs((PVOID)m_recvOverlapped.Buffer .buf ,dataCnt,SINGLE_ADDR_LEN,SINGLE_ADDR_LEN
				,(sockaddr**)&pSelfAddr,&selfAddrLen,(sockaddr**)&pPeerAddr,&peerAddrLen);

			TTRACE ("client [%d.%d.%d.%d :%d ] connect to [%d.%d.%d.%d :%d ]\r\n"
				,pPeerAddr->sin_addr.S_un.S_un_b.s_b1 ,pPeerAddr->sin_addr.S_un.S_un_b.s_b2
				,pPeerAddr->sin_addr.S_un.S_un_b.s_b3,pPeerAddr->sin_addr.S_un.S_un_b.s_b4,ntohs(pPeerAddr->sin_port )
				,pSelfAddr->sin_addr.S_un.S_un_b.s_b1 ,pSelfAddr->sin_addr.S_un.S_un_b.s_b2
				,pSelfAddr->sin_addr.S_un.S_un_b.s_b3,pSelfAddr->sin_addr.S_un.S_un_b.s_b4,ntohs(pSelfAddr->sin_port ));

			static const DWORD noDelay = 1 ;
			if (SOCKET_ERROR == setsockopt(sAccept, IPPROTO_TCP, TCP_NODELAY,(char *)&noDelay, sizeof(noDelay)) )
			{
				TTRACE ("TCP_NODELAY to  [%d.%d.%d.%d :%d ] failed [%d]!\r\n"
					,pPeerAddr->sin_addr.S_un.S_un_b.s_b1 ,pPeerAddr->sin_addr.S_un.S_un_b.s_b2
					,pPeerAddr->sin_addr.S_un.S_un_b.s_b3,pPeerAddr->sin_addr.S_un.S_un_b.s_b4
					,ntohs(pPeerAddr->sin_port ),::WSAGetLastError ());
			}

			//so need a new XhTcpConnect
			//TTRACE("%s alloc the XhTcpConnect object\r\n",__FUNCTION__);
			XhTcpConnect* pPassiveConnected = new XhTcpConnect();
			//TTRACE("%s pPassiveConnected %p\r\n",__FUNCTION__,pPassiveConnected);
			if (NULL == pPassiveConnected)
			{
				closesocket(sAccept);
			}
			else
			{
				pPassiveConnected->m_bIsServerSide = TRUE;
				pPassiveConnected->CheckDataHeaderControl(m_bNeedCheckHearder);
				DWORD theirIp	= ntohl(pPeerAddr->sin_addr.S_un.S_addr);
				WORD theirPort = ntohs(pPeerAddr->sin_port );
				UINT64 fromAddr = (UINT64)theirIp << 32 | theirPort;
				UINT64 localAddr	= (UINT64)ntohl(pSelfAddr->sin_addr.S_un.S_addr ) << 32 | ntohs(pSelfAddr->sin_port );
				pPassiveConnected ->DoPassiveConnected (sAccept,pPeerAddr,this);

				STRY;
				iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
				m_passiveConnetedMap [fromAddr] = pPassiveConnected;
				SCATCH;

				if (m_pTcpSink)
				{					
					m_pTcpSink ->OnConnected(fromAddr,localAddr);
				}
			}

			TTRACE ("deal client [%d.%d.%d.%d :%d ] connect affair finished,restart 2 accept \r\n"
				,pPeerAddr->sin_addr.S_un.S_un_b.s_b1 ,pPeerAddr->sin_addr.S_un.S_un_b.s_b2
				,pPeerAddr->sin_addr.S_un.S_un_b.s_b3,pPeerAddr->sin_addr.S_un.S_un_b.s_b4,ntohs(pPeerAddr->sin_port ));
		}

		
	}
	SCATCH;
	return DoAcceptSocket();
}

long XhTcpListen::OnGotError(IOCP_OVERLAPPED *pIocpOverlapped,DWORD errorCode)
{
	STRY;
	if (m_pTcpSink)
	{
		UINT64 listenAddr = (UINT64)m_localIp<<32 |m_localPort;
		m_pTcpSink ->OnClosed(listenAddr);
	}
	return 1;
	SCATCH;
	return -1;
}

long XhTcpListen::GetPeerAddr(TCHAR* pRemoteAddr)
{	
	STRY;
	SCATCH;
	return 0;
}

int XhTcpListen::OnRecvData(UINT64 fromAddr, BYTE* pData, int dataLen)
{
	STRY;
	if (m_pTcpSink)
	return m_pTcpSink->OnRecvData(fromAddr,pData,dataLen);
	SCATCH;
	TTRACE("!!!%s run error with sink[%p]!!!\r\n",__FUNCTION__,m_pTcpSink);
	return 0;
}

int XhTcpListen::OnConnected(UINT64 fromAddr,UINT64 localAddr)
{
	STRY;
	if (m_pTcpSink)
	return m_pTcpSink->OnConnected(fromAddr,localAddr);
	SCATCH;
	return 0;
}

int XhTcpListen::OnReConnected(UINT64 fromAddr)
{
	STRY;
	if (m_pTcpSink)
	{
		return m_pTcpSink->OnReConnected(fromAddr);
	}
	SCATCH;
	return 0;
}


int XhTcpListen::OnClosed(UINT64 fromAddr)
{
	STRY;
	ClosePeer(fromAddr);
	SCATCH;

	STRY;
	if (m_pTcpSink)
	{
		return m_pTcpSink->OnClosed(fromAddr);
	}
	SCATCH;
	return 0;
}

DWORD XhTcpListen::Static_ListenThread(void* pParam)
{
	STRY;
	XhTcpListen* pSelf = (XhTcpListen*)pParam;
	if (NULL == pSelf /*|| ::IsBadReadPtr (pSelf,sizeof(XhTcpListen))*/)
		return -1;

	return pSelf->ListenThread();
	SCATCH;
	return 0;
}

DWORD XhTcpListen::ListenThread()
{
	STRY;
	DWORD dwThreadId	=::GetCurrentThreadId ();
	UINT64 fromAddr		= 0;
	UINT64 localAddr	= 0;
	TTRACE("%s:%d started \r\n",__FUNCTION__,dwThreadId);
	
	while (FALSE == m_bQuitThread)
	{
		STRY;
		SOCKET sdAccept = WSAAccept(m_listenSocket, NULL, NULL, NULL, 0);
		if (SOCKET_ERROR == sdAccept) 
		{
			//if close the socket[m_listenSocket],the above WSAAccept call will 
			// fail and we thus break out the loop,
			TTRACE("%s WSAAccept faild reason[%d]\r\n",__FUNCTION__,WSAGetLastError());
			Sleep(100);
			continue;
		}

		struct sockaddr_in	SelfAddr = {0};
		int  selfAddrLen = sizeof(SOCKADDR_IN);

		struct sockaddr_in	PeerAddr = {0};		
		int  peerAddrLen = sizeof(SOCKADDR_IN);

		::getsockname (sdAccept,(sockaddr*)&SelfAddr,&selfAddrLen);
		::getpeername (sdAccept,(sockaddr*)&PeerAddr,&peerAddrLen);

		TTRACE ("client [%d.%d.%d.%d :%d ] connect to [%d.%d.%d.%d :%d ]\r\n"
			,PeerAddr.sin_addr.S_un.S_un_b.s_b1 ,PeerAddr.sin_addr.S_un.S_un_b.s_b2
			,PeerAddr.sin_addr.S_un.S_un_b.s_b3,PeerAddr.sin_addr.S_un.S_un_b.s_b4,ntohs(PeerAddr.sin_port )
			,SelfAddr.sin_addr.S_un.S_un_b.s_b1 ,SelfAddr.sin_addr.S_un.S_un_b.s_b2
			,SelfAddr.sin_addr.S_un.S_un_b.s_b3,SelfAddr.sin_addr.S_un.S_un_b.s_b4,ntohs(SelfAddr.sin_port ));

		static const DWORD noDelay = 1 ;
		if (SOCKET_ERROR == setsockopt(sdAccept, IPPROTO_TCP, TCP_NODELAY,(char *)&noDelay, sizeof(noDelay)) )
		{
			TTRACE ("TCP_NODELAY to  [%d.%d.%d.%d :%d ] failed [%d]!\r\n"
			,PeerAddr.sin_addr.S_un.S_un_b.s_b1 ,PeerAddr.sin_addr.S_un.S_un_b.s_b2
			,PeerAddr.sin_addr.S_un.S_un_b.s_b3,PeerAddr.sin_addr.S_un.S_un_b.s_b4
			,ntohs(PeerAddr.sin_port ),::WSAGetLastError ());
		}
		

		//so need a new XhTcpConnect
		STRY;
		XhTcpConnect* pPassiveConnected = NULL;
		
		pPassiveConnected = new XhTcpConnect();
		//TTRACE("%s create new pPassiveConnected %p\r\n",__FUNCTION__,pPassiveConnected);
		
		if (NULL == pPassiveConnected)
		{
			closesocket(sdAccept);
		}
		else
		{
			pPassiveConnected->m_bIsServerSide = TRUE;
			pPassiveConnected->CheckDataHeaderControl(m_bNeedCheckHearder);
			DWORD theirIp	= ntohl(PeerAddr.sin_addr.S_un.S_addr);
			WORD theirPort = ntohs(PeerAddr.sin_port );
			fromAddr = (UINT64)theirIp << 32 | theirPort;
			localAddr	= (UINT64)ntohl(SelfAddr.sin_addr.S_un.S_addr ) << 32 | ntohs(SelfAddr.sin_port );
			pPassiveConnected ->DoPassiveConnected (sdAccept,&PeerAddr,this);
			STRY;
			iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
			m_passiveConnetedMap [fromAddr] = pPassiveConnected;
			SCATCH;
			if (m_pTcpSink)
			{					
				m_pTcpSink ->OnConnected(fromAddr,localAddr);
			}

			TTRACE ("deal client [%d.%d.%d.%d :%d ] connect affair finished,restart 2 accept \r\n"
				,PeerAddr.sin_addr.S_un.S_un_b.s_b1 ,PeerAddr.sin_addr.S_un.S_un_b.s_b2
				,PeerAddr.sin_addr.S_un.S_un_b.s_b3,PeerAddr.sin_addr.S_un.S_un_b.s_b4,ntohs(PeerAddr.sin_port ));
		}
		SCATCH;
		
		SCATCH;
	}
	TTRACE("%s:%d exited \r\n",__FUNCTION__,dwThreadId);
	SCATCH;
	return 0;
}

XhTcpConnect* XhTcpListen::GetPeerConnect(UINT64 peerAddr)
{
	STRY;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);

	map<UINT64,XhTcpConnect*>::iterator iterIdx = m_passiveConnetedMap.find (peerAddr);
	if (iterIdx != m_passiveConnetedMap.end () && m_passiveConnetedMap.size ())
	{
		return iterIdx->second ;
	}	
	SCATCH;
	return NULL;
}

void XhTcpListen::CheckDataHeaderControl( BOOL bNeedCheck /*= FALSE*/ )
{
	m_bNeedCheckHearder = bNeedCheck;
	STRY;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
	map<UINT64,XhTcpConnect*>::iterator itor = m_passiveConnetedMap.begin();
	for(;itor!=m_passiveConnetedMap.end();itor++)
	{
		itor->second->CheckDataHeaderControl(m_bNeedCheckHearder);
	}
	SCATCH;
}