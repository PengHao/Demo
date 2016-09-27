//
//  UIFont+MLFont.swift
//  MedLinker
//
//  Created by PengHao on 16/8/9.
//  Copyright © 2016年 MedLinker. All rights reserved.
//

import Foundation
import UIKit
private let MLFontName = "FZLanTingHei-R-GBK"
extension UIFont{
    class func MLFont(size: CGFloat) -> UIFont {
        return UIFont(name: MLFontName, size: size) ?? UIFont.systemFontOfSize(size)
    }
}