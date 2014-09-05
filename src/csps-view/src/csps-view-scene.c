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

            /* Update clear color */
            glClearColor( 1.0, 1.0, 1.0, 0.0 );

            /* Enable depth test */
            glEnable( GL_DEPTH_TEST );
            glDepthFunc( GL_LEQUAL );
            glDepthMask( GL_TRUE );
            glClearDepth( 1.0 );

            /* Enable faceculling */
            glEnable( GL_CULL_FACE );
            glCullFace( GL_BACK );

            glShadeModel( GL_SMOOTH );

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
                    glColor3f( 0.92941, 0.69412, 0.0 );

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
                    glVertex3d(

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
                    glColor3f( 0.92941, 0.69412, 0.0 );

                } else {

                    /* Update line width */
                    glLineWidth( 1.0 );

                    /* Update color */
                    glColor3f( 0.98, 0.98, 0.98 );

                }

                /* Begin line strip */
                glBegin( GL_LINE_STRIP );

                /* Secondary loop on angles */
                for ( csLoopj = 0; csLoopj < 720; csLoopj ++ ) {

                    /* Compute angle */
                    csAnglej = ( csLoopj / 720.0 ) * CS_PI * 2.0; 

                    /* Send vertex */
                    glVertex3d(

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
            glColor3f( 0.88, 0.88 ,0.88 );

            /* Display solide sphere */
            glutSolidSphere( CS_SCENE_EARTH - CS_SCENE_METRE * 200.0, 360, 180 );

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
        lp_QueryPosition    csQPos;
        lp_QueryOrientation csQOri;

        /* Ask stream size */
        csSize = lp_stream_size( csPath.ptRoot, LP_DEVICE_TYPE_CAM, csPath.ptCAMd, csPath.ptCAMm );

        /* Read stream */
        csCAMsyn = lp_stream_read( csPath.ptRoot, LP_DEVICE_TYPE_CAM, csPath.ptCAMd, csPath.ptCAMm, LP_STREAM_CPN_SYN, sizeof( lp_Time_t ) * csSize );

        /* Declare display list begining */
        glNewList( csTag, GL_COMPILE ); {

            /* Update line width */
            glLineWidth( 1.0 );

            /* Begin primitive */
            glBegin( GL_LINES ); {

                /* Cartesian coordinates variables */
                double csPX = 0.0, csPY = 0.0, csPZ = 0.0;

                /* Cartesian coordinates memory variables */
                double csMX = 0.0, csMY = 0.0, csMZ = 0.0, csMF = 0.0;

                /* Cartesian coordinates frame variables */
                double csXX = 0.0, csXY = 0.0, csXZ = 0.0;
                double csYX = 0.0, csYY = 0.0, csYZ = 0.0;
                double csZX = 0.0, csZY = 0.0, csZZ = 0.0;

                /* Cartesian coordinates frame memory variables */
                double csMXX = 0.0, csMXY = 0.0, csMXZ = 0.0;
                double csMYX = 0.0, csMYY = 0.0, csMYZ = 0.0;
                double csMZX = 0.0, csMZY = 0.0, csMZZ = 0.0, csFF = 0.0;

                /* Mean position accumulators variables */
                double csLon = 0.0, csLat = 0.0, csAlt = 0.0, csAcc = 0.0;

                /* Spherical coordinate system variables */
                double csFrm[3][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } };

                /* Loop on camera records */
                for ( csParse = 0; csParse < csSize; csParse ++ ) {

                    /* Query position by timestamp */
                    csQPos = lp_query_position_by_timestamp( csPath.ptRoot, LP_DEVICE_TYPE_GPS, csPath.ptGPSd, csPath.ptGPSm, csCAMsyn[csParse] );

                    /* Check query results */
                    if ( csQPos.qrStatus == LP_TRUE ) {

                        /* Compute cartesian coordinates */
                        csPZ = + CS_VIEW_SCENE_CALT( csQPos.qrAltitude ) * cos( csQPos.qrLongitude * CS_DEG2RAD ) * sin( csQPos.qrLatitude * CS_DEG2RAD );
                        csPX = + CS_VIEW_SCENE_CALT( csQPos.qrAltitude ) * sin( csQPos.qrLongitude * CS_DEG2RAD ) * sin( csQPos.qrLatitude * CS_DEG2RAD );
                        csPY = + CS_VIEW_SCENE_CALT( csQPos.qrAltitude ) * cos( csQPos.qrLatitude  * CS_DEG2RAD );

                        /* Verify previous point memory */
                        if ( csMF > 0.5 ) {

                            /* Update color */
                            glColor3f( 0.92941, 0.69412, 0.0 );

                            /* Send position vertex */
                            glVertex3d( csMX, csMY, csMZ );
                            glVertex3d( csPX, csPY, csPZ );

                        }

                        /* Memorize position */
                        csMX = csPX; csMY = csPY; csMZ = csPZ; csMF = 1.0;

                        /* Accumulates position */
                        csLon += csQPos.qrLongitude;
                        csLat += csQPos.qrLatitude;
                        csAlt += csQPos.qrAltitude;

                        /* Update index */
                        csAcc += 1.0;

                    }

                    /* Query orientation by timestamp */
                    csQOri = lp_query_orientation_by_timestamp( csPath.ptRoot, LP_DEVICE_TYPE_IMU, csPath.ptIMUd, csPath.ptIMUm, csCAMsyn[csParse] );

                    /* Check query results */
                    if ( csQOri.qrStatus == LP_TRUE ) {

                        /* Define local spherical frame - longitudinal-vector */
                        csFrm[0][0] = + CS_SCENE_METRE * 10.0 * cos( csQPos.qrLongitude * CS_DEG2RAD );
                        csFrm[0][1] = + 0.0;
                        csFrm[0][2] = + CS_SCENE_METRE * 10.0 * sin( csQPos.qrLongitude * CS_DEG2RAD );

                        /* Define local spherical frame - latitudinal-vector */
                        csFrm[1][0] = + CS_SCENE_METRE * 10.0 * cos( csQPos.qrLatitude * CS_DEG2RAD ) * sin( csQPos.qrLongitude * CS_DEG2RAD );
                        csFrm[1][1] = + CS_SCENE_METRE * 10.0 * sin( csQPos.qrLatitude * CS_DEG2RAD );
                        csFrm[1][2] = - CS_SCENE_METRE * 10.0 * cos( csQPos.qrLatitude * CS_DEG2RAD ) * cos( csQPos.qrLongitude * CS_DEG2RAD );

                        /* Define local spherical frame - radial-vector */
                        csFrm[2][0] = + CS_SCENE_METRE * 10.0 * sin( csQPos.qrLatitude * CS_DEG2RAD ) * sin( csQPos.qrLongitude * CS_DEG2RAD );
                        csFrm[2][1] = + CS_SCENE_METRE * 10.0 * cos( csQPos.qrLatitude * CS_DEG2RAD );
                        csFrm[2][2] = + CS_SCENE_METRE * 10.0 * sin( csQPos.qrLatitude * CS_DEG2RAD ) * cos( csQPos.qrLongitude * CS_DEG2RAD );

                        /* Compute frame vectors - x-vector */
                        csXX = csPX + csQOri.qrfxx * csFrm[0][0] + csQOri.qrfxy * csFrm[1][0] + csQOri.qrfxz * csFrm[2][0];
                        csXY = csPY + csQOri.qrfxx * csFrm[0][1] + csQOri.qrfxy * csFrm[1][1] + csQOri.qrfxz * csFrm[2][1];
                        csXZ = csPZ + csQOri.qrfxx * csFrm[0][2] + csQOri.qrfxy * csFrm[1][2] + csQOri.qrfxz * csFrm[2][2];

                        /* Compute frame vectors - y-vector */
                        csYX = csPX + csQOri.qrfyx * csFrm[0][0] + csQOri.qrfyy * csFrm[1][0] + csQOri.qrfyz * csFrm[2][0];
                        csYY = csPY + csQOri.qrfyx * csFrm[0][1] + csQOri.qrfyy * csFrm[1][1] + csQOri.qrfyz * csFrm[2][1];
                        csYZ = csPZ + csQOri.qrfyx * csFrm[0][2] + csQOri.qrfyy * csFrm[1][2] + csQOri.qrfyz * csFrm[2][2];

                        /* Compute frame vectors - z-vector */
                        csZX = csPX + csQOri.qrfzx * csFrm[0][0] + csQOri.qrfzy * csFrm[1][0] + csQOri.qrfzz * csFrm[2][0];
                        csZY = csPY + csQOri.qrfzx * csFrm[0][1] + csQOri.qrfzy * csFrm[1][1] + csQOri.qrfzz * csFrm[2][1];
                        csZZ = csPZ + csQOri.qrfzx * csFrm[0][2] + csQOri.qrfzy * csFrm[1][2] + csQOri.qrfzz * csFrm[2][2];

                        /* Update color */
                        glColor3f( 0.70, 0.20, 0.10 );

                        /* Display body x-vector */
                        glVertex3d( csPX, csPY, csPZ ); 
                        glVertex3d( csXX, csXY, csXZ );

                        /* Update color */
                        glColor3f( 0.20, 0.50, 0.30 );

                        /* Display body y-vector */
                        glVertex3d( csPX, csPY, csPZ ); 
                        glVertex3d( csYX, csYY, csYZ );

                        /* Update color */
                        glColor3f( 0.20, 0.30, 0.60 );

                        /* Display body z-vector */
                        glVertex3d( csPX, csPY, csPZ ); 
                        glVertex3d( csZX, csZY, csZZ );

                        /* Verify previous point memory */
                        if ( csFF > 0.5 ) {

                            /* Update color */
                            glColor3f( 0.70, 0.20, 0.10 );

                            /* Display body x-vector */
                            glVertex3d( csMXX, csMXY, csMXZ );
                            glVertex3d( csXX , csXY , csXZ  );

                            /* Update color */
                            glColor3f( 0.20, 0.50, 0.30 );

                            /* Display body y-vector */
                            glVertex3d( csMYX, csMYY, csMYZ );
                            glVertex3d( csYX , csYY , csYZ  );

                            /* Update color */
                            glColor3f( 0.20, 0.30, 0.60 );

                            /* Display body z-vector */
                            glVertex3d( csMZX, csMZY, csMZZ );
                            glVertex3d( csZX , csZY , csZZ  );

                        }

                        /* Memorize frame */
                        csMXX = csXX; csMXY = csXY; csMXZ = csXZ;
                        csMYX = csYX; csMYY = csYY; csMYZ = csYZ;
                        csMZX = csZX; csMZY = csZY; csMZZ = csZZ; csFF = 1.0;

                    }

                }

                /* Reset initial position and assign initial means */
                cs_view_controls_reset( CS_VIEW_CONTROLS_SET, ( csLon / csAcc ), ( csLat / csAcc ), ( csAlt / csAcc ) );

            /* End primitive */
            } glEnd();

        /* Declare display list end */
        } glEndList();

        /* Unallocate stream memory */
        csCAMsyn = lp_stream_delete( csCAMsyn );

    }

