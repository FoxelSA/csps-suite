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

    # include "csps-elphel-audit.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Paths variables */
        char csPath[256] = { 0 };
        char csFile[256] = { 0 };

        /* Stream variables */
        FILE * csStream = NULL;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--path", "-p" ), argv, csPath, LC_STRING );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Directory entity enumeration */
            while ( lc_file_enum( csPath, csFile ) != LC_FALSE ) {

                /* Check entity for files */
                if ( lc_file_detect( csFile, LC_FILE ) == LC_TRUE ) {

                    /* Check logs-files tag */
                    if ( strstr( csFile, LC_PATTERN ) != 0 ) {

                        /* Create stream handle */
                        if ( ( csStream = fopen( csFile, "rb" ) ) != NULL ) {

                            /* Audit logs-file */
                            cs_elphel_audit( csFile, csStream );

                        /* Display message */
                        } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( csFile ) ); }

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Audit procedure
 */

    void cs_elphel_audit ( 

        char const * const csFile, 
        FILE       * const csStream 

    ) {

        /* Logs-file size variables */
        size_t csRead = 0;
        size_t csSize = 0;

        /* Records buffer variables */
        lp_Byte_t csBuffer[LC_RECORD] = { 0 };

        /* Timestamps variables */
        lp_Time_t csDEVstep = lp_Time_s( 0 );
        lp_Time_t csIMUtime = lp_Time_s( 0 );
        lp_Time_t csIMUinit = lp_Time_s( 0 );
        lp_Time_t csIMUprev = lp_Time_s( 0 );
        lp_Time_t csIMUstpi = lp_Time_s( 0xFFFFFFFFFFFFFFFF );
        lp_Time_t csIMUstpm = lp_Time_s( 0 );
        lp_Time_t csCAMtime = lp_Time_s( 0 );
        lp_Time_t csCAMinit = lp_Time_s( 0 );
        lp_Time_t csCAMmain = lp_Time_s( 0 );
        lp_Time_t csCAMdiff = lp_Time_s( 0 );
        lp_Time_t csCAMprev = lp_Time_s( 0 );
        lp_Time_t csCAMstpi = lp_Time_s( 0xFFFFFFFFFFFFFFFF );
        lp_Time_t csCAMstpm = lp_Time_s( 0 );
        lp_Time_t csGPStime = lp_Time_s( 0 );
        lp_Time_t csGPSinit = lp_Time_s( 0 );
        lp_Time_t csGPSprev = lp_Time_s( 0 );
        lp_Time_t csGPSstpi = lp_Time_s( 0xFFFFFFFFFFFFFFFF );
        lp_Time_t csGPSstpm = lp_Time_s( 0 );

        /* Reset initial position */
        rewind( csStream );

        /* Parse file */
        while ( ( csRead = fread( csBuffer, 1, LC_RECORD, csStream ) ) == LC_RECORD ) {

            /* Compute logs-file size */
            csSize += csRead;

            /* Event type detection */
            if ( LC_EDM( csBuffer, LC_IMU ) ) {

                /* Extract current timestamp */
                csIMUtime = LC_TSR( csBuffer );

                /* Initial timestamp check */
                if ( csIMUinit == lp_Time_s( 0 ) ) { 

                    /* Memorize initial timestamp */
                    csIMUinit = csIMUtime; 

                } else {

                    /* Compute step value */
                    csDEVstep = lp_timestamp_diff( csIMUprev, csIMUtime );

                    /* Memorize step extremums */
                    if ( lp_timestamp_ge( csDEVstep, csIMUstpm ) == LP_TRUE ) csIMUstpm = csDEVstep;
                    if ( lp_timestamp_ge( csIMUstpi, csDEVstep ) == LP_TRUE ) csIMUstpi = csDEVstep;

                }

                /* Memorize current timestamp */
                csIMUprev = csIMUtime;

            } else 
            if ( LC_EDM( csBuffer, LC_MAS ) ) {

                /* Extract current timestamp */
                csCAMtime = LC_TSR( csBuffer );

                /* Initial timestamp check */
                if ( csCAMinit == lp_Time_s( 0 ) ) { 

                    /* Read camera record timestamp */
                    csCAMmain = LC_TSR( csBuffer + lp_Size_s( 8 ) );

                    /* Compute difference */
                    csCAMdiff = lp_timestamp_diff( csCAMmain, csCAMtime );

                    /* Memorize initial timestamp */
                    csCAMinit = csCAMtime;

                } else {

                    /* Compute step value */
                    csDEVstep = lp_timestamp_diff( csCAMprev, csCAMtime );

                    /* Memorize step extremums */
                    if ( lp_timestamp_ge( csDEVstep, csCAMstpm ) == LP_TRUE ) csCAMstpm = csDEVstep;
                    if ( lp_timestamp_ge( csCAMstpi, csDEVstep ) == LP_TRUE ) csCAMstpi = csDEVstep;

                }

                /* Memorize current timestamp */
                csCAMprev = csCAMtime;

            } else 
            if ( LC_EDM( csBuffer, LC_GPS ) ) {

                /* Extract current timestamp */
                csGPStime = LC_TSR( csBuffer );

                /* Initial timestamp check */
                if ( csGPSinit == lp_Time_s( 0 ) ) {

                    /* Memorize initial timestamp */
                    csGPSinit = csGPStime;

                } else {

                    /* Compute step value */
                    csDEVstep = lp_timestamp_diff( csGPSprev, csGPStime );

                    /* Memorize step extremums */
                    if ( lp_timestamp_ge( csDEVstep, csGPSstpm ) == LP_TRUE ) csGPSstpm = csDEVstep;
                    if ( lp_timestamp_ge( csGPSstpi, csDEVstep ) == LP_TRUE ) csGPSstpi = csDEVstep;

                }

                /* Memorize current timestamp */
                csGPSprev = csGPStime;

            }

        }

        /* Finalize logs-file size computation */
        csSize += csRead;

        /* Display information */
        fprintf( LC_OUT, "Auditing file %s\n", basename( ( char * ) csFile ) );

        /* Display information - general */
        fprintf( LC_OUT, "    General information summary\n" );
        fprintf( LC_OUT, "        UTC-begin       : %s\n"         , lc_timestamp_utc( csCAMinit ) );
        fprintf( LC_OUT, "        UTC-end         : %s\n"         , lc_timestamp_utc( csCAMtime ) );
        fprintf( LC_OUT, "        Size of file    : %li Bytes\n"  , csSize                        );
        fprintf( LC_OUT, "        CGC-64          : %li\n"        , csSize % 64                   );
        fprintf( LC_OUT, "        Records count   : %li records\n", csSize >> 6                   );

        /* Display information - Master */
        fprintf( LC_OUT, "    Master events summary\n" );
        fprintf( LC_OUT, "        Range : " LC_TSM " - " LC_TSM "\n", LC_TSD( csCAMinit ), LC_TSD( csCAMtime ) );
        fprintf( LC_OUT, "        Span  : " LC_TSM "\n", LC_TSD( lp_timestamp_diff( csCAMinit, csCAMtime ) ) );
        fprintf( LC_OUT, "        Steps : " LC_TSM " - " LC_TSM "\n", LC_TSD( csCAMstpi ), LC_TSD( csCAMstpm ) );
        fprintf( LC_OUT, "        Synch : " LC_TSM " - " LC_TSM "\n", LC_TSD( csCAMmain ), LC_TSD( csCAMdiff ) );

        /* Display information - IMU */
        fprintf( LC_OUT, "    IMU events summary\n" );
        fprintf( LC_OUT, "        Range : " LC_TSM " - " LC_TSM "\n", LC_TSD( csIMUinit ), LC_TSD( csIMUtime ) );
        fprintf( LC_OUT, "        Span  : " LC_TSM "\n", LC_TSD( lp_timestamp_diff( csIMUinit, csIMUtime ) ) );
        fprintf( LC_OUT, "        Steps : " LC_TSM " - " LC_TSM "\n", LC_TSD( csIMUstpi ), LC_TSD( csIMUstpm ) );

        /* Display information - GPS */
        fprintf( LC_OUT, "    GPS events summary\n" );
        fprintf( LC_OUT, "        Range : " LC_TSM " - " LC_TSM "\n", LC_TSD( csGPSinit ), LC_TSD( csGPStime ) );
        fprintf( LC_OUT, "        Span  : " LC_TSM "\n", LC_TSD( lp_timestamp_diff( csGPSinit, csGPStime ) ) );
        fprintf( LC_OUT, "        Steps : " LC_TSM " - " LC_TSM "\n", LC_TSD( csGPSstpi ), LC_TSD( csGPSstpm ) );

    }

