//
//  VideoCompressor.m
//  VideoPanorama1
//
//  Created by Gernot Poetsch on 11.10.14.
//
//

@import VideoToolbox;

#import "VideoCompressor.h"

static void outputCallback (void *outputCallbackRefCon,
                            void *sourceFrameRefCon,
                            OSStatus status,
                            VTEncodeInfoFlags infoFlags,
                            CMSampleBufferRef sampleBuffer);

@interface VideoCompressor ()
@property VTCompressionSessionRef session;
@end

@implementation VideoCompressor

- (instancetype)initWithSize:(CGSize)size
{
    self = [super init];
    if (self) {
        VTCompressionSessionRef session;
        
        OSStatus status = VTCompressionSessionCreate(NULL, //Allocator
                                                     size.width,//Width
                                                     size.height,//height
                                                     kCMVideoCodecType_H264, //Video Codec Type
                                                     nil, //Encoder Specification dict
                                                     nil, //Source Image Buffer Attributes Dict
                                                     NULL, //Compressed Data Allocator
                                                     outputCallback, //Compression Output Callback
                                                     nil, //Output Callback
                                                     &session);
        NSLog(@"Created Session: Status is %d", (int)status);
        self.session = session;
        VTCompressionSessionPrepareToEncodeFrames(self.session);
    }
    return self;
}

- (void)dealloc {
    VTCompressionSessionInvalidate(self.session);
    CFRelease(self.session);
    [super dealloc];
}

-(void)encodeFrame:(CMSampleBufferRef)sampleBuffer {
    VTCompressionSessionEncodeFrame(self.session,
                                    CMSampleBufferGetImageBuffer(sampleBuffer),
                                    CMSampleBufferGetPresentationTimeStamp(sampleBuffer),
                                    kCMTimeInvalid,
                                    nil,
                                    self,
                                    NULL);
}

@end

static void outputCallback (void *outputCallbackRefCon,
                            void *sourceFrameRefCon,
                            OSStatus status,
                            VTEncodeInfoFlags infoFlags,
                            CMSampleBufferRef sampleBuffer) {
    NSLog(@"Got some Output");
}
