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

    /*! \file   csps-elphel-gps.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *   
     *  Software main header
     */

    /*! \mainpage csps-elphel-gps
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

    # ifndef __CS_ELPHEL_GPS__
    # define __CS_ELPHEL_GPS__

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
    # include <stdint.h>
    # include <libgen.h>
    # include <dirent.h>
    # include <csps-all.h>

/* 
    Header - Preprocessor definitions
 */

    /* Standard help */
    # define CS_HELP "Usage summary :\n"                       \
    "  csps-elphel-gps [Arguments] [Parameters] ...\n"         \
    "Short arguments and parameters summary :\n"               \
    "  -r Directory containing recomposed logs-files\n"        \
    "  -v Directory where validated logs-files are exported\n" \
    "  -m Minimum size, in bytes, for logs-file validation\n"  \
    "csps-elphel-gps - csps-suite\n"                           \
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

    /* Define boolean variables */
    # define CS_FALSE           LP_FALSE
    # define CS_TRUE            LP_TRUE

    /* Define directory entity type */
    # define CS_FILE            0
    # define CS_DIRECTORY       1

    /* Define directory structure */
    # define CS_PATH_PATTERN    ".log-"

    /* Define record length */
    # define CS_RECLEN          LP_DEVICE_EYESIS4PI_RECLEN

    /* Define events type */
    # define CS_IMU             LP_DEVICE_EYESIS4PI_IMUEVT
    # define CS_MAS             LP_DEVICE_EYESIS4PI_MASEVT
    # define CS_GPS             LP_DEVICE_EYESIS4PI_GPSEVT

    /* Define sentence buffer size */
    # define CS_NTYPE           4
    # define CS_REPET           5
    # define CS_BSIZE           CS_RECLEN * CS_NTYPE

    /* Define GPS timestamp reconstruction modes */
    # define CS_MODE_PARSE      0
    # define CS_MODE_WRITE      1
    # define CS_MODE_AVOID      2

/* 
    Header - Preprocessor macros
 */

    /* Event recognition macro */
    # define CS_TIMES(r)        ( lp_timestamp( ( lp_Void_t * ) r ) )
    # define CS_EVENT(r,e)      ( ( r[3] & lp_Byte_s( 0x0F ) ) == e )
    # define CS_NMEAT(r)        ( r[8] & 0x0F )

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
     *  The main function considers recomposed logs-files and proceed to a
     *  validation of the recomposition based on the file size. It also remove
     *  GPS-events that are too far away of their nearest IMU-event.
     *  
     *  \param argc Standard main parameter
     *  \param argv Standard main parameter
     */

    int main ( int argc, char ** argv );

    unsigned long cs_elphel_gps_process ( FILE * const csIStream, FILE * const csOStream, double csTol );

    int cs_elphel_gps_bloc ( lp_Byte_t const * const csBlock );

    lp_Time_t cs_elphel_gps_timestamp ( lp_Time_t csReference, unsigned long csRepet );

    void cs_elphel_gps_header ( lp_Time_t * const csHeader, lp_Time_t const csTime );

    /*! \brief Directory entity enumeration
     *  
     *  Enumerates entity contained in the pointed directory. The function
     *  detects automatically if an enumeration is under way and returns, one
     *  by one, the name of the found entities. When enumeration is terminated,
     *  the function closes itself the directory handle.
     *
     *  \param  csDirectory Directory to enumerates
     *  \param  csName      String that recieve the entity name, appended to the
     *                      directory path
     *
     *  \return Returns code indicating enumeration status
     */

    int cs_elphel_gps_enum ( char const * const csDirectory, char * const csName );

    /*! \brief Directory entity type detection
     *
     *  This function checks if directory entity if of the type file or
     *  directory according to the parameter.
     *
     *  \param  csEntity    Path to the entity
     *  \param  csType      Type of the entity to check
     *
     *  \return Returns CS_TRUE if verification passed, CS_FALSE otherwise
     */

    int cs_elphel_gps_detect ( char const * const csEntity, int const csType );

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

