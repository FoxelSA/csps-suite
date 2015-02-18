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

    # include "csps-elphel-gps.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csSrc[256] = { 0 };
        char csDst[256] = { 0 };
        char csInl[256] = { 0 };
        char csOul[256] = { 0 };

        /* Indexation variables */
        long csIndex = 1;

        /* Stream handle variables */
        FILE * csIStream = NULL;
        FILE * csOStream = NULL;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--source"     , "-s" ), argv, csSrc, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--destination", "-d" ), argv, csDst, LC_STRING );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Directory entity enumeration */
            while ( lc_file_enum( csSrc, csInl ) != LC_FALSE ) {

                /* Consider only file entity */
                if ( lc_file_detect( csInl, LC_FILE ) == LC_TRUE ) {

                    /* Check log-file tag */
                    if ( strstr( csInl, LC_PATTERN ) != 0 ) {

                        /* Display information */
                        fprintf( LC_OUT, "Decimating : %s\n", basename( csInl ) );

                        /* Construct output file name */
                        sprintf( csOul, "%s/log-container.log-%05li", csDst, csIndex ++ );

                        /* Create input stream */
                        if ( ( csIStream = fopen( csInl, "rb" ) ) != NULL ) {

                            /* Create output stream */
                            if ( ( csOStream = fopen( csOul, "wb" ) ) != NULL ) {

                                /* Log-file GPS processing */
                                fprintf( LC_OUT, "    %lu GPS-event(s) lost\n", cs_elphel_gps_process( csIStream, csOStream ) );

                            /* Display message */
                            } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( csOul ) ); }

                            /* Close input stream */
                            fclose( csIStream );

                        /* Display message */
                        } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( csInl ) ); }

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Logs-file GPS decimation
*/

    unsigned long cs_elphel_gps_process( 

        FILE * const csIStream, 
        FILE * const csOStream 

    ) {

        /* Records buffer stack variables */
        lp_Byte_t csStackSent[CS_NTYPE][LC_RECORD] = { { 0 } };

        /* Records types stack variables */
        lp_Byte_t csStackType[CS_NTYPE] = { 0 };

        /* Records buffer variables */
        lp_Byte_t csBuffer[LC_RECORD] = { 0 };

        /* Timestamps variables */
        lp_Time_t csTimer = 0;
        lp_Time_t csTimea = 0;
        lp_Time_t csTimeb = 0;
        lp_Time_t csTimec = 0;

        /* Parsing variables */
        unsigned long csParse = 0;

        /* Sentence type variables */
        unsigned long csType  = 0;

        /* Decimation stack variables */
        unsigned long csStack = 0;
        unsigned long csRepet = 0;

        /* Returned value variables */
        unsigned long csCount = 0;

        /* Content parsing */
        while ( fread( csBuffer, 1, LC_RECORD, csIStream ) == LC_RECORD ) {

            /* Event type verification */
            if ( LC_EDM( csBuffer, LC_GPS ) ) {

                /* Memorize very first timestamp */
                if ( csTimec == 0 ) csTimec = LC_TSR( csBuffer );
               
                /* Detect sentence type */
                csType = ( csBuffer[8] & 0x0F );

                /* Sentence type detection */
                if ( csType == LP_NMEA_IDENT_GGA ) {

                    /* Initial bloc skipping verification */
                    if ( lp_timestamp_eq( csTimea, csTimec ) == LC_FALSE ) {

                        /* Check stack state */
                        if ( csStack == CS_NTYPE ) {

                            /* Bloc validation verification */
                            if ( cs_elphel_gps_bloc( csStackType ) == LC_TRUE ) {

                                /* Check reference timestamp repetition */
                                if ( lp_timestamp_eq( csTimea, csTimeb ) == LC_FALSE ) {

                                    /* Update reference memory */
                                    csTimeb = csTimea;

                                    /* Reset repetition condition */
                                    csRepet = 0;

                                } else {

                                    /* Update repetition counter */
                                    csRepet ++;

                                }

                                /* Repetition overflow */
                                if ( csRepet < CS_REPET ) {

                                    /* Rebuild timestamp */
                                    csTimer = cs_elphel_gps_timestamp( csTimea, csRepet );

                                    /* Records exportation loop */
                                    for ( csParse = 0; csParse < CS_NTYPE; csParse ++ ) {

                                        /* Override record header */
                                        cs_elphel_gps_header( ( lp_Time_t * ) csStackSent[csParse], csTimer );

                                        /* Export stacked record */
                                        fwrite( csStackSent[csParse], 1, LC_RECORD, csOStream );

                                    }

                                }

                            /* Update decimation count */
                            } else { csCount += csStack; }

                        /* Update decimation count */
                        } else { csCount += csStack; }

                    /* Update decimation count */
                    } else { csCount += csStack; }

                    /* Reset stack state */
                    csStack = 0;

                    /* Reset stack content */
                    memset( ( void * ) csStackType, 0, CS_NTYPE );

                /* Sentence type detection */
                } else if ( csType == LP_NMEA_IDENT_RMC ) {

                    /* Read record timestamp */
                    csTimea = LC_TSR( csBuffer );

                }

                /* Check stack state */
                if ( csStack < CS_NTYPE ) {

                    /* Logging sentence type */
                    csStackType[csType] ++;

                    /* Push buffer content */
                    for ( csParse = 0; csParse < LC_RECORD; csParse ++ ) {

                        /* Bytes copy */
                        csStackSent[csStack][csParse] = csBuffer[csParse];

                    }

                }

                /* Update stack */
                csStack ++;

            } else {

                /* Export record buffer */
                fwrite( csBuffer, 1, LC_RECORD, csOStream );

            }

        }

        /* Return decimation count */
        return( csCount );

    }

/*
    Source - NMEA sentence block validation
 */

    int cs_elphel_gps_bloc( 

        lp_Byte_t const * const csBlock

    ) {

        /* Returned value variables */
        int csReturn = LC_TRUE;

        /* Parsing variables */
        int csParse = 0;

        /* Validation loop */
        while ( ( csParse < CS_NTYPE ) && ( csReturn == LC_TRUE ) ) {

            /* Validation condition */
            if ( csBlock[csParse++] != 1 ) csReturn = LC_FALSE;

        }

        /* Return results */
        return( csReturn );

    }

/*
    Source - GPS timestamp reconstruction
*/

    lp_Time_t cs_elphel_gps_timestamp( 

        lp_Time_t     const csReference,
        unsigned long const csRepet 

    ) {

        /* Computation buffer variables */
        lp_Time_t csReturn = csRepet * 200000lu;

        /* Compute reconstructed timestamp */
        return( lp_timestamp_add( csReference, lp_timestamp_compose( csReturn / 1000000, csReturn % 1000000 ) ) );

    }

/*
    Source - Record header override
*/

    void cs_elphel_gps_header( 

        lp_Time_t * const csHeader, 
        lp_Time_t   const csTime 

    ) {

        /* Override record header */
        ( * csHeader ) = csTime | ( ( * csHeader ) & 0x00000000FFF00000llu );

    }

