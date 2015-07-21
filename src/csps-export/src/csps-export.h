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

    /*! \file   csps-export.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *   
     *  Software main header
     */

    /*! \mainpage csps-export
     *
     *  \section csps-suite
     *  \section _ CSPS library front-end suite
     *
     *  The csps-suite is dedicated to CSPS processing using libcsps. It provides 
     *  a serie of softwares used for camera logs-files preparation and 
     *  validation according to manufacturers standards. It comes with a main 
     *  software that is responsible of CSPS processing of the camera logs-files 
     *  and also offers softwares for CSPS processing results visualization and
     *  exportation.
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

    # ifndef __CS_EXPORT__
    # define __CS_EXPORT__

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
    # include <json-c/json.h>

/* 
    Header - Preprocessor definitions
 */

    /* Standard help */
    # define CS_HELP "Usage summary :\n\n"           \
    "\tcsps-export [Arguments] [Parameters] ...\n\n" \
    "Short arguments and parameters summary :\n\n"   \
    "\t-p\tCSPS-processed structure path\n"          \
    "\t-e\tJSON exportation file path\n"             \
    "\t-c\tCapture trigger device CSPS-tag\n"        \
    "\t-m\tCapture trigger device CSPS-module\n"     \
    "\t-g\tGPS device CSPS-tag\n"                    \
    "\t-n\tGPS device CSPS-module\n"                 \
    "\t-i\tIMU device CSPS-tag\n"                    \
    "\t-s\tIMU device CSPS-module\n"                 \
    "\t-t\tStill range device CSPS-tag\n"            \
    "\t-k\tStill range device CSPS-module\n"         \
    "\t-f\tManual position coordinates\n\n"          \
    "csps-export - csps-suite\n"                     \
    "Copyright (c) 2013-2015 FOXEL SA\n"

/* 
    Header - Preprocessor macros
 */

/* 
    Header - Typedefs
 */

    /* Type definition simplification */
    typedef struct json_object cs_Object_t;

/* 
    Header - Structures
 */

/* 
    Header - Function prototypes
 */

    /*! \brief Software main function
     *  
     *  The main function starts by checking if a previous JSON file was created
     *  and load its content. The function then calls the exportation function
     *  that realize JSON exportation. If a previous JSON file is available, it
     *  is erased after its content importation.
     *
     *  The main function is also responsible of CSPS query structures creation
     *  and deletion.
     *  
     *  \param  argc Standard main parameter
     *  \param  argv Standard main parameter
     *  
     *  \return Returns exit code
     */

    int main ( int argc, char ** argv );

    /*! \brief Exportation function
     * 
     *  This function is responsible for JSON file exportation based on queries
     *  performed on the CSPS. It is also responsible to import some fields of
     *  the previous exportation file if available. The function expects already
     *  created CSPS query structures.
     *
     *  \param csTrigger  CSPS camera trigger query structure
     *  \param csPosition CSPS geoposition query structure
     *  \param csOrient   CSPS orientation query structure
     *  \param csStill    CSPS still range query structure
     *  \param csGPSf     GPS manual position coordinates.
     *  \param csFile     JSON exportation file
     *  \param csJson     Previous JSON file main object
     */

    void cs_export( 

        lp_Trigger_t  * const csTrigger, 
        lp_Position_t * const csPosition, 
        lp_Orient_t   * const csOrient,
        lp_Still_t    * const csStill,
        char          * const csGPSf,
        char          * const csFile,
        cs_Object_t   * const csJson

    );

    /*! \brief JSON array search
     * 
     *  This function expects a JSON object containing an array of camera pose
     *  objects. It searches and return the JSON object that corresponds to the
     *  camera pose defined by the master timestamp.
     *
     *  As a search is performed, the next search starts at the array index just
     *  below the last found one for optimization purpose.
     *
     *  \param csArray  JSON object containing a pose objects array
     *  \param csMaster Master timestamp of searched pose object
     *
     *  \return Returns the found JSON object, NULL pointer otherwise
     */

    cs_Object_t * cs_export_get_pose( 

        cs_Object_t * const csArray, 
        lp_Time_t     const csMaster 

    );

    /*! \brief Field exportation
     * 
     *  This function is designed to simplify JSON field exportation when a
     *  previous version of the JSON file is available. The function considers
     *  the provided key and checks if a previous field is available in the
     *  provided JSON object. In this case, the found field value is considered
     *  for value exportation. Otherwise, the provided value is used. In order
     *  to force the exportation of the provided value, NULL can be sent as JSON
     *  object.
     *
     *  The function allows also to give the comma character in case the field
     *  is not the last field of the currently exported JSON object. The comma
     *  parameter should then points to "," or "" string.
     *
     *  \param csKey    Field key
     *  \param csValue  Field value
     *  \param csComma  Field ending comma
     *  \param csStream Open exportation file stream
     *  \param csObject JSON object containing previous field
     */

    void cs_export_field( 

        char        const * const csKey,
        char        const * const csValue,
        char        const * const csComma,
        FILE              * const csStream,
        cs_Object_t       * const csObject

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


