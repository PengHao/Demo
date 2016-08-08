//
//  MLLogParser.swift
//  LogPlayer
//
//  Created by 彭浩 on 16/8/5.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation



class MLLog {
    static func hash(time: NSTimeInterval) -> Int32 {
        return Int32(time)
    }
    
    static var startTime: NSTimeInterval = 0
    
    var timeOffset : NSTimeInterval!
    var jsonStr: String?
    
    required init(time: NSTimeInterval, json: String) {
        if MLLog.startTime == 0 {
            MLLog.startTime = time
        }
        
        timeOffset = time - MLLog.startTime
        jsonStr = json
    }
    
    lazy var hash : Int32 = {
        return MLLog.hash(self.timeOffset)
    }()
}

class MLLogParser : NSObject {
    private var downloader: MLDownloader!
    private var currentIndex: Int = 0
    var stream: MLFStream!
    var logs = [MLLog]()
    
    
    required init(urlString: String) {
        super.init()
        let path = "\(NSHomeDirectory())/test.txt"
        print("path = \(path)")
        stream = MLFStream(filePath: path)
        downloader = MLDownloader(urlString: urlString).receiveDataHandle({ [weak self](data) in
            self?.stream.appendData(data)
        }).finishedHandle({
            print("download finished")
        }).failedHandle({ (error) in
            print("download error")
        })
    }
    
    func log(time: NSTimeInterval) -> [MLLog] {
        var rs = [MLLog]()
        for i in 0..<logs.count {
            let l : MLLog? = logs.count > currentIndex && abs(logs[i].timeOffset - time) < 0.5 ? logs[i] : nil
            if let log = l {
                rs.append(log)
            }
        }
        return rs
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

extension MLLogParser {
    
    func test(time: NSTimeInterval) {
        let t = NSString(format: "%017f", time) as String
        let j : NSString = t + "json" + t + "\n"
        if let d = j.dataUsingEncoding(NSUTF8StringEncoding) {
            stream.appendData(d)
        }
    }
    
    func rebuildIndex() {
        guard let s = stream else {
            return
        }
        
        s.jump(0)
        var rs = s.getLine()
        while rs != nil {
            if let l = parserOneLine(rs!) {
                s.addIndex(l.hash)
            }
            rs = s.getLine()
        }
    }
    
    func loadLog(time: NSTimeInterval) {
        guard let s = stream else {
            return
        }
        logs.removeAll()
        let h = MLLog.hash(time)
        s.jump(max(0, MLLog.hash(time)-1))
        var rs = s.getLine()
        while rs != nil {
            if let l = parserOneLine(rs!) {
                logs.append(l)
                if l.hash > h+2 {
                    break
                }
            }
            rs = s.getLine()
        }
    }
    
    private func parserOneLine(line: NSString) -> MLLog? {
        print(line)
        if line.length < 17 {
            return nil
        }
        let time = (line.substringToIndex(17) as NSString).doubleValue
        let json = line.substringFromIndex(17)
        
        return MLLog(time: time, json: json)
    }
}