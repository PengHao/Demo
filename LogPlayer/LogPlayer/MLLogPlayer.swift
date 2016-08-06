//
//  MLLogPlayer.swift
//  LogPlayer
//
//  Created by 彭浩 on 16/8/5.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation
class MLLogPlayer : NSObject {
    private static let timerQueue = dispatch_queue_create("ml_timer_queue", nil)
    var parser: MLLogParser? = nil
    private var timer: NSTimer? = nil
    var currentTime: NSTimeInterval = 0
    required init(urlString: String) {
        super.init()
        parser = MLLogParser(urlString: urlString)
    }
    
    deinit {
        stopTimer()
    }
    
    @objc private func fire(t: NSTimer) {
        currentTime += 0.5
        print("current time: \(currentTime)")
        guard let l = parser?.log(currentTime) else {
            return
        }
        sendLog(l)
    }
    
    private func startTimer() {
        dispatch_async(MLLogPlayer.timerQueue) { [weak self] in
            guard let ws = self else {
                return
            }
            ws.timer = NSTimer(timeInterval: 0.5, target: ws, selector: #selector(ws.fire(_:)), userInfo: nil, repeats: true)
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
    
    func start() {
        currentTime = 0
        startTimer()
    }
    
    func resume() {
        startTimer()
    }
    
    func jump(time: NSTimeInterval) {
        dispatch_async(MLLogPlayer.timerQueue) { [weak self] in
            self?.currentTime = time
            guard let logs = self?.parser?.logsBeforTime(time) else {
                return
            }
            for l in logs {
                self?.sendLog(l)
            }
        }
    }
    
    func stop() {
        stopTimer()
        currentTime = 0
    }
    
    func pause() {
        stopTimer()
    }
    
    private func sendLog(log: MLLog) {
        //todo send:
    }
}