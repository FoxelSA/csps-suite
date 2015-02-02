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

    /*! \mainpage csps-align
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
     *  stores successively longitude, latitude and altitude of each position.
     *  It follows that the number of geopositions defining the curve is given
     *  by cvSize fields over three.
     *
     *  \var cs_Descriptor_struct::cvSize
     *  Size, in type units, of the curve storage array
     *  \var cs_Descriptor_struct::cvGhost
     *  Size, in type units, of the array memory allocation
     *  \var cs_Descriptor_struct::cvData
     *  Pointer to one-dimensional array containing the geopositions
     */

    typedef struct cs_Curve_struct {

        /* Data array size */
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
     *  \var cs_Descriptor_struct::wglonm
     *  ELMFF longitude central position
     *  \var cs_Descriptor_struct::wglatm
     *  ELMFF latitude central position
     *  \var cs_Descriptor_struct::wgaltm
     *  ELMFF altitude central position
     *  \var cs_Descriptor_struct::wgfactor
     *  Factor between WGS84 angular positions and ELMFF
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
     *  and lauching the software main task. It starts by calling the curve
     *  importation procedure in order to perform alignment of visual odometry
     *  curve on GPS track curve. It then calls the WGS84 model for GPS curve
     *  modification in order to allow to compare, in terms of scale, the two 
     *  considered cruves. The linear transformation estimation procedure is
     *  then called.
     *
     *  The linear transformation is the applied to the vertex of the input PLY
     *  file. The resulting PLY file is exported in the directory provided as
     *  parameter.
     *
     *  \param  argc Standard main parameter
     *  \param  argv Standard main parameter
     *
     *  \return Code returned to system
     */

    int main ( int argc, char ** argv );

    /*! \brief WGS84 frame alignment model
     *
     *  This function applies a bijective transformation to the WGS84-GPS track
     *  provided as parameter. It computes the track geometric center in order
     *  to translate the track near frame origin. It also computes the metric
     *  factor between WGS84-attached angular position at the track mean height.
     *  
     *  The track is denormalized using the metric factor to retrieve a near
     *  metric representation of the track. This is done in order to have a
     *  curve similar in size and scale the the provided visual odometry curve,
     *  needed to perform a valid linear transformation estimation.
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
     *  visual odometry curve on the WGS84 GPS track curve. The transformation
     *  is provided using a rotation matrix and a translation vector. The input
     *  PLY file is read and the x, y and z vertex of each point of the file is
     *  transformed using the linear transformation in order to retrieve an
     *  earth-alignment point-cloud. The result is exported in the output PLY
     *  file. 
     *
     *  The input PLY file as to have x, y and z vertex to be consecutive in 
     *  terms of columns.
     *
     *  \param csiPly Path to input PLY file
     *  \param csoPly Path to output PLY file
     *  \param csR    Rotation matrix
     *  \param csT    Translation vector
     *  \param csWGS  WSG84 alignment model
     */

    void cs_earth_transform( 

        char       const * const csiPly,
        char       const * const csoPly,
        double                   csR[3][3],
        double                   csT[3],
        cs_WGS84_t const * const csWGS

    );

    /*! \brief Stanford triangle format linear transformation
     *
     *  This function simply reads a string token from the provided open file
     *  stream. If token reading failed, the function returns a NULL pointer.
     * 
     *  \param  csToken  Token char buffer
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
     *  This function reads string token from open input PLY file stream and
     *  exports it in open output PLY file stream. The end of line condition
     *  is used to add space or end of line character after string token
     *  exportation.
     *
     *  \param csiStream Open input PLY file stream
     *  \param csoStream Open output PLY file stream
     *  \param csEOL     End of line boolean parameter
     */

    void cs_earth_transform_copy(

        FILE * const csiStream,
        FILE * const csoStream,
        int    const csEOL

    );

    /*! \brief Cruve importation procedure
     *
     *  This function creates the visual odometry and GPS track curves that are
     *  considered for earth-alignment process. GPS track is retrieve using
     *  queries on CSPS-processed data when visual odometry curve is imported
     *  from OpenMVG rigid-rig specific output directory.
     *
     *  \param csPath Path to CSPS directory structure
     *  \param csRigs OpenMVG rigs-file directory
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
     *  array memory re-allocation.
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

