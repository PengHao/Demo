#pragma once
#include "iCMS_PIocpsocket.h"
#include "InnerStruct.h"
#include "TcpIocpThread.h"

struct ST_ListNode
{
	char chData[IOCP_BUFFER_SIZE];
	int  nDataLen;
	void* pUserData;
	ST_ListNode()
	{
		ZeroMemory(chData,IOCP_BUFFER_SIZE);
		nDataLen = 0;
		pUserData = NULL;
	}

	ST_ListNode(const char* pData,int nLen,void* pPrvData)
	{
		ZeroMemory(chData,sizeof(chData));
		nDataLen = nLen;
		if(nDataLen > 0 && pData)
		{
			CopyMemory(chData,pData,nDataLen);
		}
		pUserData = pPrvData;
	}

	ST_ListNode(const ST_ListNode& node)
	{
		if(node.nDataLen > 0)
		{
			CopyMemory(chData,node.chData,node.nDataLen);
			nDataLen = node.nDataLen;
		}
		else
		{
			nDataLen = 0;
		}

		pUserData = node.pUserData;
	}

	const ST_ListNode& operator =(const ST_ListNode& node)
	{
		if(node.nDataLen > 0)
		{
			CopyMemory(chData,node.chData,node.nDataLen);
			nDataLen = node.nDataLen;
		}
		else
		{
			nDataLen = 0;
		}
		return *this;
	}
};

class XhTcpConnect : public IIocpHandle
{
public:
	XhTcpConnect(void);
	virtual ~XhTcpConnect(void);

	virtual long DealIocpAffair(IOCP_OVERLAPPED *pIocpOverlapped,DWORD dataCnt);
	virtual long OnGotError(IOCP_OVERLAPPED *pIocpOverlapped,DWORD errorCode);
	virtual long GetPeerAddr(TCHAR* pRemoteAddr);
	virtual long CheckConnect(DWORD dwTicket);
	virtual DWORD GetLastRecvDataTS();

	int SetCallBackSink(ITcpCallbackSink* pSink);
	int DoPassiveConnected(SOCKET sConnected,struct sockaddr_in* pRemoteAddr,ITcpCallbackSink* pSink);
	int DoConnect(long localIp,short localPort);
	int SendData(UINT64 toAddr,BYTE* pData,int dataLen);
	int StopConnect(bool bNeedNotifySink = true);
	int GetPortInfo(UINT64& selfAddr,UINT64& remoteAddr);

	void CheckDataHeaderControl(BOOL bNeedCheck = FALSE)
	{
		m_bNeedSendHead = bNeedCheck;
	};

	int AddUseRef();
	int Release(const TCHAR* pFun,BOOL bIsNotify=TRUE);
	int GetUseRefCount();

	template <class K,class V>
	struct DoAllSend
	{
		DoAllSend(BYTE* pData,int dataLen):m_pData(pData),m_dataLen(dataLen){};
		bool operator() (const pair<K,V> elem)
		{
			bool resultCode = false;
			XhTcpConnect* pTcpConnect = (XhTcpConnect*)elem.second ;
			if (pTcpConnect /*&& !::IsBadWritePtr (pTcpConnect,sizeof(XhTcpConnect))*/)
				resultCode = pTcpConnect->SendData (pTcpConnect->m_peerAddr,m_pData,m_dataLen) >0 ? true : false;
			return resultCode;
		};
		BYTE*			m_pData;
		int				m_dataLen;
	};

// 	void* operator new(size_t size,const char* file,int line);
// 	void* operator new( size_t size );
// 	void* operator new( size_t size, void *p );
// 	/* Must be redefined here, since the other overloadings	*/
// 	/* hide the global definition.				*/
// 	void operator delete( void* obj );
// 	void operator delete( void*, void* );
// 
// 	void* operator new[]( size_t size );
// 	void* operator new[]( size_t size, void *p );
// 	void* operator new[](size_t size,const char* file,int line);
// 	void operator delete[]( void* obj );
// 	void operator delete[]( void*, void* );

	UINT64						m_peerAddr;
	UINT64						m_selfAddr;
	IOCP_OVERLAPPED				m_recvOverlapped;
	BOOL						m_bLogTransData;
	BOOL						m_bIsServerSide;
private:
	int DoRecvAgain();
	int DoPostMsg2Exit();
	BOOL	IsTheCheckConnectionPacketData(BYTE* pDataAddr,int nDataLen);

	SOCKET						m_connectSocket;
	struct sockaddr_in			m_remotePeer;
	ITcpCallbackSink*			m_pTcpSink;
	iCMSP_SimpleCS				m_simpleCS;
	iCMSP_SimpleCS				m_visitQuitFlag;
	iCMSP_SimpleCS				m_visitDealFunction;
	int							m_commSeqId;
	int							m_lastRecvSeqId;
	BYTE*						m_pRecvDataBuf;
	int							m_recvCapacity;	
	BYTE*						m_pWritePtr;
	COMM_HEADER					m_lastCommHeader;
	DWORD						m_dwLastRecvTs;
	iCMSP_SimpleCS				m_visitLastRecvTs;
	TCHAR						m_chPeerAddr[32];
	TCHAR						m_chSelfAddr[32];
	BOOL						m_bPassiveConnect;
	char						m_chLeftMem[IOCP_BUFFER_SIZE*2];
	int							m_nLeftCnt;
	BOOL						m_bQuit;
	BOOL						m_bNeedSendHead;
	int							m_nUseRef;
	iCMSP_SimpleCS				m_visitUseRefCS;
	DWORD						m_dwLastSendCheckPackegTS;
	iCMSP_SimpleCS				m_visitLastSendCheckPackegTS;
	int							m_nTrySendDataCnt;
};
