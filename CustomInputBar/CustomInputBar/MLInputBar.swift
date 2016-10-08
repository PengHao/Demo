//
//  MLInputBar.swift
//  CustomInputBar
//
//  Created by PengHao on 16/8/15.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit

typealias MLInputBarBlock = (inputBar :MLInputBar?) -> Void
class MLInputBar: UIView {
    private var _left: UIView! = UIView(frame: CGRectZero)
    private var _leftBtns = [UIButton]()
    private var _right: UIView! = UIView(frame: CGRectZero)
    private var _rightBtns = [UIButton]()
    private var _blocks : [UIButton: MLInputBarBlock] = [UIButton: MLInputBarBlock]()
    var topView: UIView! = UIView(frame: CGRectZero)
    var bottomView: UIView! = UIView(frame: CGRectZero)
    var centerView: UITextView! = UITextView(frame: CGRectZero) {
        didSet {
            
        }
    }
    
    var defaultFrame: CGRect = CGRectMake(0, 0, 100, 100)
    var edgeInsets: UIEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8)
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        addSubview(_left)
        addSubview(_right)
        addSubview(topView)
        addSubview(bottomView)
        addSubview(centerView)
    }
    
    func addLeftButton(title: String? = nil, image: UIImage? = nil, customFrame: CGRect? = nil, onClick: MLInputBarBlock) -> UIButton {
        let btn = UIButton(type: .Custom)
        btn.frame = customFrame ?? defaultFrame
        btn.setTitle(title, forState: .Normal)
        btn.setImage(image, forState: .Normal)
        btn.addTarget(self, action: #selector(onButtonTouched(_:)), forControlEvents: .TouchUpInside)
        _blocks[btn] = onClick
        btn.backgroundColor = UIColor.greenColor()
        _leftBtns.append(btn)
        _left.addSubview(btn)
        return btn
    }
    
    func addRightButton(title: String? = nil, image: UIImage? = nil, customFrame: CGRect? = nil, onClick: MLInputBarBlock) -> UIButton {
        let btn = UIButton(type: .Custom)
        btn.frame = customFrame ?? defaultFrame
        btn.setTitle(title, forState: .Normal)
        btn.setImage(image, forState: .Normal)
        btn.addTarget(self, action: #selector(onButtonTouched(_:)), forControlEvents: .TouchUpInside)
        _blocks[btn] = onClick
        btn.backgroundColor = UIColor.greenColor()
        _rightBtns.append(btn)
        _right.addSubview(btn)
        return btn
    }
    
    func removeLeftButton(button: UIButton) {
        let index = _leftBtns.indexOf { (btn) -> Bool in
            return btn == button
        }
        if let idx = index {
            _leftBtns.removeAtIndex(idx)
        }
    }
    
    func removeRightButton(button: UIButton) {
        let index = _rightBtns.indexOf { (btn) -> Bool in
            return btn == button
        }
        if let idx = index {
            _rightBtns.removeAtIndex(idx)
        }
    }
    
    private func _update() {
        let height = frame.size.height - edgeInsets.top - edgeInsets.bottom
        var x = edgeInsets.left
        for btn in _leftBtns {
            btn.frame.origin.x = x
            btn.frame.origin.y = edgeInsets.top
            btn.frame.size.height = height
            x += btn.frame.size.width + edgeInsets.left
        }
        _left.frame = CGRectMake(0, 0, x, frame.size.height)
        
        _left.backgroundColor = UIColor.redColor()
        _right.backgroundColor = UIColor.redColor()
        centerView?.backgroundColor = UIColor.blueColor()
        
        
        var rx = edgeInsets.left
        for btn in _rightBtns {
            btn.frame.origin.x = rx
            btn.frame.origin.y = edgeInsets.top
            btn.frame.size.height = height
            rx += btn.frame.size.width + edgeInsets.left
        }
        rx = frame.size.width - rx
        _right.frame = CGRectMake(rx, 0, frame.size.width - rx, frame.size.height)
        centerView?.frame = CGRectMake(x, edgeInsets.top, rx - x, height)
    }
    
    override func drawRect(rect: CGRect) {
        _update()
        super.drawRect(rect)
    }
}


extension MLInputBar : UITextViewDelegate {
    func onButtonTouched(button: UIButton) {
        _blocks[button]?(inputBar: self)
    }
    
    func textViewDidEndEditing(textView: UITextView) {
        
    }
}