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
        lc_stdp( lc_stda( argc, argv, "--source"     , "-s" ), argv, csSrc, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--destination", "-d" ), argv, csDst, LC_STRING );

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
        cs_Sort_t * csHead = NULL;
        cs_Sort_t * csLast = NULL;
        cs_Sort_t * csNear = NULL;
        cs_Sort_t * csRead = NULL;
        cs_Sort_t * csTail = NULL;

        /* Parsing variables */
        long csSize  = 0;

        /* Timestamp variables */
        lp_Time_t csTime = 0;

        /* Streams variables */
        FILE * csiStream = NULL;
        FILE * csoStream = NULL;

        /* Create and check input stream */
        if ( ( csiStream = fopen( csiFile, "r" ) ) != NULL ) {

            /* Retrieve input stream size */
            csSize = lc_file_size( csiFile ) / 64;

            /* Create and check input stream */
            if ( ( csoStream = fopen( csoFile, "w" ) ) != NULL ) {

                /* Allocating sorting array memory */
                if ( ( csTail = ( csSort = ( cs_Sort_t * ) malloc( csSize * sizeof( cs_Sort_t ) ) ) ) != NULL ) {

                    /* Input stream parsing loop */
                    while ( fread( csBuffer, 1, LC_RECORD, csiStream ) == LC_RECORD ) {

                        /* Retrieve record timestamp */
                        csTime = LC_TSR( csBuffer );

                        /* Backward parser */
                        csNear = NULL;
                        csRead = csLast;

                        long i = 0;

                        fprintf( stderr, "%010" lp_Time_p ".%06" lp_Time_p " - ", lp_timestamp_sec( csTime ), lp_timestamp_usec( csTime ) );

                        /* backward insertion */
                        while ( ( csNear == NULL ) && ( csRead != NULL ) ) {

                            /* Insertion position search */
                            if ( lp_timestamp_ge( csTime, csRead->srTime ) == LP_TRUE ) {

                                /* Select nearest */
                                csNear = csRead;

                            } else {

                                /* Continue backward insertion */
                                csRead = csRead->srp;

                            }

                            i ++; 

                        }

                        fprintf( stderr, "Found in %li\n", i );

                        /* Insert event */
                        csTail->srTime   = csTime;
                        csTail->srOffset = ftell( csiStream ) - LC_RECORD;

                        /* Update previous link */
                        if ( csNear == NULL ) {

                            /* Update links */
                            csTail->srp = NULL;
                            csTail->srn = csHead;

                            /* Check boundary */
                            if ( csHead == NULL ) {

                                /* Update boundary */
                                csLast = csTail; 

                            } else {

                                /* Update link */
                                csHead->srp = csTail;

                            }

                            /* Update boundary */
                            csHead = csTail;

                        } else {

                            /* Update link */
                            csTail->srp = csNear;

                            /* Check boundary */
                            if ( csNear->srn == NULL ) {

                                /* Update link */
                                csTail->srn = NULL;

                                /* Update boundary */
                                csLast = csTail;

                            } else {

                                /* Update link */
                                csTail->srn = csNear->srn;

                                /* Update next link */
                                ( ( cs_Sort_t * ) csNear->srn )->srp = csTail;

                            }

                            /* Update previous link */
                            csNear->srn = csTail;

                        }

                        /* Update */
                        csTail += 1;

                    }

                    /* Exportation of sorted events */
                    while ( csHead != NULL ) {

                        /* Setting offset in input stream */
                        fseek( csiStream, csHead->srOffset , SEEK_SET );

                        /* Read input stream */
                        if ( fread( csBuffer, 1, LC_RECORD, csiStream ) == LC_RECORD ) {

                            /* Write output stream */
                            fwrite( csBuffer, 1, LC_RECORD, csoStream );

                        }

                        /* Update parser */
                        csHead = csHead->srn;

                    } 

                    /* Unallocate sorting array memory */
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

/*
    Source - Mathematical functions
 */

    unsigned long cs_elphel_sort_ngoep2( 

        unsigned long const csValue 

    ) {

        /* Shifted value variables */
        unsigned long csShift = csValue;

        /* Scale value variables */
        unsigned long csScale = 0;

        /* Search largest scale */
        while ( csShift != 0 ) { csShift = csShift >> 1; csScale ++; }

        /* Return results */
        return( ( ( 1 << ( csScale - 1 ) ) == csValue ) ? 1 << ( csScale - 1 ) : 1 << csScale );

    }

