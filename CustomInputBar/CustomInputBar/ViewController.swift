//
//  ViewController.swift
//  CustomInputBar
//
//  Created by PengHao on 16/8/15.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        let bar = MLInputBar(frame: CGRectMake(0,view.frame.size.height - 40, view.frame.size.width, 40))
        bar.defaultFrame = CGRectMake(0, 0, 30, 30)
        bar.edgeInsets = UIEdgeInsetsMake(5, 5, 5, 5)
        view.addSubview(bar)
        bar.backgroundColor = UIColor.grayColor()
        bar.addLeftButton("1") { (inputBar) in
            print("123")
        }
        
        bar.addLeftButton("2") { (inputBar) in
            print("456")
        }
        bar.addRightButton("3") { (inputBar) in
            print("789")
        }
        bar.addRightButton("4") { (inputBar) in
            print("012")
        }
        
        bar.addRightButton("5", customFrame: CGRectMake(0, 0, 100, 40)) { (inputBar) in
            
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

