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

    # include "csps-frustum.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csPath[256] = { 0 };

        /* Timestamps variables */
        lp_Time_t csTSA = lp_Time_s( 0 );
        lp_Time_t csTUA = lp_Time_s( 0 );
        lp_Time_t csTSB = lp_Time_s( 0 );
        lp_Time_t csTUB = lp_Time_s( 0 );

        /* Sensor size variables */
        double csNPlane =  1.0;
        double csFPlane = 30.0;

        /* Search in parameters */
        cs_stdp( cs_stda( argc, argv, "--path"         , "-p" ), argv,   csPath   , CS_STRING );
        cs_stdp( cs_stda( argc, argv, "--second-a"     , "-a" ), argv, & csTSA    , CS_ULLONG );
        cs_stdp( cs_stda( argc, argv, "--microsecond-a", "-u" ), argv, & csTUA    , CS_ULLONG );
        cs_stdp( cs_stda( argc, argv, "--second-b"     , "-b" ), argv, & csTSB    , CS_ULLONG );
        cs_stdp( cs_stda( argc, argv, "--microsecond-b", "-v" ), argv, & csTUB    , CS_ULLONG );
        cs_stdp( cs_stda( argc, argv, "--near-plane"   , "-n" ), argv, & csNPlane , CS_DOUBLE );
        cs_stdp( cs_stda( argc, argv, "--far-plane"    , "-f" ), argv, & csFPlane , CS_DOUBLE );

        /* Execution switch */
        if ( cs_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Frustum intersection detection
 */

    int cs_frustum_intersection(

        double * csFXa,
        double * csFYa,
        double * csFZa,
        double * csFXb,
        double * csFYb,
        double * csFZb

    ) {

        /* X-Extremum variables */
        double csXAmin = 0.0;
        double csXAmax = 0.0;
        double csXBmin = 0.0;
        double csXBmax = 0.0;

        /* Y-Extremum variables */
        double csYAmin = 0.0;
        double csYAmax = 0.0;
        double csYBmin = 0.0;
        double csYBmax = 0.0;

        /* Z-Extremum variables */
        double csZAmin = 0.0;
        double csZAmax = 0.0;
        double csZBmin = 0.0;
        double csZBmax = 0.0;

        /* Result variables */
        int csCondition = 0;

        /* Extract x-extremums */
        cs_frustum_extremum( csFXa, 8, & ( csXAmax ), & ( csXAmin ) );
        cs_frustum_extremum( csFXb, 8, & ( csXBmax ), & ( csXBmin ) );

        /* Extract y-extremums */
        cs_frustum_extremum( csFYa, 8, & ( csYAmax ), & ( csYAmin ) );
        cs_frustum_extremum( csFYb, 8, & ( csYBmax ), & ( csYBmin ) );

        /* Extract z-extremums */
        cs_frustum_extremum( csFZa, 8, & ( csZAmax ), & ( csZAmin ) );
        cs_frustum_extremum( csFZb, 8, & ( csZBmax ), & ( csZBmin ) );

        /* Primary condition on x-direction */
        if ( csXAmin < csXBmin ) {

            /* Secondary condition on x-direction */
            if ( csXBmin < csXAmax ) csCondition ++;

        } else {

            /* Secondary condition on x-direction */
            if ( csXAmin < csXBmax ) csCondition ++;

        }

        /* Primary condition on y-direction */
        if ( csYAmin < csYBmin ) {

            /* Secondary condition on y-direction */
            if ( csYBmin < csYAmax ) csCondition ++;

        } else {

            /* Secondary condition on y-direction */
            if ( csYAmin < csYBmax ) csCondition ++;

        }

        /* Primary condition on z-direction */
        if ( csZAmin < csZBmin ) {

            /* Secondary condition on z-direction */
            if ( csZBmin < csZAmax ) csCondition ++;

        } else {

            /* Secondary condition on z-direction */
            if ( csZAmin < csZBmax ) csCondition ++;

        }

        /* Return results */
        if ( csCondition == 3 ) return( CS_TRUE ); else return( CS_FALSE );

    }

/*
    Source - Frustum polyhedron summit computer
 */

    void cs_frustum_summit( 

        double * csNadir, 
        double * csRight, 
        double * csPosition, 
        double   csPixel, 
        double   csFocal, 
        double   csWidth, 
        double   csHeight, 
        double   csNear, 
        double   csFar,
        double * csFX,
        double * csFY,
        double * csFZ

    ) {

        /* Bottom vector variables */
        double csBottom[3] = {

            csNadir[1] * csRight[2] - csNadir[2] * csRight[1],
            csNadir[2] * csRight[0] - csNadir[0] * csRight[2],
            csNadir[0] * csRight[1] - csNadir[1] * csRight[0],

        };

        /* Nadir near point variables */
        double csNearPoint[3] = {

            csPosition[0] + csNadir[0] * csNear,
            csPosition[1] + csNadir[1] * csNear,
            csPosition[2] + csNadir[2] * csNear

        };

        /* Nadir far point variables */
        double csFarPoint[3] = {

            csPosition[0] + csNadir[0] * csFar,
            csPosition[1] + csNadir[1] * csFar,
            csPosition[2] + csNadir[2] * csFar

        };

        /* Normalized sensor size variables */
        double csNormalWidth  = ( ( csWidth  / 2.0 ) * csPixel ) / csFocal;
        double csNormalHeight = ( ( csHeight / 2.0 ) * csPixel ) / csFocal;

        /* Compute frustum polyhedron summits x-near */
        csFX[0] = csNearPoint[0] + ( csRight[0] * csNormalWidth * csNear ) + ( csBottom[0] * csNormalHeight * csNear );
        csFX[1] = csNearPoint[0] - ( csRight[0] * csNormalWidth * csNear ) + ( csBottom[0] * csNormalHeight * csNear );
        csFX[2] = csNearPoint[0] - ( csRight[0] * csNormalWidth * csNear ) - ( csBottom[0] * csNormalHeight * csNear );
        csFX[3] = csNearPoint[0] + ( csRight[0] * csNormalWidth * csNear ) - ( csBottom[0] * csNormalHeight * csNear );

        /* Compute frustum polyhedron summits y-near */
        csFY[0] = csNearPoint[1] + ( csRight[1] * csNormalWidth * csNear ) + ( csBottom[1] * csNormalHeight * csNear );
        csFY[1] = csNearPoint[1] - ( csRight[1] * csNormalWidth * csNear ) + ( csBottom[1] * csNormalHeight * csNear );
        csFY[2] = csNearPoint[1] - ( csRight[1] * csNormalWidth * csNear ) - ( csBottom[1] * csNormalHeight * csNear );
        csFY[3] = csNearPoint[1] + ( csRight[1] * csNormalWidth * csNear ) - ( csBottom[1] * csNormalHeight * csNear );

        /* Compute frustum polyhedron summits z-near */
        csFZ[0] = csNearPoint[2] + ( csRight[2] * csNormalWidth * csNear ) + ( csBottom[2] * csNormalHeight * csNear );
        csFZ[1] = csNearPoint[2] - ( csRight[2] * csNormalWidth * csNear ) + ( csBottom[2] * csNormalHeight * csNear );
        csFZ[2] = csNearPoint[2] - ( csRight[2] * csNormalWidth * csNear ) - ( csBottom[2] * csNormalHeight * csNear );
        csFZ[3] = csNearPoint[2] + ( csRight[2] * csNormalWidth * csNear ) - ( csBottom[2] * csNormalHeight * csNear );

        /* Compute frustum polyhedron summits x-far */
        csFX[4] = csFarPoint[0] + ( csRight[0] * csNormalWidth * csFar ) + ( csBottom[0] * csNormalHeight * csFar );
        csFX[5] = csFarPoint[0] - ( csRight[0] * csNormalWidth * csFar ) + ( csBottom[0] * csNormalHeight * csFar );
        csFX[6] = csFarPoint[0] - ( csRight[0] * csNormalWidth * csFar ) - ( csBottom[0] * csNormalHeight * csFar );
        csFX[7] = csFarPoint[0] + ( csRight[0] * csNormalWidth * csFar ) - ( csBottom[0] * csNormalHeight * csFar );

        /* Compute frustum polyhedron summits y-far */
        csFY[4] = csFarPoint[1] + ( csRight[1] * csNormalWidth * csFar ) + ( csBottom[1] * csNormalHeight * csFar );
        csFY[5] = csFarPoint[1] - ( csRight[1] * csNormalWidth * csFar ) + ( csBottom[1] * csNormalHeight * csFar );
        csFY[6] = csFarPoint[1] - ( csRight[1] * csNormalWidth * csFar ) - ( csBottom[1] * csNormalHeight * csFar );
        csFY[7] = csFarPoint[1] + ( csRight[1] * csNormalWidth * csFar ) - ( csBottom[1] * csNormalHeight * csFar );

        /* Compute frustum polyhedron summits z-far */
        csFZ[4] = csFarPoint[2] + ( csRight[2] * csNormalWidth * csFar ) + ( csBottom[2] * csNormalHeight * csFar );
        csFZ[5] = csFarPoint[2] - ( csRight[2] * csNormalWidth * csFar ) + ( csBottom[2] * csNormalHeight * csFar );
        csFZ[6] = csFarPoint[2] - ( csRight[2] * csNormalWidth * csFar ) - ( csBottom[2] * csNormalHeight * csFar );
        csFZ[7] = csFarPoint[2] + ( csRight[2] * csNormalWidth * csFar ) - ( csBottom[2] * csNormalHeight * csFar );

    }

/*
    Source - Static array extremums extractor
*/

    void cs_frustum_extremum( double * csArray, int csSize, double * csMaximum, double * csMinimum ) {

        /* Parsing variables */
        int csParse = 0;

        /* Initialize extremums */
        * ( csMaximum ) = -DBL_MAX;
        * ( csMinimum ) = +DBL_MAX;

        /* Parse the array */
        for ( csParse = 0; csParse < csSize; csParse ++ ) {

            /* Update extremums */
            if ( csArray[csParse] > * ( csMaximum ) ) * ( csMaximum ) = csArray[csParse];
            if ( csArray[csParse] < * ( csMinimum ) ) * ( csMinimum ) = csArray[csParse];

        }

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

