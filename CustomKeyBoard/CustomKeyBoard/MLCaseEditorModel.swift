//
//  MLCaseEditorModel.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/9.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import Foundation
import UIKit



class MLCaseEditorItem: NSObject, MLEditorItem {
    var content: String?
    var attrContent: NSAttributedString?
    var placeholder: String?
    
    var hasData: Bool {
        return attrContent != nil && attrContent!.length > 0
    }
    
    var type: EN_MLCaseItem!
    
    init(_type: EN_MLCaseItem) {
        type = _type
        placeholder = type.placeholder()
    }
}


enum EN_MLCaseItem : Int {
    case CaseTitle = 0
    case CaseHistroy = 1
    case CaseCheck = 2
    case CaseSurge = 3
    case CaseDesc = 4
    
    func title() -> String {
        let cellTitles = [
            "请输入病例标题",
            "主诉 病史",
            "查体 辅查",
            "诊断 治疗",
            "随访 讨论",
            ]
        return cellTitles[self.rawValue]
    }
    
    func placeholder() -> String {
        
        let cellPlaceholder = [
            "请输入病例标题",
            "请输入主诉和病史",
            "请输入查体和辅查",
            "请输入诊断和治疗",
            "请输入随访和讨论",
            ]
        return cellPlaceholder[self.rawValue]
    }
    
    func desc() -> (String, UIColor) {
        if self == .CaseHistroy {
            return ("必填", UIColor.redColor())
        } else {
            return ("选填", UIColor.grayColor())
        }
    }
}

class MLCaseEditorModel {
    init() {
        
    }
    var items : [MLCaseEditorItem] = [
        MLCaseEditorItem(_type: .CaseTitle),
        MLCaseEditorItem(_type: .CaseHistroy),
        MLCaseEditorItem(_type: .CaseCheck),
        MLCaseEditorItem(_type: .CaseSurge),
        MLCaseEditorItem(_type: .CaseDesc)
    ]
}