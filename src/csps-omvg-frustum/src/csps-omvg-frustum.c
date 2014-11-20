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

    # include "csps-omvg-frustum.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csPath[256] = { 0 };
        char csList[256] = { 0 };
        char csPair[256] = { 0 };

        /* Stream pointer variables */
        char csGPSd[256] = { 0 };
        char csGPSm[256] = { 0 };
        char csIMUd[256] = { 0 };
        char csIMUm[256] = { 0 };

        /* Camera designation variables */
        char csCamera[256] = { 0 };

        /* Sensor size variables */
        double csNPlane =  1.0;
        double csFPlane = 30.0;

        /* Size variables */
        unsigned long csSize = 0;

        /* Pair composition variables */
        unsigned long csLoopA = 0;
        unsigned long csLoopB = 0;

        /* Timestamp delay variables */
        unsigned long csDelay = 0;

        /* Stack variables */
        cs_List_t * csStack = NULL;

        /* Output stream variables */
        FILE * csStream = NULL;

        /* Search in parameters */
        stdp( stda( argc, argv, "--path"      , "-p" ), argv,   csPath  , CS_STRING );
        stdp( stda( argc, argv, "--list"      , "-l" ), argv,   csList  , CS_STRING );
        stdp( stda( argc, argv, "--pairs"     , "-r" ), argv,   csPair  , CS_STRING );
        stdp( stda( argc, argv, "--camera"    , "-c" ), argv,   csCamera, CS_STRING );
        stdp( stda( argc, argv, "--gps-tag"   , "-g" ), argv,   csGPSd  , CS_STRING );
        stdp( stda( argc, argv, "--gps-mod"   , "-m" ), argv,   csGPSm  , CS_STRING );
        stdp( stda( argc, argv, "--imu-tag"   , "-i" ), argv,   csIMUd  , CS_STRING );
        stdp( stda( argc, argv, "--imu-mod"   , "-k" ), argv,   csIMUm  , CS_STRING );
        stdp( stda( argc, argv, "--plane-near", "-n" ), argv, & csNPlane, CS_DOUBLE );
        stdp( stda( argc, argv, "--plane-far" , "-f" ), argv, & csFPlane, CS_DOUBLE );
        stdp( stda( argc, argv, "--delay"     , "-d" ), argv, & csDelay , CS_ULONG  );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Import OpenMVG list */
            if ( ( csSize = cs_omvg_frusmtum_list( csList, & csStack, csDelay ) ) != 0 ) {

                /* Create output stream handle */
                if ( ( csStream = fopen( csPair, "w" ) ) != NULL ) {

                    /* CSPS query structures */
                    lp_Query_Position_t    csQpositA;
                    lp_Query_Position_t    csQpositB;
                    lp_Query_Orientation_t csQorienA;
                    lp_Query_Orientation_t csQorienB;

                    /* First level composition loop */
                    for ( csLoopA = 0; csLoopA < csSize; csLoopA ++ ) {

                        /* CSPS query - Positions */
                        csQpositA = lp_query_position_by_timestamp( csPath, LP_DEVICE_TYPE_GPS, csGPSd, csGPSm, csStack[csLoopA].lsTime );

                        /* CSPS query - Orientations */
                        csQorienA = lp_query_orientation_by_timestamp( csPath, LP_DEVICE_TYPE_IMU, csIMUd, csIMUm, csStack[csLoopA].lsTime );

                        /* Send self comparison to output stream */
                        fprintf( csStream, "%lu ", csLoopA );

                        /* Second level composition loop */
                        for ( csLoopB = 0; csLoopB < csSize; csLoopB ++ ) {

                            /* Implicit composition check */
                            if ( csLoopA != csLoopB ) {

                                /* CSPS query - Positions */
                                csQpositB = lp_query_position_by_timestamp( csPath, LP_DEVICE_TYPE_GPS, csGPSd, csGPSm, csStack[csLoopB].lsTime );

                                /* CSPS query - Orientations */
                                csQorienB = lp_query_orientation_by_timestamp( csPath, LP_DEVICE_TYPE_IMU, csIMUd, csIMUm, csStack[csLoopB].lsTime );

                                /* Check query status */
                                if ( 

                                    ( csQpositA.qrStatus == LP_TRUE ) && 
                                    ( csQpositB.qrStatus == LP_TRUE ) && 
                                    ( csQorienA.qrStatus == LP_TRUE ) && 
                                    ( csQorienB.qrStatus == LP_TRUE ) 

                                ) {

                                    /* Frustums variables */
                                    cs_Frustum_t csFrustA, csFrustB;

                                    /* Compute corrected positions - Local flat earth model */
                                    csQpositB.qrLongitude = ( csQpositB.qrLongitude - csQpositA.qrLongitude ) * ( ( ( 6367514.5 + csQpositA.qrAltitude ) * LF_PI / 180.0 ) );
                                    csQpositB.qrLatitude  = ( csQpositB.qrLatitude  - csQpositA.qrLatitude  ) * ( ( ( 6367514.5 + csQpositA.qrAltitude ) * LF_PI / 180.0 ) );
                                    csQpositB.qrAltitude  = ( csQpositB.qrAltitude  - csQpositA.qrAltitude  );

                                    /* Compute frustum of first camera */
                                    cs_omvg_frustum_eyesis4pi( 

                                        csCamera, csStack[csLoopA].lsChannel,
                                        csQorienA.qrfxx, csQorienA.qrfxy, csQorienA.qrfxz,
                                        csQorienA.qrfyx, csQorienA.qrfyy, csQorienA.qrfyz,
                                        csQorienA.qrfzx, csQorienA.qrfzy, csQorienA.qrfzz,
                                        0.0, 0.0, 0.0,
                                        csNPlane, csFPlane, & csFrustA

                                    );

                                    /* Compute frustum of second camera */
                                    cs_omvg_frustum_eyesis4pi( 

                                        csCamera, csStack[csLoopB].lsChannel,
                                        csQorienB.qrfxx, csQorienB.qrfxy, csQorienB.qrfxz,
                                        csQorienB.qrfyx, csQorienB.qrfyy, csQorienB.qrfyz,
                                        csQorienB.qrfzx, csQorienB.qrfzy, csQorienB.qrfzz,
                                        csQpositB.qrLongitude, csQpositB.qrLatitude, csQpositB.qrAltitude,
                                        csNPlane, csFPlane, & csFrustB

                                    );

                                    /* Frustum intersection detection */
                                    if ( cs_omvg_frustum_intersection( & csFrustA, & csFrustB ) == CS_TRUE ) fprintf( csStream, "%lu ", csLoopB );

                                /* Display message */
                                } else { fprintf( CS_ERR, "Warning : unable to query position/orientation with couple (%lu,%lu)\n", csLoopA, csLoopB ); }

                            }

                        }

                        /* Send eol to output stream */
                        fprintf( csStream, "\n" );

                    }

                /* Display message */
                } else { fprintf( CS_ERR, "Error : unable to open output file\n" ); }

                /* Unallocate stack */
                cs_omvg_frusmtum_list( "", & csStack, 0 );

            /* Display message */
            } else { fprintf( CS_ERR, "Error : unable to read OpenMVG list\n" ); }
            
        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - OpenMVG list importation
*/

    unsigned long cs_omvg_frusmtum_list ( 

        char const          * const csList, 
        cs_List_t           **      csStack, 
        unsigned long const         csDelay 

    ) {

        /* Reading buffer variables */
        char csBuffer[1024] = { 0 };

        /* Stack size variables */
        unsigned long csVirt = 1024;
        unsigned long csSize = 0;

        /* Timestamp composition variables */
        lp_Time_t csSec = 0;
        lp_Time_t csMic = 0;

        /* List handle variables */
        FILE * csStream = NULL;

        /* Check handle creation */
        if ( ( csStream = fopen( csList, "r" ) ) != NULL ) {

            /* Allocate stack initial segment */
            ( * csStack ) = ( cs_List_t * ) malloc( csVirt * sizeof( cs_List_t ) );

            /* Read list by line */
            while ( fgets( csBuffer, sizeof( csBuffer ), csStream ) > 0 ) {

                /* Decompose image name */
                sscanf( csBuffer, "%" lp_Time_i "_%" lp_Time_i "-%lu", & csSec, & csMic, & ( ( ( * csStack ) + csSize )->lsChannel ) );

                /* Compose timestamp */
                ( ( * csStack ) + csSize )->lsTime = lp_timestamp_compose( csSec + csDelay, csMic );

                /* Stack memory management */
                if ( ( ++ csSize ) >= csVirt ) ( * csStack ) = ( cs_List_t * ) realloc( ( * csStack ), ( csVirt += 1024 ) * sizeof( cs_List_t ) );

            }

            /* Close list handle */
            fclose( csStream );

        } else {

            /* Unallocate stack memory */
            if ( ( * csStack ) != NULL ) free( * csStack );

        }

        /* Return stack size */
        return( csSize );

    }

/*
    Source - Eyesis4Pi frustum composer
*/

    void cs_omvg_frustum_eyesis4pi(

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

    int cs_omvg_frustum_intersection(

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

