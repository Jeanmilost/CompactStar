/****************************************************************************
 * ==> CSR_Physics ---------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides physics algorithms and structures     *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2018, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#ifndef CSR_PhysicsH
#define CSR_PhysicsH

// std
#include <stddef.h>

// compactStar engine
#include "CSR_Geometry.h"

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------
#define M_CSR_Gravitation 9.81f

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

/**
* Physical body
*/
typedef struct
{
    CSR_Vector3 m_InitialForce; // initial (or previous) force applied to the particle
    float       m_Mass;         // particle mass
} CSR_Body;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Body functions
        //-------------------------------------------------------------------

        /**
        * Creates a body
        *@return newly created body, 0 on error
        *@note The body must be released when no longer used, see csrBodyRelease()
        */
        CSR_Body* csrBodyCreate(void);

        /**
        * Releases a body
        *@param[in, out] pBody - body to release
        */
        void csrBodyRelease(CSR_Body* pBody);

        /**
        * Initializes a body
        *@param[in, out] pBody - body to initialize
        */
        void csrBodyInit(CSR_Body* pBody);

        //-------------------------------------------------------------------
        // Physics functions
        //-------------------------------------------------------------------

        /**
        * Applies the gravitation force to a body
        *@param pBody - body to which the gravitation force should be applied
        *@param gravitation - the gravitation force in Newtons
        *@param[out] pR - resulting force in Newtons
        */
        void csrPhysicsApplyGravitation(const CSR_Body* pBody, float gravitation, CSR_Vector3* pR);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Physics.c"
#endif

#endif
