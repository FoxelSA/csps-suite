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

    # include "csps-elphel-repair.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csSrc[256] = { 0 };
        char csDst[256] = { 0 };
        char csFil[256] = { 0 };
        char csExp[256] = { 0 };

        /* Indexation variables */
        unsigned long csIndex = 1;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--source"     , "-s" ), argv, csSrc, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--destination", "-d" ), argv, csDst, LC_STRING );

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

                        /* Display information */
                        fprintf( LC_OUT, "Repairing : %s\n", basename( csFil ) );

                        /* Construct output file name */
                        sprintf( csExp, "%s/log-container.log-%05li", csDst, csIndex ++ );

                        /* Logs-file repair process */
                        fprintf( LC_OUT, "    %s - %lu event(s) discared\n", basename( csExp ), cs_elphel_repair( csFil, csExp ) );

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Logs-file repair process
*/

    unsigned long cs_elphel_repair(

        char * const csiFile,
        char * const csoFile

    ) {

        /* Record buffer variables */
        lp_Byte_t csBuffer[LC_RECORD] = { 0 };

        /* Discared count variables */
        unsigned long csCount = 0;

        /* Timestamp tracking variables */
        lp_Time_t csimuLast = 0;
        lp_Time_t csmasLast = 0;
        lp_Time_t csothLast = 0;

        /* Stream variables */
        FILE * csiStream = NULL;
        FILE * csoStream = NULL;

        /* Create and check input stream */
        if ( ( csiStream = fopen( csiFile, "rb" ) ) == NULL ) {

            /* Display message */
            fprintf( LC_ERR, "Error : unable to access %s\n", basename( csiFile ) );

        } else {

            /* Create and check input stream */
            if ( ( csoStream = fopen( csoFile, "wb" ) ) == NULL ) {

                /* Display message */
                fprintf( LC_ERR, "Error : unable to access %s\n", basename( csoFile ) );

            } else {

                /* Parsing input stream */
                while ( fread( csBuffer, 1, LC_RECORD, csiStream ) == LC_RECORD ) {

                    /* Checking record validity */
                    if ( cs_elphel_validate_record( csBuffer ) == LC_TRUE ) {

                        /* Detect event type */
                        if ( LC_EDM( csBuffer, LC_IMU ) ) {

                            /* Detect repetition */
                            if ( lp_timestamp_ge( csimuLast, LC_TSR( csBuffer ) ) == LP_FALSE ) {

                                /* Export events */
                                fwrite( csBuffer, 1, LC_RECORD, csoStream );

                                /* Update last-known timestamp */
                                csimuLast = LC_TSR( csBuffer );

                            }

                        } else
                        if ( LC_EDM( csBuffer, LC_GPS ) ) {

                        } else
                        if ( LC_EDM( csBuffer, LC_MAS ) ) {

                            /* Detect repetition */
                            if ( lp_timestamp_ge( csmasLast, LC_TSR( csBuffer ) ) == LP_FALSE ) {

                                /* Export events */
                                fwrite( csBuffer, 1, LC_RECORD, csoStream );

                                /* Update last-known timestamp */
                                csmasLast = LC_TSR( csBuffer );

                            }

                        } else {

                            /* Detect repetition */
                            if ( lp_timestamp_ge( csothLast, LC_TSR( csBuffer ) ) == LP_FALSE ) {

                                /* Export events */
                                fwrite( csBuffer, 1, LC_RECORD, csoStream );

                                /* Update last-known timestamp */
                                csothLast = LC_TSR( csBuffer );

                            }


                        }

                    /* Update discared count */
                    } else { csCount ++; }

                }

                /* Close input stream */
                fclose( csoStream );

            }

            /* Close input stream */
            fclose( csiStream );

        }

        /* Return discared count */
        return ( csCount );

    }

/*
    Source - Record probabilist validation
*/

    int cs_elphel_validate_record(

        lp_Byte_t const * const csBuffer

    ) {

        /* Failsafe check on record header */
        if ( ( ( * ( ( uint64_t * ) csBuffer ) ) & 0x00000000F0F00000 ) == 0 ) {

            /* Failsafe check on record tail */
            if ( * ( ( uint16_t * ) ( csBuffer + 62 ) ) == 0 ) return( LC_TRUE ); else  return( LC_FALSE );

        /* Invalid record */
        } else { return( LC_FALSE ); }

    }

/*
    Source - Logs-file GPS decimation
*/

    unsigned long cs_elphel_gps_process( 

        FILE * const csiStream, 
        FILE * const csoStream 

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
        while ( fread( csBuffer, 1, LC_RECORD, csiStream ) == LC_RECORD ) {

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
                                        fwrite( csStackSent[csParse], 1, LC_RECORD, csoStream );

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
                fwrite( csBuffer, 1, LC_RECORD, csoStream );

            }

        }

        /* Return decimation count */
        return( csCount );

    }

/*
    Source - NMEA sentence block validation
 */

    long cs_elphel_gps_bloc( 

        lp_Byte_t const * const csBlock

    ) {

        /* Returned value variables */
        long csReturn = LC_TRUE;

        /* Parsing variables */
        long csParse = 0;

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

