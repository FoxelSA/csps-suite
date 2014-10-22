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

    /*! \file   csps-view-scene.h
     *  \author Nils Hamel <n.hamel@foxel.ch>
     *   
     *  OpenGL scene drawing.
     */

/* 
    Header - Include guard
 */

    # ifndef __CS_VIEW_SCENE__
    # define __CS_VIEW_SCENE__

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/* 
    Header - Includes
 */

    # include "csps-view-features.h"
    # include "csps-view-controls.h"
    # include "csps-view-event.h"

/* 
    Header - Preprocessor definitions
 */

    /* Define geoposition to meter factor */
    # define CS_VIEW_SCENE_RAD2METER    111134.093193

    /* Define frame size */
    # define CS_VIEW_SCENE_FRAME        2.0

    /* Define box factor */
    # define CS_VIEW_SCENE_BOX          50.0

    /* Define gravity indicator depth */
    # define CS_VIEW_SCENE_GRAVITY      10.0

/* 
    Header - Preprocessor macros
 */

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

/* 
    Header - Function prototypes
 */

    /*! \brief Scene rendering function
     *  
     *  This function is called in software main infinite loop for entire scene
     *  rendering.
     */

    void cs_view_scene ( void );

    /*! \brief Scene creation/deletion function
     *  
     *  This function create OpenGL display lists at startup that contains the 
     *  parts of the entire scene rendered by the software.
     *
     *  If CS_FLAG_CREATE is provided as csFlag, the function create the display
     *  lists. Otherwise, the display lists are deleted.
     *  
     *  \param csFlag Creation/deletion flag
     */

    void cs_view_scene_compile ( int const csFlag );

    /*! \brief Specific scene compilation function
     *  
     *  This function creates a display list that contains positions and
     *  orientations of the camera and the flat earth model frame.
     *  
     *  \param csTag OpenGL display list index
     */

    void cs_view_scene_track( GLuint const csTag );

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    }
    # endif

/*
    Header - Include guard
 */

    # endif


