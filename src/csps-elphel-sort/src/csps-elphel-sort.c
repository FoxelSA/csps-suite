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
        cs_Sort_t * csInit = NULL;
        cs_Sort_t * csLast = NULL;
        cs_Sort_t * csPush = NULL;
        cs_Sort_t * csAdds = NULL;

        /* Insertion flag variables */
        int csFlag = LC_FALSE;

        /* Parsing variables */
        size_t csParse = 0;
        size_t csSize  = 0;

        /* Timestamp variables */
        lp_Time_t csTime = 0;

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
                if ( ( csAdds = ( csSort = ( cs_Sort_t * ) malloc( csSize * sizeof( cs_Sort_t ) ) ) ) != NULL ) {

                    /* Input stream parsing loop */
                    while ( fread( csBuffer, 1, LC_RECORD, csiStream ) == LC_RECORD ) {

                        /* Retrieve record timestamp */
                        csTime = LC_TSR( csBuffer );

                        /* Backward parser */
                        csPush = csLast;

                        /* Insertion flag */
                        csFlag = LC_FALSE;

                        /* backward insertion */                            
                        while ( ( csFlag == LC_FALSE ) && ( csPush != NULL ) ) {

                            /* Insertion position search */
                            if ( lp_timestamp_ge( csTime, csPush->srTime ) == LP_TRUE ) {

                                /* Update flag */
                                csFlag = LC_TRUE;

                            } else {

                                /* Update parser */
                                csPush = csPush->srp;

                            }

                        }

                        /* Insert event */
                        csAdds->srTime   = csTime;
                        csAdds->srOffset = ftell( csiStream ) - LC_RECORD;

                        /* Update previous link */
                        if ( csPush == NULL ) {

                            /* Update links */
                            csAdds->srp = NULL;
                            csAdds->srn = csInit;

                            /* Check boundary */
                            if ( csInit == NULL ) csLast = csAdds;

                            /* Update boundary */
                            csInit = csAdds;

                        } else {

                            /* Update link */
                            csAdds->srp = csPush;

                            /* Check boundary */
                            if ( csPush->srn == NULL ) {

                                /* Update link */
                                csAdds->srn = NULL;

                                /* Update boundary */
                                csLast = csAdds;

                            } else {

                                /* Update link */
                                csAdds->srn = csPush->srn;

                                /* Update next link */
                                ( ( cs_Sort_t * ) csPush->srn )->srp = csAdds;

                            }

                            /* Update previous link */
                            csPush->srn = csAdds;

                        }

                        /* Update */
                        csAdds += 1;

                    }

                    /* Exportation of sorted events */
                    do {

                        /* Setting offset in input stream */
                        fseek( csiStream, csInit->srOffset , SEEK_SET );

                        /* Read input stream */
                        if ( fread( csBuffer, 1, LC_RECORD, csiStream ) == LC_RECORD ) {

                            /* Write output stream */
                            fwrite( csBuffer, 1, LC_RECORD, csoStream );

                        }

                        /* Update parser */
                        csInit = csInit->srn;

                    } while ( csInit != NULL );

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

