//
//  MLCaseEditorTitleCell.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/9.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit

class MLCaseEditorTitleCell: UITableViewCell {
    var data: MLTitleItem? {
        didSet {
            titleTextField.text = data?.text
        }
    }
    
    @IBOutlet weak var titleTextField: UITextField!
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }
    
    func setTitle(title: String?) {
        titleTextField.placeholder = title
    }
    
    weak var mlInputAccessoryView : MLInputAccessoryView? {
        didSet {
            mlInputAccessoryView?.setRightBtnVisible(false)
            titleTextField.inputAccessoryView = mlInputAccessoryView
        }
    }
}
