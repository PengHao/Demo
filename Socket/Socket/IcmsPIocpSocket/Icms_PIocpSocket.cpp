#include "stdafx.h"
#include "Icms_PIocpSocket.h"
#include "xhtcpconnect.h"
#include "xhtcpListen.h"
#include "TcpIocpThread.h"

iCMS_PTcpListner::iCMS_PTcpListner()
:m_pInnerTcpListen(new XhTcpListen())
{
}

iCMS_PTcpListner::~iCMS_PTcpListner()
{
	if (m_pInnerTcpListen)
		delete m_pInnerTcpListen;
	m_pInnerTcpListen = NULL;
}

int iCMS_PTcpListner::SetCallBackSink(ITcpCallbackSink* pSink)
{
	if (NULL == m_pInnerTcpListen)
		return -1;
	return m_pInnerTcpListen->SetCallBackSink(pSink);
}

int iCMS_PTcpListner::DoListenOn(long localIp,short localPort)
{
	if (NULL == m_pInnerTcpListen)
		return -1;
	return m_pInnerTcpListen->DoListenOn(localIp,localPort);
}

int iCMS_PTcpListner::SendData(UINT64 toAddr,BYTE* pData,int dataLen)
{
	//TTRACE("%s want to send the data to %I64u ,the data len:%d \r\n",__FUNCTION__,toAddr,dataLen);
	if (NULL == m_pInnerTcpListen)
		return -1;
	return m_pInnerTcpListen->SendData(toAddr,pData,dataLen);
}

int iCMS_PTcpListner::StopListen()
{
	if (NULL == m_pInnerTcpListen)
		return -1;
	return m_pInnerTcpListen->StopListen();
}

int iCMS_PTcpListner::ClosePeer(UINT64 peerAddr)
{
	if (NULL == m_pInnerTcpListen)
		return -1;
	return m_pInnerTcpListen->ClosePeer(peerAddr);
}

XhTcpConnect*	iCMS_PTcpListner::GetPeerConnect(UINT64 peerAddr)
{
	if (NULL == m_pInnerTcpListen)
		return NULL;
	return m_pInnerTcpListen->GetPeerConnect(peerAddr);
}

void iCMS_PTcpListner::CheckDataHeaderControl( BOOL bNeedCheck /*= FALSE*/ )
{
	if(NULL == m_pInnerTcpListen)
		return;
	m_pInnerTcpListen->CheckDataHeaderControl(bNeedCheck);
}

iCMS_PTcpConnect::iCMS_PTcpConnect()
:m_pInnerTcpConnect(new XhTcpConnect)
{
}

iCMS_PTcpConnect::~iCMS_PTcpConnect()
{
	SAFE_DELETE(m_pInnerTcpConnect);
}

int iCMS_PTcpConnect::SetCallBackSink(ITcpCallbackSink* pSink)
{
	if (NULL == m_pInnerTcpConnect)
		return -1;
	return m_pInnerTcpConnect->SetCallBackSink(pSink);
}

int iCMS_PTcpConnect::DoConnect(long localIp,short localPort)
{
	if (NULL == m_pInnerTcpConnect)
		return -1;
	return m_pInnerTcpConnect->DoConnect(localIp,localPort);
}

int iCMS_PTcpConnect::SendData(UINT64 toAddr,BYTE* pData,int dataLen)
{
	if (NULL == m_pInnerTcpConnect)
		return -1;
	return m_pInnerTcpConnect->SendData(toAddr,pData,dataLen);
}

int iCMS_PTcpConnect::StopConnect()
{
	if (NULL == m_pInnerTcpConnect)
		return -1;
	return m_pInnerTcpConnect->Release(__FUNCTION__);
}

int iCMS_PTcpConnect::GetPortInfo(UINT64& selfAddr,UINT64& remoteAddr)
{
	if (NULL == m_pInnerTcpConnect)
		return -1;
	return m_pInnerTcpConnect->GetPortInfo(selfAddr,remoteAddr);
}

void iCMS_PTcpConnect::CheckDataHeaderControl( BOOL bNeedCheck /*= FALSE*/ )
{
	if(NULL == m_pInnerTcpConnect)
		return;
	m_pInnerTcpConnect->CheckDataHeaderControl(bNeedCheck);
}