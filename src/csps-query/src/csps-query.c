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

    # include "csps-query.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csPath[256] = { 0 };
        char csGPSd[256] = { 0 };
        char csGPSm[256] = { 0 };
        char csIMUd[256] = { 0 };
        char csIMUm[256] = { 0 };

        /* Timestamp variables */
        lp_Time_t csSec  = 0;
        lp_Time_t csUsec = 0;
        lp_Time_t csTime = 0;

        /* Search in parameters */
        stdp( stda( argc, argv, "--path"         ,"-p" ), argv,   csPath, CS_STRING );
        stdp( stda( argc, argv, "--gps-tag"      ,"-g" ), argv,   csGPSd, CS_STRING );
        stdp( stda( argc, argv, "--gps-mod"      ,"-m" ), argv,   csGPSm, CS_STRING );
        stdp( stda( argc, argv, "--imu-tag"      ,"-i" ), argv,   csIMUd, CS_STRING );
        stdp( stda( argc, argv, "--imu-mod"      ,"-n" ), argv,   csIMUm, CS_STRING );
        stdp( stda( argc, argv, "--second"       ,"-s" ), argv, & csSec , CS_ULLONG );
        stdp( stda( argc, argv, "--micro-second" ,"-u" ), argv, & csUsec, CS_ULLONG );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Query variables */
            lp_Query_Position_t    csPosition;
            lp_Query_Orientation_t csOrientation;

            /* Compute timestamp */
            csTime = lp_timestamp_compose( csSec, csUsec );

            /* Query position by timestamp */
            csPosition = lp_query_position_by_timestamp( 

                csPath, 
                LP_DEVICE_TYPE_GPS, 
                csGPSd, 
                csGPSm, 
                csTime

            );

            /* Check query results */
            if ( csPosition.qrStatus == CS_TRUE ) {

                /* Display message */
                fprintf( CS_OUT, "Position at %010" lp_Time_p ".%06" lp_Time_p " [s]\n", csSec, csUsec );

                /* Display position */
                fprintf( CS_OUT, "  Longitude : %10.6f [°]\n", csPosition.qrLongitude );
                fprintf( CS_OUT, "  Latitude  : %10.6f [°]\n", csPosition.qrLatitude  );
                fprintf( CS_OUT, "  Altitude  : %10.6f [m]\n", csPosition.qrAltitude  );

            } else { 

                /* Display message */
                fprintf( CS_OUT, "Position at %010" lp_Time_p ".%06" lp_Time_p " not in range\n", csSec, csUsec );

            }

            /* Query position by timestamp */
            csOrientation = lp_query_orientation_by_timestamp( 

                csPath, 
                LP_DEVICE_TYPE_IMU, 
                csIMUd, 
                csIMUm, 
                csTime

            );

            /* Check query results */
            if ( csOrientation.qrStatus == CS_TRUE ) {

                /* Display message */
                fprintf( CS_OUT, "Orientation at %010" lp_Time_p ".%06" lp_Time_p " [s]\n", csSec, csUsec );

                /* Display position */
                fprintf( CS_OUT, "  Matix     : %10.6f %10.6f %10.6f\n", csOrientation.qrfxx, csOrientation.qrfyx, csOrientation.qrfzx );
                fprintf( CS_OUT, "  Matix     : %10.6f %10.6f %10.6f\n", csOrientation.qrfxy, csOrientation.qrfyy, csOrientation.qrfzy );
                fprintf( CS_OUT, "  Matix     : %10.6f %10.6f %10.6f\n", csOrientation.qrfxz, csOrientation.qrfyz, csOrientation.qrfzz );

            } else { 

                /* Display message */
                fprintf( CS_OUT, "Orientation at %010" lp_Time_p ".%06" lp_Time_p " not in range\n", csSec, csUsec );

            }

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
