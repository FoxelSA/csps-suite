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

    /*! \file   common.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *
     *  Library common header file
     */

    /*! \mainpage csps-suite libcommon
     *
     *  \section csps-suite
     *  \section _ CSPS library front-end suite
     *
     *  The csps-suite is dedicated to CSPS processing using libcsps. It provides
     *  a serie of softwares used for camera logs-files preparation and 
     *  validation according to manufacturers standards. It comes with a main 
     *  software that is responsible of CSPS processing, using libcsps, of the
     *  camera logs-files and also offers software for CSPS processing results
     *  visualization.
     *
     *  \section Documentation
     *
     *  A detailed documentation can be generated through doxygen. A more general
     *  documentation can be consulted at https://github.com/niam-foxel/csps-suite/wiki.
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

    # ifndef __LC__
    # define __LC__

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
    # include <math.h>
    # include <time.h>
    # include <dirent.h>
    # include <csps-all.h>

/* 
    Header - Preprocessor definitions
 */

    /* Define standard output */
    # define LC_OUT         stdout
    # define LC_ERR         stderr

    /* Define boolean constants */
    # define LC_FALSE       LP_FALSE
    # define LC_TRUE        LP_TRUE

    /* Define log-files pattern */
    # define LC_PATTERN     ".log-"

    /* Define record buffer size */
    # define LC_RECORD      LP_DEVICE_EYESIS4PI_RECLEN

    /* Define events type */
    # define LC_IMU         LP_DEVICE_EYESIS4PI_IMUEVT
    # define LC_MAS         LP_DEVICE_EYESIS4PI_MASEVT
    # define LC_GPS         LP_DEVICE_EYESIS4PI_GPSEVT

/* 
    Header - Preprocessor macros
 */

    /* Event type detection macro */
    # define LC_EDM(r,t)    ( ( r[3] & lp_Byte_s( 0x0F ) ) == t )

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

/* 
    Header - Function prototypes
 */

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


