//
//  FileIndex.hpp
//  LogPlayer
//
//  Created by 彭浩 on 16/8/7.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

#ifndef LogFile_hpp
#define LogFile_hpp

#include <stdio.h>
#include <fstream>
#include <map>
namespace AirCpp {
    template <class Hash> class FileIndex;
    
    template <class Hash>
    class Index {
        friend class FileIndex<Hash>;
    private:
        Hash m_llHashValue;
        std::fstream::pos_type m_Postion;
        
    protected:
        Index(Hash hash, std::fstream::pos_type pos) :
        m_llHashValue(hash),
        m_Postion(pos)
        {
            
        }
        
        static size_t size() {
            size_t hashValueSize = sizeof(Hash);
            size_t positionSize = sizeof(std::fstream::pos_type);
            return hashValueSize + positionSize;
        }
        
        static Index *decode(char * data, size_t &length) {
            if (length < size()) {
                printf("not enouph data");
                return nullptr;
            }
            Hash hash;
            std::fstream::pos_type pos;
            size_t hashValueSize = sizeof(hash);
            size_t positionSize = sizeof(pos);
            
            memcpy(&hash, data, hashValueSize);
            data += hashValueSize;
            length -= hashValueSize;
            memcpy(&pos, data, positionSize);
            data += positionSize;
            length -= positionSize;
            
            Index *pIndex = new Index(hash, pos);
            return pIndex;
        };
        
        bool encode(char * data, size_t &length) const {
            size_t hashValueSize = sizeof(m_llHashValue);
            size_t positionSize = sizeof(m_Postion);
            if (length < size()) {
                printf("not enouph memery");
                return false;
            }
            memcpy(data, &m_llHashValue, sizeof(m_llHashValue));
            data += hashValueSize;
            length -= hashValueSize;
            memcpy(data, &m_Postion, sizeof(m_Postion));
            data += positionSize;
            length -= positionSize;
            return true;
        };
        
    public:
        std::fstream::pos_type getPosition() const {
            return m_Postion;
        }
    };
    
    
    template <class Hash>
    class FileIndex {
#define _Index Index<Hash>
    private:
        std::map<Hash, const _Index*> m_mapPosIndex;
        size_t const m_llIndexSize;
        std::string _strFlag;
    protected:
        
        bool encode(char * data, size_t &length) {
            for (auto pair : m_mapPosIndex) {
                pair.second->encode(data, length);
            }
            return data;
        };
        
    public:
        static std::string const sIndexFlag;
        static size_t const sIndexSize;
        
        bool addIndex(Hash hash, std::fstream::pos_type pos) {
            _Index *index = new _Index(hash, pos);
            m_mapPosIndex[hash] = index;
        }
        
        const _Index *indexOfHash(Hash hash) {
            return m_mapPosIndex[hash];
        }
        
        static FileIndex *decode(char * const data, const size_t &length) {
            char *temp = data;
            size_t len = length;
            if (std::string(temp, sIndexFlag.length()+1) != sIndexFlag ) {
                return nullptr;
            }
            temp += sIndexFlag.length()+1;
            size_t s = *(size_t *)temp;
            if (s != sIndexSize) {
                return nullptr;
            }
            
            size_t indexSize = sizeof(size_t);
            temp += indexSize;
            if (indexSize > length) {
                return nullptr;
            }
            temp += indexSize;
            
            FileIndex *index = new FileIndex(indexSize);
            _Index *idx = _Index::decode(temp, len);
            while (idx) {
                index->m_mapPosIndex[idx->m_llHashValue] = idx;
                idx = _Index::decode(temp, len);
            }
            return index;
        };
        
        FileIndex(size_t indexSize) :
        m_llIndexSize(indexSize)
        {
            
        }
        
        ~FileIndex() {
            for (auto pair : m_mapPosIndex) {
                delete pair.second;
            }
        }
    };
    template<class Hash>
    std::string const FileIndex<Hash>::sIndexFlag = "FileIndex";
    
    template<class Hash>
    size_t const FileIndex<Hash>::sIndexSize = sizeof(size_t) + sIndexFlag.length();
    
}
#endif /* FileIndex_hpp */
