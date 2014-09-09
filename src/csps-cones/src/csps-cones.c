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

    # include "csps-cones.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csPath[256] = { 0 };
        char csList[256] = { 0 };
        char csCone[256] = { 0 };

        /* Image stack */
        char csStack[8192][256] = { { 0 } };

        /* Stack size */
        long csIndex = 0;
        long csParse = 0;
        long csOther = 0;

        /* Orientation structures */
        lp_QueryOrientation lpOrientA;
        lp_QueryOrientation lpOrientB;

        /* Position structures */
        lp_QueryPosition lpPositionA;
        lp_QueryPosition lpPositionB;

        /* Timestamps */
        lp_Time_t csTimeA;
        lp_Time_t csTimeB;

        lp_Time_t csExtAA;
        lp_Time_t csExtAB;
        lp_Time_t csExtAC;
        lp_Time_t csExtBA;
        lp_Time_t csExtBB;
        lp_Time_t csExtBC;

        double vA[3];
        double vB[3];

        /* Search in parameters */
        cs_stdp( cs_stda( argc, argv,  "--path"  , "-p" ), argv, csPath , CS_STRING );
        cs_stdp( cs_stda( argc, argv,  "--list"  , "-l" ), argv, csList , CS_STRING );
        cs_stdp( cs_stda( argc, argv,  "--output", "-o" ), argv, csCone , CS_STRING );

        /* Execution switch */
        if ( cs_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            printf( "Read input list...\n" );

            FILE * csHandle = fopen( csList, "r" );

            while ( fgets( csStack[csIndex], 256, csHandle ) != NULL ) { csStack[csIndex][strlen(csStack[csIndex])-1] = '\0'; csIndex++; }

            fclose( csHandle );

            printf( "Orientation...\n" );

            csHandle = fopen( csCone, "w" );
            
            for ( csParse = 0; csParse < csIndex; csParse ++ ) {

                printf( "Comparing %s\n", csStack[csParse] );

                fprintf( csHandle, "%s; ", csStack[csParse] );

                for ( csOther = 0; csOther < csIndex; csOther ++ ) {

                    if ( csParse != csOther ) {

                        printf( "    %s\n", csStack[csOther] );

                        sscanf( csStack[csParse], "%lu_%lu-%lu", & csExtAA, & csExtAB, & csExtAC );
                        sscanf( csStack[csOther], "%lu_%lu-%lu", & csExtBA, & csExtBB, & csExtBC );

                        csTimeA = lp_timestamp_compose( csExtAA + 7200, csExtAB );
                        csTimeB = lp_timestamp_compose( csExtBA + 7200, csExtBB );

                        lpOrientA = lp_query_orientation_by_timestamp( csPath, "imu", "adis16375", "mod-IFETI", csTimeA );
                        lpOrientB = lp_query_orientation_by_timestamp( csPath, "imu", "adis16375", "mod-IFETI", csTimeB );

                        lpPositionA = lp_query_position_by_timestamp( csPath, "gps", "ls20031", "mod-SGNQF", csTimeA );
                        lpPositionB = lp_query_position_by_timestamp( csPath, "gps", "ls20031", "mod-SGNQF", csTimeA );

                        lpPositionA.qrLongitude *= ( 40075017.0 / 360.0 );
                        lpPositionA.qrLatitude  *= ( 40075017.0 / 360.0 );
                        lpPositionB.qrLongitude *= ( 40075017.0 / 360.0 );
                        lpPositionB.qrLatitude  *= ( 40075017.0 / 360.0 );

                        double dx = lpPositionA.qrLongitude - lpPositionB.qrLongitude;
                        double dy = lpPositionA.qrLatitude  - lpPositionB.qrLatitude;

                        if ( ( csExtAC == 10 ) || ( csExtAC == 24 ) ) {

                            vA[0] = + lpOrientA.qrfxx;
                            vA[1] = + lpOrientA.qrfxy;
                            vA[2] = + lpOrientA.qrfxz;

                        } else {

                            vA[0] = - lpOrientA.qrfxx;
                            vA[1] = - lpOrientA.qrfxy;
                            vA[2] = - lpOrientA.qrfxz;

                        }

                        if ( ( csExtBC == 10 ) || ( csExtBC == 24 ) ) {

                            vB[0] = + lpOrientB.qrfxx;
                            vB[1] = + lpOrientB.qrfxy;
                            vB[2] = + lpOrientB.qrfxz;

                        } else {

                            vB[0] = - lpOrientB.qrfxx;
                            vB[1] = - lpOrientB.qrfxy;
                            vB[2] = - lpOrientB.qrfxz;

                        }

                        double s = vA[0] * vB[0] + vA[1] * vB[1] + vA[2] * vB[2];

                        if ( s > 0.0 ) {

                            s = acos( s ) * ( 180.0 / 3.1415926535 );

                            if ( ( s < 22.5 ) && ( s > - 22.5 ) ) {

                                double dx = lpPositionA.qrLongitude - lpPositionB.qrLongitude;
                                double dy = lpPositionA.qrLatitude  - lpPositionB.qrLatitude;
                                double dr = sqrt( dx * dx + dy * dy );
                                
                                double cA = dr * ( vA[0] * dx + vA[1] * dy );
                                double cB = dr * ( vB[0] * dx + vB[1] * dy );

                                if ( ( cA <= 50.0 ) && ( cB <= 50.0 ) ) {

                                    fprintf( csHandle, "%s; ", csStack[csOther] );

                                }

                            }

                        }

                    }

                }

                fprintf( csHandle, "\n" );

            }

            fclose( csHandle );

            printf( "Done" );

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Arguments common handler
 */

    int cs_stda( int argc, char ** argv, const char * const ltag, const char * const stag ) {

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

    void cs_stdp( int argi, char ** argv, void * param, int type ) {

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

