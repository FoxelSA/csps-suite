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

    # include "csps-elphel-decompose.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csSrc[256] = { 0 };
        char csDst[256] = { 0 };
        char csFil[256] = { 0 };

        /* Decompostion condition variables */
        double csInterval = 1.0;

        /* Decomposition index variables */
        long csIndex = 1;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv,  "--source"     , "-s" ), argv,   csSrc     , LC_STRING );
        lc_stdp( lc_stda( argc, argv,  "--destination", "-d" ), argv,   csDst     , LC_STRING );
        lc_stdp( lc_stda( argc, argv,  "--interval"   , "-i" ), argv, & csInterval, LC_DOUBLE );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Directory entity enumeration */
            while ( lc_file_enum( csSrc, csFil ) != LC_FALSE ) {

                /* Consider only file entity */
                if ( lc_file_detect( csFil, LC_FILE ) == LC_TRUE ) {

                    /* Check log-file tag */
                    if ( strstr( csFil, LC_PATTERN ) != 0 ) {

                        /* Decomposition process */
                        csIndex = cs_elphel_decompose( csFil, csDst, csIndex, csInterval );

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Logs-file decomposer
*/

    int cs_elphel_decompose( 

        char   const * const csLog, 
        char   const * const csDirectory, 
        int                  csIndex, 
        double const         csInterval 

    ) {

        /* Decomposition path variables */
        char csSeg[256] = { 0 };

        /* Records buffer variables */
        lp_Byte_t csRec[LC_RECORD] = { 0 };

        /* Timestamp variables */
        lp_Time_t cspTime = 0;
        lp_Time_t cscTime = 0;

        /* Stream handle variables */
        FILE * csIStream = NULL;
        FILE * csOStream = NULL;

        /* Create and check input stream creation */
        if ( ( csIStream = fopen( csLog, "rb" ) ) != NULL ) {

            /* Compose initial decomposition segment path */
            sprintf( csSeg, "%s/log-container.log-%05i", csDirectory, csIndex ++ );

            /* Create and check output stream creation */
            if ( ( csOStream = fopen( csSeg, "wb" ) ) != NULL ) { 

                /* Display decomposition information */
                fprintf( LC_OUT, "Decomposing : %s\n    %s\n", basename( ( char * ) csLog ), basename( csSeg ) );

                /* Parsing input stream */
                while ( fread( csRec, 1, LC_RECORD, csIStream ) == LC_RECORD ) {

                    /* Read record timestamp */
                    cscTime = LC_TSR( csRec );

                    /* Check splitting condition */
                    if ( ( cspTime != 0 ) && ( lp_timestamp_float( lp_timestamp_diff( cscTime, cspTime ) ) > csInterval ) ) {

                        /* Update decomposition segment path */
                        sprintf( csSeg, "%s/log-container.log-%05i", csDirectory, csIndex ++ );

                        /* Reset output stream */
                        fclose( csOStream ); csOStream = fopen( csSeg, "wb" );

                        /* Display decomposition information */
                        fprintf( LC_OUT, "    %s\n", basename( csSeg ) );

                    }

                    /* Memorize previous timestamp */
                    cspTime = cscTime;

                    /* Write recored in output stream */
                    if ( csOStream != NULL ) fwrite( csRec, 1, LC_RECORD, csOStream );

                }

                /* Close output stream */
                if ( csOStream != NULL ) fclose( csIStream );

            /* Display message */
            } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( csSeg ) ); }

            /* Close input stream */
            fclose( csOStream );

        /* Display message */
        } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( ( char * ) csLog ) ); }

        /* Return decomposition index */
        return( csIndex );

    }

