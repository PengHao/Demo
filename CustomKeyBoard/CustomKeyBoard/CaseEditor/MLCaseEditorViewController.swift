//
//  MLCaseEditorViewController.swift
//  CustomKeyBoard
//
//  Created by PengHao on 16/8/9.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit

class MLCaseEditorViewController: UIViewController {
    @IBOutlet weak var tableView: UITableView!
    
    var cellsHeight = [NSIndexPath: CGFloat]()
    var caseModel = MLCaseEditorModel()
    
    let navTitle = "攥写病历"
    
    override func viewDidLoad() {
        super.viewDidLoad()

        tableView.registerNib(UINib(nibName: "MLCaseEditorTitleCell", bundle: NSBundle.mainBundle()), forCellReuseIdentifier: "MLCaseEditorTitleCell")
        tableView.registerNib(UINib(nibName: "MLCaseEditorContentCell", bundle: NSBundle.mainBundle()), forCellReuseIdentifier: "MLCaseEditorContentCell")
    }

    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        navigationController?.title = navTitle
        navigationController?.navigationItem.rightBarButtonItem = UIBarButtonItem(title: "下一步", style: .Plain, target: self, action: #selector(onNext))
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func onNext() {
        
    }

}

extension MLCaseEditorViewController : UITableViewDelegate {
    
    func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        tableView.deselectRowAtIndexPath(indexPath, animated: true)
        
        let vc = MLCaseItemEditorViewController(nibName: "MLCaseItemEditorViewController", bundle: NSBundle.mainBundle())
        
        if let index = EN_MLCaseItem(rawValue: indexPath.row), let item = caseModel.items[index] {
            vc.setEditorItem(item)
        }
        
        vc.automaticallyAdjustsScrollViewInsets = true
        navigationController?.pushViewController(vc, animated: true)
    }
    
    
    func tableView(tableView: UITableView, heightForRowAtIndexPath indexPath: NSIndexPath) -> CGFloat {
        if indexPath.row == 0 {
            return 62
        } else {
            return cellsHeight[indexPath] ?? 57
        }
    }
}

extension MLCaseEditorViewController : UITableViewDataSource {
    func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return caseModel.items.count
    }
    
    func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        if indexPath.row == 0 {
            let cell = tableView.dequeueReusableCellWithIdentifier("MLCaseEditorTitleCell", forIndexPath: indexPath) as! MLCaseEditorTitleCell
            cell.setTitle(EN_MLCaseItem(rawValue: indexPath.row)?.title())
            return cell
        } else {
            let cell = tableView.dequeueReusableCellWithIdentifier("MLCaseEditorContentCell", forIndexPath: indexPath) as! MLCaseEditorContentCell
            cell.delegate = self
            if let index = EN_MLCaseItem(rawValue: indexPath.row), let item = caseModel.items[index] {
                cell.setItem(index, item: item)
            }
            return cell
        }
    }
}

extension MLCaseEditorViewController : MLCaseEditorContentCellDelegate {
    func onCellHeightChanged(newHeight: CGFloat, cell: UITableViewCell) {
        guard let index = tableView.indexPathForCell(cell) else {
            return
        }
        cellsHeight[index] = newHeight
        tableView.beginUpdates()
        tableView.endUpdates()
    }
}
