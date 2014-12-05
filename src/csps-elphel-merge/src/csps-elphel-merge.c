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

    # include "csps-elphel-merge.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Software path variables */
        char csSrc[256] = { 0 };
        char csDst[256] = { 0 };
        char csFil[256] = { 0 };
        char csMrg[256] = { 0 };

        /* Search in parameters */
        stdp( stda( argc, argv,  "--source"     , "-s" ), argv, csSrc, CS_STRING );
        stdp( stda( argc, argv,  "--destination", "-d" ), argv, csDst, CS_STRING );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Records buffer variables */
            lp_Byte_t csBuffer[CS_RECLEN] = { 0 };

            /* Logs-files stack variables */
            cs_Descriptor_t csStack[CS_SIZE];

            /* Stack size variables */
            unsigned int csIndex  = 0;

            /* Stack decimation variables */
            signed   int csAppend  = 0;

            /* Parsing variables */
            unsigned int csParse   = 0;

            /* Selection variables */
            unsigned int csSelect  = 0;

            /* Timestamp variables */
            lp_Time_t    csPush    = 0;
            lp_Time_t    csLKnown  = 0;

            /* Appending flag variables */
            unsigned int csFlag    = CS_FALSE;

            /* Stream variables */
            FILE * csIStream = NULL;
            FILE * csOStream = NULL;

            /* Directory entity enumeration */
            while ( cs_elphel_merge_enum( csSrc, csFil ) != CS_FALSE ) {

                /* Consider only file entity */
                if ( cs_elphel_merge_detect( csFil, CS_FILE ) == CS_TRUE ) {

                    /* Check log-file tag */
                    if ( strstr( csFil, CS_PATH_PATTERN ) != 0 ) {

                        /* Assign logs-file path */
                        strcpy( csStack[csIndex].dsName, csFil );

                        /* Initialize appending flag */
                        csStack[csIndex].dsFlag = CS_FALSE;

                        /* Extract logs-file timestamp extremums */
                        if ( ( csStack[csIndex].dsFirst = cs_elphel_merge_first( csFil ) ) != lp_Time_s( CS_INFT ) ) {

                            /* Display information */
                            fprintf( CS_OUT, "Stacking : %s\n", basename( csFil ) );

                            /* Update stack size */
                            csAppend = ( ++ csIndex ); 

                        }

                    }

                }

            }

            /* Compose output file name */
            sprintf( csMrg, "%s/log-merge.log-0", csDst );

            /* Create output stream handle */
            if ( ( csOStream = fopen( csMrg, "wb" ) ) != NULL ) {

                /* Appending loop */
                while ( csAppend > 0 ) {

                    /* Push infinit time */
                    csPush = lp_Time_s( CS_INFT );

                    /* Reset selection */
                    csSelect = csIndex;

                    /* Search oldest segment */
                    for ( csParse = 0; csParse < csIndex; csParse ++ ) {

                        /* Check stack element flag */
                        if ( csStack[csParse].dsFlag == CS_FALSE ) {

                            /* Compare timestamp */
                            if ( lp_timestamp_ge( csPush, csStack[csParse].dsFirst ) == CS_TRUE ) {

                                /* Update search timestamp */
                                csPush = csStack[csParse].dsFirst;

                                /* Update selection stack index */
                                csSelect = csParse;

                            }

                        }

                    }

                    /* Display append information */
                    fprintf( CS_OUT, "Appending : %s\n", basename( csStack[csSelect].dsName ) );

                    /* Update selection state */
                    csStack[csSelect].dsFlag = CS_TRUE;

                    /* Appending condition flag reset */
                    csFlag = CS_FALSE;

                    /* Create input stream */
                    if ( ( csIStream = fopen( csStack[csSelect].dsName, "rb" ) ) != NULL ) {

                        /* Sub-appending loop */
                        while ( fread( csBuffer, 1, CS_RECLEN, csIStream ) == CS_RECLEN ) {

                            /* Check record event type */
                            if ( CS_EVENT( csBuffer, CS_IMU ) ) {

                                /* Appending condition trigger */
                                if ( ( lp_timestamp_ge( csLKnown, lp_timestamp( ( lp_Void_t * ) csBuffer ) ) ) == CS_FALSE ) {

                                    /* Set appending condition flag */
                                    csFlag = CS_TRUE;

                                }

                            }

                            /* Append condition verification */
                            if ( csFlag == CS_TRUE ) {

                                /* Append records */
                                fwrite( csBuffer, 1, CS_RECLEN, csOStream );

                                /* Memorize last known timestamp */
                                csLKnown = lp_timestamp( ( lp_Void_t * ) csBuffer );

                            }

                        }

                        /* Close input stream */
                        fclose( csIStream );

                    } else { fprintf( CS_ERR, "Error : unable to access %s\n", basename( csStack[csSelect].dsName ) ); }

                    /* Update appending loop state */
                    csAppend --;

                }

                /* Close output stream */
                fclose( csOStream );

            /* Display message */
            } else { fprintf( CS_ERR, "Error : unable to access %s\n", basename( csMrg ) ); }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - First timestamp extraction
 */

    lp_Time_t cs_elphel_merge_first( char const * const csFil ) {

        /* Records buffer variables */
        unsigned char csRec[CS_RECLEN] = { 0 };

        /* Returned value variables */
        lp_Time_t csFirst = lp_Time_s( CS_INFT );

        /* Create input stream */
        FILE * csStream = fopen( csFil, "rb" );

        /* Check stream creation */
        if ( csStream != NULL ) {

            /* Parse input stream */
            while ( ( csFirst == lp_Time_s( CS_INFT ) ) && ( fread( csRec, 1, CS_RECLEN, csStream ) == CS_RECLEN ) ) {

                /* Event type detection and timestamp assignation */
                if ( CS_EVENT( csRec, CS_IMU ) ) csFirst = lp_timestamp( ( lp_Void_t * ) csRec );

            }

            /* Close input stream */
            fclose( csStream );

        /* Display message */
        } else { fprintf( CS_ERR, "Error : unable to access %s\n", basename( ( char * ) csFil ) ); }

        /* Return first timestamp */
        return( csFirst );

    }

/*
    Source - Directory entity enumeration
 */

    int cs_elphel_merge_enum( char const * const csDirectory, char * const csName ) {

        /* Directory variables */
        static DIR           * csDirect = NULL;
        static struct dirent * csEntity = NULL;

        /* Verify enumeration mode */
        if ( csDirect == NULL ) {

            /* Create directory handle */
            csDirect = opendir( csDirectory );

            /* Recusive initialization */
            return( cs_elphel_merge_enum( csDirectory, csName ) );

        } else {

            /* Read directory entity */
            csEntity = readdir( csDirect );

            /* Check enumeration end */
            if ( csEntity == NULL ) {

                /* Delete directory handle */
                closedir( csDirect );

                /* Reset directory pointer */
                csDirect = NULL;

                /* Return negative enumeration */
                return( CS_FALSE );

            } else {

                /* Compose directory and entity path */
                sprintf( csName, "%s/%s", csDirectory, csEntity->d_name );

                /* Return positive enumeration */
                return( CS_TRUE );

            }

        }

    }

/*
    Source - Directory entity type detection
*/

    int cs_elphel_merge_detect( char const * const csEntity, int const csType ) {

        /* Check type of entity to verify */
        if ( csType == CS_FILE ) {

            /* File openning verification */
            FILE * csCheck = fopen( csEntity, "r" );

            /* Check verification stream */
            if ( csCheck != NULL ) {

                /* Close stream */
                fclose( csCheck );

                /* Return positive answer */
                return( CS_TRUE );

            } else {

                /* Return negative answer */
                return( CS_FALSE );

            }

        } else if ( csType == CS_DIRECTORY ) {

            /* Directory handle verification */
            DIR * csCheck = opendir( csEntity );

            /* Check verification handle */
            if ( csCheck != NULL ) {

                /* Delete handle */
                closedir( csCheck );

                /* Return positive answer */
                return( CS_TRUE );

            } else {

                /* Return negative answer */
                return( CS_FALSE );
            }

        } else {

            /* Return negative answer */
            return( CS_FALSE );

        }

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

