/*
 * csps-suite - CSPS library front-end suite
 *
 * Copyright (c) 2013-2015 FOXEL SA - http://foxel.ch
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
        char csPath  [256] = { 0 };
        char csList  [256] = { 0 };
        char csPair  [256] = { 0 };
        char csMount [256] = "/data/";
        char csCamera[256] = { 0 };

        /* Stream pointer variables */
        char csCAMd[256] = { 0 };
        char csCAMm[256] = { 0 };
        char csGPSd[256] = { 0 };
        char csGPSm[256] = { 0 };
        char csIMUd[256] = { 0 };
        char csIMUm[256] = { 0 };

        /* Sensor size variables */
        double csNear =  1.0;
        double csFar  = 30.0;

        /* Pair composition variables */
        unsigned long csaLoop = 0;
        unsigned long csbLoop = 0;

        /* Size variables */
        unsigned long csSize  = 0;
        unsigned long csAccum = 0;

        /* Pair storage buffer */
        unsigned long * csBuffer = NULL;

        /* Stream variables */
        FILE * csStream = NULL;

        /* Stack variables */
        cs_List_t * csStack = NULL;

        /* CSPS query structures variables */
        lp_Geopos_t  csaGeopos;
        lp_Geopos_t  csbGeopos;
        lp_Orient_t  csaOrient;
        lp_Orient_t  csbOrient;
        lp_Trigger_t csTrigger;

        /* Frustums variables */
        cs_Frustum_t csaFrustum;
        cs_Frustum_t csbFrustum;

        /* Calibration data descriptor variables */
        lf_Descriptor_t lfDesc;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--path"       , "-p" ), argv,   csPath  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--list"       , "-l" ), argv,   csList  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--pairs"      , "-r" ), argv,   csPair  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--mount-point", "-t" ), argv,   csMount , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--camera"     , "-a" ), argv,   csCamera, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-tag"    , "-c" ), argv,   csCAMd  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-mod"    , "-m" ), argv,   csCAMm  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-tag"    , "-g" ), argv,   csGPSd  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-mod"    , "-n" ), argv,   csGPSm  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-tag"    , "-i" ), argv,   csIMUd  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-mod"    , "-s" ), argv,   csIMUm  , LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--near-plane" , "-e" ), argv, & csNear  , LC_DOUBLE );
        lc_stdp( lc_stda( argc, argv, "--far-plane"  , "-f" ), argv, & csFar   , LC_DOUBLE );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Create calibration data descriptor */
            if ( lf_parse( ( lf_Char_t * ) csCamera, ( lf_Char_t * ) csMount, & lfDesc ) == LF_FALSE ) {

                /* Display message */
                fprintf( LC_ERR, "Error : unable to create calibration data descriptor\n" );

            } else {

                /* Create queries descriptors */
                csTrigger = lp_query_trigger_create    ( csPath, csCAMd, csCAMm );
                csaGeopos = lp_query_position_create   ( csPath, csGPSd, csGPSm );
                csbGeopos = lp_query_position_create   ( csPath, csGPSd, csGPSm );
                csaOrient = lp_query_orientation_create( csPath, csIMUd, csIMUm );
                csbOrient = lp_query_orientation_create( csPath, csIMUd, csIMUm );

                /* Import OpenMVG list */
                if ( ( csSize = cs_frusmtum_list( csList, & csStack, & csTrigger ) ) == 0 ) {

                    /* Display message */
                    fprintf( LC_ERR, "Error : unable to read OpenMVG list\n" );

                } else {

                    /* Allocate pair buffer memory */
                    if ( ( csBuffer = ( unsigned long * ) malloc( csSize * sizeof( unsigned long ) ) ) == NULL ) {

                        /* Display message */
                        fprintf( LC_ERR, "Error : unable to allocate pair buffer memory\n" );

                    } else {

                        /* Create output stream handle */
                        if ( ( csStream = fopen( csPair, "w" ) ) == NULL ) {

                            /* Display message */
                            fprintf( LC_ERR, "Error : unable to open output file\n" );

                        } else {

                            /* First level composition loop */
                            for ( csaLoop = 0; csaLoop < csSize; csaLoop ++ ) {

                                /* CSPS query - Positions & orientation */
                                lp_query_position   ( & csaGeopos, csStack[csaLoop].lsTime );
                                lp_query_orientation( & csaOrient, csStack[csaLoop].lsTime );

                                /* Check query status */
                                if ( ( csaGeopos.qrStatus == LP_TRUE ) && ( csaOrient.qrStatus == LP_TRUE ) ) {

                                    /* Second level composition loop */
                                    for ( csbLoop = csaLoop + 1; csbLoop < csSize; csbLoop ++ ) {

                                        /* CSPS query - Positions and orientation */
                                        lp_query_position   ( & csbGeopos, csStack[csbLoop].lsTime );
                                        lp_query_orientation( & csbOrient, csStack[csbLoop].lsTime );

                                        /* Check query status */
                                        if ( ( csbGeopos.qrStatus == LP_TRUE ) && ( csbOrient.qrStatus == LP_TRUE ) ) {

                                            /* Compute corrected positions - Local flat earth model */
                                            csbGeopos.qrLongitude = ( csbGeopos.qrLongitude - csaGeopos.qrLongitude ) * ( ( ( 6367514.5 + csaGeopos.qrAltitude ) * LF_PI / 180.0 ) );
                                            csbGeopos.qrLatitude  = ( csbGeopos.qrLatitude  - csaGeopos.qrLatitude  ) * ( ( ( 6367514.5 + csaGeopos.qrAltitude ) * LF_PI / 180.0 ) );
                                            csbGeopos.qrAltitude  = ( csbGeopos.qrAltitude  - csaGeopos.qrAltitude  );

                                            /* Compute frustum of first camera */
                                            cs_frustum_eyesis4pi( csCamera, csStack[csaLoop].lsChannel,

                                                csaOrient.qrfxx, 
                                                csaOrient.qrfxy,
                                                csaOrient.qrfxz,
                                                csaOrient.qrfyx, 
                                                csaOrient.qrfyy,
                                                csaOrient.qrfyz,
                                                csaOrient.qrfzx,
                                                csaOrient.qrfzy,
                                                csaOrient.qrfzz,
                                                0.0, 
                                                0.0, 
                                                0.0,
                                                csNear, 
                                                csFar, 
                                                
                                            & csaFrustum, & lfDesc );

                                            /* Compute frustum of second camera */
                                            cs_frustum_eyesis4pi( csCamera, csStack[csbLoop].lsChannel,

                                                csbOrient.qrfxx, 
                                                csbOrient.qrfxy, 
                                                csbOrient.qrfxz,
                                                csbOrient.qrfyx, 
                                                csbOrient.qrfyy, 
                                                csbOrient.qrfyz,
                                                csbOrient.qrfzx, 
                                                csbOrient.qrfzy, 
                                                csbOrient.qrfzz,
                                                csbGeopos.qrLongitude, 
                                                csbGeopos.qrLatitude, 
                                                csbGeopos.qrAltitude,
                                                csNear, 
                                                csFar, 

                                            & csbFrustum, & lfDesc );

                                            /* Frustum intersection detection */
                                            if ( cs_frustum_intersection( & csaFrustum, & csbFrustum ) == LC_TRUE ) csBuffer[csAccum++] = csbLoop;

                                        /* Display message */
                                        } else { fprintf( LC_ERR, "Warning : unable to query position/orientation with image %lu\n", csbLoop ); }

                                    }

                                    /* Check pair buffer state */
                                    if ( csAccum != 0 ) {

                                        /* Export reference image index */
                                        fprintf( csStream, "%lu ", csaLoop );

                                        /* Export pair buffer content */
                                        for ( csbLoop = 0; csbLoop < csAccum; csbLoop ++ ) fprintf( csStream, "%lu ", csBuffer[csbLoop] );

                                        /* Send eol to output stream */
                                        fprintf( csStream, "\n" );

                                        /* Reset pair buffer */
                                        csAccum = 0;

                                    }

                                /* Display message */
                                } else { fprintf( LC_ERR, "Warning : unable to query position/orientation with image %lu\n", csaLoop ); }

                            }

                            /* Close output stream */
                            fclose( csStream );

                        }

                        /* Unallocate pair buffer memory */
                        free( csBuffer );

                    }

                    /* Unallocate stack */
                    cs_frusmtum_list( "", & csStack, NULL );

                }

                /* Delete queries descriptor */
                lp_query_trigger_delete    ( & csTrigger );
                lp_query_position_delete   ( & csaGeopos );
                lp_query_position_delete   ( & csbGeopos );
                lp_query_orientation_delete( & csaOrient );
                lp_query_orientation_delete( & csbOrient );

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - OpenMVG list importation
*/

    unsigned long cs_frusmtum_list ( 

        char         const *  const csList, 
        cs_List_t          **       csStack, 
        lp_Trigger_t       *  const csTrigger

    ) {

        /* Reading buffer variables */
        char csBuffer[1024] = { 0 };

        /* Stack size variables */
        unsigned long csVirt = 1024;
        unsigned long csSize = 0;

        /* Timestamp composition variables */
        lp_Time_t csSec = 0;
        lp_Time_t csUse = 0;
        /* List handle variables */
        FILE * csStream = NULL;

        /* Check handle creation */
        if ( ( csStream = fopen( csList, "r" ) ) != NULL ) {

            /* Allocate stack initial segment */
            ( * csStack ) = ( cs_List_t * ) malloc( csVirt * sizeof( cs_List_t ) );

            /* Read list by line */
            while ( fgets( csBuffer, sizeof( csBuffer ), csStream ) > 0 ) {

                /* Decompose image name */
                sscanf( csBuffer, "%" lp_Time_i "_%" lp_Time_i "-%lu", & csSec, & csUse, & ( ( ( * csStack ) + csSize )->lsChannel ) );

                /* Query synchronization timestamp */
                lp_query_trigger_bymaster( csTrigger, lp_timestamp_compose( csSec, csUse ) );

                /* Check query status */
                if ( lp_query_trigger_status( csTrigger ) == LP_FALSE ) {

                    /* Display message */
                    fprintf( LC_ERR, "Warning : unable to query synchronization timestamp for image %lu\n", csSize );

                    /* Compose timestamp */
                    ( ( * csStack ) + csSize )->lsTime = lp_timestamp_compose( csSec, csUse );

                } else {

                    /* Retrieve timestamp */
                    ( ( * csStack ) + csSize )->lsTime = csTrigger->qrSynch;

                }

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

    void cs_frustum_eyesis4pi(

        char            const * const csCamera, 
        int             const         csChannel, 
        double          const         csEFxx,
        double          const         csEFxy,
        double          const         csEFxz,
        double          const         csEFyx,
        double          const         csEFyy,
        double          const         csEFyz,
        double          const         csEFzx,
        double          const         csEFzy,
        double          const         csEFzz,
        double          const         csEFpx,
        double          const         csEFpy,
        double          const         csEFpz,
        double          const         csNear,
        double          const         csFar,
        cs_Frustum_t          * const csFrustum,
        lf_Descriptor_t const * const csDesc

    ) {

        /* Eyesis4pi frame variables */
        double csEyesisframe[3] = { 0.0, 0.0, 0.0 };

        /* Eyesis4pi device calibration variables */
        double csAzimuth   = - lf_query_azimuth              ( csChannel, csDesc );
        double csHeading   = - lf_query_heading              ( csChannel, csDesc );
        double csElevation = - lf_query_elevation            ( csChannel, csDesc );
        double csRoll      = - lf_query_roll                 ( csChannel, csDesc );
        double csRadius    = + lf_query_radius               ( csChannel, csDesc );
        double csHeight    = + lf_query_height               ( csChannel, csDesc );
        double csPupil     = - lf_query_entrancePupilForward ( csChannel, csDesc );        
        double csPixel     = + lf_query_pixelSize            ( csChannel, csDesc );
        double csFocal     = + lf_query_focalLength          ( csChannel, csDesc );
        double csSenWidth  = + lf_query_pixelCorrectionWidth ( csChannel, csDesc );
        double csSenHeight = + lf_query_pixelCorrectionHeight( csChannel, csDesc );

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

        cs_Frustum_t const * const csaFrustum,
        cs_Frustum_t const * const csbFrustum

    ) {

        /* Frustum segment variables */
        int csEdge[12][2] = { 

            { 1, 0 }, { 2, 1 }, { 3, 2 }, { 0, 3 },
            { 5, 4 }, { 6, 5 }, { 7, 6 }, { 4, 7 },
            { 4, 0 }, { 5, 1 }, { 6, 2 }, { 7, 3 } 

        };

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
        int csResult = LC_FALSE;

        /* Progressive edge inclusion detection */
        while ( ( csPow < 64 ) && ( csResult == LC_FALSE ) ) {

            /* Secondary loop on polyhedron edges */
            for ( csParse = 0; csParse < 12; csParse ++ ) {

                /* Compute edge point in first frustum origin centered frame */
                csInterX = csPtn * ( csbFrustum->fsSX[csEdge[csParse][0]] - csbFrustum->fsSX[csEdge[csParse][1]] ) + csbFrustum->fsSX[csEdge[csParse][1]] - csaFrustum->fsOrg[0];
                csInterY = csPtn * ( csbFrustum->fsSY[csEdge[csParse][0]] - csbFrustum->fsSY[csEdge[csParse][1]] ) + csbFrustum->fsSY[csEdge[csParse][1]] - csaFrustum->fsOrg[1];
                csInterZ = csPtn * ( csbFrustum->fsSZ[csEdge[csParse][0]] - csbFrustum->fsSZ[csEdge[csParse][1]] ) + csbFrustum->fsSZ[csEdge[csParse][1]] - csaFrustum->fsOrg[2];

                /* Compute edge point coordinate in frist frustum nadir/left/top basis */
                csCompX = csaFrustum->fsNad[0] * csInterX + csaFrustum->fsNad[1] * csInterY + csaFrustum->fsNad[2] * csInterZ;
                csCompY = csaFrustum->fsLef[0] * csInterX + csaFrustum->fsLef[1] * csInterY + csaFrustum->fsLef[2] * csInterZ;
                csCompZ = csaFrustum->fsTop[0] * csInterX + csaFrustum->fsTop[1] * csInterY + csaFrustum->fsTop[2] * csInterZ;

                /* Verify intersection condition - principal direction */
                if ( ( csCompX >= csaFrustum->fsNear ) && ( csCompX <= csaFrustum->fsFar ) ) {

                    /* Verify intersection condition - secondary plane */
                    if ( ( fabs( csCompY ) <= ( csCompX * csaFrustum->fsLefApp ) ) && ( fabs( csCompZ ) <= ( csCompX * csaFrustum->fsTopApp ) ) ) {

                        /* Trigger intersection condition */
                        csResult = LC_TRUE;

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

