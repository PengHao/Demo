//
//  MLLogPlayer.swift
//  LogPlayer
//
//  Created by 彭浩 on 16/8/5.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation
protocol MLLogPlayerDelegate : NSObjectProtocol{
    func handleLogs(logs: [MLLog])
}
class MLLogPlayer : NSObject {
    private static let timerQueue = dispatch_queue_create("ml_timer_queue", nil)
    private var parser: MLLogParser? = nil
    private var timer: NSTimer? = nil
    private var currentIndex : Int32 = -1
    private var currentTime : NSTimeInterval = 0 {
        didSet {
            let index = MLLog.hash(currentTime)
            if currentIndex != index {
                parser?.loadLog(currentTime)
                currentIndex = index
            }
        }
    }
    
    required init(urlString: String) {
        super.init()
        parser = MLLogParser(urlString: urlString)
    }
    
    weak var delegate: MLLogPlayerDelegate?
    
    @objc private func fire(t: NSTimer) {
        currentTime += 0.5
        print("current time: \(currentTime)")
        let index = MLLog.hash(currentTime)
        if currentIndex < index {
            jump(currentTime)
            currentIndex = index
        }
        guard let logs = parser?.log(currentTime) else {
            return
        }
        sendLogs(logs)
    }
    
    private func sendLogs(logs: [MLLog]) {
        //todo send:
        delegate?.handleLogs(logs)
        for log in logs {
            print("sendLog: \(log.timeOffset), \(log.jsonStr)")
        }
    }
    
    private func startTimer(time: NSTimeInterval = 0) {
        dispatch_async(MLLogPlayer.timerQueue) { [weak self] in
            guard let ws = self else {
                return
            }
            ws.currentTime = time
            ws.timer = NSTimer(timeInterval: 0.5, target: ws, selector: #selector(ws.fire(_:)), userInfo: nil, repeats: true)
            guard let t = ws.timer else {
                return
            }
            NSRunLoop.currentRunLoop().addTimer(t, forMode: NSDefaultRunLoopMode)
            NSRunLoop.currentRunLoop().run()
        }
    }
    
    private func stopTimer() {
        dispatch_async(MLLogPlayer.timerQueue) { [weak self] in
            guard let ws = self else {
                return
            }
            ws.timer?.invalidate()
        }
    }
    
    func test() {
        for _ in 0...100 {
            parser?.test(NSDate().timeIntervalSince1970)
            usleep(100000)
        }
        parser?.stream?.flush()
        parser?.rebuildIndex()
    }
    
    func start() {
        startTimer()
    }
    
    func resume() {
        startTimer(currentTime)
    }
    
    func jump(time: NSTimeInterval) {
        dispatch_async(MLLogPlayer.timerQueue) { [weak self] in
            self?.currentTime = time
            guard let logs = self?.parser?.logsBeforTime(time) else {
                return
            }
            self?.sendLogs(logs)
        }
    }
    
    func stop() {
        stopTimer()
        currentTime = 0
    }
    
    func pause() {
        stopTimer()
    }
    
}