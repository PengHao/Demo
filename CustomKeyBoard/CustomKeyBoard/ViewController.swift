//
//  ViewController.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/9.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    @IBOutlet weak var imputView: UITextView?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        let v = MLInputAccessoryView(height: 40, _delegate: self)
        imputView?.inputAccessoryView = v
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

extension ViewController: MLInputAccessoryViewDelegate {
    func onLeftBtnTouched() {
        print("onLeftBtnTouched")
        let vc = MLCaseEditorViewController(nibName: "MLCaseEditorViewController", bundle: NSBundle.mainBundle())
        vc.automaticallyAdjustsScrollViewInsets = true
        let nvc = UINavigationController(rootViewController: vc)
        self.presentViewController(nvc, animated: false, completion: nil)
    }
    
    func onRightBtnTouched() {
        print("onRightBtnTouched")
        
    }
}

