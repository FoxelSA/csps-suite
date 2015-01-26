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
        char csSTLm[256] = { 0 };

        /* Query variables */
        lp_Trigger_t csTrigger;
        lp_Geopos_t  csGeopos;
        lp_Orient_t  csOrient;
        lp_Still_t   csStill;

        /* json-c variables */
        struct json_object * csJson = NULL;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--path"   ,"-p" ), argv, csPath, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--export" ,"-e" ), argv, csFile, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-tag","-c" ), argv, csCAMd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-mod","-m" ), argv, csCAMm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-tag","-g" ), argv, csGPSd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-mod","-n" ), argv, csGPSm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-tag","-i" ), argv, csIMUd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-mod","-s" ), argv, csIMUm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--stl-mod","-t" ), argv, csSTLm, LC_STRING );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Create queries descriptors */
            csTrigger = lp_query_trigger_create    ( csPath, csCAMd, csCAMm );
            csGeopos  = lp_query_position_create   ( csPath, csGPSd, csGPSm );
            csOrient  = lp_query_orientation_create( csPath, csIMUd, csIMUm );
            csStill   = lp_query_still_create      ( csPath, csIMUd, csSTLm );

            /* Check previous file existence */
            if ( lc_file_detect( csFile, LC_FILE ) == LC_TRUE ) {

                /* Create JSON handle */
                if ( ( csJson = json_object_from_file( csFile ) ) == NULL ) {

                    /* Display message */
                    fprintf( LC_ERR, "Error : Unable to import %s file content\n", basename( csFile ) );

                } else {

                    /* Display message */
                    fprintf( LC_OUT, "Updating %s JSON file ...\n", basename( csFile ) );

                    /* JSON exportation */
                    cs_export( & csTrigger, & csGeopos, & csOrient, & csStill, csFile, csJson );

                    /* Delete JSON handle */
                    json_object_put( csJson );

                }

            } else {

                /* Display message */
                fprintf( LC_OUT, "Creating %s JSON file ...\n", basename( csFile ) );

                /* JSON exportation */
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

    void cs_export( 

        lp_Trigger_t       * const csTrigger, 
        lp_Geopos_t        * const csGeopos, 
        lp_Orient_t        * const csOrient,
        lp_Still_t         * const csStill,
        char               * const csFile,
        struct json_object * const csJson

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

        /* JSON parser variables */
        struct json_object * csPose = NULL;
        struct json_object * csUnit = NULL;

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
            csPose = json_object_object_get( csJson, "pose" );

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

                /* Query still range detection */
                lp_query_still( csStill, csTrigger->qrSynch );

                /* Search for previous pose */
                csUnit = cs_export_json_pose( csPose, csTrigger->qrMaster );

                /* Export format */
                fprintf( csStream, "{\n" );

                /* Guessed measure detection */
                if ( csParse < csSize ) {

                    /* Guessed measure detection */
                    if ( csFlag == 0 ) {

                        /* Export to stream */
                        cs_export_field( "gps", "\"repeat\"", ",", csStream, NULL );

                    } else {

                        /* Weak measure detection */
                        if ( csGeopos->qrWeak == LP_TRUE ) {

                            /* Export to stream */
                            cs_export_field( "gps", "\"weak\"", ",", csStream, NULL );

                        } else {

                            /* Export to stream */
                            cs_export_field( "gps", "\"valid\"", ",", csStream, NULL );

                        }

                    }

                } else {

                    /* Export to stream */
                    cs_export_field( "gps", "null", ",", csStream, NULL );

                }

                /* Export to stream */
                cs_export_field( "still", ( csStill->qrStill == LP_TRUE ) ? "true" : "false", ",", csStream, NULL );
                cs_export_field( "status", "\"unknown\"", ",", csStream, csUnit );

                /* Export to stream */
                fprintf( csStream, "\"sec\":%" lp_Time_p ",\n", lp_timestamp_sec ( csTrigger->qrMaster ) );
                fprintf( csStream, "\"usc\":%" lp_Time_p ",\n", lp_timestamp_usec( csTrigger->qrMaster ) );

                /* Export to stream */
                fprintf( csStream, "\"lng\":%.12lf,\n", csGeopos->qrLongitude );
                fprintf( csStream, "\"lat\":%.12lf,\n", csGeopos->qrLatitude  );
                fprintf( csStream, "\"alt\":%.12lf,\n", csGeopos->qrAltitude  );

                /* Export to stream */
                cs_export_field( "rotation", "", "", csStream, NULL );

                /* Export format */
                fprintf( csStream, "[\n" );

                /* Export to stream */
                fprintf( csStream, "%.12lf,\n", csOrient->qrfxx );
                fprintf( csStream, "%.12lf,\n", csOrient->qrfxy );
                fprintf( csStream, "%.12lf,\n", csOrient->qrfxz );
                fprintf( csStream, "%.12lf,\n", csOrient->qrfyx );
                fprintf( csStream, "%.12lf,\n", csOrient->qrfyy );
                fprintf( csStream, "%.12lf,\n", csOrient->qrfyz );
                fprintf( csStream, "%.12lf,\n", csOrient->qrfzx );
                fprintf( csStream, "%.12lf,\n", csOrient->qrfzy );
                fprintf( csStream, "%.12lf \n", csOrient->qrfzz );

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

    struct json_object * cs_export_json_pose( 

        struct json_object * const csNode, 
        lp_Time_t            const csMaster 

    ) {

        /* Parsing variables */
        static size_t csIndex = 0;

        /* Array size variables */
        size_t csSize = 0;

        /* Returned variables */
        struct json_object * csPose = NULL;
        struct json_object * csTemp = NULL;
        struct json_object * cstSec = NULL;
        struct json_object * cstUsc = NULL;

        /* Check object consistency */
        if ( csNode != NULL ) {

            /* Retrieve array size */
            csSize = json_object_array_length( csNode );

            /* Parsing array */
            while ( ( csIndex < csSize ) && ( csPose == NULL ) ) {

                /* Create pose node */
                csTemp = json_object_array_get_idx( csNode, csIndex );

                /* Create timestamp node */
                cstSec = json_object_object_get( csTemp, "sec" );
                cstUsc = json_object_object_get( csTemp, "usc" );

                /* Detect master timestamp equality */
                if ( lp_timestamp_eq( csMaster, lp_timestamp_compose( json_object_get_int( cstSec ), json_object_get_int( cstUsc ) ) ) == LP_TRUE ) {

                    /* Assign found node */
                    csPose = csTemp;

                }

                /* Update parse index */
                csIndex ++;

            }

        }

        /* Return found allocation */
        return( csPose );
        
    }

    void cs_export_field( 

        char               const * const csKey,
        char               const * const csValue,
        char               const * const csComa,
        FILE                     * const csStream,
        struct json_object       * const csObject

    ) {

        /* Object variables */
        struct json_object * csSubObject = NULL;

        /* Search for already existing entry */
        if ( ( csSubObject = json_object_object_get( csObject, csKey ) ) == NULL ) {

            /* Export to stream */
            fprintf( csStream, "\"%s\":%s%s\n", csKey, csValue, csComa );

        } else {

            /* Retrieve object type */
            if ( json_object_get_type( csSubObject ) == json_type_null ) {

                /* Export to stream */
                fprintf( csStream, "\"%s\":null%s\n", csKey, csComa );

            } else
            if ( json_object_get_type( csSubObject ) == json_type_string ) {

                /* Export to stream */
                fprintf( csStream, "\"%s\":\"%s\"%s\n", csKey, json_object_get_string( csSubObject ), csComa );

            } else
            if ( json_object_get_type( csSubObject ) == json_type_boolean ) {

                /* Export to stream */
                fprintf( csStream, "\"%s\":%s%s\n", csKey, json_object_get_boolean( csSubObject ) ? "true": "false", csComa );

            }

        }

    }

