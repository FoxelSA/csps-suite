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

    /*! \file   common-timestamp.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *
     *  Generic timestamp operation
     */

/* 
    Header - Include guard
 */

    # ifndef __LC_TIMESTAMP__
    # define __LC_TIMESTAMP__

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/* 
    Header - Includes
 */

    # include "common.h"

/* 
    Header - Preprocessor definitions
 */

    /* Define unix timestamp limits */
    # define LC_EPOCH   lp_Time_s( 0x0000000000000000 )
    # define LC_INFTY   lp_Time_s( 0xFFFFFFFFFFFFFFFF )

    /* Define timestamp printf model */
    # define LC_TSM     "%010" lp_Time_p ".%06" lp_Time_p

/* 
    Header - Preprocessor macros
 */

    /* Define timestamp reader macro */
    # define LC_TSR(b)  lp_timestamp( ( lp_Void_t * ) b )

    /* Define timestamp decomposition macro */
    # define LC_TSD(t)  lp_timestamp_sec(t), lp_timestamp_usec(t)

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

/* 
    Header - Function prototypes
 */

    /*! \brief Human readable timestamp converter
     *
     *  This function waits a timestamp variables and convert the seconds part
     *  in a human readable form using UTC time. The result is returned using
     *  a static char array.
     * 
     *  \param  lcTimestamp     Timestamp to convert
     *
     *  \return Returns the pointer to the first character of the string
     */

    char * lc_timestamp_utc ( lp_Time_t lcTimestamp );

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

