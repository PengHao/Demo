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
            if (m_ullSize ==0) {
                return;
            }
            m_pData = (unsigned char *)calloc(m_ullSize, sizeof(unsigned char));
            m_pWrite = m_pData;
            m_pRead = m_pData;
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
        typedef std::function<void(const Package *package)> FillPackageCallBack;
    public:
        unsigned long long m_ullSettedSize;
        Package():
        m_ullSize(0),
        m_ullSettedSize(0),
        m_pData(nullptr) {
            
        }
        
        ~Package() {
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
                pDataOffset += s;
                len -= s;
                FillData(len, pDataOffset, handleFilledPackage);
            } else {
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
                    
                    len -= unsetDataSize;
                    pDataOffset += unsetDataSize;
                    if (len > 0) {
                        FillData(len, pDataOffset, handleFilledPackage);
                    }
                } else {
                    memcpy(pOffset, data, len);
                    pCurrentPackage->m_ullSettedSize += len;
                }
            }
        }
    };
    
}
#endif /* AirPackage_hpp */
