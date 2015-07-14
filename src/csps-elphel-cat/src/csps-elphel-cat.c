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
 * Contributor(s):
 *
 *      Kevin Velickovic <k.velickovic@foxel.ch>
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

    # include "csps-elphel-cat.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Paths variables */
        char csFile[256] = { 0 };
        char csFlag[256] = { 0 };

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv,  "--logs", "-l" ), argv, csFile, LC_STRING );
        lc_stdp( lc_stda( argc, argv,  "--flag", "-f" ), argv, csFlag, LC_STRING );

        /* Stream handle variables */
        FILE * csStream = NULL;

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else
        if ( lc_stda( argc, argv, "--marker", "-m" ) ) {

            /* Records buffer variables */
            lp_Byte_t csBuffer[LC_RECORD] = { 0 };

            /* Timestamp variables */
            lp_Time_t csTime = 0;

            /* Check stream creation */
            if ( ( csStream = fopen( csFile, "rb" ) ) != NULL ) {

                /* Parse stream */
                while ( ( fread( csBuffer, 1, LC_RECORD, csStream ) == LC_RECORD ) && ( csTime == 0 ) ) {

                    /* Event detection */
                    if ( LC_EDM( csBuffer, LC_MAS ) ) {

                        /* Retrieve record master timestamp */
                        csTime = LC_TSR( ( csBuffer + 8 ) );

                    }

                }

                /* Close stream */
                fclose( csStream );

            } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( csFile ) ); }

            /* Display timestamp */
            fprintf( LC_OUT, "%" lp_Time_p "\n", lp_timestamp_sec( csTime ) );

        } else {

            /* Records buffer variables */
            lp_Byte_t csBuffer[LC_RECORD] = { 0 };

            /* NMEA buffer variables */
            lp_Char_t csSentence[256] = { 0 };

            /* Timestamp variables */
            lp_Time_t csTime = 0;

            /* Check stream openning */
            if ( ( csStream = fopen( csFile, "rb" ) ) != NULL ) {

                /* Parse stream */
                while ( fread( csBuffer, 1, LC_RECORD, csStream ) == LC_RECORD ) {

                    /* Retrieve record timestamp */
                    csTime = LC_TSR( csBuffer );

                    /* Event detection */
                    if ( LC_EDM( csBuffer, LC_IMU ) ) {

                        /* Display flag verification */
                        if ( strstr( csFlag, "i" ) != LC_NULL ) {

                            /* Display event timestamp */
                            fprintf( LC_OUT, "IMU " LC_TSM , LC_TSD( csTime ) );

                            /* Display mode verification */
                            if ( strstr( csFlag, "x" ) != LC_NULL ) {

                                /* Display record binary content */
                                csps_elphel_cat_record( csBuffer );

                            } else
                            if ( strstr( csFlag, "a" ) != LC_NULL ) {

                                /* Display record binary content */
                                csps_elphel_cat_record_ascii( csBuffer );

                            }

                            /* Display EOL */
                            fprintf( LC_OUT, "\n" );

                        }

                    } else
                    if ( LC_EDM( csBuffer, LC_MAS ) ) {

                        /* Display flag verification */
                        if ( strstr( csFlag, "m" ) != LC_NULL ) {

                            /* Display event timestamp */
                            fprintf( LC_OUT, "MAS " LC_TSM, LC_TSD( csTime ) );

                            /* Display mode verification */
                            if ( strstr( csFlag, "x" ) != LC_NULL ) {

                                /* Display record binary content */
                                csps_elphel_cat_record( csBuffer );

                            } else 
                            if ( strstr( csFlag, "a" ) != LC_NULL ) {

                                /* Display record binary content */
                                csps_elphel_cat_record_ascii( csBuffer );

                            } else {

                                /* Retrieve master timestamp */
                                csTime = LC_TSR( ( csBuffer + 8 ) );

                                /* Display secondary information */
                                fprintf( LC_OUT, " " LC_TSM , LC_TSD( csTime ) );

                            }

                            /* Display EOL */
                            fprintf( LC_OUT, "\n" );

                        }

                    } else
                    if ( LC_EDM( csBuffer, LC_GPS ) ) {

                        /* Display flag verification */
                        if ( strstr( csFlag, "g" ) != LC_NULL ) {

                            /* Display event timestamp */
                            fprintf( LC_OUT, "GPS " LC_TSM, LC_TSD( csTime ) );

                            /* Display mode verification */
                            if ( strstr( csFlag, "x" ) != LC_NULL ) {

                                /* Display record binary content */
                                csps_elphel_cat_record( csBuffer );

                            }else 
                            if ( strstr( csFlag, "a" ) != LC_NULL ) {

                                /* Display record binary content */
                                csps_elphel_cat_record_ascii( csBuffer );

                            } else {

                                /* Display NMEA type */
                                switch ( lp_nmea_sentence( csBuffer + 8, ( LC_RECORD - 8 ) << 1, csSentence ) ) {

                                    case( LP_NMEA_IDENT_RMC ) : { fprintf( LC_OUT, " GPRMC,%s", csSentence ); } break;
                                    case( LP_NMEA_IDENT_GGA ) : { fprintf( LC_OUT, " GPGGA,%s", csSentence ); } break;
                                    case( LP_NMEA_IDENT_GSA ) : { fprintf( LC_OUT, " GPGSA,%s", csSentence ); } break;
                                    case( LP_NMEA_IDENT_VTG ) : { fprintf( LC_OUT, " GPVTG,%s", csSentence ); } break;

                                };

                            }

                            /* Display EOL */
                            fprintf( LC_OUT, "\n" );

                        }

                    } else
                    if ( strstr( csFlag, "o" ) != LC_NULL ) {

                        /* Display event timestamp */
                        fprintf( LC_OUT, "UNK " LC_TSM, LC_TSD( csTime ) );

                        /* Display mode verification */
                        if ( strstr( csFlag, "x" ) != LC_NULL ) {

                            /* Display record binary content */
                            csps_elphel_cat_record( csBuffer );

                        } else
                        if ( strstr( csFlag, "a" ) != LC_NULL ) {

                            /* Display record binary content */
                            csps_elphel_cat_record_ascii( csBuffer );

                        }

                        /* Display EOL */
                        fprintf( LC_OUT, "\n" );

                    }

                }

            /* Display message */
            } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( csFile ) ); }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Record buffer ASCII HEX display
*/

    void csps_elphel_cat_record( lp_Byte_t * csBuffer ) {

        /* Parsing variables */
        long csParse = 0;

        /* Display frame */
        fprintf( LC_OUT, " [ " );

        /* Display loop */
        for ( csParse = 0; csParse < LC_RECORD ; csParse ++ ) fprintf( LC_OUT, "%02X ", csBuffer[csParse] );

        /* Display frame */
        fprintf( LC_OUT, "]" );

    }

/*
    Source - Record buffer ASCII display
*/

    void csps_elphel_cat_record_ascii( lp_Byte_t * csBuffer ) {

        /* Parsing variables */
        int csParse = 0;

        /* Display frame */
        fprintf( LC_OUT, " [ " );

        /* Display loop */
        for ( csParse = 0; csParse < LC_RECORD ; csParse ++ ) {

            /* Printable char verification */
            if ( isprint( csBuffer[csParse] ) != 0 ) {

                /* Display char */
                fprintf( LC_OUT, "%c", csBuffer[csParse] );

            } else {

                /* Display symbol for unprintable char */
                fprintf( LC_OUT, "." );

            }

        }

        /* Display frame */
        fprintf( LC_OUT, " ]" );

    }

