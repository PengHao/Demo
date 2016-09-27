//
//  main.swift
//  SwiftReflection
//
//  Created by 彭浩 on 16/7/11.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation
let dic = ["123":"123"]
let s = dic["123"] ?? "321"


struct Api : Reflectable {
    static func t() {
        print("Hello, Api! \(s)")
        
    }
    struct User {
        static func t() {
            print("Hello, User! \(Api.User().dynamicType)")
        }
        struct Unfollow {
            static func t() {
                print("Hello, Unfollow! \(s)")
            }
            
        }
    }
}

Api.t()
Api.User.t()
Api.User.Unfollow.t()

print("Hello, World! \(s)")

ClassCreator("A")?.log()
ClassCreator("B")?.log()
ClassCreator("C")?.log()

ProtoCreator("ProtoA")?.log()
ProtoCreator("ProtoB")?.log()
ProtoCreator("ProtoC")?.log()

