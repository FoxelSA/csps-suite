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

    # include "csps-omvg-align.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csPath[256] = { 0 };

        /* Stream pointer variables */
        char csGPSd[256] = { 0 };
        char csGPSm[256] = { 0 };
        char csIMUd[256] = { 0 };
        char csIMUm[256] = { 0 };

        /* Search in parameters */
        stdp( stda( argc, argv, "--path"      , "-p" ), argv,   csPath  , CS_STRING );
        stdp( stda( argc, argv, "--gps-tag"   , "-g" ), argv,   csGPSd  , CS_STRING );
        stdp( stda( argc, argv, "--gps-mod"   , "-m" ), argv,   csGPSm  , CS_STRING );
        stdp( stda( argc, argv, "--imu-tag"   , "-i" ), argv,   csIMUd  , CS_STRING );
        stdp( stda( argc, argv, "--imu-mod"   , "-k" ), argv,   csIMUm  , CS_STRING );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {
            
        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Linear transformation estimation
*/

    void cs_omvg_align_lt( int const n, double const * const c1, double const * const c2, double R[3][3], double T[3] ) {

        /* Parsing variables */
        unsigned long csParse = 0;

        /* Coordinates mean variables */
        double csMean[6] = { 0.0 };

        /* Matrix variables */
        double csH[9] = { 0.0 };
        double csU[9] = { 0.0 };
        double csS[3] = { 0.0 };
        double csV[9] = { 0.0 };

        int ldh = 3, mh = 3, nh = 3;
        int info = 0;
        int lwork = -1;
        double wkopt;
        double * work;

        /* Coordinates accumulation */
        for ( csParse = 0; csParse < n; csParse ++ ) {

            csMean[0] += * ( c1 + csParse * 3     );
            csMean[1] += * ( c1 + csParse * 3 + 1 );
            csMean[2] += * ( c1 + csParse * 3 + 2 );
            csMean[3] += * ( c2 + csParse * 3     );
            csMean[4] += * ( c2 + csParse * 3 + 1 );
            csMean[5] += * ( c2 + csParse * 3 + 2 );

        }

        /* Compute mean of coordinates */
        csMean[0] /= ( double ) n;
        csMean[1] /= ( double ) n;
        csMean[2] /= ( double ) n;
        csMean[3] /= ( double ) n;
        csMean[4] /= ( double ) n;
        csMean[5] /= ( double ) n;

        /* Compute accumulated matrix */
        for ( csParse = 0; csParse < n; csParse ++ ) {

            csH[0] += ( * ( c1 + csParse * 3     ) - csMean[0] ) * ( * ( c2 + csParse * 3     ) - csMean[3] );
            csH[1] += ( * ( c1 + csParse * 3     ) - csMean[0] ) * ( * ( c2 + csParse * 3 + 1 ) - csMean[4] );
            csH[2] += ( * ( c1 + csParse * 3     ) - csMean[0] ) * ( * ( c2 + csParse * 3 + 2 ) - csMean[5] );
            csH[3] += ( * ( c1 + csParse * 3 + 1 ) - csMean[1] ) * ( * ( c2 + csParse * 3     ) - csMean[3] );
            csH[4] += ( * ( c1 + csParse * 3 + 1 ) - csMean[1] ) * ( * ( c2 + csParse * 3 + 1 ) - csMean[4] );
            csH[5] += ( * ( c1 + csParse * 3 + 1 ) - csMean[1] ) * ( * ( c2 + csParse * 3 + 2 ) - csMean[5] );
            csH[6] += ( * ( c1 + csParse * 3 + 2 ) - csMean[2] ) * ( * ( c2 + csParse * 3     ) - csMean[3] );
            csH[7] += ( * ( c1 + csParse * 3 + 2 ) - csMean[2] ) * ( * ( c2 + csParse * 3 + 1 ) - csMean[4] );
            csH[8] += ( * ( c1 + csParse * 3 + 2 ) - csMean[2] ) * ( * ( c2 + csParse * 3 + 2 ) - csMean[5] );

        }

        /* Singular values decompostion - computation buffer */
        dgesvd_( "All", "All", & mh, & nh, csH, & ldh, csS, csU, & ldh, csV, & ldh, & wkopt, & lwork, & info );

        /* Allocate computation buffer */
        work = ( double * ) malloc( lwork = ( ( int ) wkopt ) * sizeof( double ) );

        /* Singular values decompostion */
        dgesvd_( "All", "All", & mh, & nh, csH, & ldh, csS, csU, & ldh, csV, & ldh, work, & lwork, & info );

        /* Unallocate computation buffer */
        free( ( void * ) work );

        /* CompcsUte rotation matrix */
        R[0][0] = * ( csV     ) * * ( csU     ) + * ( csV + 1 ) * * ( csU + 3 ) + * ( csV + 2 ) * * ( csU + 6 );
        R[0][1] = * ( csV + 3 ) * * ( csU     ) + * ( csV + 4 ) * * ( csU + 3 ) + * ( csV + 5 ) * * ( csU + 6 );
        R[0][2] = * ( csV + 6 ) * * ( csU     ) + * ( csV + 7 ) * * ( csU + 3 ) + * ( csV + 8 ) * * ( csU + 6 );
        R[1][0] = * ( csV     ) * * ( csU + 1 ) + * ( csV + 1 ) * * ( csU + 4 ) + * ( csV + 2 ) * * ( csU + 7 );
        R[1][1] = * ( csV + 3 ) * * ( csU + 1 ) + * ( csV + 4 ) * * ( csU + 4 ) + * ( csV + 5 ) * * ( csU + 7 );
        R[1][2] = * ( csV + 6 ) * * ( csU + 1 ) + * ( csV + 7 ) * * ( csU + 4 ) + * ( csV + 8 ) * * ( csU + 7 );
        R[2][0] = * ( csV     ) * * ( csU + 2 ) + * ( csV + 1 ) * * ( csU + 5 ) + * ( csV + 2 ) * * ( csU + 8 );
        R[2][1] = * ( csV + 3 ) * * ( csU + 2 ) + * ( csV + 4 ) * * ( csU + 5 ) + * ( csV + 5 ) * * ( csU + 8 );
        R[2][2] = * ( csV + 6 ) * * ( csU + 2 ) + * ( csV + 7 ) * * ( csU + 5 ) + * ( csV + 8 ) * * ( csU + 8 );

        /* Verification of rotation matrix determinant */
        if ( (

            R[0][0] * ( R[1][1] * R[2][2] - R[2][1] * R[1][2] ) -
            R[0][1] * ( R[1][0] * R[2][2] - R[2][0] * R[1][2] ) +
            R[0][2] * ( R[1][0] * R[2][1] - R[2][0] * R[1][1] )

        ) < 0.0 ) {

            /* RecompcsUte rotation matrix */
            R[0][0] = * ( csV     ) * * ( csU     ) + * ( csV + 1 ) * * ( csU + 3 ) - * ( csV + 2 ) * * ( csU + 6 );
            R[0][1] = * ( csV + 3 ) * * ( csU     ) + * ( csV + 4 ) * * ( csU + 3 ) - * ( csV + 5 ) * * ( csU + 6 );
            R[0][2] = * ( csV + 6 ) * * ( csU     ) + * ( csV + 7 ) * * ( csU + 3 ) - * ( csV + 8 ) * * ( csU + 6 );
            R[1][0] = * ( csV     ) * * ( csU + 1 ) + * ( csV + 1 ) * * ( csU + 4 ) - * ( csV + 2 ) * * ( csU + 7 );
            R[1][1] = * ( csV + 3 ) * * ( csU + 1 ) + * ( csV + 4 ) * * ( csU + 4 ) - * ( csV + 5 ) * * ( csU + 7 );
            R[1][2] = * ( csV + 6 ) * * ( csU + 1 ) + * ( csV + 7 ) * * ( csU + 4 ) - * ( csV + 8 ) * * ( csU + 7 );
            R[2][0] = * ( csV     ) * * ( csU + 2 ) + * ( csV + 1 ) * * ( csU + 5 ) - * ( csV + 2 ) * * ( csU + 8 );
            R[2][1] = * ( csV + 3 ) * * ( csU + 2 ) + * ( csV + 4 ) * * ( csU + 5 ) - * ( csV + 5 ) * * ( csU + 8 );
            R[2][2] = * ( csV + 6 ) * * ( csU + 2 ) + * ( csV + 7 ) * * ( csU + 5 ) - * ( csV + 8 ) * * ( csU + 8 );

        }

        /* Compute translation vector */
        T[0] = - R[0][0] * csMean[0] - R[0][1] * csMean[1] - R[0][2] * csMean[2] + csMean[3];
        T[1] = - R[1][0] * csMean[0] - R[1][1] * csMean[1] - R[1][2] * csMean[2] + csMean[4];
        T[2] = - R[2][0] * csMean[0] - R[2][1] * csMean[1] - R[2][2] * csMean[2] + csMean[5];

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

