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

class MLCaseEditorContentCell: UITableViewCell {
    
    private var _item: (EN_MLCaseItem, MLCaseEditorItem)? {
        didSet {
            oldValue?.1.removeObserver(self, forKeyPath: "hasChanged")
            if let item = _item {
                item.1.addObserver(self, forKeyPath: "hasChanged", options: .New, context: nil)
            }
            refresh()
        }
    }
    
    deinit {
        _item?.1.removeObserver(self, forKeyPath: "hasChanged")
    }
    
    private func refresh() {
        
        descBtn.setImage(nil, forState: .Normal)
        descBtn.setTitle(nil, forState: .Normal)
        if let item = _item {
            titleLabel.text = NSString(format: "%02d.  %@", item.0.rawValue, item.0.title()) as String
            if item.1.hasData {
                descBtn.setImage(UIImage(named: "caseEditorBtn0"), forState: .Normal)
                descBtn.setTitle(nil, forState: .Normal)
            } else {
                let d = item.0.desc()
                descBtn.setTitle(d.0, forState: .Normal)
                descBtn.setTitleColor(d.1, forState: .Normal)
                descBtn.setImage(nil, forState: .Normal)
            }
        }
    }
    
    override func observeValueForKeyPath(keyPath: String?, ofObject object: AnyObject?, change: [String : AnyObject]?, context: UnsafeMutablePointer<Void>) {
        if keyPath == "hasChanged" {
            dispatch_async(dispatch_get_main_queue(), { [weak self] in
                self?.refresh()
            })
        }
    }

    @IBOutlet weak var contentHeight: NSLayoutConstraint!
    @IBOutlet weak var editorContentView: UIView!
    @IBOutlet weak var descBtn: UIButton!
    @IBOutlet weak var titleLabel: UILabel!
    
    weak var delegate: MLEditorContentCellDelegate?
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }
    
    @IBAction func onDescBtn() {
        let h = max(57, CGFloat(10*(1+Int(rand())%10)))
        delegate?.onCellHeightChanged(h, cell: self)
    }

    override func setSelected(selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }
    
    func setItem(index: EN_MLCaseItem, item: MLCaseEditorItem) {
        _item = (index, item)
    }
    
    
    func setContent() {
        contentHeight.constant = 100
    }
}
