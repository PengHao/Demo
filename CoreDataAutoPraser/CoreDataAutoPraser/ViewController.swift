//
//  ViewController.swift
//  CoreDataAutoPraser
//
//  Created by 彭浩 on 16/7/23.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import UIKit
import CoreData
class ViewController: UIViewController {
    var context = NSManagedObjectContext(concurrencyType: .MainQueueConcurrencyType)
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        guard let modelURL = NSBundle.mainBundle().URLForResource("CoreDataAutoPraser", withExtension: "momd"),
            let model = NSManagedObjectModel(contentsOfURL: modelURL) else {
            return
        }
        let coodinator = NSPersistentStoreCoordinator(managedObjectModel: model)
        context.persistentStoreCoordinator = coodinator
        
        
        //Swift Dictionary
        let info = [
            "mBool"     :0,
            "mDate"     :1470391786.4013271,
            "mDouble"   :12321312123.3211231312,
            "mFloat"    :999.123,
            "mInt16"    :"1236",
            "mInt32"    :"12342141",
            "mInt64"    :"123789126651641293",
            "mString"   :"gdasd qweggdsa gdsa"
        ]
        
        //NSDictionary
        let infoDic = NSDictionary(dictionary: info)
        
        let entity = NSManagedObject.CreateWithMoc(context, entityName: "Entity", info: infoDic) { (description) -> NSManagedObject? in
            let obj = NSEntityDescription.insertNewObjectForEntityForName("Entity", inManagedObjectContext: self.context)
            return obj
        }
        
        print("\(entity)")
        
        let en = entity?.enCode()
        print("\(en)")
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

