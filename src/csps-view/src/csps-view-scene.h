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
     *  \author Nils Hamel (n.hamel@foxel.ch)
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

    /* Define earth radius */
    # define CS_SCENE_METRE 0.001
    # define CS_SCENE_EARTH 6371.00090

/* 
    Header - Preprocessor macros
 */

    /* Define altitude correction macro */
    # define CS_VIEW_SCENE_CALT( x ) ( ( x * CS_SCENE_METRE ) + CS_SCENE_EARTH )

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

/* 
    Header - Function prototypes
 */

    /*! \brief Scene main function
     *  
     *  Main scene drawing function.
     */

    void cs_view_scene ( void );

    void cs_view_scene_compile( int csFlag );

    void cs_view_scene_earth( GLuint csTag );

    void cs_view_scene_track( GLuint csTag );

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


