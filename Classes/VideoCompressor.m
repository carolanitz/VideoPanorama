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

@interface VideoCompressor () <NSStreamDelegate>
@property VTCompressionSessionRef session;
@property NSData *frameDataToSend;
@end

@implementation VideoCompressor

- (instancetype)initWithSize:(CGSize)size stream:(NSOutputStream *)stream;
{
    self = [super init];
    if (self) {
        _stream = [stream retain];
        _stream.delegate = self;
        [_stream scheduleInRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
        [_stream open];
        
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

- (void)stream:(NSStream *)aStream handleEvent:(NSStreamEvent)eventCode {
    if (eventCode == NSStreamEventHasSpaceAvailable) {
        [self writeDataIfSpaceAvailable];
    }
}

- (void)writeDataIfSpaceAvailable {
    if (self.frameDataToSend && self.stream.hasSpaceAvailable) {
        NSInteger bytesWritten = [self.stream write:self.frameDataToSend.bytes maxLength:self.frameDataToSend.length];
        NSLog(@"Sent %d bytes of %d", bytesWritten, self.frameDataToSend.length);
        self.frameDataToSend = nil;
    }
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
    NSDictionary *dict = @{@"format": [NSData dataWithBytes:formatData length:formatLength],
                           @"data": [NSData dataWithBytes:dataData length:dataLength],
                           @"time": [(NSDictionary *)CMTimeCopyAsDictionary(time, NULL) autorelease]};
    dispatch_async(dispatch_get_main_queue(), ^{
        compressor.frameDataToSend = [NSKeyedArchiver archivedDataWithRootObject:dict];
        [compressor writeDataIfSpaceAvailable];
    });
}
