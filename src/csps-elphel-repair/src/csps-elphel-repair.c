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

                        /* Logs-file repair procedure */
                        fprintf( LC_OUT, "    %s - %lu event(s) discared\n", basename( csExp ), cs_elphel_repair( csFil, csExp ) );

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Logs-file repair procedure
*/

    unsigned long cs_elphel_repair(

        char * const csiFile,
        char * const csoFile

    ) {

        /* Record buffer variables */
        lp_Byte_t csBuffer[LC_RECORD] = { 0 };

        /* GPS records stack array variables */
        lp_Byte_t * csgpsStack = NULL;

        /* GPS records stack variables */
        unsigned long csgpsIndex = 0;
        unsigned long csgpsParse = 0;
        unsigned long csgpsLimit = 0;

        /* Discared count variables */
        unsigned long csCount = 0;

        /* Timestamp tracking variables */
        lp_Time_t csimuLast = 0;
        lp_Time_t csgpsLast = 0;
        lp_Time_t csmasLast = 0;
        lp_Time_t csothLast = 0;

        /* GPS timestamp reconstruction variables */
        lp_Time_t csgpsTime = 0;
        lp_Time_t csgpsRMCr = 0;

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
                    if ( cs_elphel_repair_record( csBuffer ) == LC_TRUE ) {

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

                            /* Detect GGA timestamp modification */
                            if ( ( ( csBuffer[8] & 0x0F ) == LP_NMEA_IDENT_GGA ) && ( lp_timestamp_eq( csgpsLast, LC_TSR( csBuffer ) ) == LP_FALSE ) ) {

                                    /* Detect sequence consitency */
                                    if ( ( csgpsIndex == 4 ) || ( csgpsIndex == 20 ) ) {

                                        /* Parsing GPS record stack */
                                        for ( csgpsParse = 0; csgpsParse < csgpsIndex; csgpsParse += 4 ) {

                                            /* Detect sequence consistency */
                                            if ( 

                                                ( ( * ( csgpsStack + ( ( csgpsParse + 1 ) << 6 ) + 8 ) & 0x0F ) == LP_NMEA_IDENT_GSA ) &&
                                                ( ( * ( csgpsStack + ( ( csgpsParse + 2 ) << 6 ) + 8 ) & 0x0F ) == LP_NMEA_IDENT_RMC ) &&
                                                ( ( * ( csgpsStack + ( ( csgpsParse + 3 ) << 6 ) + 8 ) & 0x0F ) == LP_NMEA_IDENT_VTG )

                                            ) {

                                                /* Memorize reference timestamp on RMC */
                                                if ( csgpsParse == 0 ) csgpsRMCr = LC_TSR( csgpsStack + ( ( csgpsParse + 2 ) << 6 ) );

                                                /* Compose rebuilded timestamp */
                                                csgpsTime = cs_elphel_repair_timestamp( csgpsRMCr, csgpsParse >> 2 );

                                                /* Rebuild GPS records timestamps */
                                                cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack + ( ( csgpsParse     ) << 6 ) ), csgpsTime );
                                                cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack + ( ( csgpsParse + 1 ) << 6 ) ), csgpsTime );
                                                cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack + ( ( csgpsParse + 2 ) << 6 ) ), csgpsTime );
                                                cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack + ( ( csgpsParse + 3 ) << 6 ) ), csgpsTime );

                                                /* Export GPS records */
                                                fwrite( csgpsStack + ( ( csgpsParse     ) << 6 ), 1, LC_RECORD, csoStream );
                                                fwrite( csgpsStack + ( ( csgpsParse + 1 ) << 6 ), 1, LC_RECORD, csoStream );
                                                fwrite( csgpsStack + ( ( csgpsParse + 2 ) << 6 ), 1, LC_RECORD, csoStream );
                                                fwrite( csgpsStack + ( ( csgpsParse + 3 ) << 6 ), 1, LC_RECORD, csoStream );

                                            }

                                        }

                                    }

                                    /* Reset GPS records stack */
                                    csgpsIndex = 0;

                                    /* GPS record stack management */
                                    if ( csgpsIndex >= csgpsLimit ) csgpsStack = realloc( csgpsStack, ( csgpsLimit += CS_GPSA * LC_RECORD ) );

                                    /* Insert GPS records in stack */
                                    memcpy( csBuffer, csgpsStack, LC_RECORD );

                                    /* Update last-known timestamp */
                                    csgpsLast = LC_TSR( csBuffer );

                            } else {

                                /* Reset stack parser */
                                csgpsParse = 0;

                                /* Parsing GPS record stack for identity detection */
                                while ( csgpsParse < csgpsIndex ) {

                                    /* GPS record equality check */
                                    if ( cs_elphel_repair_req( csBuffer, csgpsStack + ( ( csgpsParse ++ ) << 6 ) ) ) csgpsParse = csgpsIndex + 1;

                                }

                                /* Detect insertion necessities */
                                if ( csgpsParse == csgpsIndex ) {

                                    /* GPS record stack management */
                                    if ( csgpsIndex >= csgpsLimit ) csgpsStack = realloc( csgpsStack, ( csgpsLimit += CS_GPSA * LC_RECORD ) );

                                    /* Insert GPS records in stack */
                                    memcpy( csBuffer, csgpsStack + ( ( csgpsIndex ++ ) << 6 ), LC_RECORD );

                                }

                            }

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

                /* Unallocate GPS records stack */
                free( csgpsStack );

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

    int cs_elphel_repair_record(

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
    Source - Record equality check
 */

    int cs_elphel_repair_req(

        lp_Byte_t const * const csaBuffer,
        lp_Byte_t const * const csbBuffer

    ) {

        /* Parsing variables */
        unsigned long csParse = 0;

        /* Parsing buffers */
        while ( csParse < LC_RECORD ) {

            /* Check byte per byte equality */
            if ( * ( csaBuffer + csParse ) != * ( csbBuffer + csParse ) ) {

                /* Return result */
                return( LC_FALSE );

            /* Update parser */
            } else { csParse ++; }

        }

        /* Return result */
        return( LC_TRUE );

    }

/*
    Source - GPS timestamp reconstruction
*/

    lp_Time_t cs_elphel_repair_timestamp( 

        lp_Time_t     const csReference,
        unsigned long const csRepet 

    ) {

        /* Computation buffer variables */
        lp_Time_t csReturn = csRepet * 200000lu;

        /* Compute reconstructed timestamp */
        return( lp_timestamp_add( csReference, lp_timestamp_compose( csReturn / 1000000, csReturn % 1000000 ) ) );

    }

/*
    Source - GPS timestamp overide
*/

    void cs_elphel_repair_header( 

        lp_Time_t * const csHeader, 
        lp_Time_t   const csTime 

    ) {

        /* Override record header */
        ( * csHeader ) = csTime | ( ( * csHeader ) & 0x00000000FFF00000llu );

    }

