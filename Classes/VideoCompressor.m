//
//  VideoCompressor.m
//  VideoPanorama1
//
//  Created by Gernot Poetsch on 11.10.14.
//
//

@import VideoToolbox;
@import CoreMedia;
@import AVFoundation;

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
        VTSessionSetProperty(session, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);
        VTSessionSetProperty(session, kVTCompressionPropertyKey_AllowFrameReordering, kCFBooleanFalse);
        self.session = session;
        VTCompressionSessionPrepareToEncodeFrames(self.session);
    }
    return self;
}

- (void)dealloc {
    VTCompressionSessionCompleteFrames(self.session, kCMTimeInvalid);
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
    VideoCompressor *compressor = sourceFrameRefCon;
    CMBlockBufferRef formatBuffer = NULL;
    CMVideoFormatDescriptionCopyAsBigEndianImageDescriptionBlockBuffer(NULL,
                                                                       CMSampleBufferGetFormatDescription(sampleBuffer),
                                                                       CFStringGetSystemEncoding(),
                                                                       NULL,
                                                                       &formatBuffer);
    size_t formatLength;
    char* formatData;
    CMBlockBufferGetDataPointer(formatBuffer, 0, NULL, &formatLength, &formatData);
    CMTime time = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);
    CMBlockBufferRef dataBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
    size_t dataLength;
    char* dataData; //haha
    CMBlockBufferGetDataPointer(dataBuffer, 0, NULL, &dataLength, &dataData);
    NSDictionary *dict = @{@"format": [NSData dataWithBytesNoCopy:formatData length:formatLength],
                           @"data": [NSData dataWithBytesNoCopy:dataData length:dataLength],
                           @"time": [NSValue valueWithCMTime:time]};
    NSData *dataWeActuallySend = [NSKeyedArchiver archivedDataWithRootObject:dict];
    [compressor.stream write:dataWeActuallySend.bytes maxLength:dataWeActuallySend.length];
}
