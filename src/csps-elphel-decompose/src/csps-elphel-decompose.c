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

    # include "csps-elphel-decompose.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csRaw[256] = { 0 };
        char csDec[256] = { 0 };
        char csEnt[256] = { 0 };

        /* Decompostion condition variables */
        double csInterval = 1.0;

        /* Search in parameters */
        stdp( stda( argc, argv,  "--raw"       , "-r" ), argv,   csRaw, CS_STRING );
        stdp( stda( argc, argv,  "--decomposed", "-d" ), argv,   csDec, CS_STRING );
        stdp( stda( argc, argv,  "--interval"  , "-i" ), argv, & csInterval, CS_DOUBLE );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Decomposition index variables */
            int csIndex = 1;

            /* Directory entity enumeration */
            while ( cs_elphel_decompose_enum( csRaw, csEnt ) != CS_FALSE ) {

                /* Consider only file entity */
                if ( cs_elphel_decompose_detect( csEnt, CS_FILE ) == CS_TRUE ) {

                    /* Check log-file tag */
                    if ( strstr( csEnt, CS_PATH_PATTERN ) != 0 ) {

                        /* Decomposition process */
                        csIndex = cs_elphel_decompose_split( csEnt, csDec, csIndex, csInterval );

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - FPGA-log decomposer
*/

    int cs_elphel_decompose_split( char const * const csLog, char const * const csDirectory, int csIndex, double csInterval ) {

        /* Records buffer variables */
        unsigned char csRec[CS_RECLEN] = { 0 };

        /* Decomposition segment path variables */
        char csPart[256] = { 0 };

        /* Parsing flag variables */
        int csFlag = CS_FALSE;

        /* Pure split variables */
        int csPure = CS_TRUE;

        /* Timestamp variables */
        lp_Time_t csPTime = 0;
        lp_Time_t csCTime = 0;

        /* Stream handle variables */
        FILE * csIStream = NULL;
        FILE * csOStream = NULL;

        /* Compose initial decomposition segment path */
        sprintf( csPart, "%s/log-decomposition.log-%05i", csDirectory, csIndex ++ );

        /* Display decomposition information */
        fprintf( CS_OUT, "Decomposing : %s\n  %s\n", strrchr( csLog, '/' ) + 1, strrchr( csPart, '/' ) + 1 );

        /* Create input stream */
        csIStream = fopen( csLog, "rb" );

        /* Create output stream */
        csOStream = fopen( csPart, "wb" );

        /* Check stream creation */
        if ( ( csIStream != NULL ) && ( csOStream != NULL ) ) { 

            /* Parsing input stream */
            while ( fread( csRec, 1, CS_RECLEN, csIStream ) == CS_RECLEN ) {

                /* Detect IMU events */
                if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == CS_IMU ) {

                    /* Read record timestamp */
                    csCTime = lp_timestamp( ( lp_Void_t * ) csRec );

                    /* Check parsing flag */
                    if ( csFlag == CS_FALSE ) {

                        /* Update parsing flag */
                        csFlag = CS_TRUE;

                    } else {

                        /* Check splitting condition */
                        if ( lp_timestamp_float( lp_timestamp_diff( csCTime, csPTime ) ) > csInterval ) {

                            /* Update decomposition segment path */
                            sprintf( csPart, "%s/log-decomposition.log-%05i", csDirectory, csIndex ++ );

                            /* Close output stream */
                            fclose( csOStream );

                            /* Open output stream */
                            csOStream = fopen( csPart, "wb" );

                            /* Display decomposition information */
                            fprintf( CS_OUT, "  %s (%s)\n", strrchr( csPart, '/' ) + 1, csPure == CS_TRUE ? "Pure  " : "Impure" );

                        }

                    }

                    /* Memorize previous timestamp */
                    csPTime = csCTime;

                    /* Update split purity */
                    csPure = CS_TRUE;

                } else {

                    /* Update split purity */
                    csPure = CS_FALSE;

                }

                /* Write recored in output stream */
                fwrite( csRec, 1, CS_RECLEN, csOStream );

            }

            /* Close streams */
            fclose( csOStream );
            fclose( csIStream );

        /* Display message */
        } else { fprintf( CS_OUT, "Error : unable to access %s or/and %s\n", strrchr( csLog, '/' ) + 1, strrchr( csPart, '/' ) + 1 ); }

        /* Return decomposition index */
        return( csIndex );

    }

/*
    Source - Directory entity enumeration
 */

    int cs_elphel_decompose_enum( char const * const csDirectory, char * const csName ) {

        /* Directory variables */
        static DIR           * csDirect = NULL;
        static struct dirent * csEntity = NULL;

        /* Verify enumeration mode */
        if ( csDirect == NULL ) {

            /* Create directory handle */
            csDirect = opendir( csDirectory );

            /* Recusive initialization */
            return( cs_elphel_decompose_enum( csDirectory, csName ) );

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

    int cs_elphel_decompose_detect( char const * const csEntity, int const csType ) {

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

