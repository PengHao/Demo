//
//  ClassReflection.swift
//  SwiftReflection
//
//  Created by 彭浩 on 16/7/11.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation

func ClassCreator(className: String) -> A? {
    let className = A.ModuleName + "." + className
    guard let cls = NSClassFromString(className) else {
        return nil
    }
    guard let c = cls as? A.Type else {
        return nil
    }
    return c.init()
}

class A {
    static let ModuleName = "SwiftReflection"
    
    required init() {
    }
    
    func log() {
        print(self)
    }
}


class B : A {
    override func log() {
        print(self)
    }
}

class C : A {
    override func log() {
        print(self)
    }
}



