//
//  MLPostEditorViewController.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/10.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit

class MLPostEditorViewController: UIViewController {
    
    @IBOutlet weak var tableViewButtomOffset: NSLayoutConstraint!
    @IBOutlet weak var tableView: UITableView!
    let postEditorModel : MLPostEditorModel = MLPostEditorModel()
    let navTitle = "发帖"
    
    var cellsHeight = [NSIndexPath: CGFloat]()
    override func viewDidLoad() {
        super.viewDidLoad()
        title = navTitle
        navigationItem.rightBarButtonItem = UIBarButtonItem(title: "下一步", style: .Plain, target: self, action: #selector(onNext))
        tableView.registerNib(UINib(nibName: "MLCaseEditorTitleCell", bundle: NSBundle.mainBundle()), forCellReuseIdentifier: "MLCaseEditorTitleCell")
        tableView.registerNib(UINib(nibName: "MLPostEditorTableViewCell", bundle: NSBundle.mainBundle()), forCellReuseIdentifier: "MLPostEditorTableViewCell")
        tableView.registerNib(UINib(nibName: "MLPostVoteTableViewCell", bundle: NSBundle.mainBundle()), forCellReuseIdentifier: "MLPostVoteTableViewCell")
        
        
        NSNotificationCenter.defaultCenter().addObserver(self, selector: #selector(keyBoardWillChange(_:)), name: UIKeyboardWillChangeFrameNotification, object: nil)
    }
    
    func onNext() {
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}


extension MLPostEditorViewController : UITableViewDelegate {
    func tableView(tableView: UITableView, heightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        if indexPath.section == 1 {
            return max(100, cellsHeight[indexPath] ?? 100)
        } else {
            return 64
        }
    }
}

extension MLPostEditorViewController : UITableViewDataSource {
    func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return postEditorModel.data.count
    }
    
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return postEditorModel.data[section].count
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        if let identify = EN_MLPostSection(rawValue: indexPath.section)?.identify() ,
            let cell = tableView.dequeueReusableCellWithIdentifier(identify, forIndexPath: indexPath) as? MLEditorCell{
            
        
            
            if let c = cell as? MLPostVoteTableViewCell {
                c.delegate = self
            } else if let c = cell as? MLPostEditorTableViewCell {
                c.delegate = self
            }
            cell.data = postEditorModel.data(indexPath)
            cell.mlInputAccessoryView = MLInputAccessoryView(height: 40, _delegate: self)
            return cell
        }
        return UITableViewCell()
    }
}

extension MLPostEditorViewController : MLEditorContentCellDelegate {
    func onCellHeightChanged(newHeight: CGFloat, cell: UITableViewCell) {
        guard let index = tableView.indexPathForCell(cell) else {
            return
        }
        cellsHeight[index] = newHeight
        tableView.beginUpdates()
        tableView.endUpdates()
    }
}

extension MLPostEditorViewController : MLInputAccessoryViewDelegate {
    func onLeftBtnTouched() {
        guard postEditorModel.data.count > EN_MLPostSection.PostVotes.rawValue else {
            return
        }
        tableView.beginUpdates()
        var indexPaths = [NSIndexPath]()
        var votes = postEditorModel.data[EN_MLPostSection.PostVotes.rawValue]
        indexPaths.append(NSIndexPath(forRow: votes.count, inSection: 2))
        votes.append(MLVoteItem())
        postEditorModel.data.removeAtIndex(2)
        postEditorModel.data.insert(votes, atIndex: 2)
        
        tableView.insertRowsAtIndexPaths(indexPaths, withRowAnimation: .Fade)
        tableView.endUpdates()
    }
    
    func onRightBtnTouched() {
    }
}

extension MLPostEditorViewController : MLPostVoteTableViewCellDelegate {
    func onDelete(cell: MLPostVoteTableViewCell) {
        guard postEditorModel.data.count > EN_MLPostSection.PostVotes.rawValue else {
            return
        }
        var indexPaths = [NSIndexPath]()
        var votes = postEditorModel.data[EN_MLPostSection.PostVotes.rawValue]
        let index = votes.indexOf { (data) -> Bool in
            if let d = cell.data {
                return data.isEqual(d)
            } else {
                return false
            }
        }
        guard let idx = index else {
            return
        }
        tableView.beginUpdates()
        indexPaths.append(NSIndexPath(forRow: idx, inSection: 2))
        votes.removeAtIndex(idx)
        postEditorModel.data.removeAtIndex(2)
        postEditorModel.data.insert(votes, atIndex: 2)
        tableView.deleteRowsAtIndexPaths(indexPaths, withRowAnimation: .Fade)
        tableView.endUpdates()
    }
}

extension MLPostEditorViewController {
    func keyBoardWillChange(notification: NSNotification) {
        var time: Double = 0.0
        if let t = notification.userInfo?[UIKeyboardAnimationDurationUserInfoKey] as? Double  {
            time = t
        }
        
        if let rect = notification.userInfo?[UIKeyboardFrameEndUserInfoKey]?.CGRectValue {
            UIView.animateWithDuration(time, animations: { [weak self] in
                
                self?.tableViewButtomOffset.constant = UIScreen.mainScreen().bounds.size.height - rect.origin.y + 24
                })
        }
    }
}