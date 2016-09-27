//
//  CoreData+AutoPraser.swift
//  MedLinker
//
//  Created by penghao on 15/11/11.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import Foundation
import CoreData

public enum MLSynchronousStatus : Int16 {
    case Normal = 0             //正常
    case Committing = 1         //正在提交
    case Updating = 2           //正在更新
    case Deletting = 3          //正在删除
}

extension NSManagedObject {
    typealias SetPropertiesValuesCallback = (NSEntityDescription) -> NSManagedObject?
    
    class func CreateWithMoc(moc: NSManagedObjectContext, entityName: String, info: AnyObject, keyMap: [String: String]? = nil, checkCallBack: SetPropertiesValuesCallback) -> NSManagedObject? {
        var _optionPropertiesList = [String]();
        guard let entity = NSEntityDescription.entityForName(entityName, inManagedObjectContext: moc), let obj = checkCallBack(entity) else {
            return nil
        }
        for property in entity.properties {
            let attributeDesc = entity.attributesByName[property.name]
            if entity.relationshipsByName[property.name] != nil {
                _optionPropertiesList.append(property.name)
                continue
            }
            let key = keyMap?[property.name] ?? property.name
            guard let v = info.valueForKey(key) else {
                if !property.optional {
                    print("Cannot find value of key \(key) in params")
                    return nil
                } else {
                    _optionPropertiesList.append(property.name)
                    continue
                }
            }
            if let attrClsName = attributeDesc?.attributeValueClassName,
                let cls = NSClassFromString(attrClsName) {
                if !v.isKindOfClass(cls) {
                    var value: AnyObject? = nil
                    if attrClsName == "NSNumber" {
                        value = v.doubleValue
                    } else if attrClsName == "NSString" {
                        value = "\(v)"
                    } else if attrClsName == "NSDate" {
                        if let d = v.doubleValue {
                            value = NSDate(timeIntervalSince1970: d)
                        }
                    }
                    if value != nil {
                        obj.setValue(value, forKey: property.name)
                    } else if !property.optional {
                        print("Params value of key \(property.name)'s type is invalide, required is \(attrClsName), value is \(v)")
                    }
                } else {
                    obj.setValue(v, forKey: property.name)
                }
            } else {
                print("Cannot find value of key \(key) in params")
            }
        }
        return obj;
    }
    
    func enCode() -> [String: AnyObject] {
        var rs = [String: AnyObject]()
        for property in entity.properties {
            let v = valueForKey(property.name)
            rs[property.name] = v?.enCode?() ?? valueForKey(property.name)
        }
        return rs
    }
}