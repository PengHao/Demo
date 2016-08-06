//
//  ViewController.swift
//  podTest
//
//  Created by 彭浩 on 16/7/16.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import UIKit
import ReadMoreTextView

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        super.viewDidLoad()
        let textView = ReadMoreTextView()
        textView.text = "When the saints go marching in, When the saints go marching in"
        textView.maximumNumberOfLines = 3
        textView.shouldTrim = true
        textView.trimText = "Read more"
        textView.frame = CGRectMake(0, 50, 60, 100)
        self.view.addSubview(textView)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

