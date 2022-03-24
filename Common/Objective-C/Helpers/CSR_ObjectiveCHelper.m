/****************************************************************************
 * ==> CSR_ObjectiveCHelper ------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a helper class to simplify the usage  *
 *               of the CompactStar Engine with the Objective-C language    *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2022, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#include "CSR_ObjectiveCHelper.h"

// OSX
#import <Foundation/Foundation.h>

//----------------------------------------------------------------------------
// CSR_ObjectiveCHelper
//----------------------------------------------------------------------------
@implementation CSR_ObjectiveCHelper
//----------------------------------------------------------------------------
+ (void) ResourceToFileName :(NSString*)resourceName :(NSString*)resourceType :(char**)pFileName
{
    // get the file path from resources
    const NSString* pFilePath = [[NSBundle mainBundle]pathForResource:resourceName
                                                               ofType:resourceType];

    // convert the path data to UTF8 string
    const NSData* pStrData =
            [pFilePath dataUsingEncoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF8)];

    // get the file name
    *pFileName = (char*)calloc(pStrData.length + 1, sizeof(char));
    memcpy(*pFileName, pStrData.bytes, pStrData.length);
    (*pFileName)[pStrData.length] = '\0';
}
//----------------------------------------------------------------------------
@end
//----------------------------------------------------------------------------
