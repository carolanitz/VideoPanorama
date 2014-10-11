//
//  VideoDecompressor.m
//  VideoPanorama1
//
//  Created by Gernot Poetsch on 11.10.14.
//
//

#import "VideoDecompressor.h"

@interface VideoDecompressor () <NSStreamDelegate>

@end

@implementation VideoDecompressor

- (instancetype)initWithStream:(NSInputStream *)stream
{
    self = [super init];
    if (self) {
        _stream = [stream retain];
        _stream.delegate = self;
        [_stream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        [_stream open];
    }
    return self;
}

- (void)dealloc {
    [_stream release];
    NSLog(@"There goes the Decompressor");
    [super dealloc];
}

- (void)stream:(NSInputStream *)aStream handleEvent:(NSStreamEvent)eventCode
{
    NSLog(@"We got an Event: %d", eventCode);
    if (eventCode == NSStreamEventHasBytesAvailable) {
        NSMutableData *data = [[NSMutableData alloc] init];
        uint8_t buffer[1024];
        int length;
        while([aStream hasBytesAvailable]) {
            length = [aStream read:buffer maxLength:sizeof(buffer)];
            if (length > 0) {
                [data appendBytes:buffer length:length];
            }
        }
        NSLog(@"Read %d bytes", data.length);
        [data release];
    }

}

@end
