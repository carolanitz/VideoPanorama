
/*
     File: VideoPanoramaOpenCVRenderer.mm
 Abstract: n/a
  Version: 2.1
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
 Inc. ("Apple") in consideration of your agreement to the following
 terms, and your use, installation, modification or redistribution of
 this Apple software constitutes acceptance of these terms.  If you do
 not agree with these terms, please do not use, install, modify or
 redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc. may
 be used to endorse or promote products derived from the Apple Software
 without specific prior written permission from Apple.  Except as
 expressly stated in this notice, no other rights or licenses, express or
 implied, are granted by Apple herein, including but not limited to any
 patent rights that may be infringed by your derivative works or by other
 works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2014 Apple Inc. All Rights Reserved.
 
 */

#include <opencv2/highgui/highgui.hpp>
#import "VideoPanoramaOpenCVRenderer.h"

// To build OpenCV into the project:
//	- Download opencv2.framework for iOS
//	- Insert framework into project's Frameworks group
//	- Make sure framework is included under the target's Build Phases -> Link Binary With Libraries.
#import <opencv2/opencv.hpp>
#import "VideoPanoramaAppDelegate.h"
#import "Globals.h"

@implementation VideoPanoramaOpenCVRenderer{

    BOOL notSetup;
}

#pragma mark VideoPanoramaRenderer

- (BOOL)operatesInPlace
{
	return YES;
}

- (FourCharCode)inputPixelFormat
{
    notSetup = YES;
	return kCVPixelFormatType_32BGRA;
}

- (void)prepareForInputWithFormatDescription:(CMFormatDescriptionRef)inputFormatDescription outputRetainedBufferCountHint:(size_t)outputRetainedBufferCountHint
{
	// nothing to do, we are stateless
}

- (void)reset
{
	// nothing to do, we are stateless
}

- (CVPixelBufferRef)copyRenderedPixelBuffer:(CVPixelBufferRef)pixelBuffer motion:(CMDeviceMotion *)motion
{
	CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
	
	unsigned char *base = (unsigned char *)CVPixelBufferGetBaseAddress( pixelBuffer );
	size_t width = CVPixelBufferGetWidth( pixelBuffer );
	size_t height = CVPixelBufferGetHeight( pixelBuffer );
	size_t stride = CVPixelBufferGetBytesPerRow( pixelBuffer );
	size_t extendedWidth = stride / sizeof( uint32_t ); // each pixel is 4 bytes/32 bits
	
	// Since the OpenCV Mat is wrapping the CVPixelBuffer's pixel data, we must do all of our modifications while its base address is locked.
	// If we want to operate on the buffer later, we'll have to do an expensive deep copy of the pixel data, using memcpy or Mat::clone().
	
	// Use extendedWidth instead of width to account for possible row extensions (sometimes used for memory alignment).
	// We only need to work on columms from [0, width - 1] regardless.
   cv::Mat bgraImage = cv::Mat( (int)height, (int)extendedWidth, CV_8UC4, base );

   if (!isSender && isStarted)
   {
      std::vector<uchar> buffer;
      std::vector<int> params;
      params.push_back(CV_IMWRITE_JPEG_QUALITY);
      params.push_back(20);
      cv::imencode(".jpg", bgraImage, buffer, params);
      [networkSession sendData:[NSData dataWithBytes:&buffer[0] length:buffer.size()] toPeers:[networkSession connectedPeers] withMode:MCSessionSendDataReliable error:nil];
   }

   {
    cv::Vec4f motionvector;
    motionvector = cv::Vec4f(motion.attitude.quaternion.x, motion.attitude.quaternion.y,motion.attitude.quaternion.z,motion.attitude.quaternion.w);
    [VideoPanoramaAppDelegate sharedDelegate].getMatcher->updateImage1(bgraImage.clone(), motionvector, 0);
   }

	CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
	
	return (CVPixelBufferRef)CFRetain( pixelBuffer );
}

@end
