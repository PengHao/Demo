//
//  MLFStream.m
//  LogPlayer
//
//  Created by 彭浩 on 16/8/5.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

#import "MLFStream.h"
#import "MyFstream.h"

@interface MLFStream () {
    MyFstream *pReadStream;
    MyFstream *pWriteStream;
}

@end

@implementation MLFStream

- (instancetype)initWithFilePath: (NSString *)path {
    if (self = [super init]) {
        NSLog(@"initWithFilePath %@", path);
        pWriteStream = new MyFstream(path.UTF8String, OpenFileAPP);
        pReadStream = new MyFstream(path.UTF8String, OpenFileR);
        if (!pWriteStream || !pReadStream) {
            self = nil;
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
}
@end
