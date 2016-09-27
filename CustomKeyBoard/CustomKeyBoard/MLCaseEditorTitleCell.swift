//
//  MLCaseEditorTitleCell.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/9.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit


class MLEditorCell: UITableViewCell {
    var data: MLEditorItem? {
        didSet {
            oldValue?.removeChangeObserve?(self)
            data?.addChangeObserve?(self)
            update()
        }
    }

    var mlInputAccessoryView : MLInputAccessoryView?
    
    func update() {
        
    }
}



class MLCaseEditorTitleCell : MLEditorCell{
    override func update() {
        if let c = data?.content {
            titleTextField.text = c
        }
        titleTextField.placeholder = data?.placeholder
    }
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }
    
    override var mlInputAccessoryView : MLInputAccessoryView? {
        set {
            titleTextField.inputAccessoryView = newValue
            newValue?.setRightBtnVisible(false)
        }
        
        get {
            return titleTextField.inputAccessoryView as? MLInputAccessoryView
        }
    }
    
    @IBOutlet weak var titleTextField: UITextField!
}
