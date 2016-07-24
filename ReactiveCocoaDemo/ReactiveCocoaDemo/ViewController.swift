//
//  ViewController.swift
//  ReactiveCocoaDemo
//
//  Created by 彭浩 on 16/7/20.
//  Copyright © 2016年 wolfpeng. All rights reserved.
//

import UIKit
import ReactiveCocoa
import Result

//class T : SignalType {
//    var signal: Int {
//        return 0
//    }
//    
//    func observe(observer: Signal<Int, NSError>.Observer) -> Disposable? {
//        return nil
//    }
//    
//}




class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        let (signal, observe) = Signal<(UIButton?), NoError>.pipe()

        signal.observeOn(UIScheduler()).observeNext { (btn) in
            print("\(btn)")
        }
        
        signal.observeCompleted { 
            print("completed")
        }
        
        signal.observeFailed { (error) in
            print("\(error)")
        }
        
        signal.observeInterrupted { 
            print("Interrupted")
            
        }
        
        let btn = UIButton(type: .Custom)
        btn.frame = CGRectMake(50, 50, 100, 50)
        btn.setTitle("btn", forState: .Normal)
        self.view.addSubview(btn)
        let obj = NSObject()
//        obj.rac_valuesAndChangesForKeyPath(<#T##keyPath: String!##String!#>, options: <#T##NSKeyValueObservingOptions#>, observer: <#T##NSObject!#>).subscribeNext(<#T##nextBlock: ((AnyObject!) -> Void)!##((AnyObject!) -> Void)!##(AnyObject!) -> Void#>)
//        btn.rac_signalForSelector(<#T##selector: Selector##Selector#>).subscribeNext { (<#AnyObject!#>) in
//            <#code#>
//        }
//        btn.rac_signalForControlEvents(.TouchUpInside).subscribeNext { (btn) in
//            print("onTouch")
//        }
        
        
        if let url = NSURL(string: "http://www.baidu.com") {
            let request = NSURLRequest(URL: url)
            NSURLConnection.sendAsynchronousRequest(request, queue: NSOperationQueue.mainQueue()) { (response, data, error) in
                observe.sendNext((response, data, error))
            }
            NSURLConnection.rac_sendAsynchronousRequest(request).subscribeNext({ (<#AnyObject!#>) in
                <#code#>
            })
        }
        observe.sendCompleted()
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

