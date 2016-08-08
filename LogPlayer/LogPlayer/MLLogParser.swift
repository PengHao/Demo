//
//  MLLogParser.swift
//  LogPlayer
//
//  Created by 彭浩 on 16/8/5.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation

class MLLog {
    var timeOffset : NSTimeInterval!
    var jsonStr: String?
}

class MLLogParser : NSObject {
    private var downloader: MLDownloader!
    private var currentIndex: Int = 0
    var stream: MLFStream!
    var logs = [MLLog]()
    
    
    required init(urlString: String) {
        super.init()
        let path = "\(NSHomeDirectory())/test.txt"
        stream = MLFStream(filePath: path)
        stream.delegate = self
        downloader = MLDownloader(urlString: urlString).receiveDataHandle({ [weak self](data) in
            self?.stream.appendData(data)
        }).finishedHandle({ 
            print("download finished")
        }).failedHandle({ (error) in
            print("download error")
        })
    }
    
    func log(time: NSTimeInterval) -> MLLog? {
        return abs(logs[currentIndex].timeOffset - time) < 0.5 ? logs[currentIndex] : nil
    }
    
    func logsBeforTime(time: NSTimeInterval) -> [MLLog] {
        var rs = [MLLog]()
        for l in logs {
            if abs(l.timeOffset - time) < 0.5 {
                rs.append(l)
            } else {
                break
            }
        }
        return rs
    }
    
    deinit {
        
    }
}

extension MLLogParser : MLFStreamDelegate {
    func streamHasAppend() {
        parser()
    }
    
    private func parser() {
        guard let s = stream else {
            return
        }
        var rs = s.getLine()
        while rs != nil {
            if let l = parserOneLine(rs!) {
                logs.append(l)
            }
            rs = s.getLine()
        }
    }
    
    private func parserOneLine(line: String) -> MLLog? {
        print(line)
        return nil
    }
}