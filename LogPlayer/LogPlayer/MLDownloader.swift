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
    
    
    required init(urlString: String) {
        super.init()
        if let url = NSURL(string: urlString) {
            session = NSURLSession(configuration: NSURLSessionConfiguration.defaultSessionConfiguration(), delegate: self, delegateQueue: NSOperationQueue.currentQueue())
            sessionTask = session.dataTaskWithURL(url)
        }
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
        didReceiveData?(data)
    }
    
    func URLSessionDidFinishEventsForBackgroundURLSession(session: NSURLSession) {
        didFinished?()
    }
    
    func URLSession(session: NSURLSession, didBecomeInvalidWithError error: NSError?) {
        didFailed?(error)
    }
}