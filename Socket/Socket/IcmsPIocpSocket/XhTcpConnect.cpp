#include "stdafx.h"
#include "XhTcpConnect.h"
#include "TcpIocpThread.h"

#pragma pack(push,1)
struct ST_CMD_HEADER
{
	WORD				wSrcType;					//源的类型
	WORD				wCmdMainType;      //信令的主类型 
	DWORD				dwCmdSubType;     //信令的子类型
	DWORD				dwSeqID;               //信令序号
	WORD				wExtendType;           //扩展数据类型
	DWORD				dwExndSize;          //扩展数据大小
};
#pragma pack(pop)

class AutoAddRefAndRelease
{
public:
	AutoAddRefAndRelease(XhTcpConnect* pCon)
	{
		if(pCon->AddUseRef() == 1)
			m_pCon = pCon;
		else
			m_pCon = NULL;
	}

	~AutoAddRefAndRelease()
	{
		if(m_pCon)
			m_pCon->Release(__FUNCTION__);
	}

	BOOL IsAddRefSuccessed()
	{
		return (m_pCon != NULL)?TRUE:FALSE;
	}

private:
	XhTcpConnect* m_pCon;
};

XhTcpConnect::XhTcpConnect(void)
:m_peerAddr(0),m_connectSocket(INVALID_SOCKET),m_pTcpSink(NULL),m_commSeqId(0)
,m_lastRecvSeqId(0),m_recvCapacity(IOCP_BUFFER_SIZE*2),m_pRecvDataBuf(NULL)
,m_dwLastRecvTs(0),m_bPassiveConnect(FALSE),m_nLeftCnt(0),m_bQuit(FALSE),m_bLogTransData(TRUE)
,m_bNeedSendHead(TRUE),m_bIsServerSide(FALSE),m_nUseRef(1),m_dwLastSendCheckPackegTS(GetTickCount())
,m_nTrySendDataCnt(0)
{
	STRY;
	ZeroMemory(&m_remotePeer,sizeof(struct sockaddr_in));
	ZeroMemory(&m_lastCommHeader,sizeof(COMM_HEADER));
	m_pRecvDataBuf	= (BYTE*)VirtualAlloc(NULL,m_recvCapacity,MEM_COMMIT,PAGE_READWRITE);
	m_pWritePtr		= m_pRecvDataBuf;
	TCHAR chFileName [4096] = {0};
	_stprintf(chFileName,_T("%s\\data\\localSet.ini"),GetWorkingDirectory());
	m_bLogTransData = ::GetPrivateProfileInt (_T("OUTPUTDEBUGLOG"),_T("use_output"),0,chFileName);
	//CTcpIocpThread::GetTcpIocp () ->InitWinSocketEvnt();
	ZeroMemory(m_chPeerAddr,32);
	ZeroMemory(m_chLeftMem,32);
	ZeroMemory(m_chSelfAddr,32);
	SCATCH;
}

XhTcpConnect::~XhTcpConnect(void)
{
	STRY;
	if (!m_chPeerAddr/*::IsBadWritePtr (m_chPeerAddr,32)*/)
		return ;
	//Release(__FUNCTION__,FALSE);
	if(!m_bIsServerSide)
		StopConnect(FALSE);

	STRY;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
	if (NULL != m_pRecvDataBuf)
	{
		VirtualFree(m_pRecvDataBuf,0,MEM_RELEASE);
		m_pRecvDataBuf	= 0;
	}

	m_pWritePtr		= m_pRecvDataBuf;
	m_pTcpSink		= NULL;
	m_recvCapacity  = 0;
	SCATCH;
	TTRACE("%s delete %p:%s\r\n",__FUNCTION__,this,m_chPeerAddr);
	SCATCH;
}

int XhTcpConnect::SetCallBackSink(ITcpCallbackSink* pSink)
{
	STRY;
	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
	m_pTcpSink = pSink;
	SCATCH;
	return 1;
}

int XhTcpConnect::DoConnect(long remoteIp,short remotePort)
{
	STRY;
	StopConnect(false);
// 	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
// 	iCMSP_SimpleLock lockQuitFlag(&m_visitQuitFlag,__FUNCTION__);
	m_bQuit			= FALSE;
	m_nLeftCnt		= 0;
	m_connectSocket	= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED); 
	if (INVALID_SOCKET == m_connectSocket)
		return -1;

	m_remotePeer.sin_family					= AF_INET;
	m_remotePeer.sin_addr .S_un .S_addr		= htonl(remoteIp);
	m_remotePeer.sin_port					= htons(remotePort);

	//1.先将socket设为非堵塞
	unsigned long lBlocked = 1;
	ioctlsocket(m_connectSocket,FIONBIO ,&lBlocked);
	//2.设置连接超时
	//because in local network,5 s mabey long,so change it to 2 s
	//terrywang edit at 2009-12-9
	static const struct timeval Timeout = {2,0};
	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(m_connectSocket,&writefds);

	connect(m_connectSocket,(const sockaddr*)&m_remotePeer,sizeof(sockaddr_in));
	select(1,  NULL,&writefds, NULL, &Timeout);
	//3.将socket设为堵塞
	lBlocked = 0;
	ioctlsocket(m_connectSocket,FIONBIO ,&lBlocked);

	CTcpIocpThread::GetTcpIocp () ->PushIoContext ((HANDLE)m_connectSocket,(ULONG_PTR)this);

	if ( ! FD_ISSET(m_connectSocket,&writefds))
	{
		Release(__FUNCTION__);
		//StopConnect();
		TTRACE ("%s:%d overtime. connect to [%d.%d.%d.%d :%d ]\r\n"
			,__FUNCTION__,m_connectSocket,m_remotePeer.sin_addr.S_un.S_un_b.s_b1 
			,m_remotePeer.sin_addr.S_un.S_un_b.s_b2,m_remotePeer.sin_addr.S_un.S_un_b.s_b3
			,m_remotePeer.sin_addr.S_un.S_un_b.s_b4	,ntohs(m_remotePeer.sin_port ));
		return -2;
	}

	struct sockaddr_in selfAddr = {0};
	int addrLen = sizeof(struct sockaddr_in);
	if ( 0 == ::getsockname (m_connectSocket,(sockaddr*)&selfAddr,&addrLen) )
	{
		TTRACE ("[%d.%d.%d.%d-%d:%d ] connect to [%d.%d.%d.%d :%d ]\r\n"
			,selfAddr.sin_addr.S_un.S_un_b.s_b1 ,selfAddr.sin_addr.S_un.S_un_b.s_b2
			,selfAddr.sin_addr.S_un.S_un_b.s_b3,selfAddr.sin_addr.S_un.S_un_b.s_b4,ntohs(selfAddr.sin_port ),m_connectSocket
			,m_remotePeer.sin_addr.S_un.S_un_b.s_b1 ,m_remotePeer.sin_addr.S_un.S_un_b.s_b2
			,m_remotePeer.sin_addr.S_un.S_un_b.s_b3,m_remotePeer.sin_addr.S_un.S_un_b.s_b4
			,ntohs(m_remotePeer.sin_port ));

		::_stprintf (m_chSelfAddr,_T("%d.%d.%d.%d-%d"),selfAddr.sin_addr.S_un.S_un_b.s_b1 ,selfAddr.sin_addr.S_un.S_un_b.s_b2
			,selfAddr.sin_addr.S_un.S_un_b.s_b3,selfAddr.sin_addr.S_un.S_un_b.s_b4,ntohs(selfAddr.sin_port ));
	}
	
	int sendOverTime = 1000;
	setsockopt (m_connectSocket,SOL_SOCKET,SO_SNDTIMEO,(const char*)&sendOverTime,sizeof(int));
	bool bDontLinger = false;
    setsockopt(m_connectSocket,SOL_SOCKET,SO_DONTLINGER,(const char*)&bDontLinger,sizeof(bool)); 
	linger   InternalLinger = {1,0};
    setsockopt(m_connectSocket,SOL_SOCKET,SO_LINGER,(const char*)&InternalLinger,sizeof(linger));

	//disalbe the negal algorithm
	static const DWORD noDelay = 1 ;
	if (SOCKET_ERROR == setsockopt(m_connectSocket, IPPROTO_TCP, TCP_NODELAY,(char *)&noDelay, sizeof(noDelay)) )
	{
			TTRACE ("[%d.%d.%d.%d :%d ] to [%d.%d.%d.%d :%d ] failed[%d]\r\n"
			,selfAddr.sin_addr.S_un.S_un_b.s_b1 ,selfAddr.sin_addr.S_un.S_un_b.s_b2
			,selfAddr.sin_addr.S_un.S_un_b.s_b3,selfAddr.sin_addr.S_un.S_un_b.s_b4,ntohs(selfAddr.sin_port )
			,m_remotePeer.sin_addr.S_un.S_un_b.s_b1 ,m_remotePeer.sin_addr.S_un.S_un_b.s_b2
			,m_remotePeer.sin_addr.S_un.S_un_b.s_b3,m_remotePeer.sin_addr.S_un.S_un_b.s_b4
			,ntohs(m_remotePeer.sin_port ),::WSAGetLastError ());
	}
	m_nTrySendDataCnt = 0;
	if (m_pTcpSink)
	{
		m_peerAddr			= (UINT64)remoteIp << 32 | remotePort;
		m_selfAddr			= (UINT64)ntohl(selfAddr.sin_addr.S_un.S_addr ) << 32 | ntohs(selfAddr.sin_port );

		::_stprintf (m_chPeerAddr,_T("%d.%d.%d.%d:%d"),m_remotePeer.sin_addr.S_un.S_un_b.s_b1 
			,m_remotePeer.sin_addr.S_un.S_un_b.s_b2	,m_remotePeer.sin_addr.S_un.S_un_b.s_b3
			,m_remotePeer.sin_addr.S_un.S_un_b.s_b4	,ntohs(m_remotePeer.sin_port ) );
		m_pTcpSink ->OnConnected(m_peerAddr,m_selfAddr);
		STRY;
		iCMSP_SimpleLock lockUseRef(&m_visitUseRefCS,__FUNCTION__);
		if(m_nUseRef == 0) //client side
		{
			m_nUseRef ++;
		}
		SCATCH;
	}
	SCATCH;
	return DoRecvAgain();
}

int XhTcpConnect::DoPassiveConnected(SOCKET sConnected,struct sockaddr_in* pRemoteAddr,ITcpCallbackSink* pSink)
{
	STRY;

// 	iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
// 	iCMSP_SimpleLock lockQuitFlag(&m_visitQuitFlag,__FUNCTION__);

	m_bQuit			= FALSE;
	m_bPassiveConnect= TRUE;
	m_nLeftCnt		= 0;
	m_pTcpSink		= pSink;
	m_connectSocket = sConnected;
	m_peerAddr		= (UINT64)ntohl(pRemoteAddr->sin_addr .S_un.S_addr)  << 32 | htons(pRemoteAddr->sin_port );
	CopyMemory(&m_remotePeer,pRemoteAddr,sizeof(struct sockaddr_in));

	int sendOverTime = 1000;
	setsockopt (m_connectSocket,SOL_SOCKET,SO_SNDTIMEO,(const char*)&sendOverTime,sizeof(int));
	bool bDontLinger = false;
    setsockopt(m_connectSocket,SOL_SOCKET,SO_DONTLINGER,(const char*)&bDontLinger,sizeof(bool)); 
	linger   InternalLinger = {1,0};
    setsockopt(m_connectSocket,SOL_SOCKET,SO_LINGER,(const char*)&InternalLinger,sizeof(linger));

	::_stprintf (m_chPeerAddr,_T("%d.%d.%d.%d:%d"),m_remotePeer.sin_addr.S_un.S_un_b.s_b1 
		,m_remotePeer.sin_addr.S_un.S_un_b.s_b2	,m_remotePeer.sin_addr.S_un.S_un_b.s_b3
		,m_remotePeer.sin_addr.S_un.S_un_b.s_b4	,ntohs(m_remotePeer.sin_port ) );

	m_nTrySendDataCnt = 0;
	CTcpIocpThread::GetTcpIocp () ->PushIoContext ((HANDLE)m_connectSocket,(ULONG_PTR)this);

// 	TTRACE ("[%s:%d ] start 2 recv data from peer [%d.%d.%d.%d :%d ]\r\n"
// 		,m_chPeerAddr,m_connectSocket
// 		,m_remotePeer.sin_addr.S_un.S_un_b.s_b1 ,m_remotePeer.sin_addr.S_un.S_un_b.s_b2
// 		,m_remotePeer.sin_addr.S_un.S_un_b.s_b3,m_remotePeer.sin_addr.S_un.S_un_b.s_b4
// 		,ntohs(m_remotePeer.sin_port ));

	struct sockaddr_in selfAddr = {0};
	int addrLen = sizeof(struct sockaddr_in);
	if ( 0 == ::getsockname (m_connectSocket,(sockaddr*)&selfAddr,&addrLen) )
	{
		::_stprintf (m_chSelfAddr,_T("%d.%d.%d.%d-%d"),selfAddr.sin_addr.S_un.S_un_b.s_b1 ,selfAddr.sin_addr.S_un.S_un_b.s_b2
			,selfAddr.sin_addr.S_un.S_un_b.s_b3,selfAddr.sin_addr.S_un.S_un_b.s_b4,ntohs(selfAddr.sin_port ));
	}

	return DoRecvAgain();
	SCATCH;
	return 0;
}

int XhTcpConnect::DoRecvAgain()
{
	STRY;
	STRY;
//	iCMSP_SimpleLock lockQuitFlag(&m_visitQuitFlag,__FUNCTION__);
	if ( INVALID_SOCKET ==  m_connectSocket || TRUE == m_bQuit)
		return -2;
	SCATCH;
	
	//continue 2 recv data from peer
	m_recvOverlapped.IoDirection = IOCP_OVERLAPPED::IO_RECVIN;

	DWORD dwIoSize = 0;
	DWORD dwFlag   = 0;
	DWORD fromLen  = sizeof(struct sockaddr_in);
	int nRet = WSARecvFrom(m_connectSocket,&m_recvOverlapped.Buffer,1,&dwIoSize,&dwFlag
		,(sockaddr*)& m_remotePeer,(int*)&fromLen,&m_recvOverlapped.Overlapped,NULL);
	if (SOCKET_ERROR == nRet && (ERROR_IO_PENDING != WSAGetLastError())) 
		return -1;
	return 1;
	SCATCH;
	return 0;
}

int XhTcpConnect::SendData(UINT64 toAddr,BYTE* pData,int dataLen)
{
	iCMSP_SimpleLock localDealFunction(&m_visitDealFunction,__FUNCTION__);
	AutoAddRefAndRelease autoAdd(this);
	if(!autoAdd.IsAddRefSuccessed())
	{
		//TTRACE("%s the use ref count is 0,so I have to return\r\n",__FUNCTION__);
		return -3;
	}
	BYTE* pSavePtr = pData;
// 	STRY;
// 	TTRACE("%s Want to send to %s data:%s the data len:%d\r\n",__FUNCTION__,m_chPeerAddr,pSavePtr+sizeof(ST_CMD_HEADER),dataLen);
// 	SCATCH;
	STRY;
	STRY;
//	iCMSP_SimpleLock lockQuitFlag(&m_visitQuitFlag,__FUNCTION__);
	if (TRUE == m_bQuit || INVALID_SOCKET == m_connectSocket)
		return 0;
	SCATCH;
	
	if(dataLen <=0 || dataLen > 1024*1024*10) //如果发送的数据大于10M打出数据大小
		TTRACE("%s:%d send data size:%d\r\n",__FUNCTION__,__LINE__,dataLen);

	//because of 李贵成's request,check the input data ,verify the source data
	//terrywang edit at 2009-8-8
	if(m_bNeedSendHead)
	{
		if (dataLen < sizeof(ST_CMD_HEADER) || !pData /*|| TRUE == ::IsBadReadPtr (pData,dataLen)*/)
		{
			TTRACE("!!!%s at %s send%p len:%d invalite!!!\r\n",m_chPeerAddr,__FUNCTION__,pData,dataLen);
			return 0;
		}
		ST_CMD_HEADER* pCmdHeader = (ST_CMD_HEADER*)pData;
		if (pCmdHeader->dwExndSize +sizeof(ST_CMD_HEADER) != dataLen)
		{
			TTRACE("!!!%s at %s send%p len:%d with bad dwExndSize%d!!!\r\n",m_chPeerAddr,__FUNCTION__,pData,dataLen,pCmdHeader->dwExndSize);
			return 0;
		}

		//iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
		//iCMSP_SimpleLock lockQuitFlag(&m_visitQuitFlag,__FUNCTION__);
		int		nSrcDataLen      = dataLen;
		char	outDataBuf[2048] = {0};
		COMM_HEADER* pCommHeader = (COMM_HEADER*)outDataBuf;
		pCommHeader ->totalPacketSize = dataLen;
		int     nowPacketSize		  = 0;
		STRY;
		while (FALSE == m_bQuit && INVALID_SOCKET != m_connectSocket && dataLen > 0)
		{
			static int TCP_MTU = 1200;
			if(dataLen > TCP_MTU)
				nowPacketSize = TCP_MTU;
			else
				nowPacketSize = dataLen;

			pCommHeader ->seqId			= ++ m_commSeqId;
			pCommHeader ->nowPacketSize	= nowPacketSize;
			CopyMemory((outDataBuf+sizeof(COMM_HEADER)),pData,nowPacketSize);

			int nWantOutCnt = nowPacketSize+sizeof(COMM_HEADER);
			int nSendOutCnt = 0;
			int nCanLoopCnt = 8;
			while (nWantOutCnt && nSendOutCnt < nWantOutCnt && nCanLoopCnt -- > 0)
			{
				struct timeval Timeout = {60,0};
				fd_set writefds;
				FD_ZERO(&writefds);
				FD_SET(m_connectSocket,&writefds);
				select(1,  NULL,&writefds, NULL, &Timeout);

				DWORD dwSendBytes = 0;
				WSABUF wsaBuffer;
				wsaBuffer.buf = &outDataBuf[nSendOutCnt];
				wsaBuffer.len = nWantOutCnt;
				int nNowSendOutCnt = /*WSASend(m_connectSocket,&wsaBuffer,1,&dwSendBytes,0,NULL,NULL);*/send(m_connectSocket,&outDataBuf[nSendOutCnt],nWantOutCnt,0);
				if(nNowSendOutCnt != nWantOutCnt)
					TTRACE("%s:%d nWantOutCnt:%d  nNowSendOutCnt:%d: nSendOutCnt:%d\r\n",__FUNCTION__,__LINE__,nWantOutCnt,nNowSendOutCnt,nSendOutCnt);
				if (SOCKET_ERROR == nNowSendOutCnt)
				{
					TTRACE("%s [%s:%d] send failed: error code%d\r\n",__FUNCTION__,m_chPeerAddr,m_connectSocket,::WSAGetLastError ());
					//Release(__FUNCTION__);
					break;
				}

				nSendOutCnt			+= nNowSendOutCnt;
				nWantOutCnt			-= nNowSendOutCnt;
			}

			pData	+= nowPacketSize;
			dataLen -= nowPacketSize;
			//becasuse dum asked me to sleep,but why? why ? why?
			//if (dataLen)
			//	Sleep(10);
		}
		SCATCH;

		//the heart beat packet come so frequences,so not log the heart beat
		//terrywang edited at 2009-4-20
		if (18 != nSrcDataLen && m_bLogTransData)
			TTRACE("[%s] at %s 2[%s],need out[%d] left cnt[%d]\r\n",m_chSelfAddr,__FUNCTION__,m_chPeerAddr,nSrcDataLen,dataLen);
		if(dataLen == 0)
		{
			//TTRACE("Send to %s data:%s successed\r\n",m_chPeerAddr,pSavePtr+sizeof(ST_CMD_HEADER));
			return 1;
		}
		else
		{
			TTRACE("Send to %s data:%s failed\r\n",m_chPeerAddr,pSavePtr+sizeof(ST_CMD_HEADER));
			return -1;
		}
	}
	else
	{
		int		nSrcDataLen      = dataLen;
		int		nIndex = 0;
		while(nSrcDataLen > 0)
		{
			int nNowSendOutCnt = send(m_connectSocket,(const char*)&pData[nIndex],dataLen,0);
			if (SOCKET_ERROR == nNowSendOutCnt)
			{
				TTRACE("%s [%s:%d] send failed: error code%d\r\n",__FUNCTION__,m_chPeerAddr,m_connectSocket,::WSAGetLastError ());
				//Release(__FUNCTION__);
				return -1;
			}
			nSrcDataLen -= nNowSendOutCnt;
			nIndex += nNowSendOutCnt;
		}
		return -1;
		
	}
	SCATCH;
	TTRACE("%s has an exception\r\n",__FUNCTION__);
	return -1;
}

int XhTcpConnect::DoPostMsg2Exit()
{
	STRY;
	BOOL bNeedDelete = TRUE;
	if(m_bIsServerSide)
		bNeedDelete = FALSE;
	//ZeroMemory(&m_recvOverlapped,sizeof(IOCP_OVERLAPPED));
	return	CTcpIocpThread::GetTcpIocp () ->RemoveIoContext ((HANDLE)m_connectSocket,(ULONG_PTR)this,bNeedDelete);
	SCATCH;
	return 0;
}

int XhTcpConnect::StopConnect(bool bNeedNotifySink)
{
	STRY;
	iCMSP_SimpleLock localDealFunction(&m_visitDealFunction,__FUNCTION__); 
	STRY;
//	iCMSP_SimpleLock lockQuitFlag(&m_visitQuitFlag,__FUNCTION__);
	if (TRUE == m_bQuit)
		return 1;
	m_bQuit		= TRUE;
	SCATCH;

	if (INVALID_SOCKET != m_connectSocket && m_connectSocket != NULL)
	{
		shutdown(m_connectSocket,2);
		closesocket(m_connectSocket);
		m_connectSocket = INVALID_SOCKET;
	}

	DoPostMsg2Exit(); //先在完成端口中将其移除，以免发生冲突(

	STRY;
	m_nLeftCnt	= 0;
	m_selfAddr	= 0;
	if (!m_chPeerAddr/*::IsBadWritePtr (m_chPeerAddr,32)*/)
		return -1;

	if ( m_pTcpSink && m_peerAddr)
	{
		UINT64 uSaveAddr = m_peerAddr;
		ITcpCallbackSink* pSavdedSink = NULL;
		STRY;
		iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
		pSavdedSink = m_pTcpSink;
		m_peerAddr		= 0;
		SCATCH;
		STRY;
		if (pSavdedSink && !::IsBadReadPtr (pSavdedSink,sizeof(4)))
			pSavdedSink ->OnClosed (uSaveAddr);
		SCATCH;
	}

	//TTRACE("%s shutdown %d from[%s]\r\n",__FUNCTION__,m_connectSocket,m_chPeerAddr);
	
	SCATCH;
	return 1;
	SCATCH;
	return 0;
}


long XhTcpConnect::DealIocpAffair(IOCP_OVERLAPPED *pIocpOverlapped,DWORD dataCnt)
{
	AutoAddRefAndRelease autoAdd(this);
	if(!autoAdd.IsAddRefSuccessed())
		return -3;
	//iCMSP_SimpleLock localDealFunction(&m_visitDealFunction,__FUNCTION__);
	STRY;
	STRY;
//	iCMSP_SimpleLock lockQuitFlag(&m_visitQuitFlag,__FUNCTION__);
	if (TRUE == m_bQuit || INVALID_SOCKET == m_connectSocket 
		||IOCP_OVERLAPPED::IO_RECVIN != pIocpOverlapped ->IoDirection  )
		return 0;
	SCATCH;

	STRY;

	if (!m_chPeerAddr/*::IsBadWritePtr (m_chPeerAddr,32)*/)
	{
		TTRACE("%s:got badPtr:%p datalen[%d]\r\n",__FUNCTION__,this,dataCnt);
		return -2;
	}

	if (0 == dataCnt)
	{		
		TTRACE("%s:remote[%s]:local[%s] got data cnt [%d] errorCode[%d]\r\n",__FUNCTION__,m_chPeerAddr,m_chSelfAddr,dataCnt,::WSAGetLastError ());
		BOOL bNeedReConnected = (m_bPassiveConnect ? FALSE:TRUE);
		Release(__FUNCTION__);

		STRY;
		iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
		if (bNeedReConnected && m_pTcpSink && !::IsBadReadPtr (m_pTcpSink,8))
		{
			m_pTcpSink->OnReConnected(0);
		}
		SCATCH;
		ZeroMemory(&m_lastCommHeader,sizeof(COMM_HEADER));
		m_nLeftCnt = 0;
		return -4;
	}
	SCATCH;

	STRY;
	if (m_bLogTransData)
		TTRACE("%s:remote[%s]:local[%s] got data cnt [%d]\r\n",__FUNCTION__,m_chPeerAddr,m_chSelfAddr,dataCnt);

	// 	if (m_dwLastRecvTs +10* 1000 < GetTickCount())
	// 	{
	// 		ZeroMemory(&m_lastCommHeader,sizeof(COMM_HEADER));
	// 		m_pWritePtr = m_pRecvDataBuf;
	// 		m_nLeftCnt  = 0;
	// 	}

	STRY;
	iCMSP_SimpleLock lockLastRecvTs(&m_visitLastRecvTs,__FUNCTION__);
	m_dwLastRecvTs = GetTickCount();
	SCATCH;

	int nLeftSize		= dataCnt;
	BOOL bNeedAddLeftCount = TRUE;
	//some one send the multiply affair context without delay,so check the recevice should be multiply packet.
	BYTE* pRealDataFrom  = (BYTE*)pIocpOverlapped->Buffer.buf ;
	if (NULL == pRealDataFrom)
	{
		TTRACE("%s,Buffer Is NULL\n", __FUNCTION__);
	}

	DWORD dwHeaderSize	= sizeof(COMM_HEADER);
	char chRealData[IOCP_BUFFER_SIZE*2] = {0};

	if(m_bNeedSendHead)
	{
		STRY;
		while (nLeftSize > 0 )
		{
			STRY;
			if (pRealDataFrom)
			{			
				BYTE* pAddrHeader	= (BYTE*)&m_lastCommHeader;
				if (m_nLeftCnt)
				{
					CopyMemory(chRealData,m_chLeftMem,m_nLeftCnt);
					//TTRACE("nLeftSize:%d  dataCnt:%d  m_lastCommHeader.totalPacketSize:%d  m_lastCommHeader.nowPacketSize:%d\r\n",nLeftSize,dataCnt,m_lastCommHeader.totalPacketSize,m_lastCommHeader.nowPacketSize);
					if(bNeedAddLeftCount)
					{
						//TTRACE("m_nLeftCnt:%d   dataCnt:%d\r\n",m_nLeftCnt,dataCnt);
						CopyMemory(&chRealData[m_nLeftCnt],pRealDataFrom,nLeftSize);
					}
					if(m_lastCommHeader.totalPacketSize ==  0)
					{
						if(!bNeedAddLeftCount && m_nLeftCnt+nLeftSize < dwHeaderSize)
							break;
						CopyMemory(pAddrHeader,chRealData,dwHeaderSize);
					}
				//	TTRACE("nLeftSize:%d  m_nLeftCnt:%d   m_lastCommHeader.totalPacketSize:%d  m_lastCommHeader.nowPacketSize:%d\r\n",nLeftSize,m_nLeftCnt,m_lastCommHeader.totalPacketSize,m_lastCommHeader.nowPacketSize);
					pRealDataFrom = (BYTE*)chRealData;
					if(bNeedAddLeftCount)
					{
						nLeftSize += m_nLeftCnt;
						bNeedAddLeftCount = FALSE;
					}
					ZeroMemory(m_chLeftMem,m_nLeftCnt);
					m_nLeftCnt = 0;

				}
				else if(m_lastCommHeader.totalPacketSize == 0)
					CopyMemory(pAddrHeader,pRealDataFrom,dwHeaderSize);
				bNeedAddLeftCount = FALSE;
				if(m_lastCommHeader.totalPacketSize < 0)
				{
					TTRACE("%s,PeerIP:%s,totalSize:%d,nowPackageSize:%d\n",__FUNCTION__,m_chPeerAddr,m_lastCommHeader.totalPacketSize,m_lastCommHeader.nowPacketSize);
				}
			}
			SCATCH;

			COMM_HEADER* pCommHeader =(COMM_HEADER*)&m_lastCommHeader;

			int nowPacketSize   = pCommHeader->nowPacketSize;
			STRY;
			if (pCommHeader->nowPacketSize <0)
			{
				m_nLeftCnt = 0;
				m_pWritePtr = m_pRecvDataBuf;
				TTRACE("%s pCommHeader->nowPacketSize <0\r\n",__FUNCTION__);
				break;;
			}

			if(nowPacketSize > 1200)
			{
				TTRACE("尼玛啊，包错了啊！！！哪儿来的这么大嘛，丢求了。nowPacketSize:%d\r\n",nowPacketSize);
				break;

			}
			if(nowPacketSize > nLeftSize - sizeof(COMM_HEADER)) //说明此包还未接收完
			{
				//TTRACE("not the whole packet,copy the data and wrait for the next recv.nowPacketSize:%d  nLeftSize:%d\r\n",nowPacketSize,nLeftSize);
				CopyMemory(m_chLeftMem,pRealDataFrom,nLeftSize);
				m_nLeftCnt = nLeftSize;
				break;
			}

			if(m_lastCommHeader.totalPacketSize > 100*1024*1024)
			{
				TTRACE("尼玛啊，包错了啊！！！哪儿来的这么大嘛，丢求了。totalPacketSize:%d\r\n",m_lastCommHeader.totalPacketSize);
				break;
			}

			//adjust the new packet size is greater than the recv buffer size???
			if (!m_pRecvDataBuf || ((pCommHeader->totalPacketSize) > m_recvCapacity || m_recvCapacity > pCommHeader->totalPacketSize*400))
			{
				//adjust the total packet size is less than 1m?
				// 			if (/*(DWORD)(pCommHeader->totalPacketSize) < 4*1024*1024 && */(DWORD)(pCommHeader->totalPacketSize) >m_recvCapacity )
				// 			{
				//if so,release first ,then realloc
				//	TTRACE("%s m_recvCapacity %d expand to %d from %s\r\n",__FUNCTION__,m_recvCapacity,pCommHeader->totalPacketSize,m_chPeerAddr);
				STRY;
				iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
				STRY;
				//iCMSP_SimpleLock lockQuitFlag(&m_visitQuitFlag,__FUNCTION__);
				if(TRUE == m_bQuit)
					return 2;
				SCATCH;
				if(m_pRecvDataBuf)
					VirtualFree(m_pRecvDataBuf,0,MEM_RELEASE);
				m_recvCapacity	= (pCommHeader->totalPacketSize / 1024 +2) * 1024 ;
				if(m_recvCapacity < 0)
				{
					TTRACE("there is something wrong m_recvCapacity the value is :%d the pCommHeader->totalPacketSize  is:%d\r\n",m_recvCapacity,pCommHeader->totalPacketSize);
					return -3;
				}
				m_pRecvDataBuf	= (BYTE*)VirtualAlloc(NULL,m_recvCapacity,MEM_COMMIT,PAGE_READWRITE);
				m_pWritePtr		= m_pRecvDataBuf;
				SCATCH;

				if (NULL == m_pRecvDataBuf)
				{
					TTRACE("%s: expand to %d header recv size:%d,really recv size:%d,err code[%d]\r\n", __FUNCTION__, pCommHeader->totalPacketSize,pCommHeader->nowPacketSize,dataCnt,::GetLastError());
					m_recvCapacity = 0;
				}

			}
			SCATCH;
			STRY;
			//now adjust the write buffer is enough
			if (m_pWritePtr /*&& ! ::IsBadWritePtr (m_pWritePtr,nowPacketSize)*/)
			{
				STRY;
				CopyMemory(m_pWritePtr,pRealDataFrom+dwHeaderSize,nowPacketSize);
				SCATCH;
				//ATLTRACE("%s got [%d] %s \r\n",__FUNCTION__,pCommHeader->nowPacketSize ,pRealDataFrom+sizeof(COMM_HEADER));
				m_pWritePtr += nowPacketSize;

				//TTRACE("the packet seq:%d   now size:%d   totalsize:%d\r\n",pCommHeader->seqId,m_pWritePtr - m_pRecvDataBuf,pCommHeader->totalPacketSize);

				STRY;
				//adjust be got all the data
				if (m_pWritePtr - m_pRecvDataBuf >= pCommHeader ->totalPacketSize && m_pRecvDataBuf)
				{
					//TTRACE("write size:%d   totalsize:%d\r\n",m_pWritePtr - m_pRecvDataBuf,pCommHeader ->totalPacketSize);
					int nLeftCnt = m_pWritePtr - m_pRecvDataBuf - pCommHeader->totalPacketSize;
 					if(nLeftCnt > 0)
 					{
 						TTRACE("包有可能没对哦亲，相差：%d\r\n",nLeftCnt);
 						//nowPacketSize -= nLeftCnt;
 					}
					m_pRecvDataBuf [m_pWritePtr-m_pRecvDataBuf] = '\0';

					if(!IsTheCheckConnectionPacketData(m_pRecvDataBuf,m_pWritePtr-m_pRecvDataBuf))//如果不是心跳检测包就回调上层
					{
						//got all the data,give to the caller
						STRY;
//						iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
//						iCMSP_SimpleLock lockQuitFlag(&m_visitQuitFlag,__FUNCTION__);
						if (m_pTcpSink && FALSE == m_bQuit && INVALID_SOCKET != m_connectSocket)		
						{
							DWORD dwDataSize = m_pWritePtr-m_pRecvDataBuf;
							if (m_bLogTransData)
								TTRACE("%s:remote[%s]:local[%s]this:%p got data size[%d]\r\n",__FUNCTION__,m_chPeerAddr,m_chSelfAddr,this,dwDataSize);

							STRY;
							//TTRACE("Recv the %s's data:%s\r\n",m_chPeerAddr,m_pRecvDataBuf+sizeof(ST_CMD_HEADER));
							m_pTcpSink ->OnRecvData (m_peerAddr,(BYTE*)m_pRecvDataBuf ,dwDataSize);
							SCATCH;
						}
						SCATCH;
					}
					else
					{
						TTRACE("Recv the %s check connect packeg\r\n",m_chPeerAddr);
					}

					m_pWritePtr = m_pRecvDataBuf;
				}
				SCATCH;
			}	

			SCATCH;

			STRY;
			// 			if (pCommHeader->nowPacketSize	>= nowPacketSize )
			// 				pCommHeader->nowPacketSize -= nowPacketSize;	

			//reset the recive buffer ptr
			ZeroMemory(&m_lastCommHeader,sizeof(COMM_HEADER));
			m_nLeftCnt = 0;

			nowPacketSize				+= dwHeaderSize;
			pRealDataFrom				+= nowPacketSize;
			nLeftSize					-= nowPacketSize;	
			nowPacketSize				= 0;

			//sometime the left size is less than sizeof(COMM_HEADER)
			if (nLeftSize > 0)
			{
				//TTRACE("there has some data left the left size is:%d\r\n",nLeftSize);
				m_nLeftCnt = nLeftSize;
				CopyMemory(m_chLeftMem,pRealDataFrom,m_nLeftCnt);
			}
			else
			{
				nLeftSize = 0;
			}
			SCATCH;
		}
		ZeroMemory(&m_lastCommHeader,sizeof(COMM_HEADER));
		SCATCH;
	}
	else
	{
		STRY;
// 		iCMSP_SimpleLock simpleLock(&m_simpleCS,__FUNCTION__);
// 		iCMSP_SimpleLock lockQuitFlag(&m_visitQuitFlag,__FUNCTION__);
		
		if (m_pTcpSink && FALSE == m_bQuit && INVALID_SOCKET != m_connectSocket)		
		{
			DWORD dwDataSize = dataCnt;
			if (m_bLogTransData)
				TTRACE("%s:remote[%s]:local[%s]this:%p got data size[%d]\r\n",__FUNCTION__,m_chPeerAddr,m_chSelfAddr,this,dwDataSize);

			STRY;
			m_pTcpSink ->OnRecvData (m_peerAddr,pRealDataFrom ,dwDataSize);
			SCATCH;
		}
		SCATCH;
	}
	SCATCH;

	return DoRecvAgain();	
	SCATCH;
	return -1;
}

long XhTcpConnect::OnGotError(IOCP_OVERLAPPED *pIocpOverlapped,DWORD errorCode)
{	
	TTRACE("%s found %s closed ,error code[%d]\r\n",__FUNCTION__,m_chPeerAddr,errorCode);
	return Release(__FUNCTION__);
}

long XhTcpConnect::GetPeerAddr(TCHAR* pRemoteAddr)
{	
	STRY;
	return ::_stprintf (pRemoteAddr,"%d.%d.%d.%d :%d",m_remotePeer.sin_addr.S_un.S_un_b.s_b1 
			,m_remotePeer.sin_addr.S_un.S_un_b.s_b2
			,m_remotePeer.sin_addr.S_un.S_un_b.s_b3
			,m_remotePeer.sin_addr.S_un.S_un_b.s_b4
			,ntohs(m_remotePeer.sin_port ));
	SCATCH;
	return 0;
}

int XhTcpConnect::GetPortInfo(UINT64& selfAddr,UINT64& remoteAddr)
{
	STRY;
	selfAddr	= m_selfAddr;
	remoteAddr	= m_peerAddr;
	return 1;
	SCATCH;
	return 0;
}

long XhTcpConnect::CheckConnect( DWORD dwTicket )
{
	STRY;
	AutoAddRefAndRelease autoAdd(this);
	if(!autoAdd.IsAddRefSuccessed())
		return -3;
	if(!m_bNeedSendHead)
		return -1;
	DWORD dwLastTs = 0;
	if(m_bIsServerSide)
	{
		STRY;
		iCMSP_SimpleLock lockLastRecvTs(&m_visitLastRecvTs,__FUNCTION__);
		dwLastTs = m_dwLastRecvTs;
		if(dwLastTs > dwTicket)
			return 1;
		SCATCH;
		if(dwTicket - dwLastTs > (CHECK_CONNECTION_TIME_SPAN*6)) //3分钟没有收到数据包括心跳，则尝试向对方发送心跳，如果两次尝试不成功则认为断开
		{
			BYTE bSendData[150] = {0};
			const TCHAR* strCheckTag = _T("CHECK_CONNECTION");
			BYTE* pSendData = (BYTE*)bSendData;
			pSendData += sizeof(ST_CMD_HEADER);
			int nCheckTagLen = sizeof(TCHAR)*lstrlen(strCheckTag);
			ST_CMD_HEADER* pHeader = (ST_CMD_HEADER*)bSendData;
			pHeader->wExtendType = 65000;
			pHeader->dwExndSize = nCheckTagLen;
			CopyMemory(pSendData,strCheckTag,nCheckTagLen);
			int nRet = this->SendData(m_peerAddr,(BYTE*)bSendData,sizeof(ST_CMD_HEADER)+nCheckTagLen);
			if(nRet < 1)
			{
				m_nTrySendDataCnt++;
				if(m_nTrySendDataCnt == 2)
				{
					TTRACE("0x%p The server side has %ds don't recv the peer %s's data,so...,we have to stop the connect and notify the User\r\n",this,((dwTicket - dwLastTs)/1000),m_chPeerAddr);
					Release(__FUNCTION__);
				}
			}
			else
			{
				m_dwLastRecvTs = dwTicket;
				m_nTrySendDataCnt = 0;
			}
		}
	}
	else
	{
		STRY;
		iCMSP_SimpleLock lockLastSendTS(&m_visitLastSendCheckPackegTS,__FUNCTION__);
		dwLastTs = m_dwLastSendCheckPackegTS;
		if(dwLastTs > dwTicket)
			return 1;
		SCATCH;
		if(dwTicket - dwLastTs > CHECK_CONNECTION_TIME_SPAN)
		{
			BYTE bSendData[150] = {0};
			const TCHAR* strCheckTag = _T("CHECK_CONNECTION");
			BYTE* pSendData = (BYTE*)bSendData;
			pSendData += sizeof(ST_CMD_HEADER);
			int nCheckTagLen = sizeof(TCHAR)*lstrlen(strCheckTag);
			ST_CMD_HEADER* pHeader = (ST_CMD_HEADER*)bSendData;
			pHeader->wExtendType = 65000;
			pHeader->dwExndSize = nCheckTagLen;
			CopyMemory(pSendData,strCheckTag,nCheckTagLen);
			this->SendData(m_peerAddr,(BYTE*)bSendData,sizeof(ST_CMD_HEADER)+nCheckTagLen);
			STRY;
			iCMSP_SimpleLock lockLastSendTS(&m_visitLastSendCheckPackegTS,__FUNCTION__);
			m_dwLastSendCheckPackegTS = dwTicket;
			SCATCH;
			TTRACE("time:%d   addr:0x%p  send the check packet to %s\r\n",dwTicket,this,m_chPeerAddr);
		}
	}
	SCATCH;
	return -1;
}

BOOL XhTcpConnect::IsTheCheckConnectionPacketData( BYTE* pDataAddr,int nDataLen )
{
	STRY;
	const TCHAR* strCheckTag = _T("CHECK_CONNECTION");
	int nCheckTagLen = sizeof(TCHAR)*lstrlen(strCheckTag);
	if(nDataLen != sizeof(ST_CMD_HEADER)+nCheckTagLen)
		return FALSE;
	ST_CMD_HEADER *pHeader = (ST_CMD_HEADER *)pDataAddr;
	if(pHeader->wExtendType == 65000)
	{
		TCHAR chTag[20] = {0};
		CopyMemory(chTag,pDataAddr+sizeof(ST_CMD_HEADER),nCheckTagLen);
		if((CString)chTag == (CString)strCheckTag)
		{
			return TRUE;
		}
		return FALSE;
	}
	else
		return FALSE;
	SCATCH;
	return FALSE;
}

int XhTcpConnect::AddUseRef()
{
	STRY;
	iCMSP_SimpleLock lockUseRef(&m_visitUseRefCS,__FUNCTION__);
	if(m_nUseRef <= 0)//为0的时候告诉外面不能再使用这个对象了
		return -2;
	m_nUseRef ++;
	return 1;
	SCATCH;
	return -1;
}

int XhTcpConnect::Release(const TCHAR* pFun,BOOL bIsNotify)
{
	STRY;
	iCMSP_SimpleLock lockUseRef(&m_visitUseRefCS,__FUNCTION__);
	//TTRACE("%s call the 0x%p Release Func\r\n",pFun,this);
	if(m_nUseRef == 0)
	{
		return -2;
	}
	else
	{
		m_nUseRef --;
		if(m_nUseRef == 0)
		{
			//TTRACE("the refcount is 0,so...we have to stop the connect\r\n");
			if(bIsNotify)
				CTcpIocpThread::GetTcpIocp ()->PostQueuedCompletionMsg(this,-0x18);
			else
				CTcpIocpThread::GetTcpIocp ()->PostQueuedCompletionMsg(this,-0x19);
		}
			//StopConnect(bIsNotify);
		return 2;
	}
	SCATCH;
	return -1;
}

int XhTcpConnect::GetUseRefCount()
{
	iCMSP_SimpleLock lockUseRef(&m_visitUseRefCS,__FUNCTION__);
	return m_nUseRef;
}

DWORD XhTcpConnect::GetLastRecvDataTS()
{
	if(m_bIsServerSide)
	{
		iCMSP_SimpleLock lockLastRecvTs(&m_visitLastRecvTs,__FUNCTION__);
		return m_dwLastRecvTs;
	}
	else
	{
		iCMSP_SimpleLock lockLastSendTS(&m_visitLastSendCheckPackegTS,__FUNCTION__);
		return m_dwLastSendCheckPackegTS;
	}
	return 0;
}

// map<void*,int> g_mapObjAddr;
// map<void*,int> g_mapOtherAddr;
// iCMSP_SimpleCS g_mapAddrCS;
// 
// void iSOC_AddAddr(void* pAddr,int nSize,BOOL bIsObject = TRUE)
// {
// 	STRY;
// 	iCMSP_SimpleLock lockAddrmap(&g_mapAddrCS,__FUNCTION__);
// 	if(bIsObject)
// 	{
// 		g_mapObjAddr[pAddr] = nSize;
// 		TTRACE("%s XhTcpConnect对象个数为：%d\r\n",__FUNCTION__,g_mapObjAddr.size());
// 	}
// 	else
// 		g_mapOtherAddr[pAddr] = nSize;
// 	SCATCH;
// }
// 
// void iSOC_DelAddr(void* pAddr,BOOL bIsObject = TRUE)
// {
// 	STRY;
// 	iCMSP_SimpleLock lockAddrmap(&g_mapAddrCS,__FUNCTION__);
// 	if(bIsObject)
// 	{
// 		map<void*,int>::iterator itor = g_mapObjAddr.find(pAddr);
// 		if(itor != g_mapObjAddr.end())
// 			g_mapObjAddr.erase(itor);
// 		TTRACE("%s XhTcpConnect对象个数为：%d\r\n",__FUNCTION__,g_mapObjAddr.size());
// 	}
// 	else
// 	{
// 		map<void*,int>::iterator itor = g_mapOtherAddr.find(pAddr);
// 		if(itor != g_mapOtherAddr.end())
// 			g_mapOtherAddr.erase(itor);
// 	}
// 	SCATCH;
// }
// 
// class XhTestMemory
// {
// public:
// 	XhTestMemory()
// 	{
// 
// 	};
// 
// 	~XhTestMemory()
// 	{
// 		STRY;
// 		iCMSP_SimpleLock lockAddrmap(&g_mapAddrCS,__FUNCTION__);
// 		int nSize = g_mapObjAddr.size();
// 		TTRACE("%s XhTcpConnect对象个数为：%d\r\n",__FUNCTION__,g_mapObjAddr.size());
// 		
// 		SCATCH;
// 	}
// };
// 
// XhTestMemory g_test;
// 
// void* XhTcpConnect::operator new(size_t size,const char* file,int line)
// {
// 	// 	if(!g_bIsInit && g_bUseAutoGC)
// 	// 	{
// 	// 		g_bIsInit = true;
// 	// 		AGC_InitAutoGC(0);
// 	// 	}
// 	void* pAddr = operator new(size);
// 	if(pAddr != NULL)
// 	{
// 		iSOC_AddAddr(pAddr,size);
// 	}
// 	return pAddr;
// }
// 
// void* XhTcpConnect::operator new( size_t size )
// {
// 	// 	if(!g_bIsInit && g_bUseAutoGC)
// 	// 	{
// 	// 		g_bIsInit = true;
// 	// 		AGC_InitAutoGC(0);
// 	// 	}
// 	// 	if(g_bUseAutoGC)
// 	// 		return AGC_Malloc(size);
// 	void* pAddr =  malloc(size);
// 	if(pAddr != NULL)
// 	{
// 		iSOC_AddAddr(pAddr,size);
// 	}
// 	return pAddr;
// }
// 
// void* XhTcpConnect::operator new( size_t size, void *p )
// {
// 	// 	if(!g_bIsInit && g_bUseAutoGC)
// 	// 	{
// 	// 		g_bIsInit = true;
// 	// 		AGC_InitAutoGC(0);
// 	// 	}
// 	return p;
// }
// 
// void XhTcpConnect::operator delete( void* obj )
// {
// 	// 	if(!g_bIsInit && g_bUseAutoGC)
// 	// 	{
// 	// 		g_bIsInit = true;
// 	// 		AGC_InitAutoGC(0);
// 	// 	}
// 	// 	if(g_bUseAutoGC)
// 	// 		AGC_Free(obj);
// 	// 	else
// 	// 	{
// 	iSOC_DelAddr(obj);
// 	free(obj);
// 	//	}
// }
// 
// void XhTcpConnect::operator delete( void*, void* )
// {
// 	// 	if(!g_bIsInit && g_bUseAutoGC)
// 	// 	{
// 	// 		g_bIsInit = true;
// 	// 		AGC_InitAutoGC(0);
// 	// 	}
// }
// 
// void* XhTcpConnect::operator new[]( size_t size )
// {
// 	// 	if(!g_bIsInit && g_bUseAutoGC)
// 	// 	{
// 	// 		g_bIsInit = true;
// 	// 		AGC_InitAutoGC(0);
// 	// 	}
// 	// 	if(g_bUseAutoGC)
// 	// 		return AGC_Malloc(size);
// 	void* pAddr = malloc(size);
// 	if(pAddr != NULL)
// 	{
// 		iSOC_AddAddr(pAddr,size);
// 	}
// 	return pAddr;
// }
// 
// void* XhTcpConnect::operator new[]( size_t size, void *p )
// {
// 	// 	if(!g_bIsInit && g_bUseAutoGC)
// 	// 	{
// 	// 		g_bIsInit = true;
// 	// 		AGC_InitAutoGC(0);
// 	// 	}
// 	return p;
// }
// 
// void* XhTcpConnect::operator new[](size_t size,const char* file,int line)
// {
// 	// 	if(!g_bIsInit && g_bUseAutoGC)
// 	// 	{
// 	// 		g_bIsInit = true;
// 	// 		AGC_InitAutoGC(0);
// 	// 	}
// 	// 	if(g_bUseAutoGC)
// 	// 		return AGC_Malloc(size);
// 	void* pAddr =  malloc(size);
// 	if(pAddr != NULL)
// 	{
// 		iSOC_AddAddr(pAddr,size);
// 	}
// 	return pAddr;
// }
// 
// void XhTcpConnect::operator delete[]( void* obj )
// {
// 	// 	if(!g_bIsInit && g_bUseAutoGC)
// 	// 	{
// 	// 		g_bIsInit = true;
// 	// 		AGC_InitAutoGC(0);
// 	// 	}
// 	// 	if(g_bUseAutoGC)
// 	// 		AGC_Free(obj);
// 	// 	else
// 	// 	{
// 	iSOC_DelAddr(obj);
// 	free(obj);
// 	//	}
// }
// 
// void XhTcpConnect::operator delete[]( void*, void* )
// {
// 	// 	if(!g_bIsInit && g_bUseAutoGC)
// 	// 	{
// 	// 		g_bIsInit = true;
// 	// 		AGC_InitAutoGC(0);
// 	// 	}
// }
