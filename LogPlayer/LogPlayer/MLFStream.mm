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
    MyFstream *pReadStream;
    MyFstream *pWriteStream;
    TimeHashIndex *pIndex;
}

@end
#define IndexSize 10240
@implementation MLFStream

- (instancetype)initWithFilePath: (NSString *)path {
    if (self = [super init]) {
        NSLog(@"initWithFilePath %@", path);
        pWriteStream = new MyFstream(path.UTF8String, OpenFileAPP);
        pReadStream = new MyFstream(path.UTF8String, OpenFileR);
        if (!pWriteStream || !pReadStream) {
            self = nil;
        }
        
        if (pReadStream->filesize > TimeHashIndex::sIndexSize) {
            char *flag = (char *)pReadStream->read(TimeHashIndex::sIndexSize);
            pIndex = TimeHashIndex::decode(flag, TimeHashIndex::sIndexSize);
            if (!pIndex && pReadStream->filesize == 0) {
                pIndex = new TimeHashIndex(IndexSize);
                char *data = (char *)calloc(1, IndexSize);
                pIndex->decode(data, IndexSize);
                pWriteStream->write(data, IndexSize);
            }
            free(flag);
        }
    }
    return self;
}

- (NSString * __nullable )getLine {
    char *str = pReadStream->get_line();
    if (str && strlen(str) > 0) {
        return [NSString stringWithUTF8String:str];
    } else {
        return nil;
    }
}

- (void) jump:(int) hash {
    const Index<int> *index = pIndex->indexOfHash(hash);
    index->getPosition();
    
}


- (void) appendData: (NSData *)data {
    pWriteStream->write(data.bytes, data.length);
}

- (void)flush {
    pWriteStream->flush();
    [self.delegate streamHasAppend];
}

- (void)dealloc
{
    delete pWriteStream;
    delete pReadStream;
    delete pIndex;
}
@end
