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
    
    private lazy var placeHolderLabel : UILabel = {
        let l = UILabel(frame: CGRectZero)
        l.font = self.font
        l.textColor = UIColor.grayColor()
        self.addSubview(l)
        NSNotificationCenter.defaultCenter().addObserverForName(UITextViewTextDidBeginEditingNotification, object: nil, queue: NSOperationQueue.mainQueue(), usingBlock: { [weak self] (notification) in
            l.hidden = true
            })
        NSNotificationCenter.defaultCenter().addObserverForName(UITextViewTextDidEndEditingNotification, object: nil, queue: NSOperationQueue.mainQueue(), usingBlock: { [weak self] (notification) in
            self?._updatePlaceHolder ()
            })
        return l
    }()
    
    override var contentSize: CGSize {
        didSet {
            
        }
    }
    
    private func _updatePlaceHolder () {
        if placeHolder != nil && attributedText.length == 0 && text.characters.count == 0 {
            placeHolderLabel.text = placeHolder
            placeHolderLabel.hidden = false
            placeHolderLabel.frame = CGRectMake(8, contentInset.top, bounds.size.width, placeHolderLabel.font.pointSize)
        } else {
            placeHolderLabel.hidden = true
        }
    }
    
    var placeHolder : String? {
        didSet {
            _updatePlaceHolder()
        }
    }
    
    override var attributedText: NSAttributedString! {
        didSet {
            _updatePlaceHolder()
        }
    }
    
    override var text: String! {
        didSet {
            _updatePlaceHolder()
        }
    }
    
    func addImage(path: String) {
        let attachment = CustomTextAttachment()
        attachment.imagePath = path
        let attachmentStr = NSAttributedString(attachment: attachment)
        textStorage.appendAttributedString(attachmentStr)
    }
}