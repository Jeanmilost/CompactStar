/****************************************************************************
 * ==> CSR_ObjectiveCHelper ------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a helper class to simplify the usage  *
 *               of the CompactStar Engine with the Objective-C language    *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2019, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

// OSX
#import <Foundation/NSString.h>

/**
* Helper class to facilitate the usage of the CompactStar Engine with the Objective-C language
*@author Jean-Milost Reymond
*/
@interface CSR_ObjectiveCHelper : NSObject
{}

/**
* Get the path of a resource
*@param resourceName - resource name to get
*@param resourceType - resource type (generally the same as the extension)
*@param[out] pFileName - file name
*@note The file name must be freed by the caller when useless
*/
+ (void) ResourceToFileName :(NSString*)resourceName :(NSString*)resourceType :(char**)pFileName;

@end
