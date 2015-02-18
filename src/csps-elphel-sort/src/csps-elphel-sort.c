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

    # include "csps-elphel-sort.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csSrc[256] = { 0 };
        char csDst[256] = { 0 };
        char csEnt[256] = { 0 };
        char csExp[256] = { 0 };

        /* Validation index variables */
        long csIndex = 1;

        /* Minimum size variables */
        long csMinimum = 317696; /* ~ 2 Seconds */

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--source"      , "-s" ), argv,   csSrc     , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--destination" , "-d" ), argv,   csDst     , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--minimum-size", "-m" ), argv, & csMinimum , LC_LONG   );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Directory entity enumeration */
            while ( lc_file_enum( csSrc, csEnt ) != LC_FALSE ) {

                /* Consider only file entity */
                if ( lc_file_detect( csEnt, LC_FILE ) == LC_TRUE ) {

                    /* Check logs-file tag */
                    if ( strstr( csEnt, LC_PATTERN ) != 0 ) {

                        /* Validation on file size */
                        if ( lc_file_size( csEnt ) > csMinimum ) {

                            /* Build validated temporary logs-file path */
                            sprintf( csExp, "%s/log-container.log-%05li", csDst, csIndex ++ );

                            /* Display information */
                            fprintf( LC_OUT, "Validating : %s\n    Exported in %s\n", basename( csEnt ), basename( csExp ) );


                        } else {

                            /* Display information */
                            fprintf( LC_OUT, "Validating : %s\n    Not exported\n", basename( csEnt ) );

                        }

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }
