/****************************************************************************
 * ==> CSR_LevelFile -------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the functions required to read and    *
 *               write the level from/to a file                             *
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

#ifndef CSR_LevelFileH
#define CSR_LevelFileH

// classes
#include "CSR_Level.h"

/**
* Basic class to read and write a level file
*@author Jean-Milost Reymond
*/
class CSR_LevelFile
{
    public:
        CSR_LevelFile();
        virtual ~CSR_LevelFile();

        /**
        * Loads the level
        *@param fileName - level file name
        *@param[in, out] level - level to load
        *@return true on success, otherwise false
        */
        virtual bool Load(const std::string& fileName, CSR_Level& level) = 0;

        /**
        * Saves the level
        *@param fileName - level file name
        *@param level - level to save
        *@return true on success, otherwise false
        */
        virtual bool Save(const std::string& fileName, const CSR_Level& level) const = 0;
};
#endif
