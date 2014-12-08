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

    # include "csps-view.h"

/* 
    Source - Global variables (GLUT!)
 */

    cs_View_Position_t csPosition = { 0, 0, 0, 0, 0 };
    cs_View_Keyboard_t csKeyboard = { 0, 0 };
    cs_View_Mouse_t    csMouse    = { 0 };
    cs_View_List_t     csList     = { 0 };
    cs_View_Path_t     csPath     = { "", "mod-DSIDE", "mod-SGNQF", "mod-IFETI", "eyesis4pi", "ls20031", "adis16375" };

/*
    Source - Software main function
 */

    int main ( int argc, char ** argv ) {

        /* GLUT window handle */
        int csWindow = 0;

        /* Search in parameters */
        lc_stdp( lc_stda( argc, argv, "--path"   , "-p" ), argv, csPath.ptRoot, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-mod", "-m" ), argv, csPath.ptCAMm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-mod", "-n" ), argv, csPath.ptGPSm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-mod", "-s" ), argv, csPath.ptIMUm, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--cam-tag", "-c" ), argv, csPath.ptCAMd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--gps-tag", "-g" ), argv, csPath.ptGPSd, LC_STRING );
        lc_stdp( lc_stda( argc, argv, "--imu-tag", "-i" ), argv, csPath.ptIMUd, LC_STRING );

        /* Execution switch */
        if ( lc_stda( argc, argv, "--help", "-h" ) || ( argc <= 1 ) ) {

            /* Display help summary */
            printf( CS_HELP );

        } else {

            /* Initialize opengl and glut */
            glutInit( & argc, argv );

            /* Initialize windows parameters */
            glutInitWindowSize( glutGet( GLUT_SCREEN_WIDTH ), glutGet( GLUT_SCREEN_HEIGHT ) );

            /* Create windows */
            csWindow = glutCreateWindow( "csps-view" );

            /* Assign callback functions */
            glutDisplayFunc ( cs_view_scene          );
            glutReshapeFunc ( cs_view_event_reshape  );
            glutKeyboardFunc( cs_view_event_keyboard );
            glutMouseFunc   ( cs_view_event_mouse    );
            glutMotionFunc  ( cs_view_event_move     );

            /* Assign windows parameters */
            glutFullScreen();

            /* Initialize display mode */
            glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL );

            /* Hide cursor */
            glutSetCursor( GLUT_CURSOR_NONE ); 

            /* Compile scene */
            cs_view_scene_compile( CS_FLAG_CREATE );

            /* Software main loop */
            while ( csKeyboard.kbExit == 0 ) {

                /* Display scene */
                cs_view_scene();

                /* Handle events */
                glutMainLoopEvent();

            }

            /* Delete scene */
            cs_view_scene_compile( CS_FLAG_DELETE );

            /* Distroy window */
            glutDestroyWindow( csWindow );

        }

        /* Return to system */
        return( EXIT_SUCCESS );

    }

