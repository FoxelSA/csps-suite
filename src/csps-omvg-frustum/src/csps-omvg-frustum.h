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

    /*! \file   csps-omvg-frustum.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *   
     *  Software main header
     */

    /*! \mainpage csps-omvg-frustum
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

    # ifndef __CS_FRUSTUM__
    # define __CS_FRUSTUM__

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
    # include <float.h>
    # include <csps-all.h>
    # include <fastcal-all.h>

/* 
    Header - Preprocessor definitions
 */

    /* Standard help */
    # define CS_HELP "Usage summary :\n"                 \
    "  csps-omvg-frustum [Arguments] [Parameters] ...\n" \
    "Short arguments and parameters summary :\n"         \
    "  -p CSPS-processed structure path\n"               \
    "  -l OpenMVG input list file\n"                     \
    "  -r OpenMVG output pairs file\n"                   \
    "  -c Camera MAC address\n"                          \
    "  -g GPS CSPS-tag\n"                                \
    "  -m GPS CSPS-module\n"                             \
    "  -i IMU CSPS-tag\n"                                \
    "  -k IMU CSPS-module\n"                             \
    "  -n Frustum near plane\n"                          \
    "  -f Frustum far plane\n"                           \
    "csps-omvg-frustum - csps-suite\n"                   \
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
    # define CS_ERR             stderr

    /* Define detection results */
    # define CS_FALSE           LP_FALSE
    # define CS_TRUE            LP_TRUE

/* 
    Header - Preprocessor macros
 */

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

    /*! \struct cs_List_struct
     *  \brief OpenMVG list file memory structure
     *
     *  This structure is designed to store the needed information coming from
     *  provided by the OpenMVG list file.
     *
     *  \var cs_List_struct::lsChannel
     *  Camera sensor channel number
     *  \var cs_List_struct::lsTime
     *  Camera sensor image capture timestamp
     */

    typedef struct cs_List_struct {

        /* Channel */
        unsigned long lsChannel;

        /* Timestamp */
        lp_Time_t     lsTime;

    } cs_List_t;

    /*! \struct cs_Frustum_struct
     *  \brief Frustum definition
     *
     *  This structure stores the geometrical definition of a frustum.
     *
     *  \var cs_Frustum_struct::fsNad
     *  Nadir vector (principal direction)
     *  \var cs_Frustum_struct::fsLef
     *  Left vector
     *  \var cs_Frustum_struct::fsTop
     *  Top vector
     *  \var cs_Frustum_struct::fsOrg
     *  Frustum origin
     *  \var cs_Frustum_struct::fsNPP
     *  Frustum near principal point
     *  \var cs_Frustum_struct::fsFPP
     *  Frustum far principal point
     *  \var cs_Frustum_struct::fsLefApp
     *  Frustum left apperture parameter
     *  \var cs_Frustum_struct::fsTopApp
     *  Frustum top apperture parameter
     *  \var cs_Frustum_struct::fsNear
     *  Frustum near plane distance to origin
     *  \var cs_Frustum_struct::fsFar
     *  Frustum far plane distance to origin
     *  \var cs_Frustum_struct::fsSX
     *  Frustum summit x-coordinates
     *  \var cs_Frustum_struct::fsSY
     *  Frustum summit y-coordinates
     *  \var cs_Frustum_struct::fsSZ
     *  Frustum summit z-coordinates
     */ 

    typedef struct cs_Frustum_struct {

        /* Frustum nadir/left/top vectors */
        double fsNad[3];
        double fsLef[3];
        double fsTop[3];

        /* Frustum origin/near/far points */
        double fsOrg[3];
        double fsNPP[3];
        double fsFPP[3];

        /* Frustum left/top appertures */
        double fsLefApp;
        double fsTopApp;

        /* Frustum near/far planes */
        double fsNear;
        double fsFar;

        /* Frustum summit arrays */
        double fsSX[8];
        double fsSY[8];
        double fsSZ[8];

    } cs_Frustum_t;

/* 
    Header - Function prototypes
 */

    /*! \brief Software main function
     *  
     *  The main function starts by importing information contained in the
     *  OpenMVG list file. It then parses the created informations stack to
     *  determine which camera sensors have an intersection of their respective
     *  frustum.
     *  
     *  The main function fills the output file, as waited by OpenMVG, specifying
     *  which sensor image have a common view on the base of their frustum.
     *  
     *  \param  argc Standard main parameter
     *  \param  argv Standard main parameter
     *
     *  \return Code returned to system
     */

    int main ( int argc, char ** argv );

    /*! \brief OpenMVG list importation
     * 
     *  This function is responsible of informations importation from OpenMVG
     *  list file that are needed for frustum intersection computation. Those
     *  informations are the camera sensor image capture timestamp and the
     *  camera sensor channel.
     *
     *  The information are stored in a stack that is create and filled by this
     *  function. Note that, for performance purpose, the actual size, in memory,
     *  of the stack can be greater than the returned size.
     *
     *  \param  csList   Path to the OpenMVG list file
     *  \param  csStack  Pointer to the information stack base pointer
     *
     *  \return Returns the size of the created stack
     */
     
    unsigned long cs_omvg_frusmtum_list ( char const * const csList, cs_List_t ** csStack );

    /*! \brief Eyesis4Pi frustum composer
     *
     *  This function creates the frustum definition of a given sensor of an 
     *  eyesis4pi device. It takes calibration data into account to precisely 
     *  place the frustum according to the camera.
     *
     *  Moreover, the function takes a rotation matrix and a position vector in
     *  order to compute the frustum in earth frame.
     *  
     *  \param csCamera     Camera MAC address
     *  \param csChannel    Camera sensor index
     *  \param csEFxx       X-component of rotated x-unit vector
     *  \param csEFxy       Y-component of rotated x-unit vector
     *  \param csEFxz       Z-component of rotated x-unit vector
     *  \param csEFyx       X-component of rotated y-unit vector
     *  \param csEFyy       Y-component of rotated y-unit vector
     *  \param csEFyz       Z-component of rotated y-unit vector
     *  \param csEFzx       X-component of rotated z-unit vector
     *  \param csEFzy       Y-component of rotated z-unit vector
     *  \param csEFzz       Z-component of rotated z-unit vector
     *  \param csEFpx       X-coordinate of device
     *  \param csEFpy       Y-coordinate of device
     *  \param csEFpz       Z-coordinate of device
     *  \param csNear       Frustum near plane
     *  \param csFar        Frustum far plane
     *  \param csFrustum    Returned frustum definition
     */

    void cs_omvg_frustum_eyesis4pi(

        char         const * const csCamera, 
        int          const         csChannel, 
        double       const         csEFxx,
        double       const         csEFxy,
        double       const         csEFxz,
        double       const         csEFyx,
        double       const         csEFyy,
        double       const         csEFyz,
        double       const         csEFzx,
        double       const         csEFzy,
        double       const         csEFzz,
        double       const         csEFpx,
        double       const         csEFpy,
        double       const         csEFpz,
        double       const         csNear,
        double       const         csFar,
        cs_Frustum_t       * const csFrustum

    );

    /*! \brief Frustum intersection detection
     * 
     *  This function takes two frustum definitions and determines if an 
     *  polyhedral intersection appears.
     *  
     *  The intersection detection method is not an absolute method and 
     *  intersection volume that are below a given size can be missed.
     *
     *  \param  csFrus_A First frustum definition
     *  \param  csFrus_B Second frustum definition
     *
     *  \return Returns CS_TRUE on intersection and CS_FALSE otherwise.
     */

    int cs_omvg_frustum_intersection(

        cs_Frustum_t const * const csFrus_A,
        cs_Frustum_t const * const csFrus_B

    );

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


