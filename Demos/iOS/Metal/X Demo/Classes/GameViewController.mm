/****************************************************************************
 * ==> Animated X model demo -----------------------------------------------*
 ****************************************************************************
 * Description : A bone based animation using DirectX (.x) model            *
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

#import "GameViewController.h"

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_Renderer.h"

// metal
#import "Renderer.h"

//---------------------------------------------------------------------------
// GameViewController
//---------------------------------------------------------------------------
@interface GameViewController()
{
    MTKView*           m_pView;
    CSR_MetalRenderer* m_pRenderer;
}
@end
//---------------------------------------------------------------------------
@implementation GameViewController
//---------------------------------------------------------------------------
- (void) viewDidLoad
{
    [super viewDidLoad];
    
    // initialize the view
    m_pView                 = (MTKView*)self.view;
    m_pView.device          = MTLCreateSystemDefaultDevice();
    m_pView.backgroundColor = UIColor.blackColor;
    
    // succeeded?
    if (!m_pView.device)
    {
        NSLog(@"Metal is not supported on this device");
        self.view = [[UIView alloc]initWithFrame:self.view.frame];
        return;
    }

    // link the Metal renderer to the view
    m_pRenderer = [[CSR_MetalRenderer alloc]initWithMetalKitView :m_pView];
    [m_pRenderer mtkView:m_pView drawableSizeWillChange:m_pView.bounds.size];
    m_pView.delegate = m_pRenderer;
}
//---------------------------------------------------------------------------
- (void) dealloc
{}
//---------------------------------------------------------------------------
- (void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view]window] == nil))
        self.view = nil;
    
    // todo: dispose here any resource that can be recreated
}
//---------------------------------------------------------------------------
- (BOOL) prefersStatusBarHidden
{
    return YES;
}
//---------------------------------------------------------------------------
@end
//---------------------------------------------------------------------------
