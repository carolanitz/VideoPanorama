//
//  ConnectedVideoPanoramaViewController.swift
//  VideoPanorama1
//
//  Created by Gernot Poetsch on 11.10.14.
//
//

import Foundation
import MultipeerConnectivity
import VideoToolbox

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
        //We don't support receiving data yet
    }
    
    func session(session: MCSession!, didReceiveStream stream: NSInputStream!, withName streamName: String!, fromPeer peerID: MCPeerID!) {
        println("Session received a \(streamName) stream from \(peerID.displayName)")
        capturePipeline.decompressor = VideoDecompressor(stream: stream)
    }
    
    func session(session: MCSession!, didStartReceivingResourceWithName resourceName: String!, fromPeer peerID: MCPeerID!, withProgress progress: NSProgress!) {
        //Nope, not supported. #hackday
    }
    
    func session(session: MCSession!, didFinishReceivingResourceWithName resourceName: String!, fromPeer peerID: MCPeerID!, atURL localURL: NSURL!, withError error: NSError!) {
        //See above
    }
    
    @IBAction func send() {
        if session.connectedPeers.count != 1 {
            println("We assume there is only one connected peer. It's a Hackday after all…")
            return
        }
        let peer = session.connectedPeers[0] as MCPeerID
        println("About to send the video to \(peer.displayName)")
        let outputStream = session.startStreamWithName("Video", toPeer: peer, error: nil)
        let compressor = VideoCompressor(size: CGSize(width: 1280, height: 720), stream: outputStream)
        self.capturePipeline.compressor = compressor
    }
    
    
}