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
    private var editorItem: MLCaseEditorItem!
    @IBOutlet weak var textEditorBottomOffset: NSLayoutConstraint!
    @IBOutlet weak var textEditorView: MLTextEditView!
    override func viewDidLoad() {
        super.viewDidLoad()
        let v = MLInputAccessoryView(height: 40, _delegate: self)
        textEditorView.inputAccessoryView = v
        v.setLeftBtnVisible(false)
        NSNotificationCenter.defaultCenter().addObserver(self, selector: #selector(keyBoardWillChange(_:)), name: UIKeyboardWillChangeFrameNotification, object: nil)
    }
    
    deinit {
        NSNotificationCenter.defaultCenter().removeObserver(self)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    override func viewDidDisappear(animated: Bool) {
        super.viewDidDisappear(animated)
        editorItem.data = textEditorView.attributedText
    }
    
    func setEditorItem(item: MLCaseEditorItem) {
        editorItem = item
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
