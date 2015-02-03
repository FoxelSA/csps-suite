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

    /*! \file   csps-earth.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *   
     *  Software main header
     */

    /*! \mainpage csps-earth
     *
     *  \section csps-suite
     *  \section _ CSPS library front-end suite
     *
     *  The csps-suite is dedicated to CSPS processing using libcsps. It provides
     *  a serie of softwares used for camera logs-files preparation and 
     *  validation according to manufacturers standards. It comes with a main 
     *  software that is responsible of CSPS processing, using libcsps, of the
     *  camera logs-files and also offers softwares for CSPS processing results
     *  visualization and exportation
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
    # define CS_HELP "Usage summary :\n"          \
    "  csps-earth [Arguments] [Parameters] ...\n" \
    "Short arguments and parameters summary :\n"  \
    "  -p Path to CSPS directory structure\n"     \
    "  -r OpenMVG rigid-rig output directory\n"   \
    "  -i Input Stanford triangle format file\n"  \
    "  -o Output Stanford triangle format file\n" \
    "  -c Camera device CSPS-tag\n"               \
    "  -m Camera device CSPS-module\n"            \
    "  -g GPS device CSPS-tag\n"                  \
    "  -n GPS device CSPS-module\n"               \
    "csps-earth - csps-suite\n"                   \
    "Copyright (c) 2013-2015 FOXEL SA\n"

    /* Define ply reading modes */
    # define CS_EXIT   0
    # define CS_HEADER 1
    # define CS_VERTEX 2

/* 
    Header - Preprocessor macros
 */

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

    /*! \struct cs_Curve_struct
     *  \brief Cruve storage
     *
     *  This structure is used to store geopositions that define a GPS track or
     *  a visual odometry path. The data array is a one-dimensional array that
     *  stores longitude, latitude and altitude of each successive positions.
     *  It follows that the number of geopositions defining the curve is given
     *  by the cvSize field value devided by three.
     *
     *  \var cs_Curve_struct::cvSize
     *  Size, in type units, of the curve array
     *  \var cs_Curve_struct::cvGhost
     *  Size, in type units, of the curve array allocated memory
     *  \var cs_Curve_struct::cvData
     *  Pointer to one-dimensional array containing the geopositions
     */

    typedef struct cs_Curve_struct {

        /* Data array element size */
        size_t cvSize;
        
        /* Data array memory size */
        size_t cvGhost;

        /* Pointer to data memory */
        double * cvData;

    } cs_Curve_t;

    /*! \struct cs_WGS84_struct
     *  \brief WGS84 alignment model
     *
     *  This structure contains the information about the considered earth local
     *  metric flat frame (ELMFF) in which visual odometry path is aligned on
     *  GPS track.
     *
     *  The ELMFF is an euclidian three-dimensional space defined by the frame
     *  attached to a point using spherical coordinates. This point is called
     *  ELMFF central point.
     *
     *  \var cs_WGS84_struct::wglonm
     *  ELMFF central position longitude, in decimal degrees
     *  \var cs_WGS84_struct::wglatm
     *  ELMFF central position latitude, in decimal degrees
     *  \var cs_WGS84_struct::wgaltm
     *  ELMFF central position altitude, in metres
     *  \var cs_WGS84_struct::wgfactor
     *  Planimetric scale factor between WGS84 and ELMFF
     */

    typedef struct cs_WGS84_struct {

        /* WGS84 central point */
        double wglonm;
        double wglatm;
        double wgaltm;

        /* WGS84 metric factor */
        double wgfactor;

    } cs_WGS84_t;

/* 
    Header - Function prototypes
 */

    /*! \brief Software main function
     *  
     *  The main function is responsible for arguments and parameters reading 
     *  and lauching the software main task. It starts by calling the curves
     *  importation procedure in order to perform alignment of visual odometry
     *  curve on GPS track curve. It then calls the WGS84 model creation to
     *  bring visual odometry path and GPS track in the same frame. The linear
     *  transformation estimation procedure is then called.
     *
     *  The estimated linear transformation is the considered to transform x, y
     *  and z vertex of the input PLY file. The result of vertex transformation
     *  is finaly exported in the output PLY file.
     *
     *  \param  argc Standard main parameter
     *  \param  argv Standard main parameter
     *
     *  \return Software exit code
     */

    int main ( int argc, char ** argv );

    /*! \brief WGS84 frame alignment model
     *
     *  GPS track and visual odometry path curves are aligned in earth local
     *  metric flat frame (ELMFF) with a central point corresponding to GPS
     *  track geometric center. The parameters of ELMFF are computed and are
     *  returned through the alignment model specific storage structure.
     *
     *  The GPS track curve is modified in order to express GPS geopositions in
     *  the computed ELMFF.
     *
     *  \param  csGPS WGS84-GPS track curve
     *
     *  \return Returns computed alignment model structure
     */

    cs_WGS84_t cs_earth_model(

        cs_Curve_t * const csGPS

    );

    /*! \brief Stanford triangle format linear transformation
     *
     *  This function expects a linear transformation estimation that brings the
     *  visual odometry curve on the WGS84 GPS track curve in earth local metric
     *  flat frame. The transformation is provided using a rotation matrix and 
     *  a translation vector. The input PLY file is read and the x, y and z 
     *  vertex of each point of the file are transformed using the linear
     *  transformation in order to retrieve an earth-alignment point-cloud. 
     *  The result is exported in the output PLY file after ELMFF to WGS84 frame
     *  conversion.
     *
     *  The input PLY file has to have x, y and z vertex to be consecutive in 
     *  terms of columns.
     *
     *  \param csiPly Path to input PLY file
     *  \param csoPly Path to output PLY file
     *  \param csR    Rotation matrix
     *  \param csT    Translation vector
     *  \param csWGS  Alignment model storage structure
     */

    void cs_earth_transform( 

        char       const * const csiPly,
        char       const * const csoPly,
        double                   csR[3][4],
        cs_WGS84_t const * const csWGS

    );

    /*! \brief Stanford triangle format linear transformation
     *
     *  This function simply reads a string token from the provided open file
     *  stream. If token reading fails, the function returns a NULL pointer.
     * 
     *  \param  csToken  Char buffer that recieves read token
     *  \param  csStream Open file stream in which token is read
     *
     *  \return Returns pointer to provided token buffer or NULL if no token
     *          where read.
     */

    char * cs_earth_transform_token(

        char * const csToken,
        FILE * const csStream

    );

    /*! \brief Stanford triangle format linear transformation
     *
     *  This function reads a string token from open input PLY file stream and
     *  exports it in open output PLY file stream. The end of line condition
     *  is used to add space or end of line character after string token
     *  exportation.
     *
     *  \param csiStream Open input PLY file stream
     *  \param csoStream Open output PLY file stream
     *  \param csEOL     End of line boolean parameter
     */

    void cs_earth_transform_copy(

        FILE       * const csiStream,
        FILE       * const csoStream,
        int  const         csEOL

    );

    /*! \brief Cruve importation procedure
     *
     *  This function creates the visual odometry and GPS track curves that are
     *  considered for earth-alignment process. GPS track is retrieved using
     *  queries on CSPS-processed data when visual odometry curve is imported
     *  from OpenMVG rigid-rig specific output directory.
     *
     *  \param csPath Path to CSPS directory structure
     *  \param csRigs OpenMVG rigid-rig output directory
     *  \param csCAMd Camera device CSPS-tag
     *  \param csCAMm Camera device CSPS-module
     *  \param csGPSd GPS device CSPS-tag
     *  \param csGPSm GPS device CSPS-module
     *  \param csMVG  Visual odometry curve
     *  \param csGPS  GPS track curve
     */

    void cs_earth_curve( 

        char       const * const csPath, 
        char       const * const csRigs, 
        char       const * const csCAMd, 
        char       const * const csCAMm, 
        char       const * const csGPSd, 
        char       const * const csGPSm, 
        cs_Curve_t       * const csMVG,
        cs_Curve_t       * const csGPS

    );

    /*! \brief Cruve importation procedure
     *
     *  This function is used to push a geoposition definition in the provided
     *  curve structure. The function detects data array overflow and manage the
     *  array memory re-allocation to allow geoposition addition.
     * 
     *  \param csCurve     Curve in which values are pushed
     *  \param cvLongitude Longitude value to push
     *  \param cvLatitude  Latitude value to push
     *  \param cvAltitude  Altitude value to push
     */

    void cs_earth_curve_push( 

        cs_Curve_t       * const csCurve, 
        double     const         cvLongitude,
        double     const         cvLatitude,
        double     const         cvAltitude

    );

    /*! \brief Linear transformation estimation
     * 
     *  This function is designed to estimates the rotation and translation,
     *  that defines the linear tranformation needed to align two similar data
     *  sets. It is typically used to synchronize visual odometry coming from
     *  OpenMVG process and the GPS track extracted from the camera device logs.
     *
     *  It then waits two arrays storing, through one dimensional arrays, the
     *  three components of the camera sensor positions coming from visual
     *  odometry and the three WGS84 camera sensor positions coming from the
     *  GPS device. The three-coordinates are store successively as follows : 
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
     *  \param csrData  Pointer to array that stores the reference curve (GPS)
     *  \param csaData  Pointer to array that stores the curve to align on the
     *                  reference curve (MVG)
     *  \param csR      Returned estimation of rotation matrix
     *  \param csT      Returned estimation of translation vector
     */

    void cs_earth_lte( 

        int    const         csN, 
        double const * const csrData, 
        double const * const csaData, 
        double               csR[3][4]

    );

    /* Specific extern liblapack prototype */
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

