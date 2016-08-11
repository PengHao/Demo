//
//  MLCaseEditorContentCell.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/9.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit

@objc protocol MLEditorContentCellDelegate: NSObjectProtocol {
    func onCellHeightChanged(newHeight: CGFloat, cell: UITableViewCell)
}

class MLCaseEditorContentCell: MLEditorCell {
    override func update() {
        descBtn.setImage(nil, forState: .Normal)
        descBtn.setTitle(nil, forState: .Normal)
        if let item = data as? MLCaseEditorItem {
            titleLabel.text = NSString(format: "%02d.  %@", item.type.rawValue, item.type.title()) as String
            if item.hasData {
                descBtn.setImage(UIImage(named: "caseEditorBtn0"), forState: .Normal)
                descBtn.setTitle(nil, forState: .Normal)
            } else {
                let d = item.type.desc()
                descBtn.setTitle(d.0, forState: .Normal)
                descBtn.setTitleColor(d.1, forState: .Normal)
                descBtn.setImage(nil, forState: .Normal)
            }
        }
    }

    @IBOutlet weak var contentHeight: NSLayoutConstraint!
    @IBOutlet weak var editorContentView: UIView!
    @IBOutlet weak var descBtn: UIButton!
    @IBOutlet weak var titleLabel: UILabel!
    
    weak var delegate: MLEditorContentCellDelegate?
    override func awakeFromNib() {
        super.awakeFromNib()
    }
    
    @IBAction func onDescBtn() {
        let h = max(57, CGFloat(10*(1+Int(rand())%10)))
        delegate?.onCellHeightChanged(h, cell: self)
    }

    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)
    }
    
    func setContent() {
        contentHeight.constant = 100
    }
}
