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
        char csSTLd[256] = { 0 };
        char csSTLm[256] = { 0 };

        /* Query variables */
        lp_Trigger_t csTrigger;
        lp_Geopos_t  csGeopos;
        lp_Orient_t  csOrient;
        lp_Still_t   csStill;

        /* Object variables */
        cs_Object_t * csJson = NULL;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--path"     ,"-p" ), argv, csPath, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--export"   ,"-e" ), argv, csFile, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-tag"  ,"-c" ), argv, csCAMd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-mod"  ,"-m" ), argv, csCAMm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-tag"  ,"-g" ), argv, csGPSd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-mod"  ,"-n" ), argv, csGPSm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-tag"  ,"-i" ), argv, csIMUd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-mod"  ,"-s" ), argv, csIMUm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--still-tag","-t" ), argv, csSTLd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--still-mod","-k" ), argv, csSTLm, LC_STRING );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Create queries descriptors */
            csTrigger = lp_query_trigger_create    ( csPath, csCAMd, csCAMm );
            csGeopos  = lp_query_position_create   ( csPath, csGPSd, csGPSm );
            csOrient  = lp_query_orientation_create( csPath, csIMUd, csIMUm );
            csStill   = lp_query_still_create      ( csPath, csSTLd, csSTLm );

            /* Check previous file existence */
            if ( lc_file_detect( csFile, LC_FILE ) == LC_TRUE ) {

                /* Create master object */
                if ( ( csJson = json_object_from_file( csFile ) ) == NULL ) {

                    /* Display message */
                    fprintf( LC_ERR, "Error : Unable to import %s file content\n", basename( csFile ) );

                } else {

                    /* Display message */
                    fprintf( LC_OUT, "Updating %s JSON file ...\n", basename( csFile ) );

                    /* File exportation */
                    cs_export( & csTrigger, & csGeopos, & csOrient, & csStill, csFile, csJson );

                    /* Delete master object */
                    json_object_put( csJson );

                }

            } else {

                /* Display message */
                fprintf( LC_OUT, "Creating %s JSON file ...\n", basename( csFile ) );

                /* File exportation */
                cs_export( & csTrigger, & csGeopos, & csOrient, & csStill, csFile, NULL );

            }

            /* Delete queries descriptors */
            lp_query_trigger_delete    ( & csTrigger );
            lp_query_position_delete   ( & csGeopos  );
            lp_query_orientation_delete( & csOrient  );
            lp_query_still_delete      ( & csStill   );

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Exportation function
 */

    void cs_export( 

        lp_Trigger_t * const csTrigger, 
        lp_Geopos_t  * const csGeopos, 
        lp_Orient_t  * const csOrient,
        lp_Still_t   * const csStill,
        char         * const csFile,
        cs_Object_t  * const csJson

    ) {

        /* Camera stream size variables */
        lp_Size_t csSize  = 0;

        /* Parsing variables */
        lp_Size_t csParse = 0;

        /* Index memory variables */
        lp_Size_t csGuess = 0;

        /* Object variables */
        cs_Object_t * csArray = NULL;
        cs_Object_t * csPose  = NULL;

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

            /* Create pose array object */
            csArray = json_object_object_get( csJson, "pose" );

            /* Export format */
            fprintf( csStream, "{\n" );

            /* Export to stream */
            cs_export_field( "gps", ( csParse < csSize ) ? "true" : "false", ",", csStream, NULL );
            cs_export_field( "split", "false", ",", csStream, csJson );
            cs_export_field( "preview", "null", ",", csStream, csJson );
            cs_export_field( "pose", "", "", csStream, NULL );

            /* Export format */
            fprintf( csStream, "[\n" );
            
            /* Exportation loop */
            for ( csParse = 0; csParse < csSize; csParse ++ ) {

                /* Query trigger by index */
                lp_query_trigger_byindex( csTrigger, csParse );

                /* Query position */
                lp_query_position( csGeopos, csTrigger->qrSynch );

                /* Query orientation */
                lp_query_orientation( csOrient, csTrigger->qrSynch );

                /* Query still range */
                lp_query_still( csStill, csTrigger->qrSynch );

                /* Search for previous pose */
                csPose = cs_export_get_pose( csArray, csTrigger->qrMaster );

                /* Export format */
                fprintf( csStream, "{\n" );

                /* Missing measure detection */
                if ( csGuess >= csSize ) {

                    /* Export to stream */
                    cs_export_field( "gps", "null", ",", csStream, NULL );

                } else {

                    /* Query status verification */
                    if ( lp_query_position_status( csGeopos ) == LC_TRUE ) {

                        /* Export to stream */
                        cs_export_field( "gps", ( csGeopos->qrWeak == LP_TRUE ) ? "\"weak\"" : "\"valid\"", ",", csStream, NULL );

                    } else {

                        /* Export to stream */
                        cs_export_field( "gps", "\"repeat\"", ",", csStream, NULL );

                    }

                }

                /* Export to stream */
                cs_export_field( "still", ( csStill->qrStill == LP_TRUE ) ? "true" : "false", ",", csStream, NULL );
                cs_export_field( "status", "\"unknown\"", ",", csStream, csPose );

                /* Export to stream */
                fprintf( csStream, "\"sec\":%" lp_Time_p ",\n", lp_timestamp_sec ( csTrigger->qrMaster ) );
                fprintf( csStream, "\"usc\":%" lp_Time_p ",\n", lp_timestamp_usec( csTrigger->qrMaster ) );

                /* Export to stream */
                fprintf( csStream, "\"lng\":%.12" lp_Real_p ",\n", csGeopos->qrLongitude );
                fprintf( csStream, "\"lat\":%.12" lp_Real_p ",\n", csGeopos->qrLatitude  );
                fprintf( csStream, "\"alt\":%.12" lp_Real_p ",\n", csGeopos->qrAltitude  );

                /* Export to stream */
                cs_export_field( "rotation", "", "", csStream, NULL );

                /* Export format */
                fprintf( csStream, "[\n" );

                /* Export to stream */
                fprintf( csStream, "%.12" lp_Real_p ",\n", csOrient->qrfxx );
                fprintf( csStream, "%.12" lp_Real_p ",\n", csOrient->qrfxy );
                fprintf( csStream, "%.12" lp_Real_p ",\n", csOrient->qrfxz );
                fprintf( csStream, "%.12" lp_Real_p ",\n", csOrient->qrfyx );
                fprintf( csStream, "%.12" lp_Real_p ",\n", csOrient->qrfyy );
                fprintf( csStream, "%.12" lp_Real_p ",\n", csOrient->qrfyz );
                fprintf( csStream, "%.12" lp_Real_p ",\n", csOrient->qrfzx );
                fprintf( csStream, "%.12" lp_Real_p ",\n", csOrient->qrfzy );
                fprintf( csStream, "%.12" lp_Real_p " \n", csOrient->qrfzz );

                /* Export format */
                fprintf( csStream, "]\n" );

                /* Export format */
                fprintf( csStream, "}%s\n", ( csParse < ( csSize - 1 ) ) ? "," : "" );

            }

            /* Export format */
            fprintf( csStream, "]\n}\n" );

            /* Close output stream */
            fclose( csStream );

        }

    }

/*
    Source - JSON array search
 */

    cs_Object_t * cs_export_get_pose( 

        cs_Object_t * const csArray, 
        lp_Time_t     const csMaster 

    ) {

        /* Parsing variables */
        static size_t csIndex = 0;

        /* Size variables */
        size_t csSize = 0;

        /* Timestamp variables */
        lp_Time_t csTime = 0;

        /* Object variables */
        cs_Object_t * csPose = NULL;

        /* Check object consistency */
        if ( csArray != NULL ) {

            /* Retrieve array size */
            csSize = json_object_array_length( csArray );

            /* Parsing array */
            while ( ( csIndex < csSize ) && ( csPose == NULL ) ) {

                /* Create pose node */
                csPose = json_object_array_get_idx( csArray, csIndex );

                /* Compose pose timestamp */
                csTime = lp_timestamp_compose( 

                    json_object_get_int( json_object_object_get( csPose, "sec" ) ), 
                    json_object_get_int( json_object_object_get( csPose, "usc" ) ) 

                );

                /* Detect master timestamp equality */
                if ( lp_timestamp_eq( csMaster, csTime ) == LP_FALSE ) csPose = NULL;

                /* Update parse index */
                csIndex ++;

            }

        }

        /* Return found allocation */
        return( csPose );
        
    }

/*
    Source - Field exportation
 */

    void cs_export_field( 

        char        const * const csKey,
        char        const * const csValue,
        char        const * const csComma,
        FILE              * const csStream,
        cs_Object_t       * const csObject

    ) {

        /* Object variables */
        cs_Object_t * csSubObject = NULL;

        /* Search for already existing entry */
        if ( ( csSubObject = json_object_object_get( csObject, csKey ) ) == NULL ) {

            /* Export to stream */
            fprintf( csStream, "\"%s\":%s%s\n", csKey, csValue, csComma );

        } else {

            /* Retrieve object type */
            if ( json_object_get_type( csSubObject ) == json_type_null ) {

                /* Export to stream */
                fprintf( csStream, "\"%s\":null%s\n", csKey, csComma );

            } else
            if ( json_object_get_type( csSubObject ) == json_type_string ) {

                /* Export to stream */
                fprintf( csStream, "\"%s\":\"%s\"%s\n", csKey, json_object_get_string( csSubObject ), csComma );

            } else
            if ( json_object_get_type( csSubObject ) == json_type_boolean ) {

                /* Export to stream */
                fprintf( csStream, "\"%s\":%s%s\n", csKey, json_object_get_boolean( csSubObject ) ? "true": "false", csComma );

            }

        }

    }

