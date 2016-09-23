//
//  NSAttributedString+Editor.swift
//  AirCoreText
//
//  Created by 彭浩 on 16/8/1.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation
import UIKit
import YYText

protocol Content {
    var data: NSData? {get}
}

class EditorElement: Content {
    var content: AnyObject!
    required init(content: AnyObject) {
        self.content = content
    }
    
    var data: NSData? {
        guard let img = (content as? CustomTextAttachment)?.content as? UIImage else {
            return nil
        }
        return UIImageJPEGRepresentation(img, 0.5)
    }
}

class Uploader {
    /**
    class func upload(data: NSData, completion: ((operation: MLDataUploadOperation) -> Void)?) {
        let au : ImageAuthority = .IsPublic
        let mark : ImageWaterMark = .WithoutMark
        let bk :ImageBucket = .Media
        MLDataUploadManager.sharedInstance.uploadData(data, authority: au, warterMark: mark, bucket: bk, completion: completion)
    }
 **/
}

extension NSAttributedString {
    func encode() -> [EditorElement] {
        var rsStr = [Int: EditorElement]()
        let attr = self
        var result = [EditorElement]()
        if attr.length == 0 {
            return result
        }
        attr.enumerateAttributesInRange(NSRange(location: 0, length: attr.length), options: .Reverse, usingBlock: { (info, range, rs) in
            if let att = info["YYTextAttachment"] as? CustomTextAttachment {
                rsStr[range.location] = EditorElement(content: att)
            } else {
                let str = attr.attributedSubstringFromRange(range).string
                rsStr[range.location] = EditorElement(content: str)
            }
        })
        let rs = rsStr.sort { (e0, e1) -> Bool in
            return e0.0 < e1.0
        }
        
        for e in rs {
            result.append(e.1)
        }
        return result
    }
    
    
    func encodeToString() -> String {
        let elements = encode()
        var rs: String = ""
        for e in elements {
            if let str = e.content as? String {
                rs += str
            } else  if let att = e.content as? CustomTextAttachment {
                rs += att.encode() ?? ""
            }
        }
        return rs
    }
    
    class func Decode(str: String) -> NSAttributedString? {
        let rs = NSMutableAttributedString()
        let pattern = "\\.*<(med-img=.*?)>"
        let regex = try? NSRegularExpression(pattern: pattern, options: .CaseInsensitive)
        var location: Int = 0
        if let matches = regex?.matchesInString(str,
                                                options: .ReportCompletion,
                                                range: NSMakeRange(0, str.characters.count)) {
            for matche in matches {
                if matche.range.location > location {
                    let range = NSRange(location: location, length: matche.range.location - location)
                    let substr = (str as NSString).substringWithRange(range)
                    
                    
                    location = matche.range.location
                    
                    let attachmentStr = NSAttributedString(string: substr)
                    rs.appendAttributedString(attachmentStr)
                }
                
                let range = NSRange(location: location, length: matche.range.length)
                let substr = (str as NSString).substringWithRange(range)
                location += matche.range.length
                
                if let attachment = CustomTextAttachment.Decode(substr) {
                    let attachmentStr = NSMutableAttributedString.MLAttachmentString(attachment.imagePath!, contentMode: attachment.contentMode, attachmentSize: CGSizeZero, font: UIFont.systemFontOfSize(15), alignment: YYTextVerticalAlignment.Center, loadFinished: { (success) in
                        
                    })
                    rs.appendAttributedString(attachmentStr)
                }
                
            }
        }
        if location < str.characters.count {
            let range = NSRange(location: location, length: str.characters.count - location)
            let substr = (str as NSString).substringWithRange(range)
            let attachmentStr = NSAttributedString(string: substr)
            rs.appendAttributedString(attachmentStr)
            
        }
        return rs
    }
}

extension Array where Element : Content {
    func upload(onUploaded:(((AnyObject, Element))->Void)?, finish:(()->Void)?) {
        /**
        let source = dispatch_source_create(DISPATCH_SOURCE_TYPE_DATA_ADD, 0, 0, dispatch_get_main_queue())
        
        var finishCount: Int = 0
        dispatch_source_set_event_handler(source) {
            let data = dispatch_source_get_data(source)
            finishCount += Int(data)
            if finishCount == self.count {
                //finished!
                finish?()
                dispatch_source_cancel(source)
            }
        }
        dispatch_apply(count, dispatch_get_global_queue(0, 0)) { (index) in
            let e = self[index]
            guard let data = e.data else {
                dispatch_source_merge_data(source, 1)
                return
            }
            Uploader.upload(data, completion: { (operation) in
                onUploaded?(operation, e)
                dispatch_source_merge_data(source, 1)
            })
        }
        dispatch_resume(source)
         **/
    }
}
