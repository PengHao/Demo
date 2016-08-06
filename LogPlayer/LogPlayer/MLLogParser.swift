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
    var stream: MLFStream? = nil
    var logs = [MLLog]()
    
    required init(urlString: String) {
        super.init()
        let path = "\(NSHomeDirectory())/test.txt"
        stream = MLFStream(filePath: path)
        download(urlString)
    }
    
    
    private func download(url: String) {
        if let _ = NSURL(string: url) {
            
        }
    }
    
}

extension MLLogParser {
    
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