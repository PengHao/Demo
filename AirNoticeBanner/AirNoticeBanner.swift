//
//  AirNoticeBanner.swift
//  AirNoticeBanner
//
//  Created by PengHao on 16/10/8.
//  Copyright © 2016年 WolfPeng. All rights reserved.
//

import UIKit
import Foundation
import CoreGraphics

class AirNoticeBanner: UIView {
    typealias AirNoticeBannerClick = ()->Void
    private let bgImageView: UIImageView = UIImageView(frame: CGRect.zero)
    private let contentLabel: UILabel = UILabel(frame: CGRect.zero)
    private let iconImageView: UIImageView = UIImageView(frame: CGRect.zero)
    private let btn: UIButton = UIButton(type: UIButtonType.Custom)
    private static var banner = AirNoticeBanner(frame: CGRect.zero)
    private var clickBlock : AirNoticeBannerClick?
    
    private struct _Args {
        let attributeString: NSAttributedString!
        let icon: UIImage?
        let btnTitle: NSAttributedString?
        let btnImage: UIImage?
        let onTapd:AirNoticeBannerClick?
        init(attributeString: NSAttributedString, icon: UIImage? = nil, btnTitle: NSAttributedString? = nil, btnImage: UIImage? = nil, onTapd:AirNoticeBannerClick? = nil) {
            self.attributeString = attributeString
            self.icon = icon
            self.btnTitle = btnTitle
            self.btnImage = btnImage
            self.onTapd = onTapd
        }
    }
    private var isShowing = false {
        didSet {
            if !isShowing {
                if let args = bannerQueue.first {
                    _show(args)
                }
            }
        }
    }
    
    private var status: Int8 = 0
    
    private enum BannerStatus: Int8 {
        case Prepare = 0
        case Showing = 1
        
        func bitValue() -> Int8 {
            return 1 << rawValue
        }
    }
    
    private var prepare = false {
        didSet {
            if prepare {
                if !isShowing {
                    if let args = bannerQueue.first {
                        _show(args)
                    }
                }
            }
        }
    }
    
    private var bannerQueue = [_Args]()
    
    private func addQueue(args: _Args) {
        bannerQueue.append(args)
        prepare = true
    }
    
    private override init(frame: CGRect) {
        super.init(frame: frame)
        addSubview(bgImageView)
        addSubview(contentLabel)
        addSubview(iconImageView)
        addSubview(btn)
        btn.addTarget(self, action: #selector(onTapd(_:)), forControlEvents: .TouchUpInside)
        bgImageView.image = UIImage(named: "bannerBG")?.resizableImageWithCapInsets(UIEdgeInsetsMake(29, 29, 29, 29))
    }
    
    static func show(attributeString: NSAttributedString, icon: UIImage? = nil, btnTitle: NSAttributedString? = nil, btnImage: UIImage? = nil, onTapd:AirNoticeBannerClick? = nil) {
        let args = _Args(attributeString: attributeString, icon: icon, btnTitle: btnTitle, btnImage: btnImage, onTapd: onTapd)
        banner.addQueue(args)
    }
    
    private func _show (args: _Args) {
        bannerQueue.removeFirst()
        clickBlock = args.onTapd
        
        iconImageView.image = args.icon
        iconImageView.hidden = args.icon == nil
        
        btn.setImage(args.btnImage, forState: .Normal)
        btn.setAttributedTitle(args.btnTitle, forState: .Normal)
        btn.hidden = args.btnTitle == nil && args.btnImage == nil
        btn.sizeToFit()
        
        let edgeX: CGFloat = 11.0
        let edgeY: CGFloat = 11.0
        let iconWidth = iconImageView.hidden ? 0 : args.icon!.size.width
        let iconHeight = iconImageView.hidden ? 0 : args.icon!.size.height
        let btnWidth = btn.hidden ? 0 : btn.frame.width
        guard let __window = UIApplication.sharedApplication().keyWindow else {
            return
        }
        let kownWidth = edgeX + edgeX + iconWidth + btnWidth + 16.0
        let canRenderWidth = __window.bounds.width - kownWidth
        let __maxSize = CGSize(width: canRenderWidth, height: CGFloat.max)
        let renderRect = args.attributeString.boundingRectWithSize(__maxSize, options: .UsesLineFragmentOrigin, context: nil)
        contentLabel.attributedText = args.attributeString
        
        let totoalW = renderRect.size.width + kownWidth
        let totoalH = renderRect.size.height + 16 + edgeY + edgeY
        frame = CGRectMake(0, -totoalH, totoalW, totoalH)
        bgImageView.frame = CGRectMake(-8, 4, totoalW + 16, 57)
        
        iconImageView.frame = CGRect(origin: CGPoint(x: edgeX, y: (totoalH - iconHeight)/2), size: CGSize(width: iconWidth, height: iconHeight))
        contentLabel.frame = CGRect(origin: CGPoint(x: iconImageView.frame.origin.x + iconImageView.frame.size.width, y: edgeY), size: CGSize(width: renderRect.width + 16, height: renderRect.height + 16))
        contentLabel.textAlignment = .Center
        btn.frame = CGRect(origin: CGPoint(x: contentLabel.frame.origin.x + contentLabel.frame.size.width, y: (totoalH - btn.bounds.height)/2), size: btn.frame.size)
        __show()
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    func onTapd(btn: UIButton?) {
        clickBlock?()
    }
    
    private func __show() {
        if let w = UIApplication.sharedApplication().keyWindow {
            center = CGPoint(x: w.screen.bounds.size.width/CGFloat(2), y: center.y)
            w.addSubview(self)
        } else {
            return
        }
        isShowing = true
        UIView.animateWithDuration(0.5) {
            self.center = CGPoint(x: self.center.x, y: self.bounds.size.height / 2 + 20)
        }
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, Int64(NSEC_PER_SEC) * 2), dispatch_get_main_queue()) {
            UIView.animateWithDuration(0.5, animations: {
                self.center = CGPoint(x: self.center.x, y: -self.bounds.size.height)
                }, completion: { (finished) in
                    self.dismiss()
            })
        }
    }
    
    func dismiss() {
        removeFromSuperview()
        isShowing = false
    }
}
