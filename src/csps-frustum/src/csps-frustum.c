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

        /* Stream pointer variables */
        char csGPSd[256] = { 0 };
        char csGPSm[256] = { 0 };
        char csIMUd[256] = { 0 };
        char csIMUm[256] = { 0 };

        /* Camera designation variables */
        char csCameraA[256] = { 0 };
        char csCameraB[256] = { 0 };

        /* Channel designation variables */
        int csChannelA = 0;
        int csChannelB = 0;

        /* Sensor size variables */
        double csNPlane =  1.0;
        double csFPlane = 30.0;

        /* Timestamps variables */
        lp_Time_t csTSA = lp_Time_s( 0 );
        lp_Time_t csTUA = lp_Time_s( 0 );
        lp_Time_t csTSB = lp_Time_s( 0 );
        lp_Time_t csTUB = lp_Time_s( 0 );

        /* Search in parameters */
        cs_stdp( cs_stda( argc, argv, "--path"         , "-p" ), argv,   csPath    , CS_STRING );
        cs_stdp( cs_stda( argc, argv, "--camera-a"     , "-c" ), argv,   csCameraA , CS_STRING );
        cs_stdp( cs_stda( argc, argv, "--camera-b"     , "-d" ), argv,   csCameraB , CS_STRING );
        cs_stdp( cs_stda( argc, argv, "--gps-tag"      , "-g" ), argv,   csGPSd    , CS_STRING );
        cs_stdp( cs_stda( argc, argv, "--gps-mod"      , "-m" ), argv,   csGPSm    , CS_STRING );
        cs_stdp( cs_stda( argc, argv, "--imu-tag"      , "-i" ), argv,   csIMUd    , CS_STRING );
        cs_stdp( cs_stda( argc, argv, "--imu-mod"      , "-k" ), argv,   csIMUm    , CS_STRING );
        cs_stdp( cs_stda( argc, argv, "--channel-a"    , "-i" ), argv, & csChannelA, CS_INT    );
        cs_stdp( cs_stda( argc, argv, "--channel-b"    , "-j" ), argv, & csChannelB, CS_INT    );
        cs_stdp( cs_stda( argc, argv, "--plane-near"   , "-n" ), argv, & csNPlane  , CS_DOUBLE );
        cs_stdp( cs_stda( argc, argv, "--plane-far"    , "-f" ), argv, & csFPlane  , CS_DOUBLE );
        cs_stdp( cs_stda( argc, argv, "--second-a"     , "-a" ), argv, & csTSA     , CS_ULLONG );
        cs_stdp( cs_stda( argc, argv, "--microsecond-a", "-u" ), argv, & csTUA     , CS_ULLONG );
        cs_stdp( cs_stda( argc, argv, "--second-b"     , "-b" ), argv, & csTSB     , CS_ULLONG );
        cs_stdp( cs_stda( argc, argv, "--microsecond-b", "-v" ), argv, & csTUB     , CS_ULLONG );

        /* Execution switch */
        if ( cs_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Check camera devices consistency */
            if ( 

                ( ( strcasecmp( csCameraA, LF_EYESIS4PI_1 ) == 0 ) ||( strcasecmp( csCameraA, LF_EYESIS4PI_2 ) == 0 ) ) &&
                ( ( strcasecmp( csCameraB, LF_EYESIS4PI_1 ) == 0 ) ||( strcasecmp( csCameraB, LF_EYESIS4PI_2 ) == 0 ) )

            ) {

                /* Check channels consistency */
                if ( ( csChannelA >= 0 ) && ( csChannelA < 26 ) && ( csChannelB >= 0 ) && ( csChannelB < 26 ) ) {

                    /* CSPS query structures */
                    lp_QueryPosition    csQpositA;
                    lp_QueryPosition    csQpositB;
                    lp_QueryOrientation csQorienA;
                    lp_QueryOrientation csQorienB;

                    /* CSPS timestamp variables */
                    lp_Time_t csTimeA = lp_timestamp_compose( csTSA, csTUA );
                    lp_Time_t csTimeB = lp_timestamp_compose( csTSB, csTUB );

                    /* CSPS query - Positions */
                    csQpositA = lp_query_position_by_timestamp( csPath, LP_DEVICE_TYPE_GPS, csGPSd, csGPSm, csTimeA );
                    csQpositB = lp_query_position_by_timestamp( csPath, LP_DEVICE_TYPE_GPS, csGPSd, csGPSm, csTimeB );

                    /* CSPS query - Orientations */
                    csQorienA = lp_query_orientation_by_timestamp( csPath, LP_DEVICE_TYPE_IMU, csIMUd, csIMUm, csTimeA );
                    csQorienB = lp_query_orientation_by_timestamp( csPath, LP_DEVICE_TYPE_IMU, csIMUd, csIMUm, csTimeB );

                    /* Check query status */
                    if ( 

                        ( csQpositA.qrStatus == LP_TRUE ) && 
                        ( csQpositB.qrStatus == LP_TRUE ) && 
                        ( csQorienA.qrStatus == LP_TRUE ) && 
                        ( csQorienB.qrStatus == LP_TRUE ) 

                    ) {

                        /* Frustum nadir vector variables */
                        double csNadirA[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
                        double csNadirB[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

                        /* Frustum right vector variables */
                        double csRightA[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
                        double csRightB[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

                        /* Relative position variables */
                        double csPositionA[3] = { 0.0, 0.0, 0.0 };
                        double csPositionB[3] = { 0.0, 0.0, 0.0 };

                        /* Frustums summits variables */
                        double csFrustumAx[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };
                        double csFrustumAy[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };
                        double csFrustumAz[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };
                        double csFrustumBx[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };
                        double csFrustumBy[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };
                        double csFrustumBz[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };

                        /* Obtain eyesis4pi frame nadir/right/position vector */
                        cs_frustum_eyesis4pi( csCameraA, csChannelA, csNadirA, csRightA, csPositionA );
                        cs_frustum_eyesis4pi( csCameraB, csChannelB, csNadirB, csRightB, csPositionB );

                        /* Synchronize frustums nadir to earth */
                        csNadirA[3] = csNadirA[0] * csQorienA.qrfxx + csNadirA[1] * csQorienA.qrfyx + csNadirA[2] * csQorienA.qrfzx;
                        csNadirA[4] = csNadirA[0] * csQorienA.qrfxy + csNadirA[1] * csQorienA.qrfyy + csNadirA[2] * csQorienA.qrfzy;
                        csNadirA[5] = csNadirA[0] * csQorienA.qrfxz + csNadirA[1] * csQorienA.qrfyz + csNadirA[2] * csQorienA.qrfzz;
                        csNadirB[3] = csNadirB[0] * csQorienB.qrfxx + csNadirB[1] * csQorienB.qrfyx + csNadirB[2] * csQorienB.qrfzx;
                        csNadirB[4] = csNadirB[0] * csQorienB.qrfxy + csNadirB[1] * csQorienB.qrfyy + csNadirB[2] * csQorienB.qrfzy;
                        csNadirB[5] = csNadirB[0] * csQorienB.qrfxz + csNadirB[1] * csQorienB.qrfyz + csNadirB[2] * csQorienB.qrfzz;

                        /* Synchronize frustums right to earth */
                        csRightA[3] = csRightA[0] * csQorienA.qrfxx + csRightA[1] * csQorienA.qrfyx + csRightA[2] * csQorienA.qrfzx;
                        csRightA[4] = csRightA[0] * csQorienA.qrfxy + csRightA[1] * csQorienA.qrfyy + csRightA[2] * csQorienA.qrfzy;
                        csRightA[5] = csRightA[0] * csQorienA.qrfxz + csRightA[1] * csQorienA.qrfyz + csRightA[2] * csQorienA.qrfzz;
                        csRightB[3] = csRightB[0] * csQorienB.qrfxx + csRightB[1] * csQorienB.qrfyx + csRightB[2] * csQorienB.qrfzx;
                        csRightB[4] = csRightB[0] * csQorienB.qrfxy + csRightB[1] * csQorienB.qrfyy + csRightB[2] * csQorienB.qrfzy;
                        csRightB[5] = csRightB[0] * csQorienB.qrfxz + csRightB[1] * csQorienB.qrfyz + csRightB[2] * csQorienB.qrfzz;

                        /* Synchronize second position to relative earth */
                        csPositionB[0] += ( csQpositB.qrLongitude - csQpositA.qrLongitude ) * ( 6367514.500000 + csQpositA.qrAltitude );
                        csPositionB[1] += ( csQpositB.qrLatitude  - csQpositA.qrLatitude  ) * ( 6367514.500000 + csQpositA.qrAltitude );
                        csPositionB[2] += ( csQpositB.qrAltitude  - csQpositA.qrAltitude  );

                        /* Compute frustum A summits */
                        cs_frustum_summit( 

                            csNadirA,
                            csRightA,
                            csPositionA,
                            lf_query_sensor( csCameraA, LF_EYESIS4PI_PIXELSIZE, csChannelA, NULL ),
                            lf_query_sensor( csCameraA, LF_EYESIS4PI_FOCAL    , csChannelA, NULL ),
                            lf_query_sensor( csCameraA, LF_EYESIS4PI_SENWIDTH , csChannelA, NULL ),
                            lf_query_sensor( csCameraA, LF_EYESIS4PI_SENHEIGHT, csChannelA, NULL ),
                            csNPlane,
                            csFPlane,
                            csFrustumAx,
                            csFrustumAy,
                            csFrustumAz

                        );

                        /* Compute frustum B summits */
                        cs_frustum_summit( 

                            csNadirB,
                            csRightB,
                            csPositionB,
                            lf_query_sensor( csCameraB, LF_EYESIS4PI_PIXELSIZE, csChannelB, NULL ),
                            lf_query_sensor( csCameraB, LF_EYESIS4PI_FOCAL    , csChannelB, NULL ),
                            lf_query_sensor( csCameraB, LF_EYESIS4PI_SENWIDTH , csChannelB, NULL ),
                            lf_query_sensor( csCameraB, LF_EYESIS4PI_SENHEIGHT, csChannelB, NULL ),
                            csNPlane,
                            csFPlane,
                            csFrustumBx,
                            csFrustumBy,
                            csFrustumBz

                        );

                        /* Frustum intersection detection */
                        if ( cs_frustum_intersection( 

                            csFrustumAx,
                            csFrustumAy,
                            csFrustumAz,
                            csFrustumBx,
                            csFrustumBy,
                            csFrustumBz

                        ) == CS_TRUE ) fprintf( CS_OUT, "TRUE" ); else fprintf( CS_OUT, "FALSE" );
                
                    /* Display message */
                    } else { fprintf( CS_OUT, "Error : CSPS query by timestamp failed\n" ); }

                /* Display message */
                } else { fprintf( CS_OUT, "Error : Device sensor index out of calibration range\n" ); }

            /* Display message */
            } else { fprintf( CS_OUT, "Error : Unknown camera device\n" ); }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Eyesis4Pi frustum vectors composer
*/

    void cs_frustum_eyesis4pi( 

        char   const * const csCamera, 
        int    const         csChannel, 
        double       * const csNadir, 
        double       * const csRight,
        double       * const csPosition

    ) {

        /* Calibration data variables */
        double csAzimuth   = - lf_query_sensor( csCamera, LF_EYESIS4PI_AZIMUTH, csChannel, NULL ) - lf_query_sensor( csCamera, LF_EYESIS4PI_HEADING, csChannel, NULL ) + ( LF_PI / 2.0 );
        double csElevation = - lf_query_sensor( csCamera, LF_EYESIS4PI_ELEVATION, csChannel, NULL );
        double csRoll      = + lf_query_sensor( csCamera, LF_EYESIS4PI_ROLL, csChannel, NULL );
        double csRadius    = + lf_query_sensor( csCamera, LF_EYESIS4PI_RADIUS, csChannel, NULL );
        double csHeight    = + lf_query_sensor( csCamera, LF_EYESIS4PI_HEIGHT, csChannel, NULL );
        double csPupil     = - lf_query_sensor( csCamera, LF_EYESIS4PI_PUPFORWARD, csChannel, NULL );

        /* Extract roll remanant */
        csRoll = ( csRoll < 0.0 ) ? ( - ( LF_PI / 2.0 ) - csRoll ) : ( + ( LF_PI / 2.0 ) - csRoll );

        /* Trigonometric value variables */
        double csCosA = cos( csAzimuth   );
        double csSinA = sin( csAzimuth   );
        double csCosE = cos( csElevation );
        double csSinE = sin( csElevation );
        double csCosR = cos( csRoll      );
        double csSinR = sin( csRoll      );

        /* Compute frustum-nadir vectors */
        csNadir[0] = + csCosA * csCosE;
        csNadir[1] = + csSinA * csCosE;
        csNadir[2] = - csSinE;

        /* Compute frustum-right vectors */
        csRight[0] = + csCosA * csSinE * csSinR - csSinA * csCosR;
        csRight[1] = + csSinA * csSinE * csSinR + csCosA * csCosR;
        csRight[2] = + csCosE * csSinR;

        /* Compute position vector */
        csPosition[0] = + ( csRadius - csPupil ) * csCosA;
        csPosition[1] = + ( csRadius - csPupil ) * csSinA;
        csPosition[2] = + ( csSinE * csPupil ) + csHeight;

    }

/*
    Source - Frustum polyhedron summit computer
 */

    void cs_frustum_summit( 

        const double const * csNadir, 
        const double const * csRight, 
        const double const * csPosition, 
        const double         csPixel, 
        const double         csFocal, 
        const double         csWidth, 
        const double         csHeight, 
        const double         csNear, 
        const double         csFar,
        double *             csFX,
        double *             csFY,
        double *             csFZ

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
    Source - Frustum intersection detection
 */

    int cs_frustum_intersection(

        const double const * csFXa,
        const double const * csFYa,
        const double const * csFZa,
        const double const * csFXb,
        const double const * csFYb,
        const double const * csFZb

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
    Source - Static array extremums extractor
*/

    void cs_frustum_extremum( 

        const double const * csArray, 
        const int            csSize, 
        double *             csMaximum, 
        double *             csMinimum 

    ) {

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

