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

    # include "csps-process.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Structure path variables */
        char csPath[256] = { 0 };
        char csSegs[256] = { 0 };
        char csSegm[256] = { 0 };
        char csTopo[256] = { 0 };

        /* Enumeration variables */
        DIR    * csDirect = NULL;
        DIR    * csSegDir = NULL;
        DIRENT * csEntity = NULL;

        /* Date and time variables */
        time_t csTime;

        /* Search in parameters */
        stdp( stda( argc, argv,  "--path", "-p" ), argv, csPath , CS_STRING );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Retrieve time */
            time( & csTime );

            /* Display message */
            fprintf( CS_OUT, "Process performed using csps-process on %sCourse : %s\n", ctime( & csTime ), strrchr( csPath, '/' ) + 1 );

            /* Create segments path */
            sprintf( csSegs, "%s/" CS_PATH_CSPS, csPath );

            /* Create directory handle */
            if ( ( csDirect = opendir( csSegs ) ) != NULL ) {

                /* Entities enumeration loop */
                while ( ( csEntity = readdir( csDirect ) ) != NULL ) {

                    /* Create segment path */
                    sprintf( csSegm, "%s/%s", csSegs, csEntity->d_name );

                    /* Check entity for directory */
                    if ( ( csSegDir = opendir( csSegs ) ) != NULL ) {

                        /* Close handle */
                        closedir( csSegDir );

                        /* Avoid redirection directory */
                        if ( ( strcmp( csEntity->d_name, "." ) != 0 ) && ( strcmp( csEntity->d_name, ".." ) != 0 ) ) {

                            /* Create standard topology path */
                            sprintf( csTopo, "%s/" CS_PATH_TOPO, csSegm );

                            /* Display message */
                            fprintf( CS_OUT, "Processing\n" );

                            /* Display message */
                            fprintf( CS_OUT, "\tSegment  : %s\n", strrchr( csSegm, '/' ) + 1 );
                            fprintf( CS_OUT, "\tTopology : %s\n", strrchr( csTopo, '/' ) + 1 );
                            fprintf( CS_OUT, "\tSegment  : %s\n", csSegm );
                            fprintf( CS_OUT, "\tTopology : %s\n", csTopo );

                            /* Display message */
                            fprintf( CS_OUT, "\tCSPS processing ... " );

                            /* CSPS processing */
                            lp_system( csSegm, csTopo );

                            /* Display message */
                            fprintf( CS_OUT, "Done\n" );
                            

                        }

                    }

                }

                /* Close directory handle */
                closedir( csDirect );

                /* Display message */
                fprintf( CS_OUT, "Done!\n" );

            /* Display message */
            } else { fprintf( CS_OUT, "Error : Unable access master directory %s.\n", csPath ); }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

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

