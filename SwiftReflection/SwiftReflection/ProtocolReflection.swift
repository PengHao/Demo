//
//  ProtocolReflection.swift
//  SwiftReflection
//
//  Created by 彭浩 on 16/7/11.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation
protocol Proto {
    func log()
    init()
}


func ProtoCreator(className: String) -> Proto? {
    let className = ProtoA.ModuleName + "." + className
    guard let cls = NSClassFromString(className) else {
        return nil
    }
    guard let c = cls as? Proto.Type else {
        return nil
    }
    return c.init()
}


class ProtoA: Proto {
    static let ModuleName = "SwiftReflection"
    
    required init() {
    }
    
    func log() {
        print(self)
    }
}


class ProtoB : Proto {
    required init() {
    }
    func log() {
        print(self)
    }
}

class ProtoC : Proto {
    required init() {
    }
    
    func log() {
        print(self)
    }
}



