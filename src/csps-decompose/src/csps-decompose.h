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

    /*! \file   csps-append.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *   
     *  Software main header
     */

    /*! \mainpage csps-decompose
     *
     *  \section csps-suite
     *  \section _ CSPS library front-end suite
     *
     *  Not documented yet.
     *
     *  \section Documentation
     *
     *  Not documented yet.
     *
     *  \section Copyright
     * 
     *  Copyright (c) 2013-2014 FOXEL SA - http://foxel.ch \n
     *  This program is part of the FOXEL project <http://foxel.ch>.
     *  
     *  Please read the COPYRIGHT.md file for more information.
     *
     *  \section License
     *
     *  This program is licensed under the terms of the GNU Affero General Public
     *  License v3 (GNU AGPL), with two additional terms. The content is licensed
     *  under the terms of the Creative Commons Attribution-ShareAlike 4.0
     *  International (CC BY-SA) license.
     *
     *  You must read <http://foxel.ch/license> for more information about our
     *  Licensing terms and our Usage and Attribution guidelines.
     */

/* 
    Header - Include guard
 */

    # ifndef __CS_DECOMPOSE__
    # define __CS_DECOMPOSE__

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/* 
    Header - Includes
 */

    # include <stdio.h>
    # include <stdlib.h>
    # include <time.h>
    # include <string.h>
    # include <dirent.h>
    # include <sys/stat.h>
    # include <csps-all.h>

/* 
    Header - Preprocessor definitions
 */

    /* Standard help */
    # define CS_HELP "Usage summary :\n"                                \
    "  csps-decompose [Arguments] [Parameters] ...\n"                   \
    "Short arguments and parameters summary :\n"                        \
    "  -r Directory path containing the logs file to decompose\n"       \
    "  -d Directory path in which decomposed logs files are exported\n" \
    "csps-append - csps-suite\n"                                        \
    "Copyright (c) 2013-2014 FOXEL SA\n"

    /* Define standard types */
    # define CS_NULL            0
    # define CS_STRING          1
    # define CS_CHAR            2
    # define CS_SHORT           3
    # define CS_INT             4
    # define CS_LONG            5
    # define CS_LLONG           6
    # define CS_UCHAR           7
    # define CS_USHORT          8
    # define CS_UINT            9
    # define CS_ULONG           10
    # define CS_ULLONG          11
    # define CS_FLOAT           12
    # define CS_DOUBLE          13

    /* Define standard output */
    # define CS_OUT             stdout

    /* Define boolean variables */
    # define CS_FALSE           0
    # define CS_TRUE            1

    /* Define directory entity type */
    # define CS_FILE            0
    # define CS_DIRECTORY       1

    /* Define directory structure */
    # define CS_PATH_PATTERN    ".log-"

/* 
    Header - Preprocessor macros
 */

/* 
    Header - Typedefs
 */

    /* I know ! Screw you ! */
    typedef struct dirent DIRENT;

/* 
    Header - Structures
 */

/* 
    Header - Function prototypes
 */

    /*! \brief Software main function
     *  
     *  The main function calls the analysis and appending procedure in order to
     *  perform contigous log detection and appending.
     *  
     *  \param argc Standard main parameter
     *  \param argv Standard main parameter
     */

    int main ( int argc, char ** argv );

    /*! \brief Log-file decomposer
     * 
     *  This function opens the input logs file and test if 10 or more seconds
     *  gap appears on the successive timestamp. It transfers the content of
     *  the input logs file in separated output file with separation determined
     *  by the detected gaps.
     *
     *  \param  csLog       Input logs file path
     *  \param  csDirectory Output directory
     *  \param  csIndex     Current index of the decomposition
     *
     *  \return Returns the corrected decomposition index
     */

    int cs_decompose_split ( char const * const csLog, char const * const csDirectory, int csIndex );

    /*! \brief Directory entity enumeration
     *  
     *  Enumerates entity contained in the pointed directory. The function
     *  detects automatically if an enumeration is under way and returns, one
     *  by one, the name of the found entity. When enumeration is terminated,
     *  the function close itself the directory handle.
     *
     *  \param  csDirectory Directory to enumerates
     *  \param  csName      String that recieve the entity name, appended with
     *                      directory path
     *
     *  \return Returns code indicating enumeration status
     */

    int cs_decompose_enum ( char const * const csDirectory, char * const csName );

    /*! \brief Directory entity type detection
     *
     *  This function, according to the desired type (file or directory), checks
     *  if entity type is correct.
     *
     *  \param  csEntity    Path to the entity
     *  \param  csType      Type of the entity to verify
     *
     *  \return Returns CS_TRUE if verification passed, CS_FALSE otherwise
     */

    int cs_decompose_detect ( char const * const csEntity, int const csType );

    /*! \brief Arguments common handler
     *  
     *  This function searches in the argv string array the position of the
     *  argument defined through ltag/stag and returns the detected index.
     *  
     *  \param  argc    Standard main parameter
     *  \param  argv    Standard main parameter
     *  \param  ltag    Long-form argument string
     *  \param  stag    Short-form argument string
     *
     *  \return Returns index of parameter in argv
     */

    int stda ( int argc, char ** argv, char const * const ltag, char const * const stag );

    /*! \brief Parameters common handler
     *  
     *  This function interprets the parameter in the desired type and returns
     *  it through the param variable. The argi variable is typically set using
     *  stda function. If argi is set to CS_NULL, the function does nothing.
     *  
     *  \param argi     Index of the parameter in argv
     *  \param argv     Standard main parameter
     *  \param param    Pointer to the variable that recieve the interpreted
     *                  parameter
     *  \param type     Type to use for parameter interpretation
     */

    void stdp ( int argi, char ** argv, void * const param, int const type );

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


