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

                        /* Build output file path */
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

        /* GPS records stack array variables */
        lp_Byte_t csgpsStack[CS_BUFFERS][LC_RECORD] = { { 0 } };

        /* Record buffer variables */
        lp_Byte_t csBuffer[LC_RECORD] = { 0 };

        /* GPS records stack variables */
        unsigned long csgpsIndex = 0;
        unsigned long csgpsParse = 0;

        /* Discared count variables */
        unsigned long csCount = 0;

        /* Timestamp tracking variables */
        lp_Time_t csimuLast = 0;
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

                            /* Update discared */
                            } else { csCount ++; }

                        } else
                        if ( LC_EDM( csBuffer, LC_GPS ) ) {

                            /* Detect stack state */
                            if ( csgpsIndex == 0 ) {

                                /* Search initial GGA sentence */
                                if ( cs_elphel_repair_detect( csBuffer ) == LC_TRUE ) {

                                    /* Push sentence */
                                    memcpy( csgpsStack[csgpsIndex ++], csBuffer, LC_RECORD );

                                /* Update discared */
                                } else { csCount ++; }

                            } else {

                                /* Sequence consistency check */
                                if (

                                    ( ( ( csgpsIndex % CS_MEASURE ) == 0 ) && ( ( csBuffer[8] & 0x0F ) == LP_NMEA_IDENT_GGA ) ) || 
                                    ( ( ( csgpsIndex % CS_MEASURE ) == 1 ) && ( ( csBuffer[8] & 0x0F ) == LP_NMEA_IDENT_GSA ) ) ||
                                    ( ( ( csgpsIndex % CS_MEASURE ) == 2 ) && ( ( csBuffer[8] & 0x0F ) == LP_NMEA_IDENT_RMC ) ) ||
                                    ( ( ( csgpsIndex % CS_MEASURE ) == 3 ) && ( ( csBuffer[8] & 0x0F ) == LP_NMEA_IDENT_VTG ) )

                                ) {

                                    /* Push sentence */
                                    memcpy( csgpsStack[csgpsIndex ++], csBuffer, LC_RECORD );

                                /* Update discared */
                                } else { csCount ++; }

                            }

                            /* Detect stack state */
                            if ( csgpsIndex == CS_BUFFERS ) {

                                /* Repetition detection */
                                if ( lp_timestamp_eq( csgpsRMCr, LC_TSR( csgpsStack[2] ) ) == LP_FALSE ) {

                                    /* Block filtering */
                                    if ( cs_elphel_repair_filter( csgpsStack ) == LC_TRUE ) {

                                        /* Extract reference timestamp */
                                        csgpsRMCr = LC_TSR( csgpsStack[2] );

                                        /* Exportation of validated sentences */
                                        for ( csgpsParse = 0; csgpsParse < 20; csgpsParse += 4 ) {

                                            /* Compose rebuilded timestamp */
                                            csgpsTime = cs_elphel_repair_timestamp( csgpsRMCr, csgpsParse >> 2 );

                                            /* Rebuild GPS records timestamps */
                                            cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack[csgpsParse    ] ), csgpsTime );
                                            cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack[csgpsParse + 1] ), csgpsTime );
                                            cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack[csgpsParse + 2] ), csgpsTime );
                                            cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack[csgpsParse + 3] ), csgpsTime );

                                            /* Export GPS records */
                                            fwrite( csgpsStack[csgpsParse    ], 1, LC_RECORD, csoStream );
                                            fwrite( csgpsStack[csgpsParse + 1], 1, LC_RECORD, csoStream );
                                            fwrite( csgpsStack[csgpsParse + 2], 1, LC_RECORD, csoStream );
                                            fwrite( csgpsStack[csgpsParse + 3], 1, LC_RECORD, csoStream );

                                        }

                                    }

                                }

                                /* Reset stack */
                                csgpsIndex = 0;

                            }

                        } else
                        if ( LC_EDM( csBuffer, LC_MAS ) ) {

                            /* Detect repetition */
                            if ( lp_timestamp_ge( csmasLast, LC_TSR( csBuffer ) ) == LP_FALSE ) {

                                /* Export events */
                                fwrite( csBuffer, 1, LC_RECORD, csoStream );

                                /* Update last-known timestamp */
                                csmasLast = LC_TSR( csBuffer );

                            /* Update discared */
                            } else { csCount ++; }

                        } else {

                            /* Detect repetition */
                            if ( lp_timestamp_ge( csothLast, LC_TSR( csBuffer ) ) == LP_FALSE ) {

                                /* Export events */
                                fwrite( csBuffer, 1, LC_RECORD, csoStream );

                                /* Update last-known timestamp */
                                csothLast = LC_TSR( csBuffer );

                            /* Update discared */
                            } else { csCount ++; }

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
    Source - Initial GGA detection
 */

    int cs_elphel_repair_detect(

        lp_Byte_t const * const csBuffer

    ) {

        /* Sentence buffer variables */
        lp_Char_t csSentence[256] = { 0 };

        /* Milliseconds variables */
        double csClock = 0.0;

        /* Read sentence */
        if ( lp_nmea_sentence( csBuffer + 8, ( LC_RECORD - 8 ) << 1, csSentence ) == LP_NMEA_IDENT_GGA ) {

            /* Retrieve GPS clock */
            csClock = cs_elphel_repair_clock( csSentence );

            /* Analyse GPS clock milliseconds and return answer */
            if ( csClock - floor( csClock ) < 0.005 ) return( LC_TRUE ); else return( LC_FALSE );

        /* Return negative answer */
        } else { return( LC_FALSE ); }

    }

/*
    Source - GPS block validation
 */

    int cs_elphel_repair_filter(

        lp_Byte_t csgpsStack[CS_BUFFERS][LC_RECORD]

    ) {

        /* Sentence buffer variables */
        lp_Char_t csGGA[256] = { 0 };
        lp_Char_t csRMC[256] = { 0 };

        /* Reference time variables */
        double csReference = 0.0;

        /* Parsing variables */
        unsigned long csParse = 0;

        /* Parsing measure blocks group */
        for ( csParse = 0; csParse < CS_BUFFERS; csParse += 4 ) {

            /* Decode NMEA (GGA/RMC) sentences */
            lp_nmea_sentence( csgpsStack[csParse    ] + 8, ( LC_RECORD - 8 ) << 1, csGGA );
            lp_nmea_sentence( csgpsStack[csParse + 2] + 8, ( LC_RECORD - 8 ) << 1, csRMC );

            /* GPS measure fix check */
            if ( cs_elphel_repair_fix( csGGA ) == 0 ) return( LC_FALSE );

            /* GPS clock reference */
            if ( csParse == 0 ) {

                /* Retrieve reference */
                csReference = floor( cs_elphel_repair_clock( csGGA ) );

            } else {

                /* Consistency verification */
                if ( csReference != floor( cs_elphel_repair_clock( csGGA ) ) ) return( LC_FALSE );

            }

            /* Consistency verification */
            if ( csReference != floor( cs_elphel_repair_clock( csRMC ) ) ) return( LC_FALSE );

        }

        /* Return positive answer */
        return( LC_TRUE );

    }

/*
    Source - GPS clock access
 */

    double cs_elphel_repair_clock(

        lp_Char_t const * const csSentence

    ) {

        /* Returned GPS clock variables */
        double csReturn = 0.0;

        /* Retrieve GPS clock */
        sscanf( csSentence, "%lf", & csReturn );

        /* Return GPS clock */
        return( csReturn );

    }

/*
    Source - GPS fix access
 */

    unsigned long cs_elphel_repair_fix(

        lp_Char_t const * const csSentence

    ) {

        /* Parsing variables */
        unsigned long csParse = 0;
        unsigned long csStack = 0;

        /* Returned fix variables */
        unsigned long csReturn = 0;

        /* Parsing GGA sentence searching fix value */
        while ( csStack < 5 ) if ( * ( csSentence + ( csParse ++ ) ) == ',' ) csStack ++;

        /* Retrieve GGA fix value */
        sscanf( csSentence + csParse, "%lu", & csReturn );

        /* Return GPS fix value */
        return( csReturn );

    }

/*
    Source - GPS timestamp computation
 */

    lp_Time_t cs_elphel_repair_timestamp( 

        lp_Time_t     const csReference,
        unsigned long const csDistance 

    ) {

        /* Distance to reference variables */
        lp_Time_t csReturn = csDistance * lp_Time_s( 200000 );

        /* Compute and return GPS timestamp */
        return( lp_timestamp_add( csReference, lp_timestamp_compose( csReturn / lp_Time_s( 1000000 ), csReturn % lp_Time_s( 1000000 ) ) ) );

    }

/*
    Source - Replace record timestamp
 */

    void cs_elphel_repair_header( 

        lp_Time_t * const csRecord, 
        lp_Time_t   const csTime 

    ) {

        /* Replace record timestamp */
        ( * csRecord ) = csTime | ( ( * csRecord ) & lp_Time_s( 0x00000000FFF00000 ) );

    }

