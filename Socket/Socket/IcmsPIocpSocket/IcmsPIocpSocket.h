// IcmsPIocpSocket.h : IcmsPIocpSocket DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CIcmsPIocpSocketApp
// �йش���ʵ�ֵ���Ϣ������� IcmsPIocpSocket.cpp
//

class CIcmsPIocpSocketApp : public CWinApp
{
public:
	CIcmsPIocpSocketApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
