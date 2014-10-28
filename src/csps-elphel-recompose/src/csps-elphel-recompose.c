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

    # include "csps-elphel-recompose.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Software path variables */
        char csDeco[256] = { 0 };
        char csReco[256] = { 0 };
        char csFile[256] = { 0 };
        char csExpo[256] = { 0 };

        /* Composition gap variables */
        double csInterval = 1.0;

        /* Search in parameters */
        stdp( stda( argc, argv,  "--decomposed", "-d" ), argv,   csDeco, CS_STRING );
        stdp( stda( argc, argv,  "--recomposed", "-r" ), argv,   csReco, CS_STRING );
        stdp( stda( argc, argv,  "--interval"  , "-g" ), argv, & csInterval , CS_DOUBLE );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Logs-files stack variables */
            cs_Descriptor_t csStack[CS_SIZE];

            /* Stack size variables */
            unsigned int csIndex  = 0;

            /* Stack decimation variables */
            signed   int csDecim  = 0;

            /* Stack parsing variables */
            unsigned int csParse  = 0;

            /* Stack memory variables */
            unsigned int csSelect = 0;

            unsigned int csIncrem = 0;

            /* Decimation reference timestamp */
            lp_Time_t    csEpoch  = lp_Time_s( 0xFFFFFFFFFFFFFFFF );

            /* Directory entity enumeration */
            while ( cs_elphel_recompose_enum( csDeco, csFile ) != CS_FALSE ) {

                /* Consider only file entity */
                if ( cs_elphel_recompose_detect( csFile, CS_FILE ) == CS_TRUE ) {

                    /* Check log-file tag */
                    if ( strstr( csFile, CS_PATH_PATTERN ) != 0 ) {

                        /* Assign logs-file path */
                        strcpy( csStack[csIndex].dsName, csFile );

                        /* Initialize appending flag */
                        csStack[csIndex].dsFlag = CS_FALSE;

                        /* Extract logs-file timestamp extremums */
                        cs_elphel_recompose_extremum( csFile, & ( csStack[csIndex].dsFirst ), & ( csStack[csIndex].dsLast ) );

                        /* Display information */
                        fprintf( CS_OUT, "Stacking : %s\n", basename( csFile ) );

                        /* Update stack size */
                        csDecim = ( ++ csIndex ); 

                    }

                }

            }

            /* Stack decimation */
            while ( csDecim > 0 ) {

                /* Reset memory index */
                csSelect = csIndex;

                /* Check appending mode */
                if ( csEpoch == lp_Time_s( 0xFFFFFFFFFFFFFFFF ) ) {

                    /* Parse stack for oldest timestamp */
                    for ( csParse = 0; csParse < csIndex; csParse ++ ) {

                        /* Search for oldest timestamp */
                        if ( ( csStack[csParse].dsFlag == CS_FALSE ) && ( lp_timestamp_ge( csEpoch, csStack[csParse].dsLast ) ) ) {

                            /* Memorize stack reference */
                            csSelect = csParse;

                            /* Memorize reference timestamp */
                            csEpoch = csStack[csSelect].dsLast;

                        }

                    }

                    /* Compose exportation file path */
                    sprintf( csExpo, "%s/log-recomposition.log-%05u", csReco, ++ csIncrem );

                    /* Display initial segment occurence */
                    fprintf( CS_OUT, "Recomposing %s\n", basename( csExpo ) );

                } else {

                    /* Reset parsing variable */
                    csParse = 0;

                    /* Parse stack for next segment */
                    while ( ( csSelect == csIndex ) && ( csParse < csIndex ) ) {

                        /* Compute last-first timestamp distance */
                        if ( ( csStack[csParse].dsFlag == CS_FALSE ) && ( lp_timestamp_float( lp_timestamp_diff( csEpoch, csStack[csParse].dsFirst ) ) < csInterval ) ) {

                            /* Memorize stack reference */
                            csSelect = csParse;

                            /* Memorize reference timestamp */
                            csEpoch = csStack[csSelect].dsLast;

                        }

                        /* Update parser */
                        csParse ++;

                    }

                }

                /* Check appending possibility */
                if ( csSelect < csIndex ) {

                    /* Update stack decimation */
                    csDecim --;

                    /* Reset append flag */
                    csStack[csSelect].dsFlag = CS_TRUE;

                    /* Display appending information */
                    fprintf( CS_OUT, "  %s\n", strrchr( csStack[csSelect].dsName, '/' ) + 1 );

                    /* Append logs-file content */
                    cs_elphel_recompose_append( csStack[csSelect].dsName, csExpo );

                } else {

                    /* Reset epoch value */
                    csEpoch = lp_Time_s( 0xFFFFFFFFFFFFFFFF );

                }
                
            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - File content appender
*/

    void cs_elphel_recompose_append( char const * const csSource, char const * const csDestination ) {

        /* Appending buffer variables */
        char csBuffer[1024] = { 0 };

        /* Appending length variables */
        size_t csRead = 0;

        /* Create input stream */
        FILE * csIStream = fopen( csSource, "rb" );

        /* Create output stream */
        FILE * csOStream = fopen( csDestination, "ab" );

        /* Check stream creation */
        if ( ( csIStream != NULL ) && ( csOStream != NULL ) ) {

            while ( ( csRead = fread( csBuffer, 1, 1024, csIStream ) ) > 0 ) fwrite( csBuffer, 1, csRead, csOStream );

            /* Delete streams */
            fclose( csIStream );
            fclose( csOStream );

        /* Display message */
        } else { fprintf( CS_ERR, "Error : unable to access %s or/and %s\n", basename( ( char * ) csSource ), basename( ( char * ) csDestination ) ); }

    }

/*
    Source - Timestamp extremums extractors
 */

    void cs_elphel_recompose_extremum( char const * const csFile, lp_Time_t * const csFirst, lp_Time_t * const csLast ) {

        /* Records buffer variables */
        unsigned char csRec[CS_RECLEN] = { 0 };

        /* Create input stream */
        FILE * csStream = fopen( csFile, "rb" );

        /* Clear timestamps */
        * csFirst = 0;
        * csLast  = 0;

        /* Check stream creation */
        if ( csStream != NULL ) {

            /* Parse stream */
            while ( fread( csRec, 1, CS_RECLEN, csStream ) == CS_RECLEN ) {
            
                /* Event type detection - IMU */
                if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == CS_IMU ) {

                    /* Last timestamp extraction */
                    * csLast = lp_timestamp( ( lp_Void_t * ) csRec );

                    /* First timestamp extraction */
                    if ( * csFirst == 0 ) * csFirst = * csLast;
                    

                }

            }

            /* Close input stream */
            fclose( csStream );

        /* Display message */
        } else { fprintf( CS_ERR, "Error : unable to access %s\n", basename( ( char * ) csFile ) ); }

    }

/*
    Source - Directory entity enumeration
 */

    int cs_elphel_recompose_enum( char const * const csDirectory, char * const csName ) {

        /* Directory variables */
        static DIR           * csDirect = NULL;
        static struct dirent * csEntity = NULL;

        /* Verify enumeration mode */
        if ( csDirect == NULL ) {

            /* Create directory handle */
            csDirect = opendir( csDirectory );

            /* Recusive initialization */
            return( cs_elphel_recompose_enum( csDirectory, csName ) );

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

    int cs_elphel_recompose_detect( char const * const csEntity, int const csType ) {

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

