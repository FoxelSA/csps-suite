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

    # include "csps-elphel-audit.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Paths variables */
        char csPath[256] = { 0 };
        char csFile[256] = { 0 };

        /* Search in parameters */
        stdp( stda( argc, argv, "--path", "-p" ), argv, csPath, CS_STRING );

        /* Execution switch */
        if ( stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Directory entity enumeration */
            while ( cs_elphel_audit_enum( csPath, csFile ) != CS_FALSE ) {

                /* Check entity for files */
                if ( cs_elphel_audit_detect( csFile, CS_FILE ) == CS_TRUE ) {

                    /* Check logs-files tag */
                    if ( strstr( csFile, CS_PATH_PATTERN ) != 0 ) {

                        /* Audit logs-file */
                        cs_elphel_audit( csFile, cs_elphel_audit_filesize( csFile ) );

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Audit procedure
 */

    void cs_elphel_audit( char const * const csFile, size_t const csSize ) {

        /* Timestamps variables */
        lp_Time_t csDEVstep = lp_Time_s( 0 );
        lp_Time_t csIMUtime = lp_Time_s( 0 );
        lp_Time_t csIMUinit = lp_Time_s( 0 );
        lp_Time_t csIMUprev = lp_Time_s( 0 );
        lp_Time_t csIMUstpi = lp_Time_s( 0xFFFFFFFFFFFFFFFF );
        lp_Time_t csIMUstpm = lp_Time_s( 0 );
        lp_Time_t csCAMtime = lp_Time_s( 0 );
        lp_Time_t csCAMinit = lp_Time_s( 0 );
        lp_Time_t csCAMmain = lp_Time_s( 0 );
        lp_Time_t csCAMdiff = lp_Time_s( 0 );
        lp_Time_t csCAMprev = lp_Time_s( 0 );
        lp_Time_t csCAMstpi = lp_Time_s( 0xFFFFFFFFFFFFFFFF );
        lp_Time_t csCAMstpm = lp_Time_s( 0 );
        lp_Time_t csGPStime = lp_Time_s( 0 );
        lp_Time_t csGPSinit = lp_Time_s( 0 );
        lp_Time_t csGPSprev = lp_Time_s( 0 );
        lp_Time_t csGPSstpi = lp_Time_s( 0xFFFFFFFFFFFFFFFF );
        lp_Time_t csGPSstpm = lp_Time_s( 0 );

        /* Records buffer variables */
        unsigned char csRec[CS_RECLEN] = { 0 };

        /* Input stream variables */
        FILE * csIStream = fopen( csFile, "rb" );

        /* Check stream creation */
        if ( csIStream != NULL ) {

            /* Parse file */
            while ( fread( csRec, 1, CS_RECLEN, csIStream ) == CS_RECLEN ) {

                /* IMU records */
                if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == CS_IMU ) {

                    /* Extract current timestamp */
                    csIMUtime = lp_timestamp( ( lp_Void_t * ) csRec );

                    /* Initial timestamp check */
                    if ( csIMUinit == lp_Time_s( 0 ) ) { 

                        /* Memorize initial timestamp */
                        csIMUinit = csIMUtime; 

                    } else {

                        /* Compute step value */
                        csDEVstep = lp_timestamp_diff( csIMUprev, csIMUtime );

                        /* Memorize step extremums */
                        if ( lp_timestamp_ge( csDEVstep, csIMUstpm ) == LP_TRUE ) csIMUstpm = csDEVstep;
                        if ( lp_timestamp_ge( csIMUstpi, csDEVstep ) == LP_TRUE ) csIMUstpi = csDEVstep;

                    }

                    /* Memorize current timestamp */
                    csIMUprev = csIMUtime;

                /* Camera records */
                } else 
                if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == CS_MAS ) {

                    /* Extract current timestamp */
                    csCAMtime = lp_timestamp( ( lp_Void_t * ) csRec );

                    /* Initial timestamp check */
                    if ( csCAMinit == lp_Time_s( 0 ) ) { 

                        /* Read camera record timestamp */
                        csCAMmain = lp_timestamp( ( lp_Void_t * ) ( csRec + lp_Size_s( 8 ) ) );

                        /* Compute difference */
                        csCAMdiff = lp_timestamp_diff( csCAMmain, csCAMtime );

                        /* Memorize initial timestamp */
                        csCAMinit = csCAMtime;

                    } else {

                        /* Compute step value */
                        csDEVstep = lp_timestamp_diff( csCAMprev, csCAMtime );

                        /* Memorize step extremums */
                        if ( lp_timestamp_ge( csDEVstep, csCAMstpm ) == LP_TRUE ) csCAMstpm = csDEVstep;
                        if ( lp_timestamp_ge( csCAMstpi, csDEVstep ) == LP_TRUE ) csCAMstpi = csDEVstep;

                    }

                    /* Memorize current timestamp */
                    csCAMprev = csCAMtime;

                /* GPS records */
                } else 
                if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == CS_GPS ) {

                    /* Extract current timestamp */
                    csGPStime = lp_timestamp( ( lp_Void_t * ) csRec );

                    /* Initial timestamp check */
                    if ( csGPSinit == lp_Time_s( 0 ) ) {

                        /* Memorize initial timestamp */
                        csGPSinit = csGPStime;

                    } else {

                        /* Compute step value */
                        csDEVstep = lp_timestamp_diff( csGPSprev, csGPStime );

                        /* Memorize step extremums */
                        if ( lp_timestamp_ge( csDEVstep, csGPSstpm ) == LP_TRUE ) csGPSstpm = csDEVstep;
                        if ( lp_timestamp_ge( csGPSstpi, csDEVstep ) == LP_TRUE ) csGPSstpi = csDEVstep;

                    }

                    /* Memorize current timestamp */
                    csGPSprev = csGPStime;

                }

            }

            /* Delete file handle */
            fclose( csIStream );

            /* Display information */
            fprintf( CS_OUT, "Auditing : %s\n", basename( ( char * ) csFile ) );

            /* Display information - Master for human */
            fprintf( CS_OUT, "  Start       : %s\n", cs_elphel_audit_utcstring( lp_timestamp_sec( csCAMinit ) ) );
            fprintf( CS_OUT, "  Stop        : %s\n", cs_elphel_audit_utcstring( lp_timestamp_sec( csCAMtime ) ) );

            /* Display information - Length */
            fprintf( CS_OUT, "  Bytes       : %li\n", csSize      );
            fprintf( CS_OUT, "  CGC-64      : %li\n", csSize % 64 );
            fprintf( CS_OUT, "  Records     : %li\n", csSize >> 6 );

            /* Display information - Master */
            fprintf( CS_OUT, "  Range (MAS) : " CS_MD " - " CS_MD "\n", CS_TS( csCAMinit ), CS_TS( csCAMtime ) );
            fprintf( CS_OUT, "  Span  (MAS) : " CS_MD "\n", CS_TS( lp_timestamp_diff( csCAMinit, csCAMtime ) ) );
            fprintf( CS_OUT, "  Steps (MAS) : " CS_MD " - " CS_MD "\n", CS_TS( csCAMstpi ), CS_TS( csCAMstpm ) );
            fprintf( CS_OUT, "  Synch (MAS) : " CS_MD " - " CS_MD "\n", CS_TS( csCAMmain ), CS_TS( csCAMdiff ) );

            /* Display information - IMU */
            fprintf( CS_OUT, "  Range (IMU) : " CS_MD " - " CS_MD "\n", CS_TS( csIMUinit ), CS_TS( csIMUtime ) );
            fprintf( CS_OUT, "  Span  (IMU) : " CS_MD "\n", CS_TS( lp_timestamp_diff( csIMUinit, csIMUtime ) ) );
            fprintf( CS_OUT, "  Steps (IMU) : " CS_MD " - " CS_MD "\n", CS_TS( csIMUstpi ), CS_TS( csIMUstpm ) );

            /* Display information - GPS */
            fprintf( CS_OUT, "  Range (GPS) : " CS_MD " - " CS_MD "\n", CS_TS( csGPSinit ), CS_TS( csGPStime ) );
            fprintf( CS_OUT, "  Span  (GPS) : " CS_MD "\n", CS_TS( lp_timestamp_diff( csGPSinit, csGPStime ) ) );
            fprintf( CS_OUT, "  Steps (GPS) : " CS_MD " - " CS_MD "\n", CS_TS( csGPSstpi ), CS_TS( csGPSstpm ) );

        /* Display message */
        } else { fprintf( CS_ERR, "Error : unable to access %s\n", basename( ( char * ) csFile ) ); }

    }

/*
    Source - Human readable timestamp converter
 */

    char * cs_elphel_audit_utcstring( lp_Time_t csTimestamp ) {

        /* Static string variables */
        static char csHuman[256] = { 0 };

        /* Timestamp variables */
        time_t csUnixTime = csTimestamp;

        /* Decomposed time variables */
        struct tm * csTime;

        /* Create decomposed time structure */
        csTime = gmtime( & csUnixTime );

        /* Create human readable string */
        strftime ( csHuman, 256, "%Y-%m-%dT%H:%M:%S+00:00", csTime );

        /* Returns pointer to string */
        return( csHuman );

    }

/*
    Source - Directory entity enumeration
 */

    int cs_elphel_audit_enum( char const * const csDirectory, char * const csName ) {

        /* Directory variables */
        static DIR           * csDirect = NULL;
        static struct dirent * csEntity = NULL;

        /* Verify enumeration mode */
        if ( csDirect == NULL ) {

            /* Create directory handle */
            csDirect = opendir( csDirectory );

            /* Recusive initialization */
            return( cs_elphel_audit_enum( csDirectory, csName ) );

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

    int cs_elphel_audit_detect( char const * const csEntity, int const csType ) {

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

    size_t cs_elphel_audit_filesize( char const * const csFile ) {

        /* Returned variables */
        size_t csSize = 0L;

        /* Ask pointed file handle */
        FILE * csIStream = fopen( csFile, "rb" );

        /* Check file handle */
        if ( csIStream != NULL ) {

            /* Update file offset */
            fseek( csIStream, 0L, SEEK_END );

            /* Ask value of updated offset */
            csSize = ftell( csIStream );

            /* Close file handle */
            fclose( csIStream );            

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

