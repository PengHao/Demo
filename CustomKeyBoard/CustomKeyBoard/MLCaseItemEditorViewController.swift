//
//  MLCaseItemEditorViewController.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/9.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit

class MLCaseItemEditorViewController: UIViewController {
    private var editorItem: MLCaseEditorItem!
    @IBOutlet weak var textEditorView: UITextView!
    override func viewDidLoad() {
        super.viewDidLoad()
        let v = MLInputAccessoryView(height: 40, _delegate: self)
        textEditorView.inputAccessoryView = v
        v.setLeftBtnVisible(false)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        
    }
    
    
    func setEditorItem(item: MLCaseEditorItem) {
        editorItem = item
        editorItem.hasData = !editorItem.hasData
    }
}


extension MLCaseItemEditorViewController: MLInputAccessoryViewDelegate {
    func onRightBtnTouched() {
        //todo: pickup image
    }
}
