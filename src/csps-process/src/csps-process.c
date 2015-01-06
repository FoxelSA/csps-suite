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


/* 
    Source - Includes
 */

    # include "csps-process.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csPath[256] = { 0 };
        char csTopo[256] = { 0 };

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--path"    , "-p" ), argv, csPath , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--topology", "-t" ), argv, csTopo , LC_STRING );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Check processing directory */
            if ( lc_file_detect( csPath, LC_DIRECTORY ) == LC_TRUE ) {

                /* Check processing topology file */
                if ( lc_file_detect( csTopo, LC_FILE ) == LC_TRUE ) {

                    /* Display message */
                    fprintf( LC_OUT, "Processing : %s using %s topology ...", basename( csPath ), basename( csTopo ) );

                    /* CSPS processing */
                    lp_system( csPath, csTopo );

                    /* Display message */
                    fprintf( LC_OUT, "Done\n" );

                /* Display message */
                } else { fprintf( LC_ERR, "Error : unable to access %s file\n", csTopo ); }

            /* Display message */
            } else { fprintf( LC_ERR, "Error : unable to access %s directory\n", csPath ); }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

