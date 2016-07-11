#pragma once
struct IOCP_OVERLAPPED;

struct IIocpHandle
{
	virtual long DealIocpAffair(IOCP_OVERLAPPED *pIocpOverlapped,DWORD dataCnt) = 0;
	virtual long OnGotError(IOCP_OVERLAPPED *pIocpOverlapped,DWORD errorCode) =0;
	virtual long GetPeerAddr(TCHAR* pRemoteAddr)=0;
	virtual long CheckConnect(DWORD dwTicket){return 1;};
	virtual DWORD GetLastRecvDataTS(){return 0;};
	virtual ~IIocpHandle(){};
};

#pragma pack(push,1)

#define IOCP_BUFFER_SIZE 4848
struct IOCP_OVERLAPPED
{
	typedef enum {IO_IDLE=0,IO_SENDOUT,IO_RECVIN,IO_ACCEPT}IO_DIRECTION;
	OVERLAPPED		Overlapped;
	IO_DIRECTION	IoDirection;
	WSABUF			Buffer;
	DWORD			userCookie;
	char			chData[IOCP_BUFFER_SIZE];

	IOCP_OVERLAPPED()
	{
		ZeroMemory(&Overlapped,sizeof(IOCP_OVERLAPPED));
		Buffer.len		= IOCP_BUFFER_SIZE;
		Buffer.buf		= chData;
		IoDirection		= IO_IDLE;
	};
	~IOCP_OVERLAPPED()
	{
		Buffer.buf = NULL;
		Buffer.len = 0;
	};
};

struct COMM_HEADER
{
	int				seqId;
	int				totalPacketSize;
	int				nowPacketSize;
};
#pragma pack(pop)

#define CHECK_CONNECTION_TIME_SPAN (30*1000)