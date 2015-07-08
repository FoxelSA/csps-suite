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

        /* Discared count variables */
        unsigned long csCount = 0;

        /* GPS records stack variables */
        unsigned long csgpsIndex = 0;
        unsigned long csgpsParse = 0;

        /* GPS records stack array variables */
        lp_Byte_t csgpsStack[CS_BUFFERS][LC_RECORD] = { { 0 } };

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

        /* Record buffer variables */
        lp_Byte_t csBuffer[LC_RECORD] = { 0 };

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

                        /* Switch on event type */
                        if ( LC_EDM( csBuffer, LC_IMU ) ) {

                            /* Detect record repetition */
                            if ( lp_timestamp_ge( csimuLast, LC_TSR( csBuffer ) ) == LP_FALSE ) {

                                /* Export event buffer */
                                fwrite( csBuffer, 1, LC_RECORD, csoStream );

                                /* Update last-known timestamp */
                                csimuLast = LC_TSR( csBuffer );

                            /* Update discared count */
                            } else { csCount ++; }

                        } else
                        if ( LC_EDM( csBuffer, LC_GPS ) ) {

                            /* Search initial GGA sentence */
                            if ( cs_elphel_repair_detect( csBuffer ) == LC_TRUE ) {

                                /* Reset stack state */
                                csgpsIndex = 0;

                                /* Push sentence */
                                memcpy( csgpsStack[csgpsIndex ++], csBuffer, LC_RECORD );

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

                                /* Update discared count */
                                } else { csCount ++; }

                            }

                            /* Detect stack state */
                            if ( csgpsIndex == CS_BUFFERS ) {

                                /* Detect group repetition */
                                if ( lp_timestamp_eq( csgpsRMCr, LC_TSR( csgpsStack[2] ) ) == LP_FALSE ) {

                                    /* GPS measures blocks group validation */
                                    if ( cs_elphel_repair_group( csgpsStack ) == LC_TRUE ) {

                                        /* Retrieve group reference timestamp on first NMEA/RMC */
                                        csgpsRMCr = LC_TSR( csgpsStack[2] );

                                        /* Exportation of validated records */
                                        for ( csgpsParse = 0; csgpsParse < 20; csgpsParse += 4 ) {

                                            /* Compose GPS records timestamp */
                                            csgpsTime = cs_elphel_repair_timestamp( csgpsRMCr, csgpsParse >> 2 );

                                            /* Replace GPS records timestamps */
                                            cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack[csgpsParse    ] ), csgpsTime );
                                            cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack[csgpsParse + 1] ), csgpsTime );
                                            cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack[csgpsParse + 2] ), csgpsTime );
                                            cs_elphel_repair_header( ( lp_Time_t * ) ( csgpsStack[csgpsParse + 3] ), csgpsTime );

                                            /* Export event block buffers */
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

                            /* Detect record consistency */
                            if ( cs_elphel_repair_master( csBuffer ) == LC_TRUE ) {

                                /* Detect record repetition */
                                if ( lp_timestamp_ge( csmasLast, LC_TSR( csBuffer ) ) == LP_FALSE ) {

                                    /* Export event buffer */
                                    fwrite( csBuffer, 1, LC_RECORD, csoStream );

                                    /* Update last-known timestamp */
                                    csmasLast = LC_TSR( csBuffer );

                                /* Update discared count */
                                } else { csCount ++; }

                            /* Update discared count */
                            } else { csCount ++; }

                        } else {

                            /* Detect record repetition */
                            if ( lp_timestamp_ge( csothLast, LC_TSR( csBuffer ) ) == LP_FALSE ) {

                                /* Export event buffer */
                                fwrite( csBuffer, 1, LC_RECORD, csoStream );

                                /* Update last-known timestamp */
                                csothLast = LC_TSR( csBuffer );

                            /* Update discared count */
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
    Source - Master record timestamps validation
*/

    int cs_elphel_repair_master(

        lp_Byte_t const * const csBuffer

    ) {

        /* Compute time distance between master and synchronization timestamp */
        if ( lp_timestamp_diff( LC_TSR( csBuffer ), LC_TSR( csBuffer + 8 ) ) > CS_MASTER ) {

            /* Valid master record */
            return( LC_TRUE );

        /* Invalid master record */
        } else { return( LC_FALSE ); }

    }

/*
    Source - Group initial GGA detection
 */

    int cs_elphel_repair_detect(

        lp_Byte_t const * const csBuffer

    ) {

        /* GPS clock variables */
        double csClock = 0.0;

        /* Sentence buffer variables */
        lp_Char_t csSentence[256] = { 0 };

        /* Decode NMEA/GGA sentence */
        if ( lp_nmea_sentence( csBuffer + 8, ( LC_RECORD - 8 ) << 1, csSentence ) == LP_NMEA_IDENT_GGA ) {

            /* Retrieve GPS clock */
            csClock = cs_elphel_repair_clock( csSentence );

            /* Analyse GPS clock and return answer */
            if ( csClock - floor( csClock ) < 0.001 ) return( LC_TRUE ); else return( LC_FALSE );

        /* Return negative answer */
        } else { return( LC_FALSE ); }

    }

/*
    Source - GPS measures blocks group validation
 */

    int cs_elphel_repair_group(

        lp_Byte_t csgpsStack[CS_BUFFERS][LC_RECORD]

    ) {

        /* Parsing variables */
        unsigned long csParse = 0;

        /* Time reference  variables */
        double csReference = 0.0;

        /* Sentences string buffer variables */
        lp_Char_t csGGA[256] = { 0 };
        lp_Char_t csRMC[256] = { 0 };

        /* Parsing measures blocks group */
        for ( csParse = 0; csParse < CS_BUFFERS; csParse += 4 ) {

            /* Decode NMEA/GGA and NMEA/RMC sentences */
            lp_nmea_sentence( csgpsStack[csParse    ] + 8, ( LC_RECORD - 8 ) << 1, csGGA );
            lp_nmea_sentence( csgpsStack[csParse + 2] + 8, ( LC_RECORD - 8 ) << 1, csRMC );

            /* Validation on GGA fix value */
            if ( cs_elphel_repair_fix( csGGA ) == 0 ) return( LC_FALSE );

            /* Validation on GPS clock */
            if ( cs_elphel_repair_clock( csGGA ) != cs_elphel_repair_clock( csRMC ) ) return( LC_FALSE );

            /* Detect reference sentence */
            if ( csParse == 0 ) {

                /* Retrieve time reference */
                csReference = floor( cs_elphel_repair_clock( csGGA ) );

            } else {

                /* Validation on GPS clock consistency */
                if ( csReference != floor( cs_elphel_repair_clock( csGGA ) ) ) return( LC_FALSE );

            }

            /* Validation on GPS clock consistency */
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

