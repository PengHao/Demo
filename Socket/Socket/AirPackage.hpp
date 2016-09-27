//
//  AirPackage.hpp
//  Socket
//
//  Created by 彭浩 on 16/7/26.
//  Copyright © 2016年 彭浩. All rights reserved.
//

#ifndef AirPackage_hpp
#define AirPackage_hpp

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

namespace AirCpp {
    class CircleBuffer {
    protected:
        unsigned long long m_ullSize;
        unsigned char *m_pData;
        unsigned char *m_pRead;
        unsigned char *m_pWrite;
        
    public:
        CircleBuffer(unsigned long long size) :
        m_ullSize(size),
        m_pData(nullptr),
        m_pWrite(nullptr),
        m_pRead(nullptr)
        {
            if (m_ullSize > 0) {
                m_pData = (unsigned char *)calloc(m_ullSize, sizeof(unsigned char));
                m_pWrite = m_pData;
                m_pRead = m_pData;
            }
        };
        
        unsigned long long write(const char *c_data, unsigned long long length) {
            unsigned long long writeSize = 0;
            unsigned long long leftSize = m_pWrite - m_pData;
            if (leftSize < length) {
                memcpy(m_pWrite, c_data, leftSize);
                writeSize = leftSize;
                m_pWrite = m_pData;
                writeSize += write(c_data + leftSize, length - leftSize);
            } else {
                memcpy(m_pWrite, c_data, length);
                m_pWrite += length;
                writeSize = length;
            }
            return writeSize;
        }
        
        unsigned long long read(char *c_data, unsigned long long length) {
            unsigned long long readSize = 0;
            unsigned long long leftSize = m_pRead - m_pData;
            if (leftSize < length) {
                memcpy(c_data, m_pRead, leftSize);
                readSize = leftSize;
                m_pRead = m_pData;
                readSize += read(c_data + leftSize, length - leftSize);
            } else {
                memcpy(c_data, m_pRead, length);
                m_pRead += length;
                readSize = length;
            }
            return readSize;
        }
    };

    
    
    class Package {
    public:
        unsigned long long m_ullSize;
        unsigned char *m_pData;
<<<<<<< HEAD
        typedef std::function<void(const Package &package)> FillPackageCallBack;
=======
        typedef std::function<void(const Package *package)> FillPackageCallBack;
>>>>>>> e4bdcbc1cdbe18472274b2dffc28334825527857
    public:
        unsigned long long m_ullSettedSize;
        Package():
        m_ullSize(0),
        m_ullSettedSize(0),
        m_pData(nullptr) {
            
        }
        
        ~Package() {
<<<<<<< HEAD
            clean();
        }
        
        void clean(){
            m_ullSize = 0;
            m_ullSettedSize = 0;
            free(m_pData);
            m_pData = nullptr;
        }
        
        static Package _mCurrentPackage;
        
        static void FillData(unsigned long long len, char *data, FillPackageCallBack handleFilledPackage) {
            
            unsigned long long sizeofSize = sizeof(_mCurrentPackage.m_ullSize);
            unsigned char *pOffset = (unsigned char *)&_mCurrentPackage.m_ullSize;
            char *pDataOffset = data;
            if (_mCurrentPackage.m_ullSettedSize < sizeofSize) {
                pOffset += _mCurrentPackage.m_ullSettedSize;
                auto s = std::min(sizeofSize - _mCurrentPackage.m_ullSettedSize, len);
                memcpy(pOffset, pDataOffset, s);
                _mCurrentPackage.m_ullSettedSize += s;
=======
            free(m_pData);
            m_pData = nullptr;
            m_pData = 0;
        }
        static Package * pCurrentPackage;
        
        static void FillData(unsigned long long len, char *data, FillPackageCallBack handleFilledPackage) {
            if (!pCurrentPackage) {
                pCurrentPackage = new Package();
            }
            
            unsigned long long sizeofSize = sizeof(pCurrentPackage->m_ullSize);
            unsigned char *pOffset = (unsigned char *)&pCurrentPackage->m_ullSize;
            char *pDataOffset = data;
            if (pCurrentPackage->m_ullSettedSize < sizeofSize) {
                pOffset += pCurrentPackage->m_ullSettedSize;
                auto s = std::min(sizeofSize - pCurrentPackage->m_ullSettedSize, len);
                memcpy(pOffset, pDataOffset, s);
                pCurrentPackage->m_ullSettedSize += s;
>>>>>>> e4bdcbc1cdbe18472274b2dffc28334825527857
                pDataOffset += s;
                len -= s;
                FillData(len, pDataOffset, handleFilledPackage);
            } else {
<<<<<<< HEAD
                if (_mCurrentPackage.m_ullSize > 0 && _mCurrentPackage.m_pData == nullptr) {
                    _mCurrentPackage.m_pData = (unsigned char *)calloc(_mCurrentPackage.m_ullSize, sizeof(unsigned char));
                }
                pOffset = _mCurrentPackage.m_pData;
                size_t settedDataSize = _mCurrentPackage.m_ullSettedSize - sizeofSize;
                size_t unsetDataSize = _mCurrentPackage.m_ullSize - settedDataSize;
                pOffset += settedDataSize;
                if (unsetDataSize <= len) {
                    memcpy(pOffset, data, unsetDataSize);
                    _mCurrentPackage.m_ullSettedSize += unsetDataSize;
                    handleFilledPackage(_mCurrentPackage);
                    _mCurrentPackage.clean();
=======
                if (pCurrentPackage->m_ullSize > 0 && pCurrentPackage->m_pData == nullptr) {
                    pCurrentPackage->m_pData = (unsigned char *)calloc(pCurrentPackage->m_ullSize, sizeof(unsigned char));
                }
                pOffset = pCurrentPackage->m_pData;
                size_t settedDataSize = pCurrentPackage->m_ullSettedSize - sizeofSize;
                size_t unsetDataSize = pCurrentPackage->m_ullSize - settedDataSize;
                pOffset += settedDataSize;
                if (unsetDataSize <= len) {
                    memcpy(pOffset, data, unsetDataSize);
                    pCurrentPackage->m_ullSettedSize += unsetDataSize;
                    handleFilledPackage(pCurrentPackage);
                    
>>>>>>> e4bdcbc1cdbe18472274b2dffc28334825527857
                    len -= unsetDataSize;
                    pDataOffset += unsetDataSize;
                    if (len > 0) {
                        FillData(len, pDataOffset, handleFilledPackage);
                    }
                } else {
                    memcpy(pOffset, data, len);
<<<<<<< HEAD
                    _mCurrentPackage.m_ullSettedSize += len;
=======
                    pCurrentPackage->m_ullSettedSize += len;
>>>>>>> e4bdcbc1cdbe18472274b2dffc28334825527857
                }
            }
        }
    };
<<<<<<< HEAD
=======
    Package * Package::pCurrentPackage = nullptr;
>>>>>>> e4bdcbc1cdbe18472274b2dffc28334825527857
}
#endif /* AirPackage_hpp */
