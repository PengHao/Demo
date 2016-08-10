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

class MLTitleItem: NSObject {
    var text : String?
}

class MLVoteItem: NSObject {
    var text : String?
}

class MLPostItem: NSObject {
    var text: NSAttributedString?
}

class MLPostEditorModel: NSObject {
    var data : [[AnyObject]] = [
        [
            MLTitleItem()
        ],
        [
            MLPostItem()
        ],
        [
        ]
    ]
    
    func data(indexPath: NSIndexPath) -> AnyObject? {
        guard data.count > indexPath.section && data[indexPath.section].count > indexPath.row else {
            return nil
        }
        return data[indexPath.section][indexPath.row]
    }
    
    override init() {
        super.init()
    }
}
