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

    # include "csps-view-scene.h"

/* 
    Source - General variables (GLUT only)
 */

    extern cs_view_position csPosition;
    extern cs_view_keyboard csKeyboard;
    extern cs_view_mouse    csMouse;
    extern cs_view_list     csList;
    extern cs_view_path     csPath;

/*
    Source - Scene main function
 */

    void cs_view_scene( void ) {

        /* Clear buffers */
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        /* Update matrix mode */
        glMatrixMode( GL_MODELVIEW );

        /* Reset matrix */
        glLoadIdentity();

        /* Display list - Earth */
        glPushMatrix(); {

            /* Transfromation - Rotation */
            glRotatef( +csPosition.psAX, 1.0, 0.0, 0.0 );
            glRotatef( +csPosition.psAY, 0.0, 1.0, 0.0 );

            /* Transfromation - Translation */
            glTranslatef( 0, - CS_SCENE_EARTH - csPosition.psAlt, 0 );

            /* Transfromation - Rotation */
            glRotatef( -csPosition.psLat, 1.0, 0.0, 0.0 );
            glRotatef( -csPosition.psLon, 0.0, 1.0, 0.0 );

            glCallList( csList.lsEarth );

        } glPopMatrix();
        
        /* Display list - Track */
        glPushMatrix(); {

            /* Transfromation - Rotation */
            glRotatef( +csPosition.psAX, 1.0, 0.0, 0.0 );
            glRotatef( +csPosition.psAY, 0.0, 1.0, 0.0 );

            /* Transfromation - Translation */
            glTranslatef( 0, - CS_SCENE_EARTH - csPosition.psAlt, 0 );

            /* Transfromation - Rotation */
            glRotatef( -csPosition.psLat, 1.0, 0.0, 0.0 );
            glRotatef( -csPosition.psLon, 0.0, 1.0, 0.0 );

            glCallList( csList.lsCamera );

        } glPopMatrix();

        /* Force primitives draw */
        glFlush();

        /* Wait for primitives draw */
        glFinish();

    }

/*
    Source - Scene display lists
 */

    void cs_view_scene_compile( int csFlag ) {

        /* Verify exectution flag */
        if ( csFlag == CS_FLAG_CREATE ) {

            /* Enable depth test */
            glEnable( GL_DEPTH_TEST );
            glDepthMask( GL_TRUE );
            glDepthFunc( GL_LEQUAL );
            glDepthRange( 0.0f, 1.0f );

            /* Enable faceculling */
            glEnable( GL_CULL_FACE );
            glCullFace( GL_BACK );

            /* Update clear depth */
            glClearDepth(1.0f);
    
            /* Update clear color */
            glClearColor( 1.0, 1.0, 1.0, 0.0 );

            /* Assign display list index */
            csList.lsEarth  = 1;
            csList.lsCamera = 2;

            /* Compile earth display list */
            cs_view_scene_earth( csList.lsEarth );

            /* Compile camera display list */
            cs_view_scene_track( csList.lsCamera );

        } else {

            /* Delete display list */
            glDeleteLists( csList.lsEarth , 1 );
            glDeleteLists( csList.lsCamera, 1 );

        }

    }

/*
    Source - Scene earth model
 */
    
    void cs_view_scene_earth( GLuint csTag ) {

        /* Parsing variables */
        int csLoopi = 0;
        int csLoopj = 0;

        /* Angle variables */
        double csAnglei = 0.0;
        double csAnglej = 0.0;

        /* Declare display list begining */
        glNewList( csTag, GL_COMPILE ); {

            /* Meridians */
            for ( csLoopi = 0; csLoopi < 360; csLoopi ++ ) {

                /* Compute angle */
                csAnglei = ( csLoopi / 360.0 ) * CS_PI * 2.0;

                /* Detect main and secondary meridians */
                if ( ( csLoopi % 10 ) == 0 ) {

                    /* Update line width */
                    glLineWidth( 2.0 );

                    /* Update color */
                    glColor3f( 0.96, 0.70, 0.00 );

                } else {

                    /* Update line width */
                    glLineWidth( 1.0 );

                    /* Update color */
                    glColor3f( 0.96, 0.96, 0.96 );

                }

                /* Begin line strip */
                glBegin( GL_LINE_STRIP );

                /* Secondary loop on angles */
                for ( csLoopj = 10; csLoopj <= 350; csLoopj ++ ) {

                    /* Compute angle */
                    csAnglej = ( ( csLoopj - 180.0 ) / 180.0 ) * CS_PI * 0.5; 

                    /* Send vertex */
                    glVertex3f(

                        CS_SCENE_EARTH * cos( csAnglei ) * cos( csAnglej ),
                        CS_SCENE_EARTH * sin( csAnglej ),
                        CS_SCENE_EARTH * sin( csAnglei ) * cos( csAnglej )

                    );

                }

                /* End primitive */
                glEnd();

            }

            /* Paralleles */
            for ( csLoopi = 5; csLoopi <= 175; csLoopi ++ ) {

                /* Compute angle */
                csAnglei = ( ( csLoopi - 90.0 ) / 90.0 ) * CS_PI * 0.5;

                /* Detect main and secondary meridians */
                if ( ( csLoopi % 10 ) == 0 ) {

                    /* Update line width */
                    glLineWidth( 2.0 );

                    /* Update color */
                    glColor3f( 0.96, 0.70, 0.00 );

                } else {

                    /* Update line width */
                    glLineWidth( 1.0 );

                    /* Update color */
                    glColor3f( 0.96, 0.96, 0.96 );

                }

                /* Begin line strip */
                glBegin( GL_LINE_STRIP );

                /* Secondary loop on angles */
                for ( csLoopj = 0; csLoopj < 720; csLoopj ++ ) {

                    /* Compute angle */
                    csAnglej = ( csLoopj / 720.0 ) * CS_PI * 2.0; 

                    /* Send vertex */
                    glVertex3f(

                        CS_SCENE_EARTH * cos( csAnglej ) * cos( csAnglei ),
                        CS_SCENE_EARTH * sin( csAnglei ),
                        CS_SCENE_EARTH * sin( csAnglej ) * cos( csAnglei )

                    );

                }

                /* End primitive */
                glEnd();

            }

            /* Transfromation - Rotation */
            glRotatef( 90, 1.0, 0.0, 0.0 );

            /* Update color */
            glColor3f( 0.90, 0.90 ,0.90 );

            /* Display solide sphere */
            glutSolidSphere( CS_SCENE_EARTH - 0.2, 360, 180 );

        /* Declare display list end */
        } glEndList();

    }

/*
    Source - Camera track
 */

    void cs_view_scene_track( GLuint csTag ) {

        /* Parsing variables */
        lp_Size_t csParse = 0;

        /* Stream size variables */
        lp_Size_t csSize = 0;

        /* Stream memory variables */
        lp_Time_t * csCAMsyn = NULL;

        /* Query interface structures variables */
        lp_QueryPosition csQPos;

        /* Ask stream size */
        csSize = lp_stream_size( csPath.ptRoot, LP_DEVICE_TYPE_CAM, csPath.ptCAMd, csPath.ptCAMm );

        /* Read stream */
        csCAMsyn = lp_stream_read( csPath.ptRoot, LP_DEVICE_TYPE_CAM, csPath.ptCAMd, csPath.ptCAMm, LP_STREAM_CPN_SYN, sizeof( lp_Time_t ) * csSize );

        /* Declare display list begining */
        glNewList( csTag, GL_COMPILE ); {

            /* Update color */
            //glColor3f( 0.93, 0.69, 0.0 );
            glColor3f( 1.0, 0.0, 0.0 );

            /* Update line width */
            glLineWidth( 4.0 );

            glBegin( GL_LINE_STRIP ); {

                /* Cartesian coordinates variables */
                double csPX = 0.0, csPY = 0.0, csPZ = 0.0, csCR = 0.0;

                /* Mean position accumulators */
                double csLon = 0.0, csLat = 0.0, csAlt = 0.0, csAcc = 0.0;

                /* Loop on camera records */
                for ( csParse = 0; csParse < csSize; csParse ++ ) {

                    /* Query position by timestamp */
                    csQPos = lp_query_position_by_timestamp( csPath.ptRoot, LP_DEVICE_TYPE_GPS, csPath.ptGPSd, csPath.ptGPSm, csCAMsyn[csParse] );

                    /* Check query results */
                    if ( csQPos.qrStatus == LP_TRUE ) {

                        /* Compute corrected altitude */
                        csCR = csQPos.qrAltitude * 0.001 + CS_SCENE_EARTH;

                        /* Compute cartesian coordinates */
                        csPZ = + csCR * cos( csQPos.qrLongitude * CS_DEG2RAD ) * sin( csQPos.qrLatitude * CS_DEG2RAD );
                        csPX = + csCR * sin( csQPos.qrLongitude * CS_DEG2RAD ) * sin( csQPos.qrLatitude * CS_DEG2RAD );
                        csPY = + csCR * cos( csQPos.qrLatitude  * CS_DEG2RAD );

                        /* Send position vertex */
                        glVertex3f( csPX, csPY, csPZ );

                        /* Accumulates position */
                        csLon += csQPos.qrLongitude;
                        csLat += csQPos.qrLatitude;
                        csAlt += csQPos.qrAltitude;
                        csAcc += 1.0;

                    }

                }

                /* Assign initial position */
                csPosition.psLon = ( csLon / csAcc );
                csPosition.psLat = ( csLat / csAcc );
                csPosition.psAlt = ( csAlt / csAcc ) * 0.001 + 1.0;

                /* Assign initial orientation */
                csPosition.psAX = 90.0;
                csPosition.psAY =  0.0;

            } glEnd();

        /* Declare display list end */
        } glEndList();

        /* Unallocate stream memory */
        csCAMsyn = lp_stream_delete( csCAMsyn );

    }

