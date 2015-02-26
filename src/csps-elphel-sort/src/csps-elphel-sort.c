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
        cs_Sort_t * csrSort = NULL;
        cs_Sort_t * cssSort = NULL;
        cs_Sort_t * csaSwap = NULL;

        /* Merge-sort variables */
        unsigned long csScale = 0;
        unsigned long csSteps = 2;

        /* Scale parsing variables */
        unsigned long csrIndex = 0;
        unsigned long cssIndex = 0;
        unsigned long csaIndex = 0;
        unsigned long csaLimit = 0;
        unsigned long csbIndex = 0;
        unsigned long csbLimit = 0;

        /* Switch flag variables */
        int csfSort = 0;

        /* Parsing variables */
        unsigned long csParse = 0;
        unsigned long csSize  = 0;

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
                if ( ( csrSort = ( cs_Sort_t * ) malloc( csSize * sizeof( cs_Sort_t ) * 2 ) ) != NULL ) {

                    /* Creating secondary pointer */
                    cssSort = csrSort + csSize;

                    /* Computing merge-sort scale */
                    csScale = cs_elphel_sort_ngoep2( csSize );

                    /* Reset size */
                    csSize = 0;

                    /* Creating records descriptors */
                    while ( fread( csBuffer, 1, LC_RECORD, csiStream ) == LC_RECORD ) {

                        /* Assign record timestamp */
                        csrSort[csSize].srTime = LC_TSR( csBuffer );

                        /* Assign record offset */
                        csrSort[csSize].srSeek = ftell( csiStream ) - LC_RECORD;

                        /* Update pointer and size */
                        csSize ++; 

                    }

                    /* Merge sort algorithm - sequential implementation */
                    while ( csSteps <= csScale ) {

                        /* Scale parsing reset */
                        cssIndex = 0;
                        csrIndex = 0;

                        /* Scale parsing loop */
                        while ( csrIndex < csSize ) {

                            /* Merge procedure reset */                            
                            csaIndex = csrIndex;
                            csaLimit = csrIndex + ( csSteps >> 1 );
                            csbIndex = csaLimit;
                            csbLimit = csrIndex + ( csSteps );

                            /* Head limit correction */
                            csaLimit = ( csaLimit <= csSize ) ? csaLimit : csSize;
                            csbLimit = ( csbLimit <= csSize ) ? csbLimit : csSize;

                            /* Merge procedure */
                            while ( ( csaIndex < csaLimit ) || ( csbIndex < csbLimit ) ) {

                                /* Check sequences limits */
                                if ( csaIndex >= csaLimit ) {

                                    /* Update swicth */
                                    csfSort = LP_FALSE;

                                } else
                                if ( csbIndex >= csbLimit ) {

                                    /* Update swicth */
                                    csfSort = LP_TRUE;

                                } else {

                                    /* Update switch */
                                    csfSort = lp_timestamp_ge( csrSort[csbIndex].srTime, csrSort[csaIndex].srTime );

                                }

                                /* Merge node */
                                if ( csfSort == LP_TRUE ) {

                                    /* Merge element */
                                    cssSort[cssIndex].srTime = csrSort[csaIndex].srTime;
                                    cssSort[cssIndex].srSeek = csrSort[csaIndex].srSeek;

                                    /* Update head */
                                    csaIndex ++;

                                } else {

                                    /* Merge element */
                                    cssSort[cssIndex].srTime = csrSort[csbIndex].srTime;
                                    cssSort[cssIndex].srSeek = csrSort[csbIndex].srSeek;

                                    /* Update head */
                                    csbIndex ++;

                                }

                                /* Update secondary index */
                                cssIndex ++;

                            }

                            /* Update main index */
                            csrIndex += csSteps;

                        }

                        /* Update scale */
                        csSteps *= 2;

                        /* Swap array */
                        csaSwap = csrSort;
                        csrSort = cssSort;
                        cssSort = csaSwap;

                    }

                    /* Exportation of sorted events */
                    while ( csParse < csSize ) {

                        /* Setting offset in input stream */
                        fseek( csiStream, csrSort[csParse].srSeek, SEEK_SET );

                        /* Read input stream */
                        if ( fread( csBuffer, 1, LC_RECORD, csiStream ) == LC_RECORD ) {

                            /* Write output stream */
                            fwrite( csBuffer, 1, LC_RECORD, csoStream );

                        }

                        /* Update parser */
                        csParse ++;

                    } 

                    /* Unallocate sorting array memory */
                    free( csrSort < cssSort ? csrSort : cssSort );

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

