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

    /*! \file   csps-view-event.h
     *  \author Nils Hamel (n.hamel@foxel.ch)
     *   
     *  OpenGLUT callbacks function definition for keyboard and mouse.
     */

/* 
    Header - Include guard
 */

    # ifndef __CS_VIEW_EVENT__
    # define __CS_VIEW_EVENT__

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
    # include "csps-view-controls.h"

/* 
    Header - Preprocessor definitions
 */

    /* Define nominal motion velocities */
    # define CS_VIEW_EVENT_VELOCITY_AXIS 1e-1
    # define CS_VIEW_EVENT_VELOCITY_ALTM 1.0
    # define CS_VIEW_EVENT_VELOCITY_MOVE 0.1

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


    /*! \brief Window reshape callback function
     *  
     *  This function is called as GLUT callback on window
     *  reshape.
     *  
     *  \param width Width of the window
     *  \param height Height of the window
     */

    void cs_view_event_reshape ( int width, int height );

    /*! \brief Keyboard callback functions
     *
     *  This function is called as GLUT callback on keyboard 
     *  events.
     *
     *  \param key Keycode
     *  \param x Mouse x coordinates at keyevent
     *  \param y Mouse y coordinates at keyevent
     */

    void cs_view_event_keyboard ( unsigned char key, int x, int y );

    /*! \brief Mouse button callback functions
     *
     *  This function is called as GLUT callback on mouse 
     *  button events.
     *
     *  \param button Mouse button
     *  \param state Mouse button state
     *  \param x Mouse x coordinates
     *  \param y Mouse y coordinates
     */

    void cs_view_event_mouse ( int button, int state, int x, int y );

    /*! \brief Mouse motion callback functions
     *
     *  This function is called as GLUT callback on mouse 
     *  motion events.
     *
     *  \param x Mouse x coordinates
     *  \param y Mouse y coordinates
     */

    void cs_view_event_move ( int x, int y );

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


