//
//  NSAttributedString+Editor.swift
//  AirCoreText
//
//  Created by 彭浩 on 16/8/1.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation
import UIKit

protocol Content {
    var data: NSData? {get}
}

class EditorElement: Content {
    var content: AnyObject!
    required init(content: AnyObject) {
        self.content = content
    }
    
    var data: NSData? {
        guard let img = (content as? CustomTextAttachment)?.image else {
            return nil
        }
        return UIImageJPEGRepresentation(img, 0.5)
    }
}

class Uploader {
    class func upload(content: Content, complete:((success: Bool, content: Content) -> Void)?) {
        
    }
}

extension NSAttributedString {
    func encode() -> [EditorElement] {
        var rsStr = [Int: EditorElement]()
        let attr = self
        attr.enumerateAttributesInRange(NSRange(location: 0, length: attr.length), options: .Reverse, usingBlock: { (info, range, rs) in
            if let att = info["NSAttachment"] as? CustomTextAttachment {
                rsStr[range.location] = EditorElement(content: att)
            } else {
                let str = attr.attributedSubstringFromRange(range).string
                rsStr[range.location] = EditorElement(content: str)
            }
        })
        let rs = rsStr.sort { (e0, e1) -> Bool in
            return e0.0 < e1.0
        }
        print("rsStr = \(rs)")
        var result = [EditorElement]()
        for e in rs {
            result.append(e.1)
        }
        return result
    }
}

extension Array where Element : Content {
    func upload(complite:((success: Bool, failedElement: [Element])->Void)?) {
        let source = dispatch_source_create(DISPATCH_SOURCE_TYPE_DATA_ADD, 0, 0, dispatch_get_main_queue())
        var failedUpload = [Element]()
        dispatch_apply(count, dispatch_get_global_queue(0, 0)) { (index) in
            let e = self[index]
            Uploader.upload(e, complete: { (success, content) in
                if !success {
                    failedUpload.append(e)
                }
                dispatch_source_merge_data(source, 1)
            })
        }
        var finishCount: Int = 0
        dispatch_source_set_event_handler(source) {
            let data = dispatch_source_get_data(source)
            finishCount += Int(data)
            if finishCount == self.count {
                //finished!
                complite?(success: failedUpload.count == 0, failedElement: failedUpload)
                dispatch_source_cancel(source)
            }
        }
        
    }
}