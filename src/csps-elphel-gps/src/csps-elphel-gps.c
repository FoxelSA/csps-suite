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

    # include "csps-elphel-gps.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csSrc[256] = { 0 };
        char csDst[256] = { 0 };
        char csInl[256] = { 0 };
        char csOul[256] = { 0 };

        /* Tolerance variables */
        double csTol = 5.0;

        /* Indexation variables */
        unsigned long csIndex = 1;

        /* Stream handle variables */
        FILE * csIStream = NULL;
        FILE * csOStream = NULL;

        /* Search in parameters */
        stdp( stda( argc, argv, "--source"     , "-s" ), argv,   csSrc, CS_STRING );
        stdp( stda( argc, argv, "--destination", "-d" ), argv,   csDst, CS_STRING );
        stdp( stda( argc, argv, "--tolerance"  , "-t" ), argv, & csTol, CS_DOUBLE );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Directory entity enumeration */
            while ( cs_elphel_gps_enum( csSrc, csInl ) != CS_FALSE ) {

                /* Consider only file entity */
                if ( cs_elphel_gps_detect( csInl, CS_FILE ) == CS_TRUE ) {

                    /* Check log-file tag */
                    if ( strstr( csInl, CS_PATH_PATTERN ) != 0 ) {

                        /* Display information */
                        fprintf( CS_OUT, "Decimating : %s\n", basename( csInl ) );

                        /* Construct output file name */
                        sprintf( csOul, "%s/log-container.log-%05li", csDst, csIndex ++ );

                        /* Create input stream */
                        if ( ( csIStream = fopen( csInl, "rb" ) ) != NULL ) {

                            /* Create output stream */
                            if ( ( csOStream = fopen( csOul, "wb" ) ) != NULL ) {

                                /* Log-file GPS processing */
                                fprintf( CS_OUT, "  Decimated : %lu\n", cs_elphel_gps_process( csIStream, csOStream, csTol ) );

                            /* Display message */
                            } else { fprintf( CS_ERR, "Error : unable to access %s\n", basename( csOul ) ); }

                            /* Close input stream */
                            fclose( csIStream );

                        /* Display message */
                        } else { fprintf( CS_ERR, "Error : unable to access %s\n", basename( csInl ) ); }

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Log-file GPS processing
*/

    unsigned long cs_elphel_gps_process( FILE * const csIStream, FILE * const csOStream, double csTol ) {

        /* Records buffer variables */
        lp_Byte_t csBuffer[CS_RECLEN] = { 0 };

        /* Timestamp variables */
        lp_Time_t csGPStime = 0;
        lp_Time_t csIMUtime = 0;

        /* Returned value variables */
        unsigned long csCount = 0;

        /* Content parsing loop */
        while ( fread( csBuffer, 1, CS_RECLEN, csIStream ) == CS_RECLEN ) {

            /* Event selector */
            if ( CS_EVENT( csBuffer, CS_GPS ) ) {

                /* Read timestamp */
                csGPStime = lp_timestamp( ( lp_Void_t * ) csBuffer );    

                /* Decimation condition trigger */
                if ( lp_timestamp_float( lp_timestamp_diff( csGPStime, csIMUtime ) ) < csTol ) {

                     /* Export record buffer */
                     fwrite( csBuffer, 1, CS_RECLEN, csOStream ); 

                /* Decimation count */
                } else { csCount ++; }

            } else {

                /* Event selector */
                if ( CS_EVENT( csBuffer, CS_IMU ) ) csIMUtime = lp_timestamp( ( lp_Void_t * ) csBuffer );

                 /* Export record buffer */
                fwrite( csBuffer, 1, CS_RECLEN, csOStream );

            }

        }

        return( csCount );

    }

/*
    Source - Directory entity enumeration
 */

    int cs_elphel_gps_enum( char const * const csDirectory, char * const csName ) {

        /* Directory variables */
        static DIR           * csDirect = NULL;
        static struct dirent * csEntity = NULL;

        /* Verify enumeration mode */
        if ( csDirect == NULL ) {

            /* Create directory handle */
            csDirect = opendir( csDirectory );

            /* Recusive initialization */
            return( cs_elphel_gps_enum( csDirectory, csName ) );

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

    int cs_elphel_gps_detect( char const * const csEntity, int const csType ) {

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
    Source - File size extractor
 */

    size_t cs_elphel_validate_filesize( char const * const csFile ) {

        /* Returned variables */
        size_t csSize = 0L;

        /* Ask pointed file handle */
        FILE * csHandle = fopen( csFile, "rb" );

        /* Check file handle */
        if ( csHandle != NULL ) {

            /* Update file offset */
            fseek( csHandle, 0L, SEEK_END );

            /* Ask value of updated offset */
            csSize = ftell( csHandle );

            /* Close file handle */
            fclose( csHandle );            

        }

        /* Return file size */
        return( csSize );

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

