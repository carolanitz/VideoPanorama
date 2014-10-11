//
//  VideoCompressor.h
//  VideoPanorama1
//
//  Created by Gernot Poetsch on 11.10.14.
//
//

@import CoreMedia;
@import CoreVideo;

#import <Foundation/Foundation.h>

@interface VideoCompressor : NSObject

@property (strong) NSOutputStream *stream;

- (void)encodeFrame:(CMSampleBufferRef)sampleBuffer;

@end
