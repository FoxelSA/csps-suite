/*
 * csps-suite libcommon - csps-suite common library
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

    # include "common-file.h"

/*
    Source - Directory entity enumeration
 */

    int lc_file_enum( char const * const lcDirectory, char * const lcName ) {

        /* Directory variables */
        static DIR           * lcDirect = NULL;
        static struct dirent * lcEntity = NULL;

        /* Verify enumeration mode */
        if ( lcDirect == NULL ) {

            /* Create directory handle */
            lcDirect = opendir( lcDirectory );

            /* Search for first entity */
            return( lc_file_enum( lcDirectory, lcName ) );

        } else {

            /* Read directory entity */
            lcEntity = readdir( lcDirect );

            /* Check enumeration end */
            if ( lcEntity == NULL ) {

                /* Delete directory handle */
                closedir( lcDirect );

                /* Reset directory pointer */
                lcDirect = NULL;

                /* Returns no entity message */
                return( LC_FALSE );

            } else {

                /* Compose directory and entity path */
                sprintf( lcName, "%s/%s", lcDirectory, lcEntity->d_name );

                /* Returns entity message */
                return( LC_TRUE );

            }

        }

    }

/*
    Source - Directory entity type detection
*/

    int lc_file_detect( char const * const lcEntity, int const lcType ) {

        /* Check type of entity to verify */
        if ( lcType == LC_FILE ) {

            /* File openning verification */
            FILE * lcCheck = fopen( lcEntity, "r" );

            /* Check verification stream */
            if ( lcCheck != NULL ) {

                /* Close stream */
                fclose( lcCheck );

                /* Return positive answer */
                return( LC_TRUE );

            } else {

                /* Return negative answer */
                return( LC_FALSE );

            }

        } else if ( lcType == LC_DIRECTORY ) {

            /* Directory handle verification */
            DIR * lcCheck = opendir( lcEntity );

            /* Check verification handle */
            if ( lcCheck != NULL ) {

                /* Delete handle */
                closedir( lcCheck );

                /* Return positive answer */
                return( LC_TRUE );

            } else {

                /* Return negative answer */
                return( LC_FALSE );
            }

        } else {

            /* Return negative answer */
            return( LC_FALSE );

        }

    }

/*
    Source - File entity size detection
 */

    size_t lc_file_size( char const * const lcFile ) {

        /* Returned variables */
        size_t lcSize = 0L;

        /* Ask pointed file handle */
        FILE * lcStream = fopen( lcFile, "rb" );

        /* Check file handle */
        if ( lcStream != NULL ) {

            /* Update file offset */
            fseek( lcStream, 0L, SEEK_END );

            /* Ask value of updated offset */
            lcSize = ftell( lcStream );

            /* Close file handle */
            fclose( lcStream );            

        }

        /* Return file size */
        return( lcSize );

    }

