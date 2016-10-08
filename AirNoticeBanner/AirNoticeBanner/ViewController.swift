//
//  ViewController.swift
//  AirNoticeBanner
//
//  Created by PengHao on 16/10/8.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    var i = 0
    @IBAction func onShow(sender: AnyObject) {
        i = i + 1
        AirNoticeBanner.show(NSAttributedString(string: " 123321 \(i) "), icon: UIImage(named: "FailedFlag"), btnTitle: "点击关闭") {
            print("on close")
        }
    }
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewDidAppear( animated: Bool) {
        super.viewDidAppear(animated)
        
        
    }


}

