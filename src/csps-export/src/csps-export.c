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
        char csIMUd[256] = { 0 };
        char csIMUm[256] = { 0 };

        /* Query variables */
        lp_Trigger_t csTrigger;
        lp_Geopos_t  csGeopos;
        lp_Orient_t  csOrient;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--path"     ,"-p" ), argv, csPath, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--export"   ,"-e" ), argv, csFile, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-tag"  ,"-c" ), argv, csCAMd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-mod"  ,"-m" ), argv, csCAMm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-tag"  ,"-g" ), argv, csGPSd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-mod"  ,"-n" ), argv, csGPSm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-tag"  ,"-i" ), argv, csIMUd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-mod"  ,"-s" ), argv, csIMUm, LC_STRING );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Create queries descriptors */
            csTrigger = lp_query_trigger_create    ( csPath, csCAMd, csCAMm );
            csGeopos  = lp_query_position_create   ( csPath, csGPSd, csGPSm );
            csOrient  = lp_query_orientation_create( csPath, csIMUd, csIMUm );

            /* Check previous file existence */
            if ( lc_file_detect( csFile, LC_FILE ) == LC_TRUE ) {

                /* Display message */
                fprintf( LC_OUT, "Updating %s JSON file ...\n", basename( csFile ) );

                /* JSON update */
                cs_export_update( & csTrigger, & csGeopos, & csOrient, csFile );

            } else {

                /* Display message */
                fprintf( LC_OUT, "Creating %s JSON file ...\n", basename( csFile ) );

                /* JSON exportation */
                cs_export_create( & csTrigger, & csGeopos, & csOrient, csFile );

            }

            /* Delete queries descriptors */
            lp_query_trigger_delete    ( & csTrigger );
            lp_query_position_delete   ( & csGeopos  );
            lp_query_orientation_delete( & csOrient  );

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

    void cs_export_create( 

        lp_Trigger_t  * const csTrigger, 
        lp_Geopos_t   * const csGeopos, 
        lp_Orient_t   * const csOrient,
        char          * const csFile

    ) {

        /* Camera stream size variables */
        lp_Size_t csSize  = 0;

        /* Parsing variables */
        lp_Size_t csParse = 0;

        /* Index memory variables */
        lp_Size_t csGuess = 0;

        /* Position guess variables */
        lp_Size_t csFlag  = 0;

        /* Stream variables */
        FILE * csStream = NULL;

        /* Create stream */
        if ( ( csStream = fopen( csFile, "w" ) ) == NULL ) {

            /* Display message */
            fprintf( LC_ERR, "Error : unable to access %s\n", basename( csFile ) );

        } else {

            /* Trigger count query */
            csSize = lp_query_trigger_size( csTrigger );

            /* Search for initial position (for signal missing on boundaries) */
            while ( ( csGuess < csSize ) && ( lp_query_position_status( csGeopos ) == LC_FALSE ) ) {

                /* Query trigger by index */
                lp_query_trigger_byindex( csTrigger, csGuess ++ );

                /* Query timestamp position */
                lp_query_position( csGeopos, csTrigger->qrSynch );

            }

            /* Export JSON - header */
            fprintf( csStream, "{\n\"gps\":%s,\n\"split\":false,\n\"preview\":null,\n\"pose\":[\n", ( csParse < csSize ) ? "true" : "false" );
            
            /* Exportation loop */
            for ( csParse = 0; csParse < csSize; csParse ++ ) {

                /* Query trigger by index */
                lp_query_trigger_byindex( csTrigger, csParse );

                /* Verify missing position condition */
                if ( csParse >= csGuess ) {

                    /* Query position */
                    lp_query_position( csGeopos, csTrigger->qrSynch );

                    /* Update guess flag */
                    csFlag = ( lp_query_position_status( csGeopos ) == LC_TRUE ) ? 1 : 0;

                } else {

                    /* Update guess flag */
                    csFlag = 0;

                }

                /* Query orientation */
                lp_query_orientation( csOrient, csTrigger->qrSynch );

                /* Export JSON - format */
                fprintf( csStream, "{\n" );

                /* Export JSON - GPS mesure */
                if ( csParse < csSize ) {

                    /* Guessed measure detection */
                    if ( csFlag == 0 ) {

                        /* GPS mesure description */
                        fprintf( csStream, "\"gps\":\"repeat\",\n" );

                    } else {

                        /* GPS mesure description */
                        fprintf( csStream, "\"gps\":\"valid\",\n" );

                    }

                } else {

                    /* GPS mesure description */
                    fprintf( csStream, "\"gps\":null,\n" );

                }

                /* Export JSON - capture description */
                fprintf( csStream, "\"still\":\"false\",\n" );
                fprintf( csStream, "\"status\":\"unknown\",\n" );
                fprintf( csStream, "\"folder\":null,\n" );

                /* Export JSON - timestamps */
                fprintf( csStream, "\"sec\":%" lp_Time_p ",\n", lp_timestamp_sec ( csTrigger->qrMaster ) );
                fprintf( csStream, "\"usc\":%" lp_Time_p ",\n", lp_timestamp_usec( csTrigger->qrMaster ) );

                /* Export JSON - positions */
                fprintf( csStream, "\"lng\":%.10f,\n", csGeopos->qrLongitude );
                fprintf( csStream, "\"lat\":%.10f,\n", csGeopos->qrLatitude );
                fprintf( csStream, "\"alt\":%.10f,\n", csGeopos->qrAltitude );

                /* Export JSON - orientations */
                fprintf( csStream, "\"rotation\":{\n" 

                    "\"00\":%+.10f,\n\"10\":%+.10f,\n\"20\":%+.10f,\n"
                    "\"01\":%+.10f,\n\"11\":%+.10f,\n\"21\":%+.10f,\n"
                    "\"02\":%+.10f,\n\"12\":%+.10f,\n\"22\":%+.10f \n}\n",

                    csOrient->qrfxx, csOrient->qrfxy, csOrient->qrfxz,
                    csOrient->qrfyx, csOrient->qrfyy, csOrient->qrfyz,
                    csOrient->qrfzx, csOrient->qrfzy, csOrient->qrfzz

                );

                /* Export JSON - format */
                fprintf( csStream, "}%s\n", ( csParse < ( csSize - 1 ) ) ? "," : "" );

            }

            /* Export JSON - tail */
            fprintf( csStream, "]\n}\n" );

            /* Close output stream */
            fclose( csStream );

        }

    }

    void cs_export_update(

        lp_Trigger_t  * const csTrigger, 
        lp_Geopos_t   * const csGeopos, 
        lp_Orient_t   * const csOrient,
        char          * const csFile

    ) {

    }

