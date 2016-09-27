//
//  MLFStream.m
//  LogPlayer
//
//  Created by 彭浩 on 16/8/5.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

#import "MLFStream.h"
#import "MyFstream.h"
#import "FileIndex.hpp"

using namespace AirCpp;
class TimeHash {
public:
    
};
#define TimeHashIndex FileIndex<int>
@interface MLFStream () {
    struct Content{
        MyFstream *pReadStream;
        MyFstream *pWriteStream;
        
        bool init(NSString *path);
    } _content;
    
    TimeHashIndex *_pIndex;
}

@end
#define IndexSize 10240
#define LineMaxLen 1024
@implementation MLFStream

bool Content::init(NSString *path){
    pWriteStream = new MyFstream(path.UTF8String, OpenFileAPP);
    pReadStream = new MyFstream(path.UTF8String, OpenFileR);
    
    return pWriteStream && pReadStream;
}


- (instancetype)initWithFilePath: (NSString *)path {
    if (self = [super init]) {
        NSLog(@"initWithFilePath %@", path);
        _pIndex = TimeHashIndex::Create(path.UTF8String);
        if (_pIndex == nullptr || !_content.init(path)) {
            NSLog(@"init failed!");
            self = nil;
        }
    }
    return self;
}

- (NSString * __nullable )getLine {
    char *str = (char *)calloc(sizeof(char), LineMaxLen);
    _content.pReadStream->get_line(str, LineMaxLen);
    if (str && strlen(str) > 0) {
        return [NSString stringWithUTF8String:str];
    } else {
        return nil;
    }
}

- (void) addIndex:(int) hash {
    _pIndex->addIndex(hash, _content.pReadStream->position());
}

- (void) jump:(int) hash {
    const Index<int> *index = _pIndex->indexOfHash(hash);
    std::fstream::pos_type pos = index ? index->getPosition() : ios::beg;
    _content.pReadStream->jump(pos);
}


- (void) appendData: (NSData *)data {
    _content.pWriteStream->write(data.bytes, data.length);
}

- (void) flushIndex {
    _pIndex->flush();
    
}

- (void)flush {
    _content.pWriteStream->flush();
}

- (void)dealloc
{
    delete _pIndex;
}
@end
