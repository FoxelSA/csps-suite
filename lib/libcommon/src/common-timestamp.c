/*
 * csps-suite libcommon - csps-suite common library
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


/* 
    Source - Includes
 */

    # include "common-timestamp.h"

/*
    Source - Human readable timestamp converter
 */

    char * lc_timestamp_utc( lp_Time_t lcTimestamp ) {

        /* Static string variables */
        static char lcHuman[256] = { 0 };

        /* Timestamp variables */
        time_t lcUnixTime = lp_timestamp_sec( lcTimestamp );

        /* Decomposed time variables */
        struct tm * lcTime;

        /* Create decomposed time structure */
        lcTime = gmtime( & lcUnixTime );

        /* Create human readable string */
        strftime ( lcHuman, 256, "%Y-%m-%dT%H:%M:%S+00:00", lcTime );

        /* Returns pointer to string */
        return( lcHuman );

    }


