//
//  MLInputAccessoryView.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/9.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit

@objc protocol MLInputAccessoryViewDelegate : NSObjectProtocol {
    optional func onLeftBtnTouched()
    optional func onRightBtnTouched()
}

class MLInputAccessoryView: UIView {
    
    private let _rightBtn: UIButton = UIButton(type: .Custom)
    private let _leftBtn: UIButton = UIButton(type: .Custom)
    private let _topLine: UIView = UIView(frame: CGRectZero)
    private let _bottomLine: UIView = UIView(frame: CGRectZero)
    
    
    weak var delegate: MLInputAccessoryViewDelegate?
    
    private override init(frame: CGRect) {
        super.init(frame: frame)
    }
    
    init(height: CGFloat, _delegate: MLInputAccessoryViewDelegate?) {
        super.init(frame: CGRectMake(0, 0, 0, height))
        delegate = _delegate
        addSubview(_rightBtn)
        addSubview(_leftBtn)
        addSubview(_topLine)
        addSubview(_bottomLine)
        backgroundColor = UIColor.whiteColor()
        _topLine.backgroundColor = UIColor.grayColor()
        _bottomLine.backgroundColor = UIColor.grayColor()
        
        
        let at = [NSUnderlineStyleAttributeName: NSUnderlineStyle.StyleThick.rawValue,
                  NSFontAttributeName: UIFont.MLFont(12),
                  NSForegroundColorAttributeName: UIColor.blueColor()]
        let attr = NSMutableAttributedString(string: "添加投票选项", attributes: at)
        _leftBtn.setAttributedTitle(attr, forState: .Normal)
        _leftBtn.addTarget(delegate, action: #selector(MLInputAccessoryViewDelegate.onLeftBtnTouched), forControlEvents: .TouchUpInside)
        
        _rightBtn.setImage(UIImage(named: "caseEditorBtn0"), forState: .Normal)
        _rightBtn.addTarget(delegate, action: #selector(MLInputAccessoryViewDelegate.onRightBtnTouched), forControlEvents: .TouchUpInside)
        _rightBtn.backgroundColor = UIColor.grayColor()
        
        
    }
    
    func setLeftBtnVisible(visible: Bool) {
        _leftBtn.hidden = !visible
    }
    
    func setRightBtnVisible(visible: Bool) {
        _rightBtn.hidden = !visible
    }
    
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func drawRect(rect: CGRect) {
        super.drawRect(rect)
        let offSetX: CGFloat = 8
        let offSetY: CGFloat = 12
        _rightBtn.frame = CGRectMake(frame.size.width - 40 - offSetX, offSetY, 40, frame.size.height - 2*offSetY)
        _rightBtn.backgroundColor = UIColor.clearColor()
        
        _leftBtn.frame = CGRectMake(offSetX, offSetY, 100, frame.size.height - 2*offSetY)
        _leftBtn.backgroundColor = UIColor.clearColor()
        
        _topLine.frame = CGRectMake(0, 0, frame.size.width, 1/UIScreen.mainScreen().scale)
        _bottomLine.frame = CGRectMake(0, frame.size.height - 1/UIScreen.mainScreen().scale, frame.size.width, 1/UIScreen.mainScreen().scale)
    }
}
