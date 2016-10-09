//
//  Assert+MLImage.swift
//  MedLinker
//
//  Created by PengHao on 16/6/20.
//  Copyright © 2016年 MedLinker. All rights reserved.
//

import Foundation
import AssetsLibrary
import UIKit
extension ALAsset {
//    fullScreenImage
    
    
    func originalOrientationImage() -> UIImage? {
        if let ori = self.valueForProperty(ALAssetPropertyOrientation).integerValue, let orient = UIImageOrientation(rawValue: ori) {
            let def = defaultRepresentation()
            return UIImage(CGImage: def.fullResolutionImage().takeUnretainedValue(), scale: CGFloat(def.scale()), orientation: orient)
        } else {
            return nil
        }
    }
    func smallOrientationImage() -> UIImage? {
        let def = defaultRepresentation()
        return UIImage(CGImage: def.fullScreenImage().takeUnretainedValue())
    }
}
