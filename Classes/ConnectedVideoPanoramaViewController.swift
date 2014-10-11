//
//  ConnectedVideoPanoramaViewController.swift
//  VideoPanorama1
//
//  Created by Gernot Poetsch on 11.10.14.
//
//

import Foundation
import MultipeerConnectivity

class ConnectedVideoPanoramaViewController: VideoPanoramaViewController, MCAdvertiserAssistantDelegate, MCBrowserViewControllerDelegate {
    
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
    
    @IBAction func send() {
        if session.connectedPeers.count != 1 {
            println("We assume there is only one connected peer. It's a Hackday after all…")
            return
        }
        let peer = session.connectedPeers[0] as MCPeerID
        println("About to send the video to \(peer.displayName)")
    }
}