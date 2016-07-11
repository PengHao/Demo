#pragma once
#include "iCMS_PIocpsocket.h"
#include "InnerStruct.h"

class XhTcpConnect;
class XhTcpListen : public IIocpHandle,ITcpCallbackSink
{
public:
	XhTcpListen(void);
	virtual ~XhTcpListen(void);
	virtual long DealIocpAffair(IOCP_OVERLAPPED *pIocpOverlapped,DWORD dataCnt);
	virtual long OnGotError(IOCP_OVERLAPPED *pIocpOverlapped,DWORD errorCode);
	virtual long GetPeerAddr(TCHAR* pRemoteAddr);
	virtual int OnRecvData(UINT64 fromAddr, BYTE* pData, int dataLen);
	virtual int OnConnected(UINT64 fromAddr,UINT64 localAddr);
	virtual int OnClosed(UINT64 fromAddr);
	virtual int OnReConnected(UINT64 fromAddr);

	void CheckDataHeaderControl(BOOL bNeedCheck = FALSE);

	int SetCallBackSink(ITcpCallbackSink* pSink);
	int DoListenOn(long localIp,short localPort);
	int SendData(UINT64 toAddr,BYTE* pData,int dataLen);
	int StopListen();
	int ClosePeer(UINT64 peerAddr);
	XhTcpConnect*	GetPeerConnect(UINT64 peerAddr);
private:
	static DWORD __stdcall Static_ListenThread(void* pParam);
	DWORD ListenThread();
	int DoAcceptSocket();
	IOCP_OVERLAPPED				m_recvOverlapped;
	ITcpCallbackSink*			m_pTcpSink;
	long						m_localIp;
	short						m_localPort;
	SOCKET						m_listenSocket;
	iCMSP_SimpleCS				m_simpleCS;
	map<UINT64,XhTcpConnect*>	m_passiveConnetedMap;
	BOOL						m_bUseAcceptEx;
	BOOL						m_bQuitThread;
	HANDLE						m_hListenThread;
	list<XhTcpConnect*>			m_pFreedQueue;
	BOOL						m_bNeedCheckHearder;

};
