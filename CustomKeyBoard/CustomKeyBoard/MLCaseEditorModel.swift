//
//  MLCaseEditorModel.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/9.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import Foundation
import UIKit



class MLCaseEditorItem: NSObject {
    var hasChanged: Bool = false
    
    var data: NSAttributedString? {
        didSet {
            setValue(true, forKey: "hasChanged")
        }
    }
    
    var hasData: Bool {
        return data != nil && data!.length > 0
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
            "01.  主诉 病史",
            "02.  查体 辅查",
            "03.  诊断 治疗",
            "04.  随访 讨论",
            ]
        return cellTitles[self.rawValue]
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
    var items : [EN_MLCaseItem : MLCaseEditorItem] = [
        .CaseTitle      : MLCaseEditorItem(),
        .CaseHistroy    : MLCaseEditorItem(),
        .CaseCheck      : MLCaseEditorItem(),
        .CaseSurge      : MLCaseEditorItem(),
        .CaseDesc       : MLCaseEditorItem()
    ]
    
    
    
}