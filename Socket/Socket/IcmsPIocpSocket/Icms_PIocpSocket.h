#pragma once

#ifdef _iCMSP_IOCP_SOCKET_SDK
#define iCMS_IOCP_SOCKET_CLASS_MODE __declspec(dllexport)
#else
#define iCMS_IOCP_SOCKET_CLASS_MODE __declspec(dllimport)
#endif

#ifndef _iCMSP_IOCP_SOCKET_SDK
	#ifdef _DEBUG
		#ifdef _UNICODE
			#pragma comment(lib,"IcmsPIocpSocketud.lib")
		#else
			#pragma comment(lib,"IcmsPIocpSocketd.lib")
		#endif
	#else
		#ifdef _UNICODE
			#pragma comment(lib,"IcmsPIocpSocketu.lib")
		#else
			#pragma comment(lib,"IcmsPIocpSocket.lib")		
        #endif
	#endif
#endif

//!!!!!REMBER,MUST SET THE 4 METHOD BE virtual !!!!!!
//!!!!! and the class destruction function be virtual !!!!!!

//when got the data,connected to server(client),the connection is break down
//the iocp dll will call the following callback function
class  ITcpCallbackSink
{
public:
	virtual int OnRecvData(UINT64 fromAddr, BYTE* pData, int dataLen)=0;
	virtual int OnConnected(UINT64 fromAddr,UINT64 localAddr)=0;
	virtual int OnClosed(UINT64 fromAddr)=0;
	virtual int OnReConnected(UINT64 fromAddr)=0;
	virtual	~ITcpCallbackSink(){};
};

class XhTcpListen;
class XhTcpConnect;

//server should use the iCMS_PTcpListner to listen on local port
class iCMS_IOCP_SOCKET_CLASS_MODE iCMS_PTcpListner
{
public:
	iCMS_PTcpListner();
	~iCMS_PTcpListner();

	//1. first of all,when the iCMS_PTcpListner is instantiated,set the callback sink
	int SetCallBackSink(ITcpCallbackSink* pSink);
	//2. set the listen addr
	int DoListenOn(long localIp,short localPort);
	//3. send the data
	//UINT64 toAddr == peerIp << 32 | peerPort,if be -1,send data to all peer
	int SendData(UINT64 toAddr,BYTE* pData,int dataLen);
	//4.when all finished,stop the listen
	int StopListen();
	//initiative to break down the connection with remote peer
	int ClosePeer(UINT64 peerAddr);
	
	void CheckDataHeaderControl(BOOL bNeedCheck = FALSE);

	XhTcpConnect*	GetPeerConnect(UINT64 peerAddr);
private:
	XhTcpListen*		m_pInnerTcpListen;
};

//client should use iCMS_PTcpConnect to connect to server
class iCMS_IOCP_SOCKET_CLASS_MODE iCMS_PTcpConnect
{
public:
	iCMS_PTcpConnect();
	~iCMS_PTcpConnect();
	//1. first of all,when the iCMS_PTcpListner is instantiated,set the callback sink
	int SetCallBackSink(ITcpCallbackSink* pSink);
	//2. connect to server address
	int DoConnect(long remoteIp,short remotePort);
	//3. send the data
	//UINT64 toAddr == remoteIp << 32 | remotePort
	int SendData(UINT64 toAddr,BYTE* pData,int dataLen);
	//4.initiative to break down the connection with remote peer
	int StopConnect();

	void CheckDataHeaderControl(BOOL bNeedCheck = FALSE);

	int GetPortInfo(UINT64& selfAddr,UINT64& remoteAddr);
private:
	XhTcpConnect*		m_pInnerTcpConnect;
};

//the server use iocp sample is like following:
#if 0
//include "iCMS_PIocpsocket.h"
class ServerSide : public ITcpCallbackSink
{
public:
	ServerSide()
		:m_pListen(new iCMS_PTcpListner())
	{
		m_pListen ->SetCallBackSink (this);
		m_pListen ->DoListenOn (0,5000);		
	}

	virtual ~ServerSide()
	{
		delete m_pListen;
	}

	virtual int OnRecvData(UINT64 fromAddr, BYTE* pData, int dataLen)
	{
		printf("recv[%d]:%s\r\n",dataLen,pData);
		m_pListen ->SendData (-1,pData,dataLen);
		return 0;
	};
	virtual int OnConnected(UINT64 fromAddr)
	{
		return 0;
	};
	virtual int OnClosed(UINT64 fromAddr)
	{
		return 0;
	};
	virtual int OnReConnected(UINT64 fromAddr)
	{
		return 0;
	};

	int RunMsgLoop()
	{
		while ('q' != getchar() )
			;
		return 0;
	}
private:
	iCMS_PTcpListner* m_pListen ;
};

int main()
{
	ServerSide serverSide;
	
	return serverSide.RunMsgLoop();
}
#endif