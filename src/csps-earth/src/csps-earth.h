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

    /*! \file   csps-earth.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *   
     *  Software main header
     */

    /*! \mainpage csps-align
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

    # ifndef __CS_EARTH__
    # define __CS_EARTH__

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
    # include <csps-all.h>
    # include <common-all.h>
    # include <libgen.h>

/* 
    Header - Preprocessor definitions
 */

    /* Standard help */
    # define CS_HELP "Usage summary :\n"             \
    "  csps-earth [Arguments] [Parameters] ...\n"    \
    "Short arguments and parameters summary :\n"     \
    "  -p CSPS-processed structure root directory\n" \
    "  -r OpenMVG rigid rig output directory\n"      \
    "  -i Input PLY file\n"                          \
    "  -o Output PLY file\n"                         \
    "  -c Camera device CSPS-tag\n"                  \
    "  -m Camera device CSPS-module\n"               \
    "  -g GPS device CSPS-tag\n"                     \
    "  -n GPS device CSPS-module\n"                  \
    "  -d Timestamp delay\n"                         \
    "csps-earth - csps-suite\n"                      \
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

    typedef struct cs_Curve_struct {

        size_t cvSize;
        size_t cvGhost;

        double * cvData;

    } cs_Curve_t;

    typedef struct cs_WGS84_struct {

        double wglonm;
        double wglatm;
        double wgaltm;

        double wgfactor;

    } cs_WGS84_t;

/* 
    Header - Function prototypes
 */

    /*! \brief Software main function
     *  
     *  \param  argc Standard main parameter
     *  \param  argv Standard main parameter
     *
     *  \return Code returned to system
     */

    int main ( int argc, char ** argv );

    /*! \brief Linear transformation estimation
     * 
     *  This function is designed to estimates the rotation and translation,
     *  that defines the linear tranform, needed to align two similar data
     *  sets. It is typically used to synchronize visual odometry coming from
     *  OpenMVG process and the GPS track extracted from the camera device logs.
     *
     *  It then waits two arrays storing, through one dimensional arrays, the
     *  three components of the camera sensor positions coming from visual
     *  odometry and the three WGS84 camera sensor positions coming from the
     *  GPS device. The 3-coordinates are store successively as follows : 
     * 
     *      x1, y1, z1, x2, y2, z2, ... xn, yn, zn
     * 
     *  Moreover, for good estimation of the rotation and translation between
     *  the two sets of data, the information stored in each array have to be
     *  as similar as possible.
     *
     *  \param csN      Number of coordinates stored in each array; multiplying
     *                  this value by three gives the size, in type units, of
     *                  each array
     *  \param csrData  Pointer to array that store the reference curve (GPS)
     *  \param csaData  Pointer to array that store the curve to align on the
     *                  reference curve
     *  \param csR      Returned estimation of rotation matrix
     *  \param csT      Returned estimation of translation vector
     */

    void cs_earth_lte( 

        int    const         csN, 
        double const * const csrData, 
        double const * const csaData, 
        double               csR[3][3], 
        double               csT[3]

    );

    cs_WGS84_t cs_earth_wgs84_align(

        cs_Curve_t * const csGPS

    );

    void cs_earth_curve( 

        char       const * const csPath, 
        char       const * const csRigs, 
        char       const * const csCAMd, 
        char       const * const csCAMm, 
        char       const * const csGPSd, 
        char       const * const csGPSm, 
        cs_Curve_t       * const csMVG,
        cs_Curve_t       * const csGPS,
        long       const         csDelay

    );

    void cs_earth_curve_push( 

        cs_Curve_t * const csCurve, 
        double const cvLng,
        double const cvLat,
        double const cvAlt

    );

    void cs_earth_process( 

        char const * const csiPly,
        char const * const csoPly,
        double csR[3][3],
        double csT[3],
        cs_WGS84_t const * const csWGS

    );

    /* Specific liblapack prototype :( */
    extern void dgesvd_( 

        char   * jobu, 
        char   * jobvt, 
        int    * m, 
        int    * n, 
        double * a, 
        int    * lda, 
        double * s, 
        double * u, 
        int    * ldu, 
        double * vt, 
        int    * ldvt,
        double * work, 
        int    * lwork, 
        int    * info

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

