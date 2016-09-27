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

class MLPostEditorTableViewCell: MLEditorCell, UITextViewDelegate {

    @IBOutlet weak var textEditorView: MLTextEditView!
    
    weak var delegate: MLEditorContentCellDelegate?
    
    override func update() {
        textEditorView.attributedText = data?.attrContent ?? nil
        textEditorView.placeHolder = data?.placeholder
    }
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
        textEditorView.addObserver(self, forKeyPath: "contentSize", options: .New, context: nil)
    }
    
    override func observeValueForKeyPath(keyPath: String?, ofObject object: AnyObject?, change: [String : AnyObject]?, context: UnsafeMutablePointer<Void>) {
        if "contentSize" == keyPath {
            delegate?.onCellHeightChanged(textEditorView.contentSize.height + 24, cell: self)
        }
    }

    override var mlInputAccessoryView : MLInputAccessoryView? {
        didSet {
            mlInputAccessoryView?.setRightBtnVisible(true)
            textEditorView.inputAccessoryView = mlInputAccessoryView
        }
    }
    
    @objc func textViewDidEndEditing(textView: UITextView) {
        data?.attrContent = textView.attributedText
    }
}

