/*
 * csps-suite - CSPS library front-end suite
 *
 * Copyright (c) 2013-2014 FOXEL SA - http://foxel.ch
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

    # include "csps-elphel-cat.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Paths variables */
        char csFile[256] = { 0 };
        char csFlag[256] = { 0 };

        /* Search in parameters */
        stdp( stda( argc, argv,  "--logs", "-l" ), argv, csFile, CS_STRING );
        stdp( stda( argc, argv,  "--flag", "-f" ), argv, csFlag, CS_STRING );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else 
        if ( stda( argc, argv, "--marker", "-m" ) ) {

            /* Records buffer variables */
            lp_Byte_t csRec[CS_RECLEN] = { 0 };

            /* Timestamp variables */
            lp_Time_t csTime = 0;

            /* Create input stream */
            FILE * csStream = fopen( csFile, "rb" );

            /* Parse stream */
            while ( ( fread( csRec, 1, CS_RECLEN, csStream ) == CS_RECLEN ) && ( csTime == 0 ) ) {            

                /* Event type detection - IMU */
                if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == CS_MAS ) {

                    /* Retrieve record timestamp */
                    csTime = lp_timestamp( ( lp_Void_t * ) csRec );                    

                }

            }

            /* Close stream */
            fclose( csStream );

            /* Display timestamp */
            fprintf( CS_OUT, "%" lp_Time_p "\n", lp_timestamp_sec( csTime ) );

        } else { 

            /* Records buffer variables */
            lp_Byte_t csRec[CS_RECLEN] = { 0 };

            /* NMEA buffer variables */
            lp_Char_t csNMEAchar[256] = { 0 };

            /* Timestamp variables */
            lp_Time_t csTime = 0;

            /* Create input stream */
            FILE * csStream = fopen( csFile, "rb" );

            /* Check stream openning */
            if ( csStream != NULL ) {

                /* Parse stream */
                while ( fread( csRec, 1, CS_RECLEN, csStream ) == CS_RECLEN ) {

                    /* Retrieve record timestamp */
                    csTime = lp_timestamp( ( lp_Void_t * ) csRec );

                    /* Event type detection - IMU */
                    if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == CS_IMU ) {

                        /* Check display flag */
                        if ( strstr( csFlag, "i" ) != CS_NULL ) {

                            /* Display event timestamp */
                            fprintf( CS_OUT, "IMU : %010" lp_Time_p ".%06" lp_Time_p, lp_timestamp_sec( csTime ), lp_timestamp_usec( csTime ) );

                            /* Display line end */
                            fprintf( CS_OUT, "\n" );

                        }

                    /* Event type detection - Master */
                    } else
                    if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == CS_MAS ) {

                        /* Check display flag */
                        if ( strstr( csFlag, "m" ) != CS_NULL ) {

                            /* Display event timestamp */
                            fprintf( CS_OUT, "MAS : %010" lp_Time_p ".%06" lp_Time_p, lp_timestamp_sec( csTime ), lp_timestamp_usec( csTime ) );

                            /* Retrieve master timestamp */
                            csTime = lp_timestamp( ( lp_Void_t * ) ( csRec + 8 ) );

                            /* Display secondary information */
                            fprintf( CS_OUT, " : %010" lp_Time_p ".%06" lp_Time_p, lp_timestamp_sec( csTime ), lp_timestamp_usec( csTime ) );

                            /* Display line end */
                            fprintf( CS_OUT, "\n" );

                        }

                    /* Event type detection - GPS */
                    } else
                    if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == CS_GPS ) {

                        /* Check display flag */
                        if ( strstr( csFlag, "g" ) != CS_NULL ) {

                            /* Display event timestamp */
                            fprintf( CS_OUT, "GPS : %010" lp_Time_p ".%06" lp_Time_p, lp_timestamp_sec( csTime ), lp_timestamp_usec( csTime ) );

                            /* Display NMEA type */
                            switch ( lp_nmea_sentence( csRec + 8, ( CS_RECLEN - 8 ) << 1, csNMEAchar ) ) {

                                case( LP_NMEA_IDENT_RMC ) : { fprintf( CS_OUT, " : NMEA,RMC," ); } break;
                                case( LP_NMEA_IDENT_GGA ) : { fprintf( CS_OUT, " : NMEA,GGA," ); } break;
                                case( LP_NMEA_IDENT_GSA ) : { fprintf( CS_OUT, " : NMEA,GSA," ); } break;
                                case( LP_NMEA_IDENT_VTG ) : { fprintf( CS_OUT, " : NMEA,VTG," ); } break;

                            };

                            /* Display NMEA sentence */
                            fprintf( CS_OUT, "%s", csNMEAchar );

                            /* Display line end */
                            fprintf( CS_OUT, "\n" );

                        }

                    }

                }

            /* Display message */
            } else { fprintf( CS_OUT, "Error : Unable to open file %s\n", strrchr( csFile, '/' ) + 1 ); }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Arguments common handler
 */

    int stda( int argc, char ** argv, char const * const ltag, char const * const stag ) {

        /* Search for argument */
        while ( ( -- argc ) > 0 ) {

            /* Search for tag matching */
            if ( ( strcmp( argv[ argc ], ltag ) == 0 ) || ( strcmp( argv[ argc ], stag ) == 0 ) ) {

                /* Return pointer to argument parameter */
                return( argc + 1 );

            }

        /* Argument not found */
        } return( CS_NULL );

    }

/*
    Source - Parameters common handler
 */

    void stdp( int argi, char ** argv, void * const param, int const type ) {

        /* Index consistency */
        if ( argi == CS_NULL ) return;

        /* Select type */
        switch ( type ) {

            /* Specific reading operation - Integers */
            case ( CS_CHAR   ) : { * ( signed char        * ) param = atoi ( ( const char * ) argv[argi] ); } break;
            case ( CS_SHORT  ) : { * ( signed short       * ) param = atoi ( ( const char * ) argv[argi] ); } break;
            case ( CS_INT    ) : { * ( signed int         * ) param = atoi ( ( const char * ) argv[argi] ); } break;
            case ( CS_LONG   ) : { * ( signed long        * ) param = atol ( ( const char * ) argv[argi] ); } break;
            case ( CS_LLONG  ) : { * ( signed long long   * ) param = atoll( ( const char * ) argv[argi] ); } break;
            case ( CS_UCHAR  ) : { * ( unsigned char      * ) param = atol ( ( const char * ) argv[argi] ); } break;
            case ( CS_USHORT ) : { * ( unsigned short     * ) param = atol ( ( const char * ) argv[argi] ); } break;
            case ( CS_UINT   ) : { * ( unsigned int       * ) param = atol ( ( const char * ) argv[argi] ); } break;
            case ( CS_ULONG  ) : { * ( unsigned long      * ) param = atoll( ( const char * ) argv[argi] ); } break;
            case ( CS_ULLONG ) : { * ( unsigned long long * ) param = atoll( ( const char * ) argv[argi] ); } break;

            /* Specific reading operation - Floating point */
            case ( CS_FLOAT  ) : { * ( float              * ) param = atof ( ( const char * ) argv[argi] ); } break;
            case ( CS_DOUBLE ) : { * ( double             * ) param = atof ( ( const char * ) argv[argi] ); } break;

            /* Specific reading operation - String */
            case ( CS_STRING ) : { strcpy( ( char * ) param, ( const char * ) argv[argi] );  } break;

        };

    }

