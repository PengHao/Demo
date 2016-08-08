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
#include "MyFstream.h"
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
        
        bool encode(char * data, size_t len) const {
            size_t hashValueSize = sizeof(m_llHashValue);
            size_t positionSize = sizeof(m_Postion);
            if (len < size()) {
                printf("not enouph memery");
                return false;
            }
            char *temp = data;
            memcpy(temp, &m_llHashValue, hashValueSize);
            temp += hashValueSize;
            memcpy(temp, &m_Postion, positionSize);
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
        std::string _strFlag;
        
        struct Stream{
            MyFstream *pReadStream;
            MyFstream *pWriteStream;
            bool init(const std::string &path){
                pWriteStream = new MyFstream(path+".temp", OpenFileCleanW);
                pReadStream = new MyFstream(path, OpenFileR);
                return pWriteStream && pReadStream;
            }
            Stream() :
            pWriteStream(nullptr),
            pReadStream(nullptr){
                
            }
            ~Stream() {
                remove(pReadStream->filename.c_str());
                rename(pWriteStream->filename.c_str(), pReadStream->filename.c_str());
                delete pReadStream;
                delete pWriteStream;
            }
        } *_pStream;
        
        
        FileIndex(Stream *stream) :
        _pStream(stream),
        m_mapPosIndex(std::map<Hash, const _Index*>()){
            
        }
        
        bool init() {
            size_t len = _pStream->pReadStream->filesize;
            if (len == 0) {
            } else {
                if (sIndexFlag.length() > len) {
                    return false;
                }
                char *data = (char *)calloc(len, sizeof(char));
                if (data) {
                    _pStream->pReadStream->read(data, len);
                    char *temp = data;
                    return decode(temp, len);
                } else {
                    printf("calloc data failed");
                    return false;
                }
                
            }
            return true;
        }
        
        
        bool decode(char * const data, const size_t &length) {
            char *temp = data;
            size_t len = length;
            std::string flag(temp, sIndexFlag.length());
            if (flag != sIndexFlag ) {
                return false;
            }
            temp += sIndexFlag.length()+1;
            
            _Index *idx = _Index::decode(temp, len);
            while (idx) {
                m_mapPosIndex[idx->m_llHashValue] = idx;
                idx = _Index::decode(temp, len);
            }
            return true;
        };
    protected:
        
        bool encode(char * data, size_t &length) {
            for (auto pair : m_mapPosIndex) {
                pair.second->encode(data, length);
            }
            return data;
        };
        
    public:
        static FileIndex *Create(const std::string &path) {
            Stream *stream = new Stream();
            if(!stream->init(path + ".index")) {
                delete stream;
                return nullptr;
            }
            auto p_rs =  new FileIndex(stream);
            if (! p_rs->init() ) {
                delete p_rs;
                p_rs = nullptr;
            }
            return p_rs;
        }
        
        static std::string const sIndexFlag;
        
        void addIndex(Hash hash, std::fstream::pos_type pos) {
            if (m_mapPosIndex[hash] == nullptr || m_mapPosIndex[hash]->getPosition() != pos) {
                _Index *index = new _Index(hash, pos);
                m_mapPosIndex[hash] = index;
            } else {
                
            }
        }
        
        const _Index *indexOfHash(Hash hash) {
            return m_mapPosIndex[hash];
        }
        
        void flush() {
            _pStream->pWriteStream->write(sIndexFlag.c_str(), sIndexFlag.length());
            size_t s = _Index::size();
            char *data = (char *)calloc(sizeof(char), s);
            for (auto pair : m_mapPosIndex) {
                memset(data, 0, s);
                if (pair.second->encode(data, s)) {
                    _pStream->pWriteStream->write(data, s);
                }
            }
            free(data);
            _pStream->pWriteStream->flush();
            
        }
        
        ~FileIndex() {
            //newfile
            flush();
            delete _pStream;
        }
    };
    template<class Hash>
    std::string const FileIndex<Hash>::sIndexFlag = "FileIndex";
}
#endif /* FileIndex_hpp */
