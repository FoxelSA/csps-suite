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

    # include "csps-elphel-extract.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Paths variables */
        char csFile[256] = { 0 };
        char csExtr[256] = { 0 };

        /* Timestamp variables */
        unsigned long csLsec  = 0;
        unsigned long csLusec = 0;
        unsigned long csHsec  = 0;
        unsigned long csHusec = 0;

        /* Search in parameters */
        stdp( stda( argc, argv,  "--logs"       , "-l" ), argv,   csFile , CS_STRING );
        stdp( stda( argc, argv,  "--extraction" , "-e" ), argv,   csExtr , CS_STRING );
        stdp( stda( argc, argv,  "--second-low" , "-a" ), argv, & csLsec , CS_ULONG  );
        stdp( stda( argc, argv,  "--micro-low"  , "-u" ), argv, & csLusec, CS_ULONG  );
        stdp( stda( argc, argv,  "--second-high", "-b" ), argv, & csHsec , CS_ULONG  );
        stdp( stda( argc, argv,  "--micro-high" , "-v" ), argv, & csHusec, CS_ULONG  );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Record counter variables */
            unsigned long csCount = 0;

            /* Records buffer variables */
            lp_Byte_t csRec[CS_RECLEN] = { 0 };

            /* Timestamp variables */
            lp_Time_t csCTime = 0;
            lp_Time_t csLTime = lp_timestamp_compose( csLsec, csLusec );
            lp_Time_t csHTime = lp_timestamp_compose( csHsec, csHusec );

            /* Create streams */
            FILE * csIStream = fopen( csFile, "rb" );
            FILE * csOStream = fopen( csExtr, "wb" );

            /* Check stream creation */
            if ( ( csIStream != NULL ) && ( csOStream != NULL ) ) {

                /* Display message */
                fprintf( CS_OUT, "Extracting from %s in %s\n", strrchr( csFile, '/' ) + 1, strrchr( csExtr, '/' ) + 1 );

                /* Display message */
                fprintf( CS_OUT, "  From    : %010lu.%06lu\n  To      : %010lu.%06lu\n", csLsec, csLusec, csHsec, csHusec );

                /* Parse stream */
                while ( fread( csRec, 1, CS_RECLEN, csIStream ) == CS_RECLEN ) {

                    /* Retrieve current timestamp */
                    csCTime = lp_timestamp( ( lp_Void_t * ) csRec );

                    /* Exportation condition verification */
                    if (

                        ( lp_timestamp_ge( csCTime, csLTime ) == CS_TRUE ) &&
                        ( lp_timestamp_ge( csHTime, csCTime ) == CS_TRUE )

                    ) {

                        /* Write record in extraction file */
                        fwrite( csRec, 1, CS_RECLEN, csOStream );

                        /* Update record counter */
                        csCount ++; 

                    }

                }

                /* Display information */
                fprintf( CS_OUT, "  Records : %lu\n", csCount );

                /* Close streams */
                fclose( csIStream );
                fclose( csOStream );

            /* Display message */
            } else { fprintf( CS_OUT, "Error : unable to access %s or/and %s\n", strrchr( csFile, '/' ) + 1, strrchr( csExtr, '/' ) + 1 ); }

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

