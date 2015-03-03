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

    # include "csps-elphel-merge.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Records buffer variables */
        lp_Byte_t csBuffer[LC_RECORD] = { 0 };

        /* Software path variables */
        char csSrc[256] = { 0 };
        char csDst[256] = { 0 };
        char csFil[256] = { 0 };
        char csExp[256] = { 0 };

        /* Stream variables */
        FILE * csiStream = NULL;
        FILE * csoStream = NULL;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--source"     , "-s" ), argv, csSrc, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--destination", "-d" ), argv, csDst, LC_STRING );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Compose output file name */
            sprintf( csExp, "%s/log-container.log-00001", csDst );

            /* Create and check output stream */
            if ( ( csoStream = fopen( csExp, "wb" ) ) == NULL ) {

                /* Display message */
                fprintf( LC_ERR, "Error : unable to access %s\n", basename( csExp ) );

            } else {

                /* Display information */
                fprintf( LC_OUT, "Merging : %s\n", basename( csExp ) );

                /* Directory entity enumeration */
                while ( lc_file_enum( csSrc, csFil ) != LC_FALSE ) {

                    /* Consider only file entity */
                    if ( lc_file_detect( csFil, LC_FILE ) == LC_TRUE ) {

                        /* Check log-file tag */
                        if ( strstr( csFil, LC_PATTERN ) != 0 ) {

                            /* Create and check input stream */
                            if ( ( csiStream = fopen( csFil, "rb" ) ) == NULL ) {

                                /* Display message */
                                fprintf( LC_ERR, "Error : unable to access %s\n", basename( csFil ) );

                            } else {

                                /* Display information */
                                fprintf( LC_OUT, "    %s\n", basename( csFil ) );

                                /* Copy records buffer */
                                while ( fread( csBuffer, 1, LC_RECORD, csiStream ) == LC_RECORD ) {

                                    /* Export record buffer */
                                    fwrite( csBuffer, 1, LC_RECORD, csoStream );

                                }

                                /* Close input stream */
                                fclose( csiStream );

                            }

                        }

                    }

                }

                /* Close output stream */
                fclose( csoStream );

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

