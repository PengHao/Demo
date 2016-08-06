//
//  MLFStream.h
//  LogPlayer
//
//  Created by 彭浩 on 16/8/5.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol MLFStreamDelegate <NSObject>

- (void)streamHasAppend;

@end

@interface MLFStream : NSObject
@property(nonatomic, weak, nullable)id<MLFStreamDelegate> delegate;

- (instancetype __nullable)initWithFilePath: (NSString * __nonnull)path;
- (NSString *__nullable)getLine;
- (void) appendData: (NSData * __nonnull)data;
- (void) flush;
@end
