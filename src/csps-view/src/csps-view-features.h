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

    /*! \file   csps-view-features.h
     *  \author Nils Hamel (n.hamel@foxel.ch)
     *   
     *  csps-view general features definition.
     */

/* 
    Header - Include guard
 */

    # ifndef __CS_VIEW_FEATURES__
    # define __CS_VIEW_FEATURES__

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/* 
    Header - Includes
 */

    /* Standard includes */
    # include <stdio.h>
    # include <stdlib.h>
    # include <string.h>
    # include <unistd.h>
    # include <math.h>

    /* OpenGL includes */
    # include <GL/gl.h>
    # include <GL/glu.h>
    # include <GL/glut.h>

    /* CSPS includes */
    # include <csps-all.h>

/* 
    Header - Preprocessor definitions
 */

    /* Define execution flag */
    # define CS_FLAG_CREATE 0
    # define CS_FLAG_DELETE 1

    /* Define angle conversion factor */
    # define CS_DEG2RAD     ( 3.1415926536 / 180.0 )

/* 
    Header - Preprocessor macros
 */

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

    typedef struct cs_view_position_struct {

        double psAX;
        double psAY;
        double psLon;
        double psLat;
        double psAlt;

    } cs_view_position;

    typedef struct cs_view_keyboard_struct {

        int   kbExit;
        int   kbKey;
        float kbScale;

    } cs_view_keyboard;

    typedef struct cs_view_mouse_struct {

        int msMode;
        int msX;
        int msY;

    } cs_view_mouse;

    typedef struct cs_view_list_struct {

        GLuint lsEarth;
        GLuint lsCamera;

    } cs_view_list;

    typedef struct cs_view_path_struct {

        char ptRoot[256];
        char ptCAMm[256];
        char ptGPSm[256];
        char ptIMUm[256];
        char ptCAMd[256];
        char ptGPSd[256];
        char ptIMUd[256];

    } cs_view_path;

/* 
    Header - Function prototypes
 */

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


