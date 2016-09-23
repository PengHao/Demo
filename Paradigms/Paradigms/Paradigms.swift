//
//  Paradigms.swift
//  Paradigms
//
//  Created by 彭浩 on 16/7/17.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation

class Paradigms<T> {
    let s = "123"
    var t: T?
    init() {
//        t?.test()
    }
    func a()  {
        
    }
}

protocol Proto {
    associatedtype Type
    var data: Type{set get}
    func test() -> Type
}

protocol Proto1 : Proto {
    func proto1()
}


protocol Proto2 : Proto {
    func proto2()
}

extension Proto where Type == Paradigms<A> {
    func testParadigmsA() { print("\(data.s)") }
}

extension Proto where Type == Int {
    func testInt() { print("\(data)") }
}

extension Paradigms where T : Proto1 {
    func testProto() {
        print("testProto1")
    }
}

extension Paradigms where T : Proto2 {
    func testProto() {
        print("testProto2")
    }
}

class A : Proto1 {
    var data: Paradigms<A> = Paradigms<A>()
    func test() -> Paradigms<A> {
        print("test \(self)")
        return data
    }
    func proto1() {
        print("proto1")
    }
}

class B : Proto2 {
    var data: Int = 0
    func test() -> Int {
        print("test \(self)")
        return data
    }
    func proto2() {
        print("proto2")
    }
}


func genericFunc<T>(ts : [T]) -> Bool {
    return true; // not very exciting
}

protocol EquatableSelf {
    func equals(other : Self) -> Bool
}
//https://developer.apple.com/videos/play/wwdc2015/408/  面向协议编程

// By adopting `EquatableSelf`, every occurrence of `Self` in the protocol gets
// semantically replaced with `ImplicitStruct`
struct ImplicitStruct : EquatableSelf {
    var val : Int64
    func equals(other: ImplicitStruct) -> Bool {
        return self.val == other.val;
    }
}

func test() {
    //偏特
    let pA = Paradigms<A>()
    pA.t = A()
    pA.testProto()
    pA.t?.test()
    pA.t?.testParadigmsA()
    pA.t?.proto1()
    
    let pB = Paradigms<B>()
    pB.t = B()
    pB.testProto()
    pB.t?.test()
    pB.t?.testInt()
    pB.t?.proto2()
}