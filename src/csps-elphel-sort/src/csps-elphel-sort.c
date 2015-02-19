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

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--source"     , "-s" ), argv,   csSrc     , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--destination", "-d" ), argv,   csDst     , LC_STRING );

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

                        /* Build validated logs-file path */
                        sprintf( csExp, "%s/log-container.log-%05li", csDst, csIndex ++ );

                        /* Display information */
                        fprintf( LC_OUT, "Sorting : %s\n    Exported in %s\n", basename( csEnt ), basename( csExp ) );

                        /* Sorting procedure */
                        cs_elphel_sort( csEnt, csExp );

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Sorting procedure
 */

    void cs_elphel_sort(

        char const * const csiFile,
        char const * const csoFile

    ) {

        /* Records buffer variables */
        lp_Byte_t csBuffer[LC_RECORD] = { 0 };

        /* Sorting structure variables */
        cs_Sort_t * csSort = NULL;
        cs_Sort_t * csLast = NULL;
        cs_Sort_t * csPush = NULL;

        /* Timestamp variables */
        lp_Time_t csTime = 0;

        /* Parsing variables */
        size_t csParse = 0;
        size_t csSize  = 0;

        /* Streams variables */
        FILE * csiStream = NULL;
        FILE * csoStream = NULL;

        /* Create and check input stream */
        if ( ( csiStream = fopen( csiFile, "r" ) ) != NULL ) {

            /* Retrieve input stream size */
            csSize = lc_file_size( csiFile );

            /* Create and check input stream */
            if ( ( csoStream = fopen( csoFile, "w" ) ) != NULL ) {

                /* Allocating sorting array memory */
                if ( ( csSort = ( cs_Sort_t * ) malloc( csSize * sizeof( cs_Sort_t ) ) ) != NULL ) {

                    /* Initialize memory structure */
                    for ( csParse = 0; csParse < csSize; csParse ++ ) {

                        /* Clear chain pointers */
                        csSort[0].srp = NULL;
                        csSort[0].srn = NULL;

                    }

                    /* Input stream parsing loop */
                    while ( fread( csBuffer, 1, LC_RECORD, csiStream ) == LC_RECORD ) {

                        /* Retrieve record timestamp */
                        csTime = LC_TSR( csBuffer );

                        /* Insertion procedure */
                        if ( csLast == NULL ) {

                            /* Insert first event */
                            csSort->srTime   = csTime;
                            csSort->srOffset = ftell( csiStream ) - LC_RECORD;

                            /* Update last element */
                            csLast = csSort;

                        } else {
                            
                            /* Insertion backward loop */
                            while ( csPush->srp != NULL ) {

                                /* Insertion check */
                                if ( lp_timestamp_ge( csTime, csPush->srTime ) == LP_TRUE ) {

                                    /* Insert event */
                                    ( csLast + 1 )->srTime   = csTime;
                                    ( csLast + 1 )->srOffset = ftell( csiStream ) - LC_RECORD;

                                    /* Update current node */
                                    ( csLast + 1 )->srn = csPush->srn;
                                    ( csLast + 1 )->srp = csPush;

                                    /* Update previous node */
                                    csPush->srn = csLast + 1;

                                }

                                /* Update backward parser */
                                csPush = csPush->srp;

                            }

                        }

                        /* Setting backward parser */
                        csPush = csLast;

                    }

                    /* Unallocate sorting array memery */
                    free( csSort );

                /* Display message */
                } else { fprintf( LC_ERR, "Error : unable to allocate memory\n" ); }

                /* Close output stream */
                fclose( csoStream );

            /* Display message */
            } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( ( char * ) csoFile ) ); }

            /* Close input stream */
            fclose( csiStream );

        /* Display message */
        } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( ( char * ) csiFile ) ); }

    }

