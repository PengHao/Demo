//
//  ViewController.swift
//  LogPlayer
//
//  Created by 彭浩 on 16/8/5.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    
//    let player = MLLogPlayer(urlString: "http://www.baidu.com")
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
//        player.start()
        let player = MLLogPlayer(urlString: "http://www.baidu.com")
        player.test()
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

