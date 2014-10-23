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

    # include "csps-export.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csPath[256] = { 0 };
        char csFile[256] = { 0 };
        char csCAMd[256] = { 0 };
        char csCAMm[256] = { 0 };
        char csGPSd[256] = { 0 };
        char csGPSm[256] = { 0 };

        /* Default GPS position */
        double csLongitude = 0.0;
        double csLatitude  = 0.0;
        double csAltitude  = 0.0;

        /* Search in parameters */
        stdp( stda( argc, argv, "--path"     ,"-p" ), argv,   csPath     , CS_STRING );
        stdp( stda( argc, argv, "--export"   ,"-e" ), argv,   csFile     , CS_STRING );
        stdp( stda( argc, argv, "--cam-tag"  ,"-c" ), argv,   csCAMd     , CS_STRING );
        stdp( stda( argc, argv, "--cam-mod"  ,"-m" ), argv,   csCAMm     , CS_STRING );
        stdp( stda( argc, argv, "--gps-tag"  ,"-g" ), argv,   csGPSd     , CS_STRING );
        stdp( stda( argc, argv, "--gps-mod"  ,"-n" ), argv,   csGPSm     , CS_STRING );
        stdp( stda( argc, argv, "--longitude","-l" ), argv, & csLongitude, CS_DOUBLE );
        stdp( stda( argc, argv, "--latitude" ,"-t" ), argv, & csLatitude , CS_DOUBLE );
        stdp( stda( argc, argv, "--altitude" ,"-a" ), argv, & csAltitude , CS_DOUBLE );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Camera stream size variables */
            lp_Size_t csSize  = 0;

            /* Parsing variables */
            lp_Size_t csParse = 0;

            /* Position guess variables */
            lp_Size_t csFlag  = 0;

            /* Index memory variables */
            lp_Size_t csIndex = 0;

            /* Position variables */
            lp_Real_t csGPSlon = csLongitude;
            lp_Real_t csGPSlat = csLatitude;
            lp_Real_t csGPSalt = csAltitude;

            /* Camera stream variables */
            lp_Time_t * csCAMtag = NULL;
            lp_Time_t * csCAMsyn = NULL;

            /* Stream variables */
            FILE * csStream = NULL;

            /* Query variables */
            lp_QueryPosition csPosition = { CS_FALSE, 0.0, 0.0, 0.0 };

            /* Create stream */
            if ( ( csStream = fopen( csFile, "w" ) ) != NULL ) {

                /* Display message */
                fprintf( CS_OUT, "Creating %s JSON file ...\n", basename( csFile ) );

                /* Ask stream size */
                csSize = lp_stream_size( csPath, LP_DEVICE_TYPE_CAM, csCAMd, csCAMm );

                /* allocate stream memory */
                csCAMtag = lp_stream_read( csPath, LP_DEVICE_TYPE_CAM, csCAMd, csCAMm, LP_STREAM_CPN_TAG, sizeof( lp_Time_t ) * csSize );
                csCAMsyn = lp_stream_read( csPath, LP_DEVICE_TYPE_CAM, csCAMd, csCAMm, LP_STREAM_CPN_SYN, sizeof( lp_Time_t ) * csSize );

                /* Search for initial position (for signal missing on boundaries) */
                while ( ( csParse < csSize ) && ( csPosition.qrStatus == CS_FALSE ) ) {

                    /* Query timestamp position */
                    csPosition = lp_query_position_by_timestamp( 

                        csPath, 
                        LP_DEVICE_TYPE_GPS, 
                        csGPSd, 
                        csGPSm, 
                        csCAMsyn[csParse]

                    );

                    /* Update search index */
                    csParse ++;

                }

                /* Memorize initial position index */
                csIndex = csParse - 1;

                /* Check position avialability */
                if ( csParse < csSize ) {

                    /* Assign initial position */
                    csGPSlon = csPosition.qrLongitude;
                    csGPSlat = csPosition.qrLatitude;
                    csGPSalt = csPosition.qrAltitude;

                }

                /* Initialize JSON */
                fprintf( csStream, "{\n \"gps\":%s,\n \"pose\":[\n", ( csParse < csSize ) ? "true" : "false" );
                
                /* Exportation loop */
                for ( csParse = 0; csParse < csSize; csParse ++ ) {

                    /* Verify missing position condition */
                    if ( csParse > csIndex ) {

                        /* Query position */
                        csPosition = lp_query_position_by_timestamp( 

                            csPath, 
                            LP_DEVICE_TYPE_GPS, 
                            csGPSd, 
                            csGPSm, 
                            csCAMsyn[csParse]

                        );

                        /* Check query status */
                        if ( csPosition.qrStatus == CS_TRUE ) {

                            /* Assign new position */
                            csGPSlon = csPosition.qrLongitude;
                            csGPSlat = csPosition.qrLatitude;
                            csGPSalt = csPosition.qrAltitude;

                            /* Update guess flag */
                            csFlag = 1;

                        } else {

                            /* Update guess flag */
                            csFlag = 0;

                        }

                    }

                    /* Export JSON - format */
                    fprintf( csStream, "  {\n" );

                    /* Export JSON - availability flag */
                    fprintf( csStream, "   \"guess\":%s,\n", ( csFlag == 0 ) ? "true" : "false" );

                    /* Export JSON - capture completion flag */
                    fprintf( csStream, "   \"status\":\"unknown\",\n" );

                    /* Export JSON - positions */
                    fprintf( csStream, "   \"lng\":%.8f,\n", csGPSlon );
                    fprintf( csStream, "   \"lat\":%.8f,\n", csGPSlat );
                    fprintf( csStream, "   \"alt\":%.8f,\n", csGPSalt );

                    /* Export JSON - timestamps */
                    fprintf( csStream, "   \"sec\":%" lp_Time_p ",\n", lp_timestamp_sec ( csCAMtag[csParse] ) );
                    fprintf( csStream, "   \"usc\":%" lp_Time_p " \n", lp_timestamp_usec( csCAMtag[csParse] ) );

                    /* Export JSON - format */
                    fprintf( csStream, "  }%s\n", ( csParse < ( csSize - 1 ) ) ? "," : "" );

                }

                /* Terminate JSON */
                fprintf( csStream, " ]\n}\n" );

                /* Unallocate stream memory */
                csCAMtag = lp_stream_delete( csCAMtag );
                csCAMsyn = lp_stream_delete( csCAMsyn );

            /* Display message */
            } else { fprintf( CS_OUT, "Error : unable to access %s\n", basename( csFile ) ); }

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

