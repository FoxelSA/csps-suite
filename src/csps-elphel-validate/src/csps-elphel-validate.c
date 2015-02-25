/*
 * csps-suite - CSPS library front-end suite
 *
 * Copyright (c) 2013-2015 FOXEL SA - http://foxel.ch
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
        char csSrc[256] = { 0 };
        char csDst[256] = { 0 };
        char csEnt[256] = { 0 };
        char csExp[256] = { 0 };

        /* Validation index variables */
        long csIndex = 1;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--source"     , "-s" ), argv, csSrc, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--destination", "-d" ), argv, csDst, LC_STRING );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Directory entity enumeration */
            while ( lc_file_enum( csSrc, csEnt ) != LC_FALSE ) {

                /* Consider only file entity */
                if ( lc_file_detect( csEnt, LC_FILE ) == LC_TRUE ) {

                    /* Check logs-file tag */
                    if ( strstr( csEnt, LC_PATTERN ) != 0 ) {

                        /* Build validated temporary logs-file path */
                        sprintf( csExp, "%s/log-container.log-%05li", csDst, csIndex ++ );

                        /* Display information */
                        fprintf( LC_OUT, "Validating : %s\n    Exported in %s\n", basename( csEnt ), basename( csExp ) );

                        /* Validation procedure */
                        fprintf( LC_OUT, "    %li sentence(s) discared\n", cs_elphel_validate( csEnt, csExp );

                    }

                }

            }

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

/*
    Source - File validation
*/

    size_t cs_elphel_validate( 

        char const * const csiFile, 
        char const * const csoFile 

    ) {

        /* Records buffer variables */
        lp_Byte_t csBuffer[LC_RECORD] = { 0 };

        /* Returned value variables */
        size_t csDiscared = 0;

        /* File handle variables */
        FILE * csiStream = NULL;
        FILE * csoStream = NULL;

        /* Create and check input stream */
        if ( ( csiStream = fopen( csiFile, "rb" ) ) != NULL ) {

            /* Create and check output stream */
            if ( ( csoStream = fopen( csoFile, "wb" ) ) != NULL ) {

                /* Parsing input stream */
                while ( fread( csBuffer, 1, LC_RECORD, csiStream ) == LC_RECORD ) {

                    /* Record buffer validation */
                    if ( cs_elphel_validate_record( csBuffer ) == LP_TRUE ) {

                        /* Export record buffer */
                        fwrite( csBuffer, 1, LC_RECORD, csoStream );

                    /* Update discared sentences count */
                    } else { csDiscared ++; }

                }

                /* Close output stream */
                fclose( csoStream );

            /* Display message */
            } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( ( char * ) csiFile ) ); }

            /* Close input stream */
            fclose( csiStream );

        /* Display message */
        } else { fprintf( LC_ERR, "Error : unable to access %s\n", basename( ( char * ) csiFile ) ); }

        /* Return discared count */
        return( csDiscared );

    }

/*
    Source - Record validation
*/

    int cs_elphel_validate_record(

        lp_Byte_t const * const csBuffer

    ) {

        /* Failsafe check on record header */
        if ( ( ( * ( ( uint64_t * ) csBuffer ) ) & 0x00000000F0F00000 ) == 0 ) {

            /* Failsafe check on record tail */
            if ( * ( ( uint16_t * ) ( csBuffer + 62 ) ) == 0 ) {

                /* Valide record */
                return( LC_TRUE );

            } else {

                /* Invalide record */
                return( LC_FALSE );

            }

        } else {

            /* Invalide record */
            return( LC_FALSE );

        }

    }

