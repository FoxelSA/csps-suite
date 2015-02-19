/*
 * csps-suite - CSPS library front-end suite
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

    /*! \file   csps-elphel-sort.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *   
     *  Software main header
     */

    /*! \mainpage csps-elphel-sort
     *
     *  \section csps-suite
     *  \section _ CSPS library front-end suite
     *
     *  The csps-suite is dedicated to CSPS processing using libcsps. It provides
     *  a serie of softwares used for camera logs-files preparation and 
     *  validation according to manufacturers standards. It comes with a main 
     *  software that is responsible of CSPS processing, using libcsps, of the
     *  camera logs-files and also offers softwares for CSPS processing results
     *  visualization and exportation.
     *
     *  \section Documentation
     *
     *  A detailed documentation can be generated through doxygen. A more general
     *  documentation can be consulted at https://github.com/FoxelSA/csps-suite/wiki.
     *
     *  \section Copyright
     * 
     *  Copyright (c) 2013-2015 FOXEL SA - http://foxel.ch \n
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

    # ifndef __CS_ELPHEL_SORT__
    # define __CS_ELPHEL_SORT__

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
    # include <string.h>
    # include <libgen.h>
    # include <csps-all.h>
    # include <common-all.h>

/* 
    Header - Preprocessor definitions
 */

    /* Standard help */
    # define CS_HELP "Usage summary :\n"                    \
    "  csps-elphel-sort [Arguments] [Parameters] ...\n"     \
    "Short arguments and parameters summary :\n"            \
    "  -s Directory containing source logs-files\n"         \
    "  -d Directory where sorted logs-files are exported\n" \
    "csps-elphel-sort - csps-suite\n"                       \
    "Copyright (c) 2013-2015 FOXEL SA\n"

/* 
    Header - Preprocessor macros
 */

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

    /*! \struct cs_Sort_struct
     *  \brief Sorting array structure
     *
     *  This structure is used to create a chained list in order to sort events
     *  provided by through a logs-file using insertion sorting algorithm.
     *
     *  \var cs_Sort_struct::srTime
     *  Field that stores record timestamp
     *  \var cs_Sort_struct::srOffset
     *  Field that stores record offset in input file
     *  \var cs_Sort_struct::srp
     *  Pointer to previous node of sorted structure
     *  \var cs_Sort_struct::srn
     *  Pointer to next node of sorted structure
     */ 

    typedef struct cs_Sort_struct {

        /* Event timestamp */
        lp_Time_t   srTime;

        /* Offset of record */
        size_t      srOffset;

        /* Chained list pointers */
        void      * srp;
        void      * srn;

    } cs_Sort_t;

/* 
    Header - Function prototypes
 */

    /*! \brief Software main function
     *  
     *  The main function lists the logs-files stored in the source directory
     *  and send them to the sorting procedure. Each sorted logs-file are then
     *  exported in the destination directory.
     *
     *  \param  argc Standard main parameter
     *  \param  argv Standard main parameter
     *
     *  \return Returns exit code
     */

    int main ( int argc, char ** argv );

    /*! \brief Sorting procedure
     *
     *  This procedure takes a logs-file path as input in order to reads the 
     *  event records and to perform a time-based sorting of the events. The
     *  sorted events are the exported in the output logs-files.
     *
     *  This function is designed to consume as less as memory space as possible
     *  avoiding to load entire logs-file.
     *
     *  The function implement an insertion sorting algorithm due to the fact 
     *  that records have to be read at least one time to get their timestamp
     *  and because incomming logs-file are assumed, most of the time, to be 
     *  nearly sorted.
     * 
     *  \param csiFile Path to input logs-file
     *  \param csiFile Path to output logs-file
     */

    void cs_elphel_sort(

        char const * const csiFile,
        char const * const csoFile

    );

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


