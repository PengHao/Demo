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
    override init() {
        super.init()
    }
    
    @objc private func fire(t: NSTimer) {
        currentTime += 0.5
    }
    
    func start() {
        dispatch_async(MLLogPlayer.timerQueue) { [weak self] in
            guard let ws = self else {
                return
            }
            ws.timer = NSTimer(timeInterval: 0.5, target: ws, selector: #selector(ws.fire(_:)), userInfo: nil, repeats: true)
            NSRunLoop.currentRunLoop().run()
        }
    }
    
    func jump(time: NSTimeInterval) {
        currentTime = time
    }
    
    func stop() {
        currentTime = 0
        dispatch_async(MLLogPlayer.timerQueue) { [weak self] in
            guard let ws = self else {
                return
            }
            ws.timer?.invalidate()
        }
    }
    
    func pause() {
        dispatch_async(MLLogPlayer.timerQueue) { [weak self] in
            guard let ws = self else {
                return
            }
            ws.timer?.invalidate()
        }
    }
}