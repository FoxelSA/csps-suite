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

        /* Record root directory variables */
        char csPath[256] = { 0 };

        /* Record log file path variables */
        char csFile[256] = { 0 };

        /* Enumeration variables */
        DIR    * csDirect = NULL;
        DIRENT * csEntity = NULL;

        /* Date and time variables */
        time_t csTime;

        /* Search in parameters */
        cs_stdp( cs_stda( argc, argv,  "--path", "-p" ), argv, csPath , CS_STRING );

        /* Execution switch */
        if ( cs_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {
            
            /* Display message */
            time( & csTime ); fprintf( CS_OUT, "Audit performed using csps-audit on %s", ctime( & csTime ) );
        
            /* Display message */
            fprintf( CS_OUT, "Course %s\nMaster directory audit :\n", csPath );

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
            } else { fprintf( CS_OUT, "Error : Unable access master directory %s.\n", csPath ); }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Logs audit coprocess
 */

    void cs_audit_audit( const char * const csFile ) {

        /* Timestamp analysis variables */
        lp_Time_t csDEVstep = lp_Time_s( 0 );
        lp_Time_t csIMUtime = lp_Time_s( 0 );
        lp_Time_t csIMUinit = lp_Time_s( 0 );
        lp_Time_t csIMUprev = lp_Time_s( 0 );
        lp_Time_t csIMUstpi = lp_timestamp_compose( lp_Time_s( 1000000000 ), lp_Time_s( 0 ) );
        lp_Time_t csIMUstpm = lp_Time_s( 0 );
        lp_Time_t csCAMtime = lp_Time_s( 0 );
        lp_Time_t csCAMinit = lp_Time_s( 0 );
        lp_Time_t csCAMmain = lp_Time_s( 0 );
        lp_Time_t csCAMdiff = lp_Time_s( 0 );
        lp_Time_t csCAMprev = lp_Time_s( 0 );
        lp_Time_t csCAMstpi = lp_timestamp_compose( lp_Time_s( 1000000000 ), lp_Time_s( 0 ) );
        lp_Time_t csCAMstpm = lp_Time_s( 0 );
        lp_Time_t csGPStime = lp_Time_s( 0 );
        lp_Time_t csGPSinit = lp_Time_s( 0 );
        lp_Time_t csGPSprev = lp_Time_s( 0 );
        lp_Time_t csGPSstpi = lp_timestamp_compose( lp_Time_s( 1000000000 ), lp_Time_s( 0 ) );
        lp_Time_t csGPSstpm = lp_Time_s( 0 );

        /* Records buffer variables */
        unsigned char csRec[LP_DEVICE_EYESIS4PI_RECLEN] = { 0 };

        /* Handle variables */
        FILE * csHandle = NULL;

        /* File size variables */
        long csSize = 0;

        /* Query file size */
        if ( ( csSize = cs_audit_filesize( csFile ) ) != 0 ) {

            /* Display message */
            fprintf( CS_OUT, "Auditing : %s\n", csFile ); 

            /* Display message */
            fprintf( CS_OUT, "    File size     : +%li Bytes (%f Mo)\n", csSize, ( double ) csSize / 1048576.0 );

            /* Display information */
            fprintf( CS_OUT, "    Congruence-64 : +%li ", csSize % 64 );

            /* Display message */
            if ( ( csSize % 64 ) == 0 ) {

                /* Display additionnal message */
                fprintf( CS_OUT, "(Valid)\n" ); 

            } else  {

                /* Display additionnal message */
                fprintf( CS_OUT, "(Invalid : missing %li bytes to last record\n", 64 - ( csSize % 64 ) );

            }

            /* Display information */
            fprintf( CS_OUT, "    Records count : +%li\n", csSize >> 6 );

            /* Create file handle */
            csHandle = fopen( csFile, "rb" );

            /* Read log records */
            while ( fread( csRec, 1, LP_DEVICE_EYESIS4PI_RECLEN, csHandle ) == LP_DEVICE_EYESIS4PI_RECLEN ) {

                /* IMU record filter */
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

                /* CAM record filter */
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

                /* GPS record filter */
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

            /* Display information */
            fprintf( CS_OUT, "    IMU range     : +%010" lp_Time_p ".%06" lp_Time_p "s to +%010" lp_Time_p ".%06" lp_Time_p "s\n", 

                lp_timestamp_sec ( csIMUinit ), 
                lp_timestamp_usec( csIMUinit ),
                lp_timestamp_sec ( csIMUtime ), 
                lp_timestamp_usec( csIMUtime ) 

            );

            /* Display information */
            fprintf( CS_OUT, "    IMU duration  : +%010" lp_Time_p ".%06" lp_Time_p "s (~ %" lp_Time_p " minutes)\n", 

                lp_timestamp_sec ( lp_timestamp_diff( csIMUinit, csIMUtime ) ), 
                lp_timestamp_usec( lp_timestamp_diff( csIMUinit, csIMUtime ) ),
                lp_timestamp_sec ( lp_timestamp_diff( csIMUinit, csIMUtime ) ) / 60

            );

            /* Display information */
            fprintf( CS_OUT, "    IMU steps     : +%010" lp_Time_p ".%06" lp_Time_p "s   +%010" lp_Time_p ".%06" lp_Time_p "s\n",

                lp_timestamp_sec ( csIMUstpi ), 
                lp_timestamp_usec( csIMUstpi ),
                lp_timestamp_sec ( csIMUstpm ), 
                lp_timestamp_usec( csIMUstpm )

            );

            /* Display information */
            fprintf( CS_OUT, "    CAM range     : +%010" lp_Time_p ".%06" lp_Time_p "s to +%010" lp_Time_p ".%06" lp_Time_p "s\n", 

                lp_timestamp_sec ( csCAMinit ), 
                lp_timestamp_usec( csCAMinit ),
                lp_timestamp_sec ( csCAMtime ), 
                lp_timestamp_usec( csCAMtime ) 

            );

            /* Display information */
            fprintf( CS_OUT, "    CAM duration  : +%010" lp_Time_p ".%06" lp_Time_p "s (~ %" lp_Time_p " minutes)\n", 

                lp_timestamp_sec ( lp_timestamp_diff( csCAMinit, csCAMtime ) ), 
                lp_timestamp_usec( lp_timestamp_diff( csCAMinit, csCAMtime ) ),
                lp_timestamp_sec ( lp_timestamp_diff( csCAMinit, csCAMtime ) ) / 60

            );

            /* Display information */
            fprintf( CS_OUT, "    CAM steps     : +%010" lp_Time_p ".%06" lp_Time_p "s    +%010" lp_Time_p ".%06" lp_Time_p "s\n",

                lp_timestamp_sec ( csCAMstpi ), 
                lp_timestamp_usec( csCAMstpi ),
                lp_timestamp_sec ( csCAMstpm ), 
                lp_timestamp_usec( csCAMstpm )

            );

            /* Display information */
            fprintf( CS_OUT, "    CAM async.    : +%010" lp_Time_p ".%06" lp_Time_p "s to +%010" lp_Time_p ".%06" lp_Time_p "s\n", 

                lp_timestamp_sec ( csCAMinit ), 
                lp_timestamp_usec( csCAMinit ),
                lp_timestamp_sec ( csCAMmain ), 
                lp_timestamp_usec( csCAMmain ) 

            );

            /* Display information */
            fprintf( CS_OUT, "    CAM async.    : " );

            /* Verify asynchronous value between event-logger and camera record timestamps */
            if ( lp_timestamp_ge( csCAMinit, csCAMmain ) == LP_TRUE ) fprintf( CS_OUT, "-" ); else fprintf( CS_OUT, "-" );

            /* Display information */
            fprintf( CS_OUT, "%010" lp_Time_p ".%06" lp_Time_p "s\n", 

                lp_timestamp_sec ( csCAMdiff ), 
                lp_timestamp_usec( csCAMdiff )

            );

            /* Display information */
            fprintf( CS_OUT, "    GPS range     : +%010" lp_Time_p ".%06" lp_Time_p "s to +%010" lp_Time_p ".%06" lp_Time_p "s\n", 

                lp_timestamp_sec ( csGPSinit ), 
                lp_timestamp_usec( csGPSinit ),
                lp_timestamp_sec ( csGPStime ), 
                lp_timestamp_usec( csGPStime ) 

            );

            /* Display information */
            fprintf( CS_OUT, "    GPS duration  : +%010" lp_Time_p ".%06" lp_Time_p "s (~ %" lp_Time_p " minutes)\n", 

                lp_timestamp_sec ( lp_timestamp_diff( csGPSinit, csGPStime ) ), 
                lp_timestamp_usec( lp_timestamp_diff( csGPSinit, csGPStime ) ),
                lp_timestamp_sec ( lp_timestamp_diff( csGPSinit, csGPStime ) ) / 60

            );

            /* Display information */
            fprintf( CS_OUT, "    GPS steps     : +%010" lp_Time_p ".%06" lp_Time_p "s    +%010" lp_Time_p ".%06" lp_Time_p "s\n",

                lp_timestamp_sec ( csGPSstpi ), 
                lp_timestamp_usec( csGPSstpi ),
                lp_timestamp_sec ( csGPSstpm ), 
                lp_timestamp_usec( csGPSstpm )

            );

        } else { 

            /* Display message */
            fprintf( CS_OUT, "Auditing : %s\n    Unable access file\n", csFile ); 

        }

    }

/*
    Source - File size extractor
 */

    long cs_audit_filesize( const char * const csFile ) {

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

