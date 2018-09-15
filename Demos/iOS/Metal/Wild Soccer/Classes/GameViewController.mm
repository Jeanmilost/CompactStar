/*****************************************************************************
 * ==> Wild soccer game demo ------------------------------------------------*
 *****************************************************************************
 * Description : Wild soccer game demo view controller                       *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#import "GameViewController.h"

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Collision.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_Renderer.h"
#include "CSR_Physics.h"
#include "CSR_Sound.h"

// classes
#import "GameLogic.h"

// metal
#import "Renderer.h"

//----------------------------------------------------------------------------
// GameViewController
//----------------------------------------------------------------------------
@interface GameViewController()
{
    MTKView*           m_pView;
    CSR_MetalRenderer* m_pRenderer;
    CSR_GameLogic*     m_pGameLogic;
}

/**
* Called when screen is long pressed
*@param pSender - recognizer that raised the event
*/
- (void) OnLongPress :(UIGestureRecognizer*)pSender;

@end
//----------------------------------------------------------------------------
@implementation GameViewController
//----------------------------------------------------------------------------
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

    // create the game logic controller
    m_pGameLogic = [[CSR_GameLogic alloc]init];

    // link the Metal renderer to the view
    m_pRenderer = [[CSR_MetalRenderer alloc]initWithMetalKitView :m_pView :m_pGameLogic];
    [m_pRenderer mtkView:m_pView drawableSizeWillChange:m_pView.bounds.size];
    m_pView.delegate = m_pRenderer;

    // add a long press gesture to the view
    UILongPressGestureRecognizer* pGestureRecognizer =
    [[UILongPressGestureRecognizer alloc]initWithTarget:self
                                                 action:@selector(OnLongPress:)];
    
    pGestureRecognizer.minimumPressDuration = 0;
    
    // add gesture recognizer to view
    [self.view addGestureRecognizer: pGestureRecognizer];
}
//----------------------------------------------------------------------------
- (void) dealloc
{}
//----------------------------------------------------------------------------
- (void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view]window] == nil))
        self.view = nil;
    
    // todo: dispose here any resource that can be recreated
}
//----------------------------------------------------------------------------
- (BOOL) prefersStatusBarHidden
{
    return YES;
}
//----------------------------------------------------------------------------
- (void)OnLongPress :(UIGestureRecognizer*)pSender
{
    const CGPoint touchPos = [pSender locationInView :nil];
    
    switch (pSender.state)
    {
        case UIGestureRecognizerStateBegan:
            // initialize the position. NOTE inverted because the screen is in landscape mode
            m_pGameLogic.m_pTouchOrigin->m_X   = touchPos.y;
            m_pGameLogic.m_pTouchOrigin->m_Y   = touchPos.x;
            m_pGameLogic.m_pTouchPosition->m_X = touchPos.y;
            m_pGameLogic.m_pTouchPosition->m_Y = touchPos.x;
            break;
            
        case UIGestureRecognizerStateChanged:
            // get the next position. NOTE inverted because the screen is in landscape mode
            m_pGameLogic.m_pTouchPosition->m_X = touchPos.y;
            m_pGameLogic.m_pTouchPosition->m_Y = touchPos.x;
            break;
            
        case UIGestureRecognizerStateEnded:
            if (m_pGameLogic.m_pTouchPosition->m_X == m_pGameLogic.m_pTouchOrigin->m_X && m_pGameLogic.m_pTouchPosition->m_Y == m_pGameLogic.m_pTouchOrigin->m_Y)
                [m_pGameLogic Shoot];
            
            // reset the position
            m_pGameLogic.m_pTouchOrigin->m_X   = 0;
            m_pGameLogic.m_pTouchOrigin->m_Y   = 0;
            m_pGameLogic.m_pTouchPosition->m_X = 0;
            m_pGameLogic.m_pTouchPosition->m_Y = 0;
            break;
            
        default:
            break;
    }
}
//----------------------------------------------------------------------------
@end
//----------------------------------------------------------------------------
