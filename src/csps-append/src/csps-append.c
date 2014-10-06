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

    # include "csps-append.h"

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* Descriptors table variables */
        cs_Descriptor csDescriptor[CS_STACK_SIZE];

        /* Structure path variables */
        char csPath[256] = { 0 };

        /* Date and time variables */
        time_t csTime;

        /* Search in parameters */
        cs_stdp( cs_stda( argc, argv,  "--path", "-p" ), argv, csPath , CS_STRING );

        /* Execution switch */
        if ( cs_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Retrieve time */
            time( & csTime );

            /* Display message */
            fprintf( CS_OUT, "Append performed using csps-append on %sCourse : %s\n", ctime( & csTime ), strrchr( csPath, '/' ) + 1 );

            /* Appending procedure */
            cs_append_append( csPath, csDescriptor, cs_append_create( csPath, csDescriptor ) );

            /* Display message */
            fprintf( CS_OUT, "Done!\n" );

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - Raw logs analysis
*/

    int cs_append_create( char * csPath, cs_Descriptor * csDescriptors ) {

        /* Records buffer variables */
        unsigned char csRec[LP_DEVICE_EYESIS4PI_RECLEN] = { 0 };

        /* Records reading variables */
        int csRead = 0;

        /* Stack index variables */
        int csStack = 0;

        /* Paths variables */
        char csPathDir[256] = { 0 };

        /* Enumeration variables */
        DIR    * csDirect = NULL;
        DIRENT * csEntity = NULL;

        /* Handle variables */
        FILE * csHandle = NULL;

        /* Create specific path */
        sprintf( csPathDir, "%s" CS_PATH_RAW, csPath );

        /* Create directory handle */
        if ( ( csDirect = opendir( csPathDir ) ) != NULL ) {

            /* Display message */
            fprintf( CS_OUT, "Master directory analysis :\n" );

            /* Enumerates directory entities */
            while ( ( ( csEntity = readdir( csDirect ) ) != NULL ) && ( csStack < CS_STACK_SIZE ) ) {

                /* Device file detection */
                if ( ( csEntity->d_type == DT_REG ) && ( strstr( csEntity->d_name, CS_PATH_PATTERN ) != NULL ) ) {

                    /* Create file path */
                    sprintf( csDescriptors[csStack].dsName, "%s/%s", csPathDir, csEntity->d_name );

                    /* Open detected file */
                    if ( ( csHandle = fopen( csDescriptors[csStack].dsName, "rb" ) ) != NULL ) {

                        /* Reset descriptor data */
                        csDescriptors[csStack].dsFlag  = CS_FALSE;
                        csDescriptors[csStack].dsFirst = 0;
                        csDescriptors[csStack].dsLast  = 0;

                        /* Search file initial and final IMU timestamp */
                        while ( ( csRead = fread( csRec, 1, LP_DEVICE_EYESIS4PI_RECLEN, csHandle ) ) == LP_DEVICE_EYESIS4PI_RECLEN ) {

                            /* Record filter */
                            if ( ( csRec[3] & lp_Byte_s( 0x0F ) ) == LP_DEVICE_EYESIS4PI_IMUEVT ) {

                                /* Assign last timestamp */
                                csDescriptors[csStack].dsLast = lp_timestamp( ( lp_Void_t * ) csRec );

                                /* Assign first timestamp */
                                if ( csDescriptors[csStack].dsFirst == 0 ) csDescriptors[csStack].dsFirst = csDescriptors[csStack].dsLast;

                            }

                        }

                        /* Broken file check */
                        fprintf( CS_OUT, "\t%s : %s\n", csRead == 0 ? "Stacking " : "Borken   ", strrchr( csDescriptors[csStack].dsName, '/' ) + 1 );

                        /* Update stack size */
                        csStack ++;

                    }

                }

            }

            /* Delete directory handle */
            closedir( csDirect );

        /* Display message */
        } else { fprintf( CS_OUT, "Error : Unable access master directory %s.\n", csPath ); }

        /* Return descriptor count */
        return( csStack );

    }

/*
    Source - Contigous logs appending
*/

    void cs_append_append( char * csPath, cs_Descriptor * csDescriptors, int csStack ) {

        /* Remaining segment variables */
        int csRemains = csStack;

        /* Paths variables */
        char csPathCSPS[256];
        char csPathCSEG[256];
        char csPathSTRU[256];

        /* Parsing variables */
        int csParse = 0;
        int csSegIx = 0;

        /* Searching variables */
        int csCurrent = 0;

        /* Appending flag variables */
        int csAppended = 0;

        /* Handle variables */
        FILE * csHandle = NULL;

        /* Timestamp variables */
        lp_Time_t csInfemum = 0;
        lp_Time_t csLast = 0;

        /* Display message */
        fprintf( CS_OUT, "Master directory processing :\n" );

        /* Create master structure csps-sub-directory */
        sprintf( csPathCSPS, "%s/csps", csPath ); mkdir( csPathCSPS, 0700 );

        /* Appending procedure */
        while ( csRemains > 0 ) {

            /* Reset searching variables */
            csCurrent = csStack;

            /* Reset infemum buffer */
            csInfemum = lp_Time_s( 0xFFFFFFFFFFFFFFFF );

            /* Search oldest unappended file */
            for ( csParse = 0; csParse < csStack; csParse ++ ) {

                /* Check unappended flag */
                if ( csDescriptors[csParse].dsFlag == CS_FALSE ) {

                    /* Check chronology */
                    if ( lp_timestamp_ge( csInfemum, csDescriptors[csParse].dsLast ) == LP_TRUE ) {

                        /* Update infemum buffer */
                        csInfemum = csDescriptors[csParse].dsLast;

                        /* Stores corresponding index */
                        csCurrent = csParse;

                    }

                }

            }

            /* Create current segment directory */
            sprintf( csPathCSEG, "%s/" CS_PATH_SEGMENT "-%02i", csPathCSPS, ++ csSegIx ); mkdir( csPathCSEG, 0700 );

            /* Create sub-structure directories */
            sprintf( csPathSTRU, "%s/" CS_PATH_STREAMS, csPathCSEG ); mkdir( csPathSTRU, 0700 );
            sprintf( csPathSTRU, "%s/" CS_PATH_DEVICES, csPathCSEG ); mkdir( csPathSTRU, 0700 );
            sprintf( csPathSTRU, "%s/" CS_PATH_EYESIS4, csPathSTRU ); mkdir( csPathSTRU, 0700 );

            /* Create sub-structure devices concatenated file  */
            sprintf( csPathSTRU, "%s/" CS_PATH_LOGFILE, csPathSTRU );

            /* Open concatenated file */
            if ( ( csHandle = fopen( csPathSTRU, "wb" ) ) != NULL ) {

                /* Display message */
                fprintf( CS_OUT, "\tAppending : %s ", strrchr( csDescriptors[csCurrent].dsName, '/' ) + 1 );

                /* Append initial descriptor */
                csLast = cs_append_push( csDescriptors + csCurrent, csHandle );

                /* Update remaining counter */
                csRemains --;

                /* Search contigous descriptors */
                do {

                    /* Reset search variable */
                    csParse = 0;

                    /* Reset appended flag */
                    csAppended = CS_FALSE;

                    /* Search for contigous descriptor */
                    while ( ( csParse < csStack ) && ( csAppended == CS_FALSE ) ) {

                        /* Check unappended flag */
                        if ( csDescriptors[csParse].dsFlag == CS_FALSE ) {

                            /* Compute timestamp distance */
                            if ( lp_timestamp_float( lp_timestamp_diff( csLast, csDescriptors[csParse].dsFirst ) ) < 1.0 ) {

                                /* Display message */
                                fprintf( CS_OUT, "%s ", strrchr( csDescriptors[csParse].dsName, '/' ) + 1 );

                                /* Append current descriptor */
                                csLast = cs_append_push( csDescriptors + csParse, csHandle );

                                /* Update appended flag */
                                csAppended = CS_TRUE;

                                /* Update remaining counter */
                                csRemains --;

                            }

                        }

                        /* Update search index */
                        csParse ++;

                    }

                } while ( csAppended > 0 );

                /* Display message */
                fprintf( CS_OUT, "\n" );

                /* Close appended file */
                fclose( csHandle );

            /* Display message */
            } else { fprintf( CS_OUT, "Error : Unable to create concatenated file %s.\n", csPathSTRU ); }

        }

    }

/*
    Source - Appending coprocess
*/

    lp_Time_t cs_append_push( cs_Descriptor * csDescriptor, FILE * csHandle ) {

        /* Buffer variables */
        unsigned char csBuffer[CS_BUFFER_SIZE] = { 0 };

        /* Transfert variables */
        int csRead = 0;

        /* Open input file */
        FILE * csInput = fopen( csDescriptor->dsName, "rb" );

        /* Update descriptor flag */
        csDescriptor->dsFlag = CS_TRUE;

        /* Transfert content */
        while ( ( csRead = fread( csBuffer, 1, CS_BUFFER_SIZE, csInput ) ) > 0 ) fwrite( csBuffer, 1, csRead, csHandle );

        /* Close input file */
        fclose( csInput );

        /* Return input file last timestamp */
        return( csDescriptor->dsLast );

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

