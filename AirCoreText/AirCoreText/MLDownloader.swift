//
//  MLDownloader.swift
//  LogPlayer
//
//  Created by PengHao on 16/8/6.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation

class MLDownloader: NSObject {
    private var didReceiveData: ((NSData)->Void)?
    private var didFinished: (()->Void)?
    private var didFailed: ((NSError?)->Void)?

    private var session: NSURLSession!
    private var sessionTask: NSURLSessionTask!
    
    func read(stream: UnsafeMutablePointer<FILE>) {
        let buffer = UnsafeMutablePointer<UInt8>(malloc(10240))
        memset(buffer, 0, 10240)
        let size = fread(buffer, sizeof(CChar), 10240, stream)
        if size > 0 {
            let data = NSData(bytes: buffer, length: size)
            didReceiveData?(data)
        }
        if size == 10240 {
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, Int64(NSEC_PER_MSEC) * 1), dispatch_get_main_queue(), {
                self.read(stream)
            })
        } else {
            didFinished?()
        }
        free(buffer)
    }
    
    required init(urlString: String) {
        super.init()
        
        let s = fopen((urlString as NSString).UTF8String, "r")
        guard s != nil else {
            if let url = NSURL(string: urlString) {
                session = NSURLSession(configuration: NSURLSessionConfiguration.defaultSessionConfiguration(), delegate: self, delegateQueue: NSOperationQueue.currentQueue())
                sessionTask = session.dataTaskWithURL(url)
            }
            return
        }
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, Int64(NSEC_PER_SEC) * 1), dispatch_get_main_queue(), {
            self.read(s)
        })
    }
    
    func start() -> MLDownloader {
        sessionTask.resume()
        return self
    }
    
    func receiveDataHandle(handle: (NSData)->Void) -> MLDownloader {
        didReceiveData = handle
        return self
    }
    
    func finishedHandle(handle: ()->Void) -> MLDownloader {
        didFinished = handle
        return self
    }
    
    func failedHandle(handle: (NSError?)->Void) -> MLDownloader {
        didFailed = handle
        return self
    }
    
    deinit {
        sessionTask.cancel()
    }
}


extension MLDownloader : NSURLSessionDataDelegate{
    
    func URLSession(session: NSURLSession, dataTask: NSURLSessionDataTask, didReceiveData data: NSData) {
        data.enumerateByteRangesUsingBlock { [weak self] (cdata, range, context) in
            self?.didReceiveData?(NSData(bytes: cdata, length: range.length))
        }
    }
    
    func URLSession(session: NSURLSession, task: NSURLSessionTask, didCompleteWithError error: NSError?) {
        if let err = error {
            didFailed?(err)
        } else {
            didFinished?()
        }
    }
}
