//
//  ConnectedVideoPanoramaViewController.swift
//  VideoPanorama1
//
//  Created by Gernot Poetsch on 11.10.14.
//
//

import Foundation
import MultipeerConnectivity
import CoreMedia

class ConnectedVideoPanoramaViewController: VideoPanoramaViewController, MCAdvertiserAssistantDelegate, MCBrowserViewControllerDelegate, MCSessionDelegate {
   
   let serviceType = "VideoPanorama"
   var peerID: MCPeerID
   var advertiser: MCAdvertiserAssistant
   
   required init(coder aDecoder: NSCoder) {
      peerID = MCPeerID(displayName: UIDevice.currentDevice().name)
      networkSession = MCSession(peer: peerID)
      advertiser = MCAdvertiserAssistant(serviceType: serviceType, discoveryInfo: nil, session: networkSession)
      super.init(coder: aDecoder)
    }
   
   
    override func viewDidLoad() {
        super.viewDidLoad()
        networkSession.delegate = self
        advertiser.delegate = self
        println("Starting Advertiser")
        advertiser.start()
        navigationController?.navigationBar.barTintColor = UIColor(red: 43/255, green: 180/255, blue: 172/255, alpha: 1)
    }
    
    @IBAction func connect() {
        let browser = MCBrowserViewController(serviceType: serviceType, session: networkSession)
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
      if (isSender)
      {
         println("GOt HUUUUGE Data");
         sendDataToMatcher(data);
         
         
         let datab = "abc".dataUsingEncoding(NSUTF8StringEncoding)

         networkSession.sendData(datab, toPeers: networkSession.connectedPeers, withMode: .Reliable, error: nil)
      }
      else
      {
         if (isStarted)
         {
            isReadyForData = true;
         }
         else
         {
         isStarted = true;
        println("Got camera settings")
        var myDictionary: NSDictionary = NSKeyedUnarchiver.unarchiveObjectWithData(data) as NSDictionary
        self.capturePipeline.videoDevice.lockForConfiguration(nil)

        var whiteBalanceGainData = myDictionary.objectForKey("whiteBalance") as NSData
        var whiteBalanceGain:AVCaptureWhiteBalanceGains = AVCaptureWhiteBalanceGains(redGain: 0,greenGain: 0,blueGain: 0)
        whiteBalanceGainData.getBytes(&whiteBalanceGain, length: sizeof(AVCaptureWhiteBalanceGains))
        self.capturePipeline.videoDevice.setWhiteBalanceModeLockedWithDeviceWhiteBalanceGains(whiteBalanceGain, completionHandler: nil)

        var focusData = myDictionary.objectForKey("focus") as NSData
        var focus:Float = Float(0.0)
        focusData.getBytes(&focus, length: sizeof(Float))
        self.capturePipeline.videoDevice.setFocusModeLockedWithLensPosition(focus, completionHandler: nil)

        var ISOData = myDictionary.objectForKey("ISO") as NSData
        var iso:Float = Float(0.0)
        ISOData.getBytes(&iso, length: sizeof(Float))

        var exposure = CMTimeMakeFromDictionary(myDictionary.objectForKey("exposure") as NSDictionary)
        self.capturePipeline.videoDevice.setExposureModeCustomWithDuration(exposure, ISO: iso, completionHandler: nil)

        self.capturePipeline.videoDevice.unlockForConfiguration()
         }
      }
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
      isSender = true;
      isStarted = true;
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

            var focus = self.capturePipeline.videoDevice.lensPosition
            data = NSData(bytes:&focus, length: sizeof(Float))
            dict.setObject(data, forKey: "focus")

            var exposure = CMTimeCopyAsDictionary(self.capturePipeline.videoDevice.exposureDuration, kCFAllocatorDefault)
            dict.setObject(exposure, forKey: "exposure")

            var iso = self.capturePipeline.videoDevice.ISO
            data = NSData(bytes:&iso, length: sizeof(Float))
            dict.setObject(data, forKey: "ISO")

            var myData = NSKeyedArchiver.archivedDataWithRootObject(dict)
            networkSession.sendData(myData, toPeers: networkSession.connectedPeers, withMode: .Reliable, error: nil)
        })
        self.capturePipeline.videoDevice.unlockForConfiguration()
        println("settings")
    }
    
    
}