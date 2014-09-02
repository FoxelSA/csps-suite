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

    # include "csps-auditlog.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Record root directory variables */
        char csRoot[256] = { 0 };

        /* Record log directory variables */
        char csLogd[256] = { 0 };

        /* Record log file path variables */
        char csLogp[256] = { 0 };

        /* Record log size variables */
        long csSize = 0L;

        /* File enumeration variables */
        DIR           * csDir = NULL;
        struct dirent * csEnt = NULL;

        /* Search in parameters */
        cs_stdp( cs_stda( argc, argv,  "--root", "-r" ), argv, csRoot , CS_STRING );

        /* Execution switch */
        if ( cs_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Build log directory */
            sprintf( csLogd, "%s/mov/1", csRoot );

            /* Check if directory is available */
            if ( cs_auditlog_is_directory( csLogd ) == CS_TRUE ) {

                /* Ask directory handle */
                if ( ( csDir = opendir( csLogd ) ) != NULL ) {

                    /* File enumeration loop */
                    while ( ( csEnt = readdir( csDir ) ) != NULL ) {

                        /* Check for regular files and pattern */
                        if ( ( csEnt->d_type == DT_REG ) && ( strstr( csEnt->d_name, ".log-" ) != NULL ) ) {

                            /* Build log file path */
                            sprintf( csLogp, "%s/%s", csLogd, csEnt->d_name );

                            /* Display information */
                            fprintf( CS_OUT, "Auditing : %s\n", csLogp );

                            /* Ask log file size */
                            csSize = cs_auditlog_filesize( csLogp );

                            /* Display information */
                            fprintf( CS_OUT, "    Size          : %li Bytes (%f Mo)\n", csSize, ( double ) csSize / 1048576.0 );

                            /* Display information */
                            fprintf( CS_OUT, "    64-congruence : %li ", csSize % 64 );

                            /* Audit - 64 congruence check */
                            if ( ( csSize % 64 ) == 0 ) fprintf( CS_OUT, "(Valid)\n" ); else fprintf( CS_OUT, "(Invalid : missing %li bytes to last record\n", 64 - ( csSize % 64 ) );

                            /* Display information */
                            fprintf( CS_OUT, "    Records count : %li\n", csSize >> 6 );

                        }

                    }

                /* Display message */
                } else { fprintf( CS_OUT, "Error : Unable to enumerate files in logs directory\n" ); }


            /* Display message */
            } else { fprintf( CS_OUT, "Error : Unable to find logs directory\n" ); }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - File and directory operations
 */

    int cs_auditlog_is_directory( const char * const csDirectory ) {

        /* Ask directory handle */
        DIR * csDir = opendir( csDirectory );

        /* Check directory handle */
        if ( csDir != NULL ) {

            /* Close directory handle */
            closedir( csDir );

            /* Return positive answer */
            return( CS_TRUE );

        } else {

            /* Return negative answer */
            return( CS_FALSE );

        }       

    }

    long cs_auditlog_filesize( const char * const csFile ) {

        /* Returned variables */
        long csSize = 0L;

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

    int cs_stda( int argc, char ** argv, const char * const ltag, const char * const stag ) {

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

    void cs_stdp( int argi, char ** argv, void * param, int type ) {

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

