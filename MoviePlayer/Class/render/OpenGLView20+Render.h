//
//  OpenGLView20+Render.h
//  MoviePlayer
//
//  Created by 刘杨 on 2020/3/16.
//  Copyright © 2020 liu. All rights reserved.
//

#import "OpenGLView20.h"

@interface OpenGLView20 (Render) 
- (CGRect)glViewRect;
- (void)render:(FokFrame *)frame;
@end
