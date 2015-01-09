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
     *  visualization and exportation.
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
    # include <csps-all.h>
    # include <common-all.h>

/* 
    Header - Preprocessor definitions
 */

    /* Standard help */
    # define CS_HELP "Usage summary :\n"                       \
    "  csps-elphel-gps [Arguments] [Parameters] ...\n"         \
    "Short arguments and parameters summary :\n"               \
    "  -s Directory containing logs-files to decimate\n"       \
    "  -d Directory where decimated logs-files are exported\n" \
    "csps-elphel-gps - csps-suite\n"                           \
    "Copyright (c) 2013-2014 FOXEL SA\n"

    /* Define sentence buffer size */
    # define CS_NTYPE           4
    # define CS_REPET           5

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
     *  The main function expects logs-files that have been processed in order
     *  to remove anomalous records or block of records. It parses the logs-files
     *  contained in the source directory and calls the GPS decimation process
     *  for each logs-files. The decimated logs-files are the written in the
     *  destination directory.
     *  
     *  \param  argc Standard main parameter
     *  \param  argv Standard main parameter
     *
     *  \return Return exit code
     */

    int main ( int argc, char ** argv );

    /*! \brief Logs-file GPS decimation
     *
     *  The way this function works is highly related to Elphel camera device
     *  hardware. The function performs a GPS decimation based on block of
     *  NMEA sentence completion and on the timestamp of the GPS-events. It also
     *  rebuild the timestamps of the GPS-events after validation of the block.
     *
     *  \param  csIStream Openned input logs-file handle
     *  \param  csOStream Openned output logs-file handle
     *
     *  \return Returns the number of discarded GPS-events
     */

    unsigned long cs_elphel_gps_process ( FILE * const csIStream, FILE * const csOStream );

    /*! \brief NMEA sentence block validation
     *
     *  This function waits an array with each of its element associated to a
     *  type of NMEA sentence. Each array element are expected to carry the count
     *  of NMEA sentence of the type its related to.
     *
     *  This array is then parsed by the function in order to be sure that, for
     *  a given GPS measure, each NMEA type appears only one. The block of
     *  sentence can then be discarded otherwise.
     * 
     *  \param  csBlock Pointer to first element of the array
     *
     *  \return Returns true if each sentence type appears only once, false
     *          otherwise
     */

    int cs_elphel_gps_bloc ( lp_Byte_t const * const csBlock );

    /*! \brief GPS timestamp reconstruction
     * 
     *  This function rebuild the GPS-event timestamp on the base of the last
     *  reference timestamp and the number of repetition of this reference
     *  timestamp, assuming a frequency of measure of five hertz.
     *
     *  \param  csReference Last reference timestamp
     *  \param  csRepet     Number of repetition of the reference timestamp
     *
     *  \return Returns rebuilded GPS-event timestamp
     */

    lp_Time_t cs_elphel_gps_timestamp ( lp_Time_t const csReference, unsigned long const csRepet );

    /*! \brief Record header override
     *  
     *  This function interprets the eight first bytes of the logs-file record
     *  buffer to replace the original timestamp by the rebuilded one.
     * 
     *  \param csHeader Pointer to first byte of record buffer
     *  \param csTime   Timestamp to write in recrod header
     */

    void cs_elphel_gps_header ( lp_Time_t * const csHeader, lp_Time_t const csTime );

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


