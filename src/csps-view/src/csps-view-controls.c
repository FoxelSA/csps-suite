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


/* 
    Source - Includes
 */

    # include "csps-view-controls.h"

/* 
    Source - General variables (GLUT only)
 */

    extern cs_view_position csPosition;
    extern cs_view_keyboard csKeyboard;
    extern cs_view_mouse    csMouse;
    extern cs_view_list     csList;
    extern cs_view_path     csPath;

/*
    Source - Initial position reset
 */

    void cs_view_controls_reset( int csFlag, double csMeanLon, double csMeanLat, double csMeanAlt ) {

        /* Declare static variables */
        static double csInitLon = 0.0;
        static double csInitLat = 0.0;
        static double csInitAlt = 0.0;

        /* Check function call flag */
        if ( csFlag == CS_VIEW_CONTROLS_SET ) {

            /* Assign initial position */
            csInitLon = csMeanLon;
            csInitLat = csMeanLat;
            csInitAlt = csMeanAlt;

        }

        /* Reset position according to initial means */
        csPosition.psLon = csInitLon;
        csPosition.psLat = csInitLat;
        csPosition.psAlt = csInitAlt + 1.0;

        /* Reset orientation */
        csPosition.psAX = 90.0;
        csPosition.psAY =  0.0;

        /* Reset boost */
        csPosition.psBst = 1.0;

    }

/*
    Source - Altitude adaptative step function
 */

    double cs_view_controls_altstep( void ) {

        /* Return adaptative step function */
        return( csPosition.psBst * 0.001 * exp( ( log( 10000.0 ) / 1000.0 ) * csPosition.psAlt ) );

    }

