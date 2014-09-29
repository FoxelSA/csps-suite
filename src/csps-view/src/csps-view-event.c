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

    # include "csps-view-event.h"

/* 
    Source - General variables (GLUT only)
 */

    extern cs_view_position csPosition;
    extern cs_view_keyboard csKeyboard;
    extern cs_view_mouse    csMouse;
    extern cs_view_list     csList;
    extern cs_view_path     csPath;

/*
    Source - Window reshape callback function
 */

    void cs_view_event_reshape( int width, int height ) {

        /* Update viewport */
        glViewport( 0, 0, width, height );

        /* Update matrix mode */
        glMatrixMode( GL_PROJECTION );

        /* Reset matrix */
        glLoadIdentity();

        /* Compute projection matrix */
        gluPerspective( 45, ( float ) width / ( float ) height, 1.0, 10000.0 );

    }

/*
    Source - Keyboard callback functions
 */

    void cs_view_event_keyboard( unsigned char key, int x, int y ) {

        /* Update keyborad structure */
        csKeyboard.kbKey = key;

        /* Switch on key */
        switch ( key ) {

            case ( 27 ) : {

                /* Leave software */
                csKeyboard.kbExit = 1;

            } break;

            case ( 'r' ) : {

                /* Reset position */
                cs_view_controls_reset( CS_VIEW_CONTROLS_RESET, 0.0, 0.0, 0.0 );

            } break;

            case ( 'v' ) : {

                /* Change velocity */
                csPosition.psVel = ( csPosition.psVel < 2.0 ) ? 5.0 : 1.0;

            } break;

        };

    }

/*
    Source - Mouse callback functions
*/


    void cs_view_event_mouse( int button, int state, int x, int y ) {

        /* Mouse mode selection */
        if ( ( button == GLUT_LEFT_BUTTON ) && ( state == GLUT_DOWN ) ) {

            /* Memorize pointer position */
            csMouse.msX = x;
            csMouse.msY = y;

            /* Update mouse mode */
            csMouse.msMode = 1;

        } else if ( ( button == GLUT_RIGHT_BUTTON ) && ( state == GLUT_DOWN ) ) {

            /* Memorize pointer position */
            csMouse.msX = x;
            csMouse.msY = y;

            /* Update mouse mode */
            csMouse.msMode = 2;

        } else if ( ( button == GLUT_MIDDLE_BUTTON ) && ( state == GLUT_DOWN ) ) {

            /* Memorize pointer position */
            csMouse.msX = x;
            csMouse.msY = y;

            /* Update mouse mode */
            csMouse.msMode = 3;

        } else if ( ( button == 3 ) || ( button == 4 ) )  {

            /* Update position */
            csPosition.psAlt -= csPosition.psVel * ( ( button == 3 ) ? - CS_VIEW_EVENT_VELOCITY_ALTM : + CS_VIEW_EVENT_VELOCITY_ALTM );

        } else if ( state == GLUT_UP ) {

            /* Clear mode */
            csMouse.msMode = 0;

        }

    }

    void cs_view_event_move( int x, int y ) {

        /* Mouse mode selection */
        if ( csMouse.msMode == 1 ) {

            /* Angular modification */
            csPosition.psAX += CS_VIEW_EVENT_VELOCITY_AXIS * ( y - csMouse.msY );
            csPosition.psAY += CS_VIEW_EVENT_VELOCITY_AXIS * ( x - csMouse.msX );

            /* Pointer memorization */
            csMouse.msX = x;
            csMouse.msY = y;

        } else if ( csMouse.msMode == 2 ) {

            /* Update longitude and latitude */
            csPosition.psLon -= csPosition.psVel * CS_VIEW_EVENT_VELOCITY_MOVE * sin( csPosition.psAY * CS_DEG2RAD ) * ( y - csMouse.msY ) * cos( csPosition.psAX * CS_DEG2RAD );
            csPosition.psAlt += csPosition.psVel * CS_VIEW_EVENT_VELOCITY_MOVE * sin( csPosition.psAX * CS_DEG2RAD ) * ( y - csMouse.msY );
            csPosition.psLat += csPosition.psVel * CS_VIEW_EVENT_VELOCITY_MOVE * cos( csPosition.psAY * CS_DEG2RAD ) * ( y - csMouse.msY ) * cos( csPosition.psAX * CS_DEG2RAD );

            /* Pointer memorization */
            csMouse.msX = x;
            csMouse.msY = y;

        } else if ( csMouse.msMode == 3 ) {

            /* Update longitude and latitude */
            csPosition.psLon += csPosition.psVel * CS_VIEW_EVENT_VELOCITY_MOVE * cos( csPosition.psAY * CS_DEG2RAD ) * ( x - csMouse.msX ) * cos( csPosition.psAX * CS_DEG2RAD );
            csPosition.psLat += csPosition.psVel * CS_VIEW_EVENT_VELOCITY_MOVE * sin( csPosition.psAY * CS_DEG2RAD ) * ( x - csMouse.msX ) * cos( csPosition.psAX * CS_DEG2RAD );

            /* Pointer memorization */
            csMouse.msX = x;
            csMouse.msY = y;

        }

    }

