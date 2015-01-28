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

    # include "csps-align.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csPath[256] = { 0 };
        char csRigs[256] = { 0 };
        char csiPly[256] = { 0 };
        char csoPly[256] = { 0 };

        /* Stream pointer variables */
        char csGPSd[256] = { 0 };
        char csGPSm[256] = { 0 };

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--path"      , "-p" ), argv, csPath, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--rigs-path" , "-r" ), argv, csRigs, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--input-ply" , "-i" ), argv, csiPly, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--output-ply", "-o" ), argv, csoPly, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-tag"   , "-g" ), argv, csGPSd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-mod"   , "-n" ), argv, csGPSm, LC_STRING );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

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

    void cs_omvg_align_lt( int const csN, double const * const csrData, double const * const csaData, double csR[3][3], double csT[3] ) {

        /* Parsing variables */
        unsigned long csParse = 0;

        /* Coordinates mean variables */
        double csMean[6] = { 0.0 };

        /* Matrix variables */
        double csH[9] = { 0.0 };
        double csU[9] = { 0.0 };
        double csS[3] = { 0.0 };
        double csV[9] = { 0.0 };

        /* Matrix parameter variables */
        int csld   = 3;
        int csRows = 3;
        int csCols = 3;

        /* Decomposition buffer variables */
        int csReturn = 0;
        int csWork   = -1;

        /* Decomposition buffer variables */
        double   csQuery  = 0.0;
        double * csBuffer = NULL;

        /* Coordinates accumulation */
        for ( csParse = 0; csParse < csN; csParse ++ ) {

            csMean[0] += * ( csrData + csParse * 3     );
            csMean[1] += * ( csrData + csParse * 3 + 1 );
            csMean[2] += * ( csrData + csParse * 3 + 2 );
            csMean[3] += * ( csaData + csParse * 3     );
            csMean[4] += * ( csaData + csParse * 3 + 1 );
            csMean[5] += * ( csaData + csParse * 3 + 2 );

        }

        /* Compute mean of coordinates */
        csMean[0] /= ( double ) csN;
        csMean[1] /= ( double ) csN;
        csMean[2] /= ( double ) csN;
        csMean[3] /= ( double ) csN;
        csMean[4] /= ( double ) csN;
        csMean[5] /= ( double ) csN;

        /* Compute accumulated matrix */
        for ( csParse = 0; csParse < csN; csParse ++ ) {

            csH[0] += ( * ( csrData + csParse * 3     ) - csMean[0] ) * ( * ( csaData + csParse * 3     ) - csMean[3] );
            csH[1] += ( * ( csrData + csParse * 3     ) - csMean[0] ) * ( * ( csaData + csParse * 3 + 1 ) - csMean[4] );
            csH[2] += ( * ( csrData + csParse * 3     ) - csMean[0] ) * ( * ( csaData + csParse * 3 + 2 ) - csMean[5] );
            csH[3] += ( * ( csrData + csParse * 3 + 1 ) - csMean[1] ) * ( * ( csaData + csParse * 3     ) - csMean[3] );
            csH[4] += ( * ( csrData + csParse * 3 + 1 ) - csMean[1] ) * ( * ( csaData + csParse * 3 + 1 ) - csMean[4] );
            csH[5] += ( * ( csrData + csParse * 3 + 1 ) - csMean[1] ) * ( * ( csaData + csParse * 3 + 2 ) - csMean[5] );
            csH[6] += ( * ( csrData + csParse * 3 + 2 ) - csMean[2] ) * ( * ( csaData + csParse * 3     ) - csMean[3] );
            csH[7] += ( * ( csrData + csParse * 3 + 2 ) - csMean[2] ) * ( * ( csaData + csParse * 3 + 1 ) - csMean[4] );
            csH[8] += ( * ( csrData + csParse * 3 + 2 ) - csMean[2] ) * ( * ( csaData + csParse * 3 + 2 ) - csMean[5] );

        }

        /* Singular values decompostion - computation buffer */
        dgesvd_( "All", "All", & csRows, & csCols, csH, & csld, csS, csU, & csld, csV, & csld, & csQuery, & csWork, & csReturn );

        /* Allocate computation buffer */
        csBuffer = ( double * ) malloc( csWork = ( ( int ) csQuery ) * sizeof( double ) );

        /* Singular values decompostion */
        dgesvd_( "All", "All", & csRows, & csCols, csH, & csld, csS, csU, & csld, csV, & csld, csBuffer, & csWork, & csReturn );

        /* Unallocate computation buffer */
        free( ( void * ) csBuffer );

        /* CompcsUte rotation matrix */
        csR[0][0] = * ( csV     ) * * ( csU     ) + * ( csV + 1 ) * * ( csU + 3 ) + * ( csV + 2 ) * * ( csU + 6 );
        csR[0][1] = * ( csV + 3 ) * * ( csU     ) + * ( csV + 4 ) * * ( csU + 3 ) + * ( csV + 5 ) * * ( csU + 6 );
        csR[0][2] = * ( csV + 6 ) * * ( csU     ) + * ( csV + 7 ) * * ( csU + 3 ) + * ( csV + 8 ) * * ( csU + 6 );
        csR[1][0] = * ( csV     ) * * ( csU + 1 ) + * ( csV + 1 ) * * ( csU + 4 ) + * ( csV + 2 ) * * ( csU + 7 );
        csR[1][1] = * ( csV + 3 ) * * ( csU + 1 ) + * ( csV + 4 ) * * ( csU + 4 ) + * ( csV + 5 ) * * ( csU + 7 );
        csR[1][2] = * ( csV + 6 ) * * ( csU + 1 ) + * ( csV + 7 ) * * ( csU + 4 ) + * ( csV + 8 ) * * ( csU + 7 );
        csR[2][0] = * ( csV     ) * * ( csU + 2 ) + * ( csV + 1 ) * * ( csU + 5 ) + * ( csV + 2 ) * * ( csU + 8 );
        csR[2][1] = * ( csV + 3 ) * * ( csU + 2 ) + * ( csV + 4 ) * * ( csU + 5 ) + * ( csV + 5 ) * * ( csU + 8 );
        csR[2][2] = * ( csV + 6 ) * * ( csU + 2 ) + * ( csV + 7 ) * * ( csU + 5 ) + * ( csV + 8 ) * * ( csU + 8 );

        /* Verification of rotation matrix determinant */
        if ( (

            csR[0][0] * ( csR[1][1] * csR[2][2] - csR[2][1] * csR[1][2] ) -
            csR[0][1] * ( csR[1][0] * csR[2][2] - csR[2][0] * csR[1][2] ) +
            csR[0][2] * ( csR[1][0] * csR[2][1] - csR[2][0] * csR[1][1] )

        ) < 0.0 ) {

            /* RecompcsUte rotation matrix */
            csR[0][0] = * ( csV     ) * * ( csU     ) + * ( csV + 1 ) * * ( csU + 3 ) - * ( csV + 2 ) * * ( csU + 6 );
            csR[0][1] = * ( csV + 3 ) * * ( csU     ) + * ( csV + 4 ) * * ( csU + 3 ) - * ( csV + 5 ) * * ( csU + 6 );
            csR[0][2] = * ( csV + 6 ) * * ( csU     ) + * ( csV + 7 ) * * ( csU + 3 ) - * ( csV + 8 ) * * ( csU + 6 );
            csR[1][0] = * ( csV     ) * * ( csU + 1 ) + * ( csV + 1 ) * * ( csU + 4 ) - * ( csV + 2 ) * * ( csU + 7 );
            csR[1][1] = * ( csV + 3 ) * * ( csU + 1 ) + * ( csV + 4 ) * * ( csU + 4 ) - * ( csV + 5 ) * * ( csU + 7 );
            csR[1][2] = * ( csV + 6 ) * * ( csU + 1 ) + * ( csV + 7 ) * * ( csU + 4 ) - * ( csV + 8 ) * * ( csU + 7 );
            csR[2][0] = * ( csV     ) * * ( csU + 2 ) + * ( csV + 1 ) * * ( csU + 5 ) - * ( csV + 2 ) * * ( csU + 8 );
            csR[2][1] = * ( csV + 3 ) * * ( csU + 2 ) + * ( csV + 4 ) * * ( csU + 5 ) - * ( csV + 5 ) * * ( csU + 8 );
            csR[2][2] = * ( csV + 6 ) * * ( csU + 2 ) + * ( csV + 7 ) * * ( csU + 5 ) - * ( csV + 8 ) * * ( csU + 8 );

        }

        /* Compute translation vector */
        csT[0] = - csR[0][0] * csMean[0] - csR[0][1] * csMean[1] - csR[0][2] * csMean[2] + csMean[3];
        csT[1] = - csR[1][0] * csMean[0] - csR[1][1] * csMean[1] - csR[1][2] * csMean[2] + csMean[4];
        csT[2] = - csR[2][0] * csMean[0] - csR[2][1] * csMean[1] - csR[2][2] * csMean[2] + csMean[5];

    }

