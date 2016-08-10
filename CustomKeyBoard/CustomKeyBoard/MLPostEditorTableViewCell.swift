//
//  MLPostEditorTableViewCell.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/10.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit
protocol MLPostEditorTableViewCellDelegate : NSObjectProtocol {
    func onAddVoteOption(cell: UITableViewCell)
    func onPickImage(cell: UITableViewCell)
}


class MLPostEditorTableViewCell: UITableViewCell {

    @IBOutlet weak var textEditorView: MLTextEditView! {
        didSet {
            textEditorView.delegate = self
        }
    }
    var data: MLPostItem! {
        didSet {
            
        }
    }
    
    weak var delegate: MLEditorContentCellDelegate?
    
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
        
        textEditorView.placeHolder = "输入正文内容"
        textEditorView.addObserver(self, forKeyPath: "contentSize", options: .New, context: nil)
    }
    
    override func observeValueForKeyPath(keyPath: String?, ofObject object: AnyObject?, change: [String : AnyObject]?, context: UnsafeMutablePointer<Void>) {
        if "contentSize" == keyPath {
            delegate?.onCellHeightChanged(textEditorView.contentSize.height + 24, cell: self)
        }
    }

    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }
    
    weak var mlInputAccessoryView : MLInputAccessoryView? {
        didSet {
            mlInputAccessoryView?.setRightBtnVisible(true)
            textEditorView.inputAccessoryView = mlInputAccessoryView
        }
    }
}

extension MLPostEditorTableViewCell : UITextViewDelegate {
    func textViewDidEndEditing(textView: UITextView) {
        data.text = textView.attributedText
    }
}

