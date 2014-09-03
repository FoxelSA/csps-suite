/*
 * csps-suite - CSPS library front-end suite
 *
 * Copyright (c) 2013-2014 FOXEL SA - http://foxel.ch
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

    /*! \file   csps-view.h
     *  \author Nils Hamel (n.hamel@foxel.ch)
     *   
     *  Software main header
     */

    /*! \mainpage csps-view
     *
     *  Front-end suite for CSPS library.
     */

/* 
    Header - Include guard
 */

    # ifndef __CS_VIEW__
    # define __CS_VIEW__

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/* 
    Header - Includes
 */

    # include "csps-view-features.h"
    # include "csps-view-event.h"
    # include "csps-view-scene.h"

/* 
    Header - Preprocessor definitions
 */

    /* Standard help */
    # define CS_HELP "Usage summary :\n"         \
    "  csps-view [Arguments] [Parameters] ...\n" \
    "Short arguments and parameters summary :\n" \
    "  -r Record root directory\n"               \
    "csps-view - csps-suite\n"                   \
    "Copyright (c) 2013-2014 FOXEL SA\n"

    /* Define standard types */
    # define CS_NULL        0
    # define CS_STRING      1
    # define CS_CHAR        2
    # define CS_SHORT       3
    # define CS_INT         4
    # define CS_LONG        5
    # define CS_LLONG       6
    # define CS_UCHAR       7
    # define CS_USHORT      8
    # define CS_UINT        9
    # define CS_ULONG       10
    # define CS_ULLONG      11
    # define CS_FLOAT       12
    # define CS_DOUBLE      13

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

    /*! \brief Software main function
     *  
     *  The main function simply initialize graphic library and launches
     *  event callback function. A infinite loop is performed until escape
     *  key is pressed.
     *  
     *  \param argc Standard main parameter
     *  \param argv Standard main parameter
     */

    int main ( int argc, char ** argv );

    /*! \brief Search agrument position in argv
     *  
     *  This function search in the argv string array the position of the argument
     *  defined through ltag/stag and return the index of the corresponding parameter
     *  in argv.
     *  
     *  \param argc Standard main parameter
     *  \param argv Standard main parameter
     *  \param ltag Long-form argument string (--argument)
     *  \param stag Short-form argument string (-a)
     *  \return Index of parameter in argv
     */

    int  cs_stda ( int argc, char ** argv, const char * const ltag, const char * const stag );

    /*! \brief Parameter reader in argv
     *  
     *  This function interpret the parameter in the desired type and return it through
     *  the param variable. The argi variable is typically set using stda function. If
     *  argi is set to CS_NULL, the function does nothing.
     *  
     *  \param argi Index of the parameter in argv
     *  \param argv Standard main parameter
     *  \param param Pointer to the variable that recieve the parameter
     *  \param type Type to use for parameter interpretation
     */

    void cs_stdp ( int argi, char ** argv, void * param, int type );

    /* Ok, well, hum, that's a prototype without being one ... */
    extern void glutMainLoopEvent ( void );

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


