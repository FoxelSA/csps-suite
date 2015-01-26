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

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--path"     ,"-p" ), argv, csPath, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--export"   ,"-e" ), argv, csFile, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-tag"  ,"-c" ), argv, csCAMd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-mod"  ,"-m" ), argv, csCAMm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-tag"  ,"-g" ), argv, csGPSd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-mod"  ,"-n" ), argv, csGPSm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-tag"  ,"-i" ), argv, csIMUd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-mod"  ,"-s" ), argv, csIMUm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--stl-mod"  ,"-t" ), argv, csSTLm, LC_STRING );

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

                /* File size variables */
                size_t csSize = 0;

                /* Reading buffer variables */
                char * csBuffer = NULL;

                /* File stream handle variables */
                FILE * csStream = NULL;

                /* Create input stream */
                if ( ( csStream = fopen( csFile, "rb" ) ) != NULL ) {

                    /* Offset to end of file */
                    fseek( csStream, 0L, SEEK_END );

                    /* Retrieve file length */
                    csSize = ftell( csStream );

                    /* Allocate buffer memory */
                    if ( ( csBuffer = ( char * ) malloc( sizeof( char ) * csSize ) ) != NULL ) {

                        /* Offset to begining of file */
                        fseek( csStream, 0L, SEEK_SET );

                        /* Read input stream content */
                        if ( fread( csBuffer, sizeof( char ), csSize, csStream ) != csSize ) {

                            /* Unallocate buffer memory */
                            free( csBuffer );

                            /* Invalidate pointer */
                            csBuffer = NULL;

                        }

                    }

                    /* Close input stream */
                    fclose( csStream );

                }

                /* Check previous file importation */
                if ( csBuffer == NULL ) {

                    /* Display message */
                    fprintf( LC_ERR, "Error : Unable to import %s file content\n", basename( csFile ) );

                } else {

                    /* Display message */
                    fprintf( LC_OUT, "Updating %s JSON file ...\n", basename( csFile ) );

                    /* JSON exportation */
                    cs_export( & csTrigger, & csGeopos, & csOrient, & csStill, csFile, csBuffer );

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

        lp_Trigger_t  * const csTrigger, 
        lp_Geopos_t   * const csGeopos, 
        lp_Orient_t   * const csOrient,
        lp_Still_t    * const csStill,
        char          * const csFile,
        char          * const csPrevious

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
        struct json_object * csJSON = NULL;
        struct json_object * csNode = NULL;
        struct json_object * csPose = NULL;
        struct json_object * csProp = NULL;

        /* Create stream */
        if ( ( csStream = fopen( csFile, "w" ) ) == NULL ) {

            /* Display message */
            fprintf( LC_ERR, "Error : unable to access %s\n", basename( csFile ) );

        } else {

            /* Check for previous content */
            if ( csPrevious != NULL ) {

                /* Create previous content handle */
                csJSON = json_tokener_parse( csPrevious );

            }

            /* Trigger count query */
            csSize = lp_query_trigger_size( csTrigger );

            /* Search for initial position (for signal missing on boundaries) */
            while ( ( csGuess < csSize ) && ( lp_query_position_status( csGeopos ) == LC_FALSE ) ) {

                /* Query trigger by index */
                lp_query_trigger_byindex( csTrigger, csGuess ++ );

                /* Query timestamp position */
                lp_query_position( csGeopos, csTrigger->qrSynch );

            }

            /* JSON - header - GPS */
            fprintf( csStream, "{\n\"gps\":%s,\n", ( csParse < csSize ) ? "true" : "false" );

            /* Check for previous content */
            if ( csPrevious != NULL ) {

                /* Create split node */
                csNode = json_object_object_get( csJSON, "split" );

                /* JSON - header - Split */
                fprintf( csStream, "\"split\":%s,\n", json_object_get_boolean( csNode ) ? "true": "false" );

                /* Create preview node */
                csNode = json_object_object_get( csJSON, "preview" );

                /* Detect null value */
                if ( json_object_get_type( csNode ) == json_type_null ) {

                    /* JSON - header - Preview */
                    fprintf( csStream, "\"preview\":null,\n" );

                } else {

                    /* JSON - header - Preview */
                    fprintf( csStream, "\"preview\":\"%s\",\n", json_object_get_string( csNode ) );

                }

            } else {

                /* JSON - header - Split */
                fprintf( csStream, "\"split\":false,\n" );

                /* JSON - header - Preview */
                fprintf( csStream, "\"preview\":null,\n" );

            }

            /* JSON - poses - array */
            fprintf( csStream, "\"pose\":[\n" );

            /* Check for previous content */
            if ( csPrevious != NULL ) {

                /* Create poses array node */
                csNode = json_object_object_get( csJSON, "pose" );

            }
            
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

                /* Check for previous content */
                if ( csNode != NULL ) {

                    /* Search for previous pose */
                    csPose = cs_export_json_pose( csNode, csTrigger->qrMaster );

                }

                /* JSON - poses - pose */
                fprintf( csStream, "{\n" );

                /* Guessed measure detection */
                if ( csParse < csSize ) {

                    /* Guessed measure detection */
                    if ( csFlag == 0 ) {

                        /* GPS mesure description */
                        fprintf( csStream, "\"gps\":\"repeat\",\n" );

                    } else {

                        /* Weak measure detection */
                        if ( csGeopos->qrWeak == LP_TRUE ) {

                            /* JSON - poses - GPS */
                            fprintf( csStream, "\"gps\":\"weak\",\n" );

                        } else {

                            /* JSON - poses - GPS */
                            fprintf( csStream, "\"gps\":\"valid\",\n" );

                        }

                    }

                } else {

                    /* JSON - poses - GPS */
                    fprintf( csStream, "\"gps\":null,\n" );

                }

                /* JSON - poses - still */
                fprintf( csStream, "\"still\":%s,\n", ( csStill->qrStill == LP_TRUE ) ? "true" : "false" );

                /* Check for previous content */
                if ( csPose != NULL ) {

                    /* Create status node */
                    csProp = json_object_object_get( csPose, "status" );

                    /* JSON - poses - status */
                    fprintf( csStream, "\"status\":\"%s\",\n", json_object_get_string( csProp ) );                    

                    /* Create folder node */
                    csProp = json_object_object_get( csPose, "folder" );

                    /* Detect null value */
                    if ( json_object_get_type( csProp ) == json_type_null ) {

                        /* JSON - poses - folder */
                        fprintf( csStream, "\"folder\":null,\n" );

                    } else {

                        /* JSON - poses - folder */
                        fprintf( csStream, "\"folder\":\"%s\",\n", json_object_get_string( csProp ) );

                    }                    

                } else {

                    /* JSON - poses - status */
                    fprintf( csStream, "\"status\":\"unknown\",\n" );

                    /* JSON - poses - folder */
                    fprintf( csStream, "\"folder\":null,\n" );

                }

                /* JSON - poses - timestamp */
                fprintf( csStream, "\"sec\":%" lp_Time_p ",\n", lp_timestamp_sec ( csTrigger->qrMaster ) );
                fprintf( csStream, "\"usc\":%" lp_Time_p ",\n", lp_timestamp_usec( csTrigger->qrMaster ) );

                /* JSON - poses - geoposition */
                fprintf( csStream, "\"lng\":%.10f,\n", csGeopos->qrLongitude );
                fprintf( csStream, "\"lat\":%.10f,\n", csGeopos->qrLatitude );
                fprintf( csStream, "\"alt\":%.10f,\n", csGeopos->qrAltitude );

                /* JSON - poses - orientation */
                fprintf( csStream, "\"rotation\":[\n" 

                    "%.10lf,\n%.10lf,\n%.10lf,\n"
                    "%.10lf,\n%.10lf,\n%.10lf,\n"
                    "%.10lf,\n%.10lf,\n%.10lf\n]\n",

                    csOrient->qrfxx, csOrient->qrfxy, csOrient->qrfxz,
                    csOrient->qrfyx, csOrient->qrfyy, csOrient->qrfyz,
                    csOrient->qrfzx, csOrient->qrfzy, csOrient->qrfzz

                );

                /* JSON - poses - pose */
                fprintf( csStream, "}%s\n", ( csParse < ( csSize - 1 ) ) ? "," : "" );

            }

            /* JSON - tail */
            fprintf( csStream, "]\n}\n" );

            /* Delete previous content handle */ 
            if ( csPrevious != NULL ) json_object_put( csJSON );

            /* Close output stream */
            fclose( csStream );

        }

    }

    struct json_object * cs_export_json_pose( 

        struct json_object * const csNode, 
        lp_Time_t            const csMaster 

    ) {

        /* Parsing variables */
        size_t csIndex = 0;

        /* Retrieve array size */
        size_t csSize = json_object_array_length( csNode );

        /* Returned variables */
        struct json_object * csPose = NULL;
        struct json_object * csTemp = NULL;
        struct json_object * cstSec = NULL;
        struct json_object * cstUsc = NULL;

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

        /* Return found allocation */
        return( csPose );
        
    }

