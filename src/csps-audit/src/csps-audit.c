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

    # include "csps-audit.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Paths variables */
        char csPath[256] = { 0 };
        char csFile[256] = { 0 };

        /* Directory variables */
        DIR    * csDirect = NULL;
        DIRENT * csEntity = NULL;

        /* Time variables */
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
            fprintf( CS_OUT, "Audit performed using csps-audit on %sCourse : %s\n", ctime( & csTime ), strrchr( csPath, '/' ) + 1 );

            /* Create directory handle */
            if ( ( csDirect = opendir( strcat( csPath, "/" CS_PATH_RAW ) ) ) != NULL ) {

                /* Enumerates directory entities */
                while ( ( csEntity = readdir( csDirect ) ) != NULL ) {

                    /* Device file detection */
                    if ( ( csEntity->d_type == DT_REG ) && ( strstr( csEntity->d_name, CS_PATH_PATTERN ) != NULL ) ) {

                        /* Construct file path */
                        sprintf( csFile, "%s/%s", csPath, csEntity->d_name );

                        /* File audit procedure */
                        cs_audit_audit( csFile );

                    }

                }

                /* Delete directory handle */
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
    Source - Audit procedure
 */

    void cs_audit_audit( char const * const csFile ) {

        /* Records buffer variables */
        unsigned char csRec[LP_DEVICE_EYESIS4PI_RECLEN] = { 0 };

        /* Timestamps variables */
        lp_Time_t csDEVstep = lp_Time_s( 0 );
        lp_Time_t csIMUtime = lp_Time_s( 0 );
        lp_Time_t csIMUinit = lp_Time_s( 0 );
        lp_Time_t csIMUprev = lp_Time_s( 0 );
        lp_Time_t csIMUstpi = lp_timestamp_compose( lp_Time_s( 4000000000 ), lp_Time_s( 0 ) );
        lp_Time_t csIMUstpm = lp_Time_s( 0 );
        lp_Time_t csCAMtime = lp_Time_s( 0 );
        lp_Time_t csCAMinit = lp_Time_s( 0 );
        lp_Time_t csCAMmain = lp_Time_s( 0 );
        lp_Time_t csCAMdiff = lp_Time_s( 0 );
        lp_Time_t csCAMprev = lp_Time_s( 0 );
        lp_Time_t csCAMstpi = lp_timestamp_compose( lp_Time_s( 4000000000 ), lp_Time_s( 0 ) );
        lp_Time_t csCAMstpm = lp_Time_s( 0 );
        lp_Time_t csGPStime = lp_Time_s( 0 );
        lp_Time_t csGPSinit = lp_Time_s( 0 );
        lp_Time_t csGPSprev = lp_Time_s( 0 );
        lp_Time_t csGPSstpi = lp_timestamp_compose( lp_Time_s( 4000000000 ), lp_Time_s( 0 ) );
        lp_Time_t csGPSstpm = lp_Time_s( 0 );

        /* Handle variables */
        FILE * csHandle = NULL;

        /* Size variables */
        long csSize = 0;

        /* Retrieve file size */
        if ( ( csSize = cs_audit_filesize( csFile ) ) != 0 ) {

            /* Create file handle */
            csHandle = fopen( csFile, "rb" );

            /* Parse file */
            while ( fread( csRec, 1, LP_DEVICE_EYESIS4PI_RECLEN, csHandle ) == LP_DEVICE_EYESIS4PI_RECLEN ) {

                /* IMU records */
                if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == LP_DEVICE_EYESIS4PI_IMUEVT ) {

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

                /* CAM records */
                } else if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == LP_DEVICE_EYESIS4PI_MASEVT ) {

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
                } else if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == LP_DEVICE_EYESIS4PI_GPSEVT ) {

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
            fclose( csHandle );

            /* Display message */
            fprintf( CS_OUT, "Auditing : %s\n", strrchr( csFile, '/' ) + 1 ); 

            /* Display message */
            fprintf( CS_OUT, "\tLength      : +%li Bytes (%f Mo)\n", csSize, ( double ) csSize / 1048576.0 );
            fprintf( CS_OUT, "\t64-CGC      : +%li (%s)\n", csSize % 64, ( csSize % 64 ) == 0 ? "Valid" : "Broken" );
            fprintf( CS_OUT, "\tRecords     : +%li\n", csSize >> 6 );

            /* Display message */
            fprintf( CS_OUT, "\tRange (CAM) : " ); CS_TIMESTAMP( csCAMinit ); CS_SPACE; CS_TIMESTAMP( csCAMtime ); CS_ENDL;
            fprintf( CS_OUT, "\tSpan  (CAM) : " ); CS_TIMESTAMP( lp_timestamp_diff( csCAMinit, csCAMtime ) ); CS_ENDL;
            fprintf( CS_OUT, "\tSteps (CAM) : " ); CS_TIMESTAMP( csCAMstpi ); CS_SPACE; CS_TIMESTAMP( csCAMstpm ); CS_ENDL;
            fprintf( CS_OUT, "\tSynch (CAM) : " ); CS_TIMESTAMP( csCAMinit ); CS_SPACE; CS_TIMESTAMP( csCAMmain ); CS_SPACE; CS_TIMESTAMP( csCAMdiff ); CS_ENDL;
            fprintf( CS_OUT, "\tRange (IMU) : " ); CS_TIMESTAMP( csIMUinit ); CS_SPACE; CS_TIMESTAMP( csIMUtime ); CS_ENDL;
            fprintf( CS_OUT, "\tSpan  (IMU) : " ); CS_TIMESTAMP( lp_timestamp_diff( csIMUinit, csIMUtime ) ); CS_ENDL;
            fprintf( CS_OUT, "\tSteps (IMU) : " ); CS_TIMESTAMP( csIMUstpi ); CS_SPACE; CS_TIMESTAMP( csIMUstpm ); CS_ENDL;
            fprintf( CS_OUT, "\tRange (GPS) : " ); CS_TIMESTAMP( csGPSinit ); CS_SPACE; CS_TIMESTAMP( csGPStime ); CS_ENDL;
            fprintf( CS_OUT, "\tSpan  (GPS) : " ); CS_TIMESTAMP( lp_timestamp_diff( csCAMinit, csCAMtime ) ); CS_ENDL;
            fprintf( CS_OUT, "\tSteps (GPS) : " ); CS_TIMESTAMP( csGPSstpi ); CS_SPACE; CS_TIMESTAMP( csGPSstpm ); CS_ENDL;

        } else { 

            /* Display message */
            fprintf( CS_OUT, "Auditing : %s\n    Unable access file\n", csFile ); 

        }

    }

/*
    Source - File length extractor
 */

    long cs_audit_filesize( char const * const csFile ) {

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
    Source - Directory entity enumeration
 */

    int cs_audit_enum( char const * const csDirectory, char * const csName ) {

        /* Directory variables */
        static DIR    * csDirect = NULL;
        static DIRENT * csEntity = NULL;

        /* Verify enumeration mode */
        if ( csDirect == NULL ) {

            /* Create directory handle */
            csDirect = opendir( csDirectory );

            /* Return negative enumeration */
            return( CS_FALSE );

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

                /* Copy entity name */
                strcpy( csName, csEntity->d_name );

                /* Return positive enumeration */
                return( CS_TRUE );

            }

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

