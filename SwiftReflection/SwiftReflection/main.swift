//
//  main.swift
//  SwiftReflection
//
//  Created by 彭浩 on 16/7/11.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation

print("Hello, World!")

ClassCreator("A")?.log()
ClassCreator("B")?.log()
ClassCreator("C")?.log()

ProtoCreator("ProtoA")?.log()
ProtoCreator("ProtoB")?.log()
ProtoCreator("ProtoC")?.log()