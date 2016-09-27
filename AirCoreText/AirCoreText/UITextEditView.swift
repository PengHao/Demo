//
//  UITextEditView.swift
//  AirCoreText
//
//  Created by 彭浩 on 16/8/1.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation
import UIKit
import AssetsLibrary
import YYText


extension UIImage {
    class func asyncLoadImage(path: String, loadImageComplite:(UIImage?)->Void) {
        if let img = UIImage(named: path) ?? UIImage(contentsOfFile: path) {
            loadImageComplite(img)
        } else if let url = NSURL(string: path){
            //ALAssetsLibraryAssetForURLResultBlock
            ALAssetsLibrary().assetForURL(url, resultBlock: {(asset: ALAsset!) in
                loadImageComplite(asset.smallOrientationImage())
                }, failureBlock: { (error) in
                    print("get asset failed \(path)")
                    loadImageComplite(nil)
            })
        } else {
            loadImageComplite(nil)
        }
    }
}

class CustomTextAttachment : YYTextAttachment, Content {
    
    class func Decode(str: String) -> CustomTextAttachment? {
        let begin = "<med-img="
        let end = ">"
        let path = (str as NSString).substringWithRange(NSRange(location: begin.characters.count, length: str.characters.count - begin.characters.count - end.characters.count))
        let rs = CustomTextAttachment(path: path) { (success) in
            
        }
        return rs
    }
    
    func encode() -> String? {
        guard let path = imagePath else {
            return nil
        }
        let begin = "<med-img="
        let end = ">"
        return begin + path + end
    }
    var uploadedPath: String?
    var isUploaded: Bool = false
    private var _imgPath: String?
    var imagePath: String? {
        get {
            return _imgPath
        }
    }
    var font: UIFont = UIFont.systemFontOfSize(15)
    var alignment: YYTextVerticalAlignment = .Center
    var delegate : YYTextRunDelegate?
    var attachmentSize: CGSize = CGSizeZero
    let yyDelegate = YYTextRunDelegate()
    func setImagePath(path: String, complite: (Bool, CustomTextAttachment?)->Void) {
        if imagePath != path {
            _imgPath = path
            UIImage.asyncLoadImage(path) { [weak self] (img) in
                guard let ws = self else {
                    complite(img != nil, nil)
                    return
                }
                ws.content = img
                
                let attachmentSize = img?.size ?? ws.attachmentSize
                let font = ws.font
                let yyDelegate = ws.yyDelegate
                yyDelegate.width = attachmentSize.width
                switch ws.alignment {
                case .Top:
                    yyDelegate.ascent = font.ascender
                    yyDelegate.descent = attachmentSize.height - font.ascender
                    if yyDelegate.descent < 0 {
                        yyDelegate.descent = 0
                        yyDelegate.ascent = attachmentSize.height
                    }
                    break
                case .Center:
                    let fontHeight = font.ascender - font.descender;
                    let yOffset = font.ascender - fontHeight * 0.5;
                    yyDelegate.ascent = attachmentSize.height * 0.5 + yOffset
                    yyDelegate.descent = attachmentSize.height - yyDelegate.ascent
                    if yyDelegate.descent < 0 {
                        yyDelegate.descent = 0
                        yyDelegate.ascent = attachmentSize.height
                    }
                    break
                case .Bottom:
                    yyDelegate.ascent = attachmentSize.height + font.descender
                    yyDelegate.descent = -font.descender
                    if yyDelegate.ascent < 0 {
                        yyDelegate.ascent = 0
                        yyDelegate.descent = attachmentSize.height
                    }
                    break
                }
                complite(img != nil, ws)
            }
        }
    }
    
    init(path: String, contentMode: UIViewContentMode = .ScaleAspectFit, attachmentSize: CGSize = CGSizeZero, font: UIFont = UIFont.systemFontOfSize(15), alignment: YYTextVerticalAlignment = .Center, loadFinished: (Bool, CustomTextAttachment?)->Void) {
        super.init()
        self.contentMode = contentMode
        self.attachmentSize = attachmentSize
        self.alignment = alignment
        self.font = font
        setImagePath(path) { (success, att) in
            loadFinished(success, att)
        }
        
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    
    var data: NSData? {
        guard let img = content as? UIImage else {
            return nil
        }
        return UIImageJPEGRepresentation(img, 0.5)
    }
}


extension UITextView {
    func getSelectedRect()->CGRect {
        if let r = selectedTextRange {
            return caretRectForPosition(r.start)
        }
        return CGRectZero
    }
}

extension NSMutableAttributedString {
    static func MLAttachmentString(path: String, contentMode: UIViewContentMode, attachmentSize: CGSize, font: UIFont, alignment: YYTextVerticalAlignment, loadFinished: (Bool)->Void) -> NSMutableAttributedString  {
        let atr = NSMutableAttributedString(string: YYTextAttachmentToken)
        let attach = CustomTextAttachment(path: path, contentMode: contentMode, attachmentSize: attachmentSize, font: font, alignment: alignment) { (success, att) in
            let delegateRef = att?.yyDelegate.CTRunDelegate()
            atr.yy_setRunDelegate(delegateRef, range: NSMakeRange(0, atr.length))
            loadFinished(success)
        }

        atr.yy_setTextAttachment(attach, range: NSMakeRange(0, atr.length))
        return atr
    }
    
    static func MLAttachmentString(attach: CustomTextAttachment) -> NSMutableAttributedString  {
        let atr = NSMutableAttributedString(string: YYTextAttachmentToken)
        atr.yy_setTextAttachment(attach, range: NSMakeRange(0, atr.length))
        let delegateRef = attach.yyDelegate.CTRunDelegate()
        atr.yy_setRunDelegate(delegateRef, range: NSMakeRange(0, atr.length))
        return atr
    }
}

class MLTextEditView: YYTextView {
    
    var customTypingAttributes: [String : AnyObject]?
/**
    override func copy(sender: AnyObject?) {
        UIPasteboard.generalPasteboard().string = attributedText?.attributedSubstringFromRange(selectedRange).encodeToString()
    }
    
    override func paste(sender: AnyObject?) {
        if let attr = NSAttributedString.Decode(UIPasteboard.generalPasteboard().string ?? "") {
            
            let newAttr = attributedText == nil ? NSMutableAttributedString() : NSMutableAttributedString(attributedString: attributedText!)
            newAttr.replaceCharactersInRange(selectedRange, withAttributedString: attr)
            attributedText = newAttr
        }
    }
    
    override func cut(sender: AnyObject?) {
        guard let attr = attributedText else {
            return
        }
        let newAttr = NSMutableAttributedString(attributedString: attr)
        UIPasteboard.generalPasteboard().string = attr.attributedSubstringFromRange(selectedRange).encodeToString()
        newAttr.deleteCharactersInRange(selectedRange)
        attributedText = newAttr
    }
   **/
    override func becomeFirstResponder() -> Bool {
        let rs = super.becomeFirstResponder()
        if let attrs = customTypingAttributes {
            typingAttributes = attrs
        }
        return rs
    }
    
    
    
    var placeholder : String? {
        set {
            placeholderText = newValue
        }
        get {
            return placeholderText
        }
    }
    
    private func _update () {
        if let atts = customTypingAttributes {
            let rs = NSMutableAttributedString(attributedString: _customAttributeText)
            rs.addAttributes(atts, range: NSRange(location: 0, length: _customAttributeText.length))
            super.attributedText = rs
        } else {
            super.attributedText = _customAttributeText
        }
        delegate?.textViewDidChange?(self)
    }
    
    private var _customAttributeText: NSAttributedString! {
        didSet {
            guard _customAttributeText != nil else {
                return
            }
            let tempAttributeText = _customAttributeText
            let attrs = tempAttributeText.encode()
            var totalCount = attrs.count
            guard totalCount > 0 else {
                _update()
                return
            }
            let rs = dispatch_source_create(DISPATCH_SOURCE_TYPE_DATA_ADD, 0, 0, dispatch_get_main_queue())
            dispatch_source_set_event_handler(rs) { [weak self] in
                guard let ws = self else {
                    return
                }
                let count = dispatch_source_get_data(rs)
                totalCount = totalCount - Int(count)
                if totalCount == 0 {
                    ws._update()
                    dispatch_source_cancel(rs)
                }
            }
            dispatch_resume(rs)
            
            dispatch_apply(totalCount, dispatch_get_global_queue(0, 0)) { (i) in
                if let attr = attrs[i].content as? CustomTextAttachment, let path = attr.imagePath where attr.content == nil {
                    UIImage.asyncLoadImage(path, loadImageComplite: { (img) in
                        attr.content = img
                        dispatch_source_merge_data(rs, 1)
                    })
                } else {
                    dispatch_source_merge_data(rs, 1)
                }
            }
        }
    }
    
    func addImage(path: String, isEditing: Bool = true, complite: ((Bool)->Void)? = nil) {

        let newAttr = attributedText == nil ? NSMutableAttributedString() : NSMutableAttributedString(attributedString: attributedText!)
        let attachmentStr = NSMutableAttributedString.MLAttachmentString(path, contentMode: .ScaleAspectFit, attachmentSize: CGSizeMake(bounds.size.width, 100), font: UIFont.systemFontOfSize(15), alignment: .Center) { [weak self] (success) in
            guard let ws = self else {
                return
            }
            ws.setValue(newAttr, forKey: "attributedText")
        }
        
        if isEditing {
            newAttr.insertAttributedString(attachmentStr, atIndex: selectedRange.location)
        } else {
            newAttr.appendAttributedString(attachmentStr)
        }
        setValue(newAttr, forKey: "attributedText")
        setMarkedText(attachmentStr.string, selectedRange: NSMakeRange(attachmentStr.length + 1, 0))
    }
}
