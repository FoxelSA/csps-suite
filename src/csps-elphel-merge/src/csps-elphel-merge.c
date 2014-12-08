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

    # include "csps-elphel-merge.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Software path variables */
        char csSrc[256] = { 0 };
        char csDst[256] = { 0 };
        char csFil[256] = { 0 };
        char csExp[256] = { 0 };

        /* Stack size variables */
        unsigned int csIndex = 0;

        /* Stack decimation variables */
        signed   int csAppend = 0;

        /* Parsing variables */
        unsigned int csParse = 0;

        /* Selection variables */
        unsigned int csSelect = 0;

        /* Appending flag variables */
        unsigned int csFlag = LC_FALSE;

        /* Logs-files stack variables */
        cs_Descriptor_t csStack[8192];

        /* Records buffer variables */
        lp_Byte_t csBuffer[LC_RECORD] = { 0 };

        /* Timestamp variables */
        lp_Time_t csPush   = 0;
        lp_Time_t csLKnown = 0;

        /* Stream variables */
        FILE * csIStream = NULL;
        FILE * csOStream = NULL;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv,  "--source"     , "-s" ), argv, csSrc, LC_STRING );
        lc_stdp( lc_stda( argc, argv,  "--destination", "-d" ), argv, csDst, LC_STRING );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Display information */
            fprintf( LC_OUT, "Stacking for merge :\n" );

            /* Directory entity enumeration */
            while ( lc_file_enum( csSrc, csFil ) != LC_FALSE ) {

                /* Consider only file entity */
                if ( lc_file_detect( csFil, LC_FILE ) == LC_TRUE ) {

                    /* Check log-file tag */
                    if ( strstr( csFil, LC_PATTERN ) != 0 ) {

                        /* Assign logs-file path */
                        strcpy( csStack[csIndex].dsName, csFil );

                        /* Initialize appending flag */
                        csStack[csIndex].dsFlag = LC_FALSE;

                        /* Extract logs-file timestamp extremums */
                        if ( ( csStack[csIndex].dsFirst = cs_elphel_merge_first( csFil ) ) != 0 ) {

                            /* Display information */
                            fprintf( LC_OUT, "    %s\n", basename( csFil ) );

                            /* Update stack size */
                            csAppend = ( ++ csIndex ); 

                        }

                    }

                }

            }

            /* Compose output file name */
            sprintf( csExp, "%s/log-container.log-00001", csDst );

            /* Display information */
            fprintf( LC_OUT, "Merge : %s\n", basename( csExp ) );

            /* Create output stream handle */
            if ( ( csOStream = fopen( csExp, "wb" ) ) != NULL ) {

                /* Appending loop */
                while ( csAppend > 0 ) {

                    /* Push infinit time */
                    csPush = lp_Time_s( CS_INFTY );

                    /* Reset selection */
                    csSelect = csIndex;

                    /* Search oldest segment */
                    for ( csParse = 0; csParse < csIndex; csParse ++ ) {

                        /* Check stack element flag */
                        if ( csStack[csParse].dsFlag == LC_FALSE ) {

                            /* Compare timestamp */
                            if ( lp_timestamp_ge( csPush, csStack[csParse].dsFirst ) == LC_TRUE ) {

                                /* Update search timestamp */
                                csPush = csStack[csParse].dsFirst;

                                /* Update selection stack index */
                                csSelect = csParse;

                            }

                        }

                    }

                    /* Display append information */
                    fprintf( LC_OUT, "    %s\n", basename( csStack[csSelect].dsName ) );

                    /* Update selection state */
                    csStack[csSelect].dsFlag = LC_TRUE;

                    /* Appending condition flag reset */
                    csFlag = LC_FALSE;

                    /* Create input stream */
                    if ( ( csIStream = fopen( csStack[csSelect].dsName, "rb" ) ) != NULL ) {

                        /* Sub-appending loop */
                        while ( fread( csBuffer, 1, LC_RECORD, csIStream ) == LC_RECORD ) {

                            /* Check record event type */
                            if ( LC_EDM( csBuffer, LC_IMU ) ) {

                                /* Appending condition trigger */
                                if ( ( lp_timestamp_ge( csLKnown, lp_timestamp( ( lp_Void_t * ) csBuffer ) ) ) == LC_FALSE ) {

                                    /* Set appending condition flag */
                                    csFlag = LC_TRUE;

                                }

                            }

                            /* Append condition verification */
                            if ( csFlag == LC_TRUE ) {

                                /* Append records */
                                fwrite( csBuffer, 1, LC_RECORD, csOStream );

                                /* Memorize last known timestamp */
                                csLKnown = lp_timestamp( ( lp_Void_t * ) csBuffer );

                            }

                        }

                        /* Close input stream */
                        fclose( csIStream );

                    } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( csStack[csSelect].dsName ) ); }

                    /* Update appending loop state */
                    csAppend --;

                }

                /* Close output stream */
                fclose( csOStream );

            /* Display message */
            } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( csExp ) ); }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - First timestamp detection
 */

    lp_Time_t cs_elphel_merge_first( char const * const csFile ) {

        /* Records buffer variables */
        lp_Byte_t csBuffer[LC_RECORD] = { 0 };

        /* Returned value variables */
        lp_Time_t csFirst = 0;

        /* Create input stream */
        FILE * csStream = fopen( csFile, "rb" );

        /* Check stream creation */
        if ( csStream != NULL ) {

            /* Parse input stream */
            while ( ( csFirst == 0 ) && ( fread( csBuffer, 1, LC_RECORD, csStream ) == LC_RECORD ) ) {

                /* Event type detection and timestamp assignation */
                if ( LC_EDM( csBuffer, LC_IMU ) ) csFirst = lp_timestamp( ( lp_Void_t * ) csBuffer );

            }

            /* Close input stream */
            fclose( csStream );

        /* Display message */
        } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( ( char * ) csFile ) ); }

        /* Return first timestamp */
        return( csFirst );

    }

