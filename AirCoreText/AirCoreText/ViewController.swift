//
//  ViewController.swift
//  AirCoreText
//
//  Created by 彭浩 on 16/8/1.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import UIKit
import YYText

class ViewController: UIViewController {
    var tv: MLTextEditView? = nil {
        didSet {
            tv?.delegate = self
            tv?.enableFixedSelectedTextRange = true
            tv?.allowsPasteAttributedString = true
            tv?.allowsCopyAttributedString = true
            tv?.allowsPasteImage = true
        }
    }
    
    func onHeightChanged(notification: NSNotification) {
        tableView.beginUpdates()
        tableView.endUpdates()
        
    }
    
    
    let tableView: UITableView = UITableView(frame: CGRectZero, style: .Plain)
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        tableView.frame = view.bounds
        tableView.delegate = self
        tableView.dataSource = self
        tableView.registerClass(UITableViewCell.self, forCellReuseIdentifier: "tableViewCell")
        view.addSubview(tableView)
        
        let btn = UIButton(type: .Custom)
        btn.frame = CGRectMake(0, view.bounds.size.height - 100, 100, 100)
        btn.addTarget(self, action: #selector(ViewController.onSelect(_:)), forControlEvents: .TouchUpInside)
        btn.backgroundColor = UIColor.blueColor()
        view.addSubview(btn)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    func onSelect(sender: AnyObject?) {
        tv?.addImage("2");
        guard let attr = tv?.attributedText else {
            return
        }
        /**
        attr.encode().upload { (success, failedElement) in
            if success {
                
            } else {
                //retry
                failedElement.upload({ (success, failedElement) in
                    
                })
            }
        }
 **/
    }
}

extension ViewController : YYTextViewDelegate {
    func textViewDidChange(textView: YYTextView) {
        textView.frame = CGRectMake(textView.frame.origin.x, textView.frame.origin.y, textView.contentSize.width, textView.contentSize.height)
        tableView.beginUpdates()
        tableView.endUpdates()
    }
    
    func textView(textView: YYTextView, shouldChangeTextInRange range: NSRange, replacementText text: String) -> Bool {
        if text == "图" {
            tv?.addImage("2");
            return false
        }
        return true
    }
}

extension ViewController: UITableViewDataSource, UITableViewDelegate {
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return 1
    }
    
    func tableView(tableView: UITableView, heightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        if let tv = tv {
            return tv.frame.size.height
        } else {
            return 50
        }
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("tableViewCell", forIndexPath: indexPath)
        
        let v = MLTextEditView(frame: view.bounds)
        v.backgroundColor = UIColor.grayColor()
        v.tag = 999
        v.frame = cell.bounds
        cell.addSubview(v)
//        v.addImage("QQ")
        tv = v
        return cell
    }
}

