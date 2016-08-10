//
//  MLPostVoteTableViewCell.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/10.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit
protocol MLPostVoteTableViewCellDelegate : NSObjectProtocol{
    func onDelete(cell: MLPostVoteTableViewCell)
}

class MLPostVoteTableViewCell: MLEditorCell , UITextFieldDelegate {
    weak var delegate: MLPostVoteTableViewCellDelegate?
    
    @IBOutlet weak var voteInputField: UITextField! {
        didSet {
            voteInputField.delegate = self
        }
    }
    
    @IBAction func onDelete(sender: AnyObject) {
        delegate?.onDelete(self)
    }
    
    override var mlInputAccessoryView : MLInputAccessoryView? {
        didSet {
            mlInputAccessoryView?.setRightBtnVisible(false)
            voteInputField.inputAccessoryView = mlInputAccessoryView
        }
    }
    
    override func update() {
        voteInputField.placeholder = data?.placeholder
        voteInputField.text = data?.content ?? nil
    }
    
    @objc func textFieldDidEndEditing(textField: UITextField) {
        data?.content = voteInputField.text
    }
}