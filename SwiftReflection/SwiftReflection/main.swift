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



class CustomTextAttachment : NSTextAttachment {
    var imagePath: String? {
        didSet {
            guard let path = imagePath else {
                return
            }
            image = UIImage(named: path)
        }
    }
    
    func scaleImageSizeToWidth(width: CGFloat) -> CGRect {
        guard let img = image else {
            return CGRectZero
        }
        let factor = CGFloat(width / img.size.width)
        return CGRectMake(0, 0, img.size.width * factor, img.size.height * factor)
    }
    
    override func attachmentBoundsForTextContainer(textContainer: NSTextContainer?, proposedLineFragment lineFrag: CGRect, glyphPosition position: CGPoint, characterIndex charIndex: Int) -> CGRect {
        let attachmentWidth = CGRectGetWidth(lineFrag) - (textContainer?.lineFragmentPadding ?? 0)
        return scaleImageSizeToWidth(attachmentWidth)
    }
}

func Decode(str: String) -> NSAttributedString? {
    let rs = NSMutableAttributedString()
    let pattern = "\\.*<(image .*?)>"
    let regex = try? NSRegularExpression(pattern: pattern, options: .CaseInsensitive)
    var location: Int = 0
    if let matches = regex?.matchesInString(str,
                                            options: .ReportCompletion,
                                            range: NSMakeRange(0, str.characters.count)) {
        for matche in matches {
            if matche.range.location > location {
                let range = NSRange(location: location, length: matche.range.location)
                let substr = (str as NSString).substringWithRange(range)
                
                let attachmentStr = NSAttributedString(string: substr)
                rs.appendAttributedString(attachmentStr)
                
                print(substr)
                location = matche.range.location
            }
            
            let range = NSRange(location: location, length: matche.range.length)
            let substr = (str as NSString).substringWithRange(range)
            
            let attachment = CustomTextAttachment()
            attachment.imagePath = substr
            let attachmentStr = NSAttributedString(attachment: attachment)
            rs.appendAttributedString(attachmentStr)
            
            print(substr)
            location += matche.range.length
        }
    }
    if location < str.characters.count {
        let range = NSRange(location: location, length: str.characters.count - location)
        let substr = (str as NSString).substringWithRange(range)
        let attachmentStr = NSAttributedString(string: substr)
        rs.appendAttributedString(attachmentStr)
        print(substr)
        
    }
    return rs
}


Decode("//xxx<image id = \\d{1,}><image 123>ccccas")
