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
        lp_Trigger_t  csTrigger;
        lp_Position_t csPosition;
        lp_Orient_t   csOrient;
        lp_Still_t    csStill;

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

            /* Create query structure */
            csTrigger = lp_query_trigger_create( csPath, csCAMd, csCAMm );

            /* Verify query structures */
            if ( lp_query_trigger_state( & csTrigger ) == LP_FALSE ) {

                /* Display message */
                fprintf( LC_ERR, "Error : unable to create query structure on camera trigger\n" );

            } else {

                /* Create query structure */
                csPosition = lp_query_position_create( csPath, csGPSd, csGPSm );
                csOrient   = lp_query_orientation_create( csPath, csIMUd, csIMUm );
                csStill    = lp_query_still_create( csPath, csSTLd, csSTLm );

                /* Check previous file existence */
                if ( lc_file_detect( csFile, LC_FILE ) == LC_TRUE ) {

                    /* Create master object */
                    if ( ( csJson = json_object_from_file( csFile ) ) == NULL ) {

                        /* Display message */
                        fprintf( LC_ERR, "Error : unable to import %s file content\n", basename( csFile ) );

                    } else {

                        /* Display message */
                        fprintf( LC_OUT, "Updating %s JSON file ...\n", basename( csFile ) );

                        /* File exportation */
                        cs_export( & csTrigger, & csPosition, & csOrient, & csStill, csFile, csJson );

                        /* Delete master object */
                        json_object_put( csJson );

                    }

                } else {

                    /* Display message */
                    fprintf( LC_OUT, "Creating %s JSON file ...\n", basename( csFile ) );

                    /* File exportation */
                    cs_export( & csTrigger, & csPosition, & csOrient, & csStill, csFile, NULL );

                }

                /* Delete query structure */
                lp_query_still_delete      ( & csStill    );
                lp_query_orientation_delete( & csOrient   );
                lp_query_position_delete   ( & csPosition );
                lp_query_trigger_delete    ( & csTrigger  );

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Exportation function
 */

    void cs_export( 

        lp_Trigger_t  * const csTrigger, 
        lp_Position_t * const csPosition, 
        lp_Orient_t   * const csOrient,
        lp_Still_t    * const csStill,
        char          * const csFile,
        cs_Object_t   * const csJson

    ) {

        /* Camera stream size variables */
        lp_Size_t csSize  = 0;

        /* Parsing variables */
        lp_Size_t csParse = 0;

        /* Stream variables */
        FILE * csStream = NULL;

        /* Object variables */
        cs_Object_t * csArray = NULL;
        cs_Object_t * csPose  = NULL;

        /* Create stream */
        if ( ( csStream = fopen( csFile, "w" ) ) == NULL ) {

            /* Display message */
            fprintf( LC_ERR, "Error : unable to access %s\n", basename( csFile ) );

        } else {

            /* Trigger count query */
            csSize = lp_query_trigger_size( csTrigger );

            /* Create pose array object */
            csArray = json_object_object_get( csJson, "pose" );

            /* Export format */
            fprintf( csStream, "{\n" );

            /* Export to stream */
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
                lp_query_position( csPosition, csTrigger->qrSynch );

                /* Query orientation */
                lp_query_orientation( csOrient, csTrigger->qrSynch );

                /* Query still range */
                lp_query_still( csStill, csTrigger->qrSynch );

                /* Search for previous pose */
                csPose = cs_export_get_pose( csArray, csTrigger->qrMaster );

                /* Export format */
                fprintf( csStream, "{\n" );

                /* Export to stream */
                cs_export_field( "still", ( csStill->qrStill == LP_TRUE ) ? "true" : "false", ",", csStream, NULL );
                cs_export_field( "raw", "\"unknown\"", ",", csStream, csPose );

                /* Export to stream */
                fprintf( csStream, "\"sec\":%"  lp_Time_p ",\n", lp_timestamp_sec ( csTrigger->qrMaster ) );
                fprintf( csStream, "\"usec\":%" lp_Time_p ",\n", lp_timestamp_usec( csTrigger->qrMaster ) );

                /* Check position availability */
                if ( lp_query_position_status( csPosition ) == LP_FALSE ) {

                    /* Export to stream */
                    cs_export_field( "position", "null", ",", csStream, NULL );

                } else {

                    /* Exception management */
                    if ( 

                        ( isnormal( csPosition->qrAltitude  ) == 0 ) || 
                        ( isnormal( csPosition->qrLongitude ) == 0 ) || 
                        ( isnormal( csPosition->qrLatitude  ) == 0 ) 

                    ) {

                        /* Export to stream */
                        cs_export_field( "position", "null", ",", csStream, NULL );

                    } else {

                        /* Export to stream */
                        cs_export_field( "position", "", "", csStream, NULL );

                        /* Export format */
                        fprintf( csStream, "[\n" ); 

                        /* Export to stream */
                        fprintf( csStream, "%.16e,\n", csPosition->qrAltitude  );
                        fprintf( csStream, "%.16e,\n", csPosition->qrLongitude );
                        fprintf( csStream, "%.16e,\n", csPosition->qrLatitude  );
                        fprintf( csStream, "%.16e \n", csPosition->qrWeak      );

                        /* Export format */
                        fprintf( csStream, "],\n" );

                    }

                }

                /* Check orientation availability */
                if ( lp_query_orientation_status( csOrient ) == LP_FALSE ) {

                    /* Export to stream */
                    cs_export_field( "orientation", "null", "", csStream, NULL );

                } else {

                    /* Exception management */
                    if ( 

                         ( isnormal( csOrient->qrfxx ) == 0 ) ||
                         ( isnormal( csOrient->qrfxy ) == 0 ) ||
                         ( isnormal( csOrient->qrfxz ) == 0 ) ||
                         ( isnormal( csOrient->qrfyx ) == 0 ) ||
                         ( isnormal( csOrient->qrfyy ) == 0 ) ||
                         ( isnormal( csOrient->qrfyz ) == 0 ) ||
                         ( isnormal( csOrient->qrfzx ) == 0 ) ||
                         ( isnormal( csOrient->qrfzy ) == 0 ) ||
                         ( isnormal( csOrient->qrfzz ) == 0 ) 

                    ) {

                        /* Export to stream */
                        cs_export_field( "orientation", "null", "", csStream, NULL );

                    } else {

                        /* Export to stream */
                        cs_export_field( "orientation", "", "", csStream, NULL );

                        /* Export format */
                        fprintf( csStream, "[\n" );

                        /* Export to stream */
                        fprintf( csStream, "%.16e,\n", csOrient->qrfxx  );
                        fprintf( csStream, "%.16e,\n", csOrient->qrfxy  );
                        fprintf( csStream, "%.16e,\n", csOrient->qrfxz  );
                        fprintf( csStream, "%.16e,\n", csOrient->qrfyx  );
                        fprintf( csStream, "%.16e,\n", csOrient->qrfyy  );
                        fprintf( csStream, "%.16e,\n", csOrient->qrfyz  );
                        fprintf( csStream, "%.16e,\n", csOrient->qrfzx  );
                        fprintf( csStream, "%.16e,\n", csOrient->qrfzy  );
                        fprintf( csStream, "%.16e,\n", csOrient->qrfzz  );
                        fprintf( csStream, "%.16e \n", csOrient->qrWeak );

                        /* Export format */
                        fprintf( csStream, "]\n" );

                    }

                }

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
                    json_object_get_int( json_object_object_get( csPose, "usec" ) ) 

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

