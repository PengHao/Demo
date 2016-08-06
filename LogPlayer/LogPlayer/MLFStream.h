//
//  MLFStream.h
//  LogPlayer
//
//  Created by 彭浩 on 16/8/5.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MLFStream : NSObject

- (instancetype __nullable)initWithFilePath: (NSString * __nonnull)path;
- (NSString *__nullable)getLine;
- (void) appendData: (NSData * __nonnull)data;
- (void)flush;
@end
