//
//  VideoDecompressor.h
//  VideoPanorama1
//
//  Created by Gernot Poetsch on 11.10.14.
//
//

#import <Foundation/Foundation.h>

@interface VideoDecompressor : NSObject

- (instancetype)initWithStream:(NSInputStream *)stream;

@property (retain, readonly) NSInputStream *stream;

@end
