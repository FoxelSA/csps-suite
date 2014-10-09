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

                        /* Frustums variables */
                        cs_Frustum_t csFrus_A, csFrus_B;

                        /* Compute corrected positions - Local flat earth model */
                        csQpositB.qrLongitude = ( csQpositB.qrLongitude - csQpositA.qrLongitude ) * ( ( ( 6367514.5 + csQpositA.qrAltitude ) * LF_PI / 180.0 ) );
                        csQpositB.qrLatitude  = ( csQpositB.qrLatitude  - csQpositA.qrLatitude  ) * ( ( ( 6367514.5 + csQpositA.qrAltitude ) * LF_PI / 180.0 ) );
                        csQpositB.qrAltitude  = ( csQpositB.qrAltitude  - csQpositA.qrAltitude  );

                        /* Compute frustum of first camera */
                        cs_frustum_eyesis4pi( 

                            csCameraA, csChannelA,
                            csQorienA.qrfxx, csQorienA.qrfxy, csQorienA.qrfxz,
                            csQorienA.qrfyx, csQorienA.qrfyy, csQorienA.qrfyz,
                            csQorienA.qrfzx, csQorienA.qrfzy, csQorienA.qrfzz,
                            0.0, 0.0, 0.0,
                            csNPlane, csFPlane, & csFrus_A

                        );

                        /* Compute frustum of second camera */
                        cs_frustum_eyesis4pi( 

                            csCameraB, csChannelB,
                            csQorienB.qrfxx, csQorienB.qrfxy, csQorienB.qrfxz,
                            csQorienB.qrfyx, csQorienB.qrfyy, csQorienB.qrfyz,
                            csQorienB.qrfzx, csQorienB.qrfzy, csQorienB.qrfzz,
                            csQpositB.qrLongitude, csQpositB.qrLatitude, csQpositB.qrAltitude,
                            csNPlane, csFPlane, & csFrus_B

                        );

                        /* Frustum intersection detection */
                        if ( cs_frustum_intersection( & csFrus_A, & csFrus_B ) == CS_TRUE ) fprintf( CS_OUT, "TRUE" ); else fprintf( CS_OUT, "FALSE" );
                
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
    Source - Eyesis4Pi frustum composer
*/

    void cs_frustum_eyesis4pi(

        char         const * const csCamera, 
        int          const         csChannel, 
        double       const         csEFxx,
        double       const         csEFxy,
        double       const         csEFxz,
        double       const         csEFyx,
        double       const         csEFyy,
        double       const         csEFyz,
        double       const         csEFzx,
        double       const         csEFzy,
        double       const         csEFzz,
        double       const         csEFpx,
        double       const         csEFpy,
        double       const         csEFpz,
        double       const         csNear,
        double       const         csFar,
        cs_Frustum_t       * const csFrustum

    ) {

        /* Eyesis4pi frame variables */
        double csEyesisframe[3] = { 0.0, 0.0, 0.0 };

        /* Eyesis4pi device calibration variables */
        double csAzimuth   = - lf_query_sensor( csCamera, LF_EYESIS4PI_AZIMUTH   , csChannel, NULL );
        double csHeading   = - lf_query_sensor( csCamera, LF_EYESIS4PI_HEADING   , csChannel, NULL );
        double csElevation = - lf_query_sensor( csCamera, LF_EYESIS4PI_ELEVATION , csChannel, NULL );
        double csRoll      = - lf_query_sensor( csCamera, LF_EYESIS4PI_ROLL      , csChannel, NULL );
        double csRadius    = + lf_query_sensor( csCamera, LF_EYESIS4PI_RADIUS    , csChannel, NULL );
        double csHeight    = + lf_query_sensor( csCamera, LF_EYESIS4PI_HEIGHT    , csChannel, NULL );
        double csPupil     = - lf_query_sensor( csCamera, LF_EYESIS4PI_PUPFORWARD, csChannel, NULL );        
        double csPixel     = + lf_query_sensor( csCamera, LF_EYESIS4PI_PIXELSIZE , csChannel, NULL );
        double csFocal     = + lf_query_sensor( csCamera, LF_EYESIS4PI_FOCAL     , csChannel, NULL );
        double csSenWidth  = + lf_query_sensor( csCamera, LF_EYESIS4PI_SENWIDTH  , csChannel, NULL );
        double csSenHeight = + lf_query_sensor( csCamera, LF_EYESIS4PI_SENHEIGHT , csChannel, NULL );

        /* Compute corrected azimuth angle */
        csAzimuth = csAzimuth + csHeading + ( LF_PI / 2.0 );

        /* Compute frustum left/top appertures */
        csFrustum->fsLefApp = ( ( csSenWidth  / 2.0 ) * csPixel ) / csFocal;
        csFrustum->fsTopApp = ( ( csSenHeight / 2.0 ) * csPixel ) / csFocal;

        /* Compute frustum near/far planes */
        csFrustum->fsNear = csNear;
        csFrustum->fsFar  = csFar;

        /* Compute frustum nadir-vector in eyesis frame */
        csEyesisframe[0] = + cos( csElevation ) * cos( csAzimuth );
        csEyesisframe[1] = + cos( csElevation ) * sin( csAzimuth );
        csEyesisframe[2] = - sin( csElevation );

        /* Compute frustum nadir-vector in earth frame */
        csFrustum->fsNad[0] = csEyesisframe[0] * csEFxx + csEyesisframe[1] * csEFyx + csEyesisframe[2] * csEFzx;
        csFrustum->fsNad[1] = csEyesisframe[0] * csEFxy + csEyesisframe[1] * csEFyy + csEyesisframe[2] * csEFzy;
        csFrustum->fsNad[2] = csEyesisframe[0] * csEFxz + csEyesisframe[1] * csEFyz + csEyesisframe[2] * csEFzz;

        /* Compute frustum left-vector in eyesis frame */
        csEyesisframe[0] = + sin( csElevation ) * sin( csRoll ) * cos( csAzimuth ) - cos( csRoll ) * sin( csAzimuth );
        csEyesisframe[1] = + sin( csElevation ) * sin( csRoll ) * sin( csAzimuth ) + cos( csRoll ) * cos( csAzimuth );
        csEyesisframe[2] = + cos( csElevation ) * sin( csRoll );

        /* Compute frustum nadir-vector in earth frame */
        csFrustum->fsLef[0] = csEyesisframe[0] * csEFxx + csEyesisframe[1] * csEFyx + csEyesisframe[2] * csEFzx;
        csFrustum->fsLef[1] = csEyesisframe[0] * csEFxy + csEyesisframe[1] * csEFyy + csEyesisframe[2] * csEFzy;
        csFrustum->fsLef[2] = csEyesisframe[0] * csEFxz + csEyesisframe[1] * csEFyz + csEyesisframe[2] * csEFzz;

        /* Compute frustum top-vector in earth frame */
        csFrustum->fsTop[0] = csFrustum->fsNad[1] * csFrustum->fsLef[2] - csFrustum->fsNad[2] * csFrustum->fsLef[1];
        csFrustum->fsTop[1] = csFrustum->fsNad[2] * csFrustum->fsLef[0] - csFrustum->fsNad[0] * csFrustum->fsLef[2];
        csFrustum->fsTop[2] = csFrustum->fsNad[0] * csFrustum->fsLef[1] - csFrustum->fsNad[1] * csFrustum->fsLef[0];

        /* Compute frustum origin point in earth frame */
        csFrustum->fsOrg[0] = + csEFpx + ( csRadius - csPupil ) * cos( csAzimuth );
        csFrustum->fsOrg[1] = + csEFpy + ( csRadius - csPupil ) * sin( csAzimuth );
        csFrustum->fsOrg[2] = + csEFpz + ( sin( csElevation ) * csPupil ) + csHeight;

        /* Compute frustum near-principal point in earth frame */
        csFrustum->fsNPP[0] = csFrustum->fsOrg[0] + csFrustum->fsNad[0] * csFrustum->fsNear;
        csFrustum->fsNPP[1] = csFrustum->fsOrg[1] + csFrustum->fsNad[1] * csFrustum->fsNear;
        csFrustum->fsNPP[2] = csFrustum->fsOrg[2] + csFrustum->fsNad[2] * csFrustum->fsNear;

        /* Compute frustum far-principal point in earth frame */
        csFrustum->fsFPP[0] = csFrustum->fsOrg[0] + csFrustum->fsNad[0] * csFrustum->fsFar;
        csFrustum->fsFPP[1] = csFrustum->fsOrg[1] + csFrustum->fsNad[1] * csFrustum->fsFar;
        csFrustum->fsFPP[2] = csFrustum->fsOrg[2] + csFrustum->fsNad[2] * csFrustum->fsFar;

        /* Compute frustum summits in earth frame x-near-coordinates */
        csFrustum->fsSX[0] = csFrustum->fsNPP[0] + ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSX[1] = csFrustum->fsNPP[0] - ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSX[2] = csFrustum->fsNPP[0] - ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSX[3] = csFrustum->fsNPP[0] + ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsNear );

        /* Compute frustum summits in earth frame y-near-coordinates */
        csFrustum->fsSY[0] = csFrustum->fsNPP[1] + ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSY[1] = csFrustum->fsNPP[1] - ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSY[2] = csFrustum->fsNPP[1] - ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSY[3] = csFrustum->fsNPP[1] + ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsNear );

        /* Compute frustum summits in earth frame z-near-coordinates */
        csFrustum->fsSZ[0] = csFrustum->fsNPP[2] + ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSZ[1] = csFrustum->fsNPP[2] - ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsNear ) + ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSZ[2] = csFrustum->fsNPP[2] - ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsNear );
        csFrustum->fsSZ[3] = csFrustum->fsNPP[2] + ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsNear ) - ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsNear );

        /* Compute frustum summits in earth frame x-far-coordinates */
        csFrustum->fsSX[4] = csFrustum->fsFPP[0] + ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSX[5] = csFrustum->fsFPP[0] - ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSX[6] = csFrustum->fsFPP[0] - ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSX[7] = csFrustum->fsFPP[0] + ( csFrustum->fsLef[0] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[0] * csFrustum->fsTopApp * csFrustum->fsFar );

        /* Compute frustum summits in earth frame y-far-coordinates */
        csFrustum->fsSY[4] = csFrustum->fsFPP[1] + ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSY[5] = csFrustum->fsFPP[1] - ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSY[6] = csFrustum->fsFPP[1] - ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSY[7] = csFrustum->fsFPP[1] + ( csFrustum->fsLef[1] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[1] * csFrustum->fsTopApp * csFrustum->fsFar );

        /* Compute frustum summits in earth frame z-far-coordinates */
        csFrustum->fsSZ[4] = csFrustum->fsFPP[2] + ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSZ[5] = csFrustum->fsFPP[2] - ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsFar ) + ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSZ[6] = csFrustum->fsFPP[2] - ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsFar );
        csFrustum->fsSZ[7] = csFrustum->fsFPP[2] + ( csFrustum->fsLef[2] * csFrustum->fsLefApp * csFrustum->fsFar ) - ( csFrustum->fsTop[2] * csFrustum->fsTopApp * csFrustum->fsFar );

    }

/*
    Source - Frustum intersection detection
 */

    int cs_frustum_intersection(

        cs_Frustum_t const * const csFrus_A,
        cs_Frustum_t const * const csFrus_B

    ) {

        /* Frustum segment index variables */
        static int csEdge[12][2] = { { 1, 0 }, { 2, 1 }, { 3, 2 }, { 0, 3 }, { 5, 4 }, { 6, 5 }, { 7, 6 }, { 4, 7 }, { 4, 0 }, { 5, 1 }, { 6, 2 }, { 7, 3 } };

        /* Secondary loop variables */
        int csParse = 0;

        /* Edge point coordinates variables */
        double csInterX = 0.0;
        double csInterY = 0.0;
        double csInterZ = 0.0;

        /* Edge point component variables */
        double csCompX = 0.0;
        double csCompY = 0.0;
        double csCompZ = 0.0;

        /* Progressive rational range variables */
        double csStp = 1.0;
        double csPow = 2.0;
        double csPtn = 1.0 / 2.0;

        /* Return variables */
        int csResult = CS_FALSE;

        /* Progressive edge inclusion detection */
        while ( ( csPow < 64 ) && ( csResult == CS_FALSE ) ) {

            /* Secondary loop on polyhedron edges */
            for ( csParse = 0; csParse < 12; csParse ++ ) {

                /* Compute edge point in first frustum origin centered frame */
                csInterX = csPtn * ( csFrus_B->fsSX[csEdge[csParse][0]] - csFrus_B->fsSX[csEdge[csParse][1]] ) + csFrus_B->fsSX[csEdge[csParse][1]] - csFrus_A->fsOrg[0];
                csInterY = csPtn * ( csFrus_B->fsSY[csEdge[csParse][0]] - csFrus_B->fsSY[csEdge[csParse][1]] ) + csFrus_B->fsSY[csEdge[csParse][1]] - csFrus_A->fsOrg[1];
                csInterZ = csPtn * ( csFrus_B->fsSZ[csEdge[csParse][0]] - csFrus_B->fsSZ[csEdge[csParse][1]] ) + csFrus_B->fsSZ[csEdge[csParse][1]] - csFrus_A->fsOrg[2];

                /* Compute edge point coordinate in frist frustum nadir/left/top basis */
                csCompX = csFrus_A->fsNad[0] * csInterX + csFrus_A->fsNad[1] * csInterY + csFrus_A->fsNad[2] * csInterZ;
                csCompY = csFrus_A->fsLef[0] * csInterX + csFrus_A->fsLef[1] * csInterY + csFrus_A->fsLef[2] * csInterZ;
                csCompZ = csFrus_A->fsTop[0] * csInterX + csFrus_A->fsTop[1] * csInterY + csFrus_A->fsTop[2] * csInterZ;

                /* Verify intersection condition - principal direction */
                if ( ( csCompX >= csFrus_A->fsNear ) && ( csCompX <= csFrus_A->fsFar ) ) {

                    /* Verify intersection condition - secondary plane */
                    if ( ( fabs( csCompY ) <= ( csCompX * csFrus_A->fsLefApp ) ) && ( fabs( csCompZ ) <= ( csCompX * csFrus_A->fsTopApp ) ) ) {

                        /* Trigger intersection condition */
                        csResult = CS_TRUE;

                    }

                }

            }

            /* Compute next point */
            if ( ( csPtn += csStp ) > 1.0 ) {

                /* Parsing next rational scale */
                csStp = 1.0 / ( csPow );
                csPtn = 1.0 / ( csPow *= 2.0 );

            }

        }

        /* Return intersection detection results */
        return( csResult );

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

