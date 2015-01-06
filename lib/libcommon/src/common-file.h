/*
 * csps-suite libcommon - csps-suite common library
 *
 * Copyright (c) 2013-2015 FOXEL SA - http://foxel.ch
 * Please read <http://foxel.ch/license> for more information.
 *
 *
 * Author(s):
 *
 *      Nils Hamel <n.hamel@foxel.ch>
 *
 *
 * This file is part of the FOXEL project <http://foxel.ch>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Additional Terms:
 *
 *      You are required to preserve legal notices and author attributions in
 *      that material or in the Appropriate Legal Notices displayed by works
 *      containing it.
 *
 *      You are required to attribute the work as explained in the "Usage and
 *      Attribution" section of <http://foxel.ch/license>.
 */

    /*! \file   common-file.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *
     *  Generic operations on files and directories
     */

/* 
    Header - Include guard
 */

    # ifndef __LC_FILE__
    # define __LC_FILE__

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/* 
    Header - Includes
 */

    # include "common.h"

/* 
    Header - Preprocessor definitions
 */

    /* Define entity type */
    # define LC_FILE        0
    # define LC_DIRECTORY   1

/* 
    Header - Preprocessor macros
 */

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

/* 
    Header - Function prototypes
 */

    /*! \brief Directory entity enumeration
     *  
     *  Enumerates directory entities contained in the pointed directory. The
     *  function detects automatically if an enumeration is under way and
     *  returns, one by one, the name of the found entities. When enumeration
     *  is terminated, the function closes itself the directory handle.
     *
     *  \param  lcDirectory Directory to enumerates
     *  \param  lcName      String that recieve the entity name, appended to the
     *                      directory path
     *
     *  \return Returns code indicating enumeration status
     */

    int lc_file_enum ( char const * const lcDirectory, char * const lcName );

    /*! \brief Directory entity type detection
     *
     *  This function returns true if directory entity pointed by lcEntity is
     *  of type provided by lcType, false otherwise.
     *
     *  \param  lcEntity    Path to the entity
     *  \param  lcType      Type of the entity to check
     *
     *  \return Returns true if verification passed, false otherwise
     */

    int lc_file_detect ( char const * const lcEntity, int const lcType );

    /*! \brief File entity size detection
     *  
     *  This function returns the size, in bytes, of the provided file. If the
     *  file is not found or inaccessible, the function returns zero.
     *
     *  \param  lcFile Path to the file
     *
     *  \return Returns the size of file in bytes.
     */

    size_t lc_file_size ( char const * const lcFile );

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    } 
    # endif

/*
    Header - Include guard
 */

    # endif

