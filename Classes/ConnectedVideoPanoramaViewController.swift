//
//  ConnectedVideoPanoramaViewController.swift
//  VideoPanorama1
//
//  Created by Gernot Poetsch on 11.10.14.
//
//

import Foundation
import MultipeerConnectivity

class ConnectedVideoPanoramaViewController: VideoPanoramaViewController, MCAdvertiserAssistantDelegate, MCBrowserViewControllerDelegate, MCSessionDelegate {
    
    let serviceType = "VideoPanorama"
    var peerID: MCPeerID
    var session: MCSession
    var advertiser: MCAdvertiserAssistant
    
    required init(coder aDecoder: NSCoder) {
        peerID = MCPeerID(displayName: UIDevice.currentDevice().name)
        session = MCSession(peer: peerID)
        advertiser = MCAdvertiserAssistant(serviceType: serviceType, discoveryInfo: nil, session: session)
        super.init(coder: aDecoder)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        session.delegate = self
        advertiser.delegate = self
        println("Starting Advertiser")
        advertiser.start()
    }
    
    @IBAction func connect() {
        let browser = MCBrowserViewController(serviceType: serviceType, session: session)
        browser.delegate = self
        presentViewController(browser, animated: true, completion: nil)
    }
    
    func browserViewControllerDidFinish(browserViewController: MCBrowserViewController!) {
        dismissViewControllerAnimated(true, completion: nil)
    }
    
    func browserViewControllerWasCancelled(browserViewController: MCBrowserViewController!) {
        dismissViewControllerAnimated(true, completion: nil)
    }
    
    func session(session: MCSession!, peer peerID: MCPeerID!, didChangeState state: MCSessionState) {
        println("Session changed State: \(state)")
    }
    
    func session(session: MCSession!, didReceiveData data: NSData!, fromPeer peerID: MCPeerID!) {
        println("Got camera settings")
        var myDictionary: NSDictionary = NSKeyedUnarchiver.unarchiveObjectWithData(data) as NSDictionary
        self.capturePipeline.videoDevice.lockForConfiguration(nil)
        var whiteBalanceGainData = myDictionary.objectForKey("whiteBalance") as NSData
        var whiteBalanceGain:AVCaptureWhiteBalanceGains = AVCaptureWhiteBalanceGains(redGain: 0,greenGain: 0,blueGain: 0)
        whiteBalanceGainData.getBytes(&whiteBalanceGain, length: sizeof(AVCaptureWhiteBalanceGains))
        self.capturePipeline.videoDevice.setWhiteBalanceModeLockedWithDeviceWhiteBalanceGains(whiteBalanceGain, completionHandler: nil)


    }
    
    func session(session: MCSession!, didReceiveStream stream: NSInputStream!, withName streamName: String!, fromPeer peerID: MCPeerID!) {
        println("Session received a \(streamName) stream from \(peerID.displayName)")
    }
    
    func session(session: MCSession!, didStartReceivingResourceWithName resourceName: String!, fromPeer peerID: MCPeerID!, withProgress progress: NSProgress!) {
        //Nope, not supported. #hackday
    }
    
    func session(session: MCSession!, didFinishReceivingResourceWithName resourceName: String!, fromPeer peerID: MCPeerID!, atURL localURL: NSURL!, withError error: NSError!) {
        //See above
    }
    
    @IBAction func send() {
        self.capturePipeline.startRunning()
        var timer = NSTimer.scheduledTimerWithTimeInterval(1.0, target: self, selector: Selector("sendSettings"), userInfo: nil, repeats: false)

//        if session.connectedPeers.count != 1 {
//            println("We assume there is only one connected peer. It's a Hackday after all…")
//            return
//        }
//        let peer = session.connectedPeers[0] as MCPeerID
//        println("About to send the video to \(peer.displayName)")
//        let stream = session.startStreamWithName("Video", toPeer: peer, error: nil)

    }

    func sendSettings () {
        self.capturePipeline.videoDevice.lockForConfiguration(nil)
        self.capturePipeline.videoDevice.whiteBalanceMode = .Locked
        self.capturePipeline.videoDevice.exposureMode = .Locked
        self.capturePipeline.videoDevice.setFocusModeLockedWithLensPosition(AVCaptureLensPositionCurrent, completionHandler: { (CMTime) -> Void in
            var dict = NSMutableDictionary()
            var whiteBalance = self.capturePipeline.videoDevice.deviceWhiteBalanceGains
            var data = NSData(bytes:&whiteBalance, length: sizeof(AVCaptureWhiteBalanceGains))
            dict.setObject(data, forKey: "whiteBalance")
            var myData = NSKeyedArchiver.archivedDataWithRootObject(dict)
            self.session.sendData(myData, toPeers: self.session.connectedPeers, withMode: .Reliable, error: nil)
        })
        self.capturePipeline.videoDevice.unlockForConfiguration()
        println("settings")
    }
    
    
}