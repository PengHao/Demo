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

class MLPostVoteTableViewCell: UITableViewCell {
    @IBOutlet weak var voteInputField: UITextField! {
        didSet {
            voteInputField.delegate = self
        }
    }
    
    var data: MLVoteItem! {
        didSet {
            
        }
    }
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }
    
    @IBAction func onDelete(sender: AnyObject) {
        delegate?.onDelete(self)
    }

    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }
    
    weak var mlInputAccessoryView : MLInputAccessoryView? {
        didSet {
            mlInputAccessoryView?.setRightBtnVisible(false)
            voteInputField.inputAccessoryView = mlInputAccessoryView
        }
    }
    
    weak var delegate: MLPostVoteTableViewCellDelegate?
}


extension MLPostVoteTableViewCell : UITextFieldDelegate {
    func textFieldDidEndEditing(textField: UITextField) {
        data.text = voteInputField.text
    }
}