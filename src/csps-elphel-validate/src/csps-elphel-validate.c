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

    # include "csps-elphel-validate.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csRec[256] = { 0 };
        char csVal[256] = { 0 };
        char csEnt[256] = { 0 };
        char csExp[256] = { 0 };

        /* Minimum size variables */
        long csMinimum = 2524160;

        /* Validation index variables */
        long csIndex = 1;

        /* Search in parameters */
        stdp( stda( argc, argv, "--recomposed"  , "-r" ), argv,   csRec     , CS_STRING );
        stdp( stda( argc, argv, "--validated"   , "-v" ), argv,   csVal     , CS_STRING );
        stdp( stda( argc, argv, "--minimum-size", "-m" ), argv, & csMinimum , CS_LONG   );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Directory entity enumeration */
            while ( cs_elphel_validate_enum( csRec, csEnt ) != CS_FALSE ) {

                /* Consider only file entity */
                if ( cs_elphel_validate_detect( csEnt, CS_FILE ) == CS_TRUE ) {

                    /* Check log-file tag */
                    if ( strstr( csEnt, CS_PATH_PATTERN ) != 0 ) {

                        /* Check file length */
                        if ( cs_elphel_validate_filesize( csEnt ) > csMinimum ) {

                            /* Build validated temporary logs-file path */
                            sprintf( csExp, "%s/log-validated.log-%05li", csVal, csIndex ++ );

                            /* Display information */
                            fprintf( CS_OUT, "Validating   : %s\n  Exportation    : %s\n", basename( csEnt ), basename( csExp ) );

                            /* Copy validated log-file */
                            cs_elphel_validate_copy( csEnt, csExp );

                        } else {

                            /* Display information */
                            fprintf( CS_OUT, "Invalidating : %s\n  Exportation    : Not exported\n", basename( csEnt ) );

                        }

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - File copy
*/

    void cs_elphel_validate_copy( char const * const csIFile, char const * const csOFile ) {

        /* Records buffer variables */
        unsigned char csRec[CS_RECLEN] = { 0 };

        /* File handle variables */
        FILE * csIStream = fopen( csIFile, "rb" );
        FILE * csOStream = fopen( csOFile, "wb" );

        /* Check stream creation */
        if ( ( csIStream != NULL ) && ( csOStream != NULL ) ) {

            /* Records copy loop */
            while ( fread( csRec, 1, CS_RECLEN, csIStream ) == CS_RECLEN ) fwrite( csRec, 1, CS_RECLEN, csOStream );

            /* Close streams */
            fclose( csIStream );
            fclose( csOStream );

        /* Display message */
        } else { fprintf( CS_ERR, "Error : unable to access %s or/and %s\n", basename( ( char * ) csIFile ), basename( ( char * ) csOFile ) ); }

    }

/*
    Source - Directory entity enumeration
 */

    int cs_elphel_validate_enum( char const * const csDirectory, char * const csName ) {

        /* Directory variables */
        static DIR           * csDirect = NULL;
        static struct dirent * csEntity = NULL;

        /* Verify enumeration mode */
        if ( csDirect == NULL ) {

            /* Create directory handle */
            csDirect = opendir( csDirectory );

            /* Recusive initialization */
            return( cs_elphel_validate_enum( csDirectory, csName ) );

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

    int cs_elphel_validate_detect( char const * const csEntity, int const csType ) {

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

