// File:        test.cpp
// Description: ---
// Notes:       ---
// Author:      penghao <penghao@medlinker.com>
// Revision:    2016-04-28 by penghao
//
// Copyright (C) 2014-2016 Medlinker. All Rights Reserved.
// This is unpublished proprietary source code of Medlinker Ltd. The copyright
// notice above does not evidence any actual or intended publication of such
// source code. UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE SUBJECT
// TO CIVIL AND CRIMINAL PENALTIES.

#include "AirBSDSocketIO.h"

int main() {
    AirBSDSocketIO *pIO = new AirBSDSocketIO();
    pIO->connect_to_host("www.baidu.com", 80);
    delete pIO; 
}

// vim:ts=4:sw=4:et:
