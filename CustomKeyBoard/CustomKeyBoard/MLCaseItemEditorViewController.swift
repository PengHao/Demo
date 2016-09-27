//
//  MLCaseItemEditorViewController.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/9.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit
import Foundation

class MLCaseItemEditorViewController: UIViewController {
    var editorItem: MLCaseEditorItem!
    @IBOutlet weak var textEditorBottomOffset: NSLayoutConstraint!
    @IBOutlet weak var textEditorView: MLTextEditView!
    override func viewDidLoad() {
        super.viewDidLoad()
        let v = MLInputAccessoryView(height: 40, _delegate: self)
        textEditorView.inputAccessoryView = v
        v.setLeftBtnVisible(false)
        
        NSNotificationCenter.defaultCenter().addObserver(self, selector: #selector(keyBoardWillChange(_:)), name: UIKeyboardWillChangeFrameNotification, object: nil)
        
        navigationItem.leftBarButtonItem = UIBarButtonItem(image: nil, style: .Plain, target: self, action: #selector(onBack))
        navigationItem.leftBarButtonItem?.title = nil
        
        navigationItem.rightBarButtonItem = UIBarButtonItem(title: "完成", style: .Plain, target: self, action: #selector(onFinish))
        
        title = editorItem.type.title()
        textEditorView.placeHolder = editorItem.placeholder
    }
    
    func onBack() {
        navigationController?.popViewControllerAnimated(true)
    }
    
    func onFinish() {
        editorItem.attrContent = textEditorView.attributedText
        navigationController?.popViewControllerAnimated(true)
    }
    
    deinit {
        NSNotificationCenter.defaultCenter().removeObserver(self)
    }
}

extension MLCaseItemEditorViewController {
    func keyBoardWillChange(notification: NSNotification) {
        var time: Double = 0.0
        if let t = notification.userInfo?[UIKeyboardAnimationDurationUserInfoKey] as? Double  {
            time = t
        }
        
        if let rect = notification.userInfo?[UIKeyboardFrameEndUserInfoKey]?.CGRectValue {
            UIView.animateWithDuration(time, animations: { [weak self] in
                self?.textEditorBottomOffset.constant = rect.height + 24
            })
        }
    }
}

extension MLCaseItemEditorViewController: MLInputAccessoryViewDelegate {
    func onRightBtnTouched() {
        //todo: pickup image
        textEditorView.addImage("caseEditorBtn0")
    }
}
