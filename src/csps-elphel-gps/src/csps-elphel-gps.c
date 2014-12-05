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

        /* Records buffer stack variables */
        lp_Byte_t csStackSent[CS_NTYPE][CS_RECLEN] = { { 0 } };

        /* Records types stack variables */
        lp_Byte_t csStackType[CS_NTYPE] = { 0 };

        /* Records buffer variables */
        lp_Byte_t csBuffer[CS_RECLEN] = { 0 };

        /* Timestamps variables */
        lp_Time_t csTimer = 0;
        lp_Time_t csTimea = 0;
        lp_Time_t csTimeb = 0;
        lp_Time_t csTimec = 0;

        /* Parsing variables */
        unsigned int csParse = 0;

        /* Sentence type variables */
        unsigned int csType  = 0;

        /* Decimation stack variables */
        unsigned long csStack = 0;
        unsigned long csRepet = 0;

        /* Returned value variables */
        unsigned long csCount = 0;

        /* Content parsing */
        while ( fread( csBuffer, 1, CS_RECLEN, csIStream ) == CS_RECLEN ) {

            /* Event type verification */
            if ( CS_EVENT( csBuffer, CS_GPS ) ) {

                /* Memorize very first timestamp */
                if ( csTimec == 0 ) csTimec = CS_TIMES( csBuffer );
               
                /* Detect sentence type */
                csType = CS_NMEAT( csBuffer );

                /* Sentence type detection */
                if ( csType == LP_NMEA_IDENT_GGA ) {

                    /* Initial bloc skipping verification */
                    if ( lp_timestamp_eq( csTimea, csTimec ) == CS_FALSE ) {

                        /* Check stack state */
                        if ( csStack == CS_NTYPE ) {

                            /* Bloc validation verification */
                            if ( cs_elphel_gps_bloc( csStackType ) == CS_TRUE ) {

                                /* Check reference timestamp repetition */
                                if ( lp_timestamp_eq( csTimea, csTimeb ) == CS_FALSE ) {

                                    /* Update reference memory */
                                    csTimeb = csTimea;

                                    /* Reset repetition condition */
                                    csRepet = 0;

                                } else {

                                    /* Update repetition counter */
                                    csRepet ++;

                                }

                                /* Repetition overflow */
                                if ( csRepet < CS_REPET ) {

                                    /* Rebuild timestamp */
                                    csTimer = cs_elphel_gps_timestamp( csTimea, csRepet );

                                    /* Records exportation loop */
                                    for ( csParse = 0; csParse < CS_NTYPE; csParse ++ ) {

                                        /* Override record header */
                                        cs_elphel_gps_header( ( lp_Time_t * ) csStackSent[csParse], csTimer );

                                        /* Export stacked record */
                                        fwrite( csStackSent[csParse], 1, CS_RECLEN, csOStream );

                                    }

                                }

                            /* Update decimation count */
                            } else { csCount += csStack; }

                        /* Update decimation count */
                        } else { csCount += csStack; }

                    /* Update decimation count */
                    } else { csCount += csStack; }

                    /* Reset stack state */
                    csStack = 0;

                    /* Reset stack content */
                    memset( ( void * ) csStackType, 0, CS_NTYPE );

                /* Sentence type detection */
                } else if ( csType == LP_NMEA_IDENT_RMC ) {

                    /* Read record timestamp */
                    csTimea = CS_TIMES( csBuffer );

                }

                /* Check stack state */
                if ( csStack < CS_NTYPE ) {

                    /* Logging sentence type */
                    csStackType[csType] ++;

                    /* Push buffer content */
                    for ( csParse = 0; csParse < CS_RECLEN; csParse ++ ) {

                        /* Bytes copy */
                        csStackSent[csStack][csParse] = csBuffer[csParse];

                    }

                }

                /* Update stack */
                csStack ++;

            } else {

                /* Export record buffer */
                fwrite( csBuffer, 1, CS_RECLEN, csOStream );

            }

        }

        /* Return decimation count */
        return( csCount );

    }

/*
    Source - NMEA sentence block validation
 */

    int cs_elphel_gps_bloc( lp_Byte_t const * const csBlock ) {

        /* Parsing variables */
        int csParse = 0;

        /* Returned value variables */
        int csReturn = CS_TRUE;

        /* Validation loop */
        while ( ( csParse < CS_NTYPE ) && ( csReturn == CS_TRUE ) ) {

            /* Validation condition */
            if ( csBlock[csParse++] != 1 ) csReturn = CS_FALSE;

        }

        /* Return results */
        return( csReturn );

    }

/*
    Source - GPS timestamp reconstruction
*/

    lp_Time_t cs_elphel_gps_timestamp( lp_Time_t csReference, unsigned long csRepet ) {

        /* Computation buffer variables */
        lp_Time_t csReturn = csRepet * 200000lu;

        /* Compute reconstructed timestamp */
        return( lp_timestamp_add( csReference, lp_timestamp_compose( csReturn / 1000000, csReturn % 1000000 ) ) );


    }

/*
    Source - Record header override
*/

    void cs_elphel_gps_header( lp_Time_t * const csHeader, lp_Time_t const csTime ) {

        /* Override record header */
        ( * csHeader ) = csTime | ( ( * csHeader ) & 0x00000000FFF00000llu );

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

