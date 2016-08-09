//
//  AirCoreText.swift
//  CoreText
//
//  Created by 彭浩 on 16/7/20.
//  Copyright © 2016年 彭浩. All rights reserved.
//

import Foundation
import UIKit
import CoreGraphics

class AirCoreText: UIView {
    
    override func drawRect(rect: CGRect) {
        let c = UIGraphicsGetCurrentContext()
        // 3
        
        let path = CGPathCreateMutable();
        
        CGPathAddEllipseInRect(path, nil, bounds);
        
        // 4
        
        let attString = NSAttributedString(string: "Hello World!")
        let framesetter =  CTFramesetterCreateWithAttributedString(attString)
        
        CTFrameRef frame = CTFramesetterCreateFrame(framesetter, CFRangeMake(0, [attString length]), path, NULL);
        
        // 5
        
        CTFrameDraw(frame, context);
        
        // 6
        
        CFRelease(frame);
        
        CFRelease(path);
        
        CFRelease(framesetter);
        
        文／FLC_FY（简书作者）
        原文链接：http://www.jianshu.com/p/7c604e08dd6b
        著作权归作者所有，转载请联系作者获得授权，并标注“简书作者”。
    }
    
}