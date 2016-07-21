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
        
        let (signal, observe) = Signal<Int, NoError>.pipe()
        signal.observeOn(UIScheduler()).map { (s) -> String in
            return "str \(s)"
        }.observeNext { (r) in
            print("next \(r)")
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
        
//        observe.sendFailed(TError())
        observe.sendNext(2)
//        observe.sendInterrupted()
        observe.sendNext(3)
        observe.sendCompleted()
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

