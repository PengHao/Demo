//
//  Entity+CoreDataProperties.swift
//  CoreDataAutoPraser
//
//  Created by 彭浩 on 16/7/23.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//
//  Choose "Create NSManagedObject Subclass…" from the Core Data editor menu
//  to delete and recreate this implementation file for your updated model.
//

import Foundation
import CoreData

extension Entity {

    @NSManaged var mInt64: NSNumber?
    @NSManaged var mInt32: NSNumber?
    @NSManaged var mInt16: NSNumber?
    @NSManaged var mString: String?
    @NSManaged var mFloat: NSNumber?
    @NSManaged var mDouble: NSNumber?
    @NSManaged var mBool: NSNumber?
    @NSManaged var mDate: NSDate?

}
