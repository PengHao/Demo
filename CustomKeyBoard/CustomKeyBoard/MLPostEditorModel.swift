//
//  MLPostEditorModel.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/10.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit

enum EN_MLPostSection : Int {
    case PostTitle = 0
    case PostContent = 1
    case PostVotes = 2
    
    func identify() -> String {
        let cellIdentify = [
            "MLCaseEditorTitleCell",
            "MLPostEditorTableViewCell",
            "MLPostVoteTableViewCell"
        ]
        return cellIdentify[self.rawValue]
    }
    
    func title(row: Int) -> String? {
        let cellTitles = [
            [
                "请输入帖子标题",
            ],
            [
                "输入正文内容",
            ],
            [
                "输入投票内容",
            ]
        ]
        guard cellTitles[self.rawValue].count > row else {
            return nil
        }
        return cellTitles[self.rawValue][row]
    }
}

@objc protocol MLEditorItem : NSObjectProtocol {
    
    var content: String? {get set}
    var attrContent: NSAttributedString? {get set}
    var placeholder: String? {get set}
    optional func addChangeObserve(observer: NSObject)
    optional func removeChangeObserve(observer: NSObject)
}

extension MLEditorItem where Self : NSObject {
    func addChangeObserve(observer: NSObject) {
        addObserver(observer, forKeyPath: "content", options: .New, context: nil)
    }
    
    func removeChangeObserve(observer: NSObject) {
        removeObserver(observer, forKeyPath: "content")
    }
}

class MLTitleItem: NSObject, MLEditorItem {
    var content : String?
    var attrContent: NSAttributedString?
    var placeholder: String?
    init(content : String? = nil) {
        super.init()
        self.content = content
        self.placeholder = "请输入帖子标题"
    }
}

class MLVoteItem: NSObject, MLEditorItem {
    var content : String?
    var attrContent: NSAttributedString?
    var placeholder: String?
    init(content : String? = nil) {
        super.init()
        self.content = content
        self.placeholder = "请输入投票内容"
    }
}

class MLPostItem: NSObject, MLEditorItem {
    var content : String?
    var attrContent: NSAttributedString?
    var placeholder: String?
    init(attrContent : NSAttributedString? = nil) {
        super.init()
        self.attrContent = attrContent
        self.placeholder = "请输入帖子内容"
    }
}

class MLPostEditorModel: NSObject {
    var data : [[MLEditorItem]] = [
        [
            MLTitleItem()
        ],
        [
            MLPostItem()
        ],
        [
        ]
    ]
    
    
    func data(indexPath: NSIndexPath) -> MLEditorItem? {
        guard data.count > indexPath.section && data[indexPath.section].count > indexPath.row else {
            return nil
        }
        return data[indexPath.section][indexPath.row]
    }
    
    override init() {
        super.init()
    }
}
