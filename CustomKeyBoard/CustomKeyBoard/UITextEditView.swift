//
//  UITextEditView.swift
//  AirCoreText
//
//  Created by 彭浩 on 16/8/1.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation
import UIKit

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


class MLTextEditView: UITextView {
    override var contentSize: CGSize {
        didSet {
//            frame.size.height = contentSize.height
            NSNotificationCenter.defaultCenter().postNotificationName("UITextEditViewChanged", object: nil)
        }
    }
    
    func addImage(path: String) {
        let attachment = CustomTextAttachment()
        attachment.imagePath = path
        let attachmentStr = NSAttributedString(attachment: attachment)
        textStorage.appendAttributedString(attachmentStr)
    }
}