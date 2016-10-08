//
//  main.swift
//  Paradigms
//
//  Created by 彭浩 on 16/7/17.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation

print("Hello, World!")

let time = 3678.0
let sec = Int(time)
let h = sec / 3600
let m = sec % 3600 / 60
let s = sec % 60

print(NSString(format: "%02d:%02d:%02d, total: %.2f", h, m, s, time))

//test()
