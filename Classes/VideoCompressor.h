//
//  VideoCompressor.h
//  VideoPanorama1
//
//  Created by Gernot Poetsch on 11.10.14.
//
//

#import <CoreMedia/CoreMedia.h>
#import <CoreVideo/CoreVideo.h>

#import <Foundation/Foundation.h>

@interface VideoCompressor : NSObject

- (instancetype)initWithSize:(CGSize)size stream:(NSOutputStream *)stream NS_DESIGNATED_INITIALIZER;

@property (strong, readonly) NSOutputStream *stream;

- (void)encodeFrame:(CMSampleBufferRef)sampleBuffer;


@end
