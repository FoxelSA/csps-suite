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

    /*! \file   csps-view-controls.h
     *  \author Nils Hamel (n.hamel@foxel.ch)
     *   
     *  OpenGLUT callbacks function definition for keyboard and mouse.
     */

/* 
    Header - Include guard
 */

    # ifndef __CS_VIEW_CONTROLS__
    # define __CS_VIEW_CONTROLS__

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
    # include "csps-view-scene.h"

/* 
    Header - Preprocessor definitions
 */

    /* Define control reset flags */
    # define CS_VIEW_CONTROLS_SET   0
    # define CS_VIEW_CONTROLS_RESET 1

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

    /*! \brief Camera position and orientation reset
     *  
     *  This function expect initial camera position and orientation
     *  provided using CS_VIEW_CONTROLS_SET and the parameters. If
     *  flag CS_VIEW_CONTROLS_RESET is sent, the position and orientation
     *  of the camera is reset using data previously provided.
     *  
     *  \param csFlag Usage flag
     *  \param csMeanLon Reset longitude value
     *  \param csMeanLat Reset latitude value
     *  \param csMeanAlt Reset altitude value
     */

    void cs_view_controls_reset ( int csFlag, double csMeanLon, double csMeanLat, double csMeanAlt );

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


