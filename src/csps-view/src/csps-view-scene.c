/*
 * csps-suite - CSPS library front-end suite
 *
 * Copyright (c) 2013-2015 FOXEL SA - http://foxel.ch
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
    Source - Global variables (GLUT!)
 */

    extern cs_View_Position_t csPosition;
    extern cs_View_Keyboard_t csKeyboard;
    extern cs_View_Mouse_t    csMouse;
    extern cs_View_List_t     csList;
    extern cs_View_Path_t     csPath;

/*
    Source - Scene rendering function
 */

    void cs_view_scene( void ) {

        /* Clear buffers */
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        /* Update matrix mode */
        glMatrixMode( GL_MODELVIEW );

        /* Reset matrix */
        glLoadIdentity();
        
        /* Display list - Track */
        glPushMatrix(); {

            /* Transformation - Symetry */
            glScalef( 1.0, 1.0, -1.0 );

            /* Transfromation - Rotation */
            glRotated( +csPosition.psAX, 1.0, 0.0, 0.0 );
            glRotated( +csPosition.psAY, 0.0, 1.0, 0.0 );

            /* Transfromation - Translation */
            glTranslated( - csPosition.psLon, - csPosition.psAlt, - csPosition.psLat );

            /* Call display list */
            glCallList( csList.lsTrack );

        } glPopMatrix();

        /* Force primitives draw */
        glFlush();

        /* Wait for primitives draw */
        glFinish();

    }

/*
    Source - Scene creation/deletion function
 */

    void cs_view_scene_compile( int const csFlag ) {

        /* Verify exectution flag */
        if ( csFlag == CS_FLAG_CREATE ) {

            /* Update clear color */
            glClearColor( 1.0, 1.0, 1.0, 0.0 );

            /* Enable depth test */
            glEnable( GL_DEPTH_TEST );
            glDepthFunc( GL_LEQUAL );
            glDepthMask( GL_TRUE );
            glClearDepth( 1.0 );

            /* Set shade model */
            glShadeModel( GL_SMOOTH );

            /* Assign display list index */
            csList.lsTrack = 1;

            /* Compile camera display list */
            cs_view_scene_track( csList.lsTrack );

        } else {

            /* Delete display list */
            glDeleteLists( csList.lsTrack, 1 );

        }

    }

/*
    Source - Specific scene compilation function
 */

    void cs_view_scene_track( GLuint const csTag ) {

        /* Parsing variables */
        lp_Size_t csParse = 0;

        /* CSPS query variables */
        lp_Trigger_t csTrigger;
        lp_Geopos_t  csGeopos;
        lp_Orient_t  csOrient;

        /* Define flags */
        int csMF = 0, csFF = 0, csIF = 0;

        /* Cartesian coordinates variables */
        double csPX = 0.0, csPY = 0.0, csPZ = 0.0;

        /* Cartesian coordinates memory variables */
        double csMX = 0.0, csMY = 0.0, csMZ = 0.0;

        /* Cartesian coordinates frame variables */
        double csXX = 0.0, csXY = 0.0, csXZ = 0.0;
        double csYX = 0.0, csYY = 0.0, csYZ = 0.0;
        double csZX = 0.0, csZY = 0.0, csZZ = 0.0;

        /* Cartesian coordinates frame memory variables */
        double csMXX = 0.0, csMXY = 0.0, csMXZ = 0.0;
        double csMYX = 0.0, csMYY = 0.0, csMYZ = 0.0;
        double csMZX = 0.0, csMZY = 0.0, csMZZ = 0.0;

        /* Initial position memory */
        double csIX = 0.0, csIY = 0.0, csIZ = 0.0;

        /* Mean position accumulators variables */
        double csLon = 0.0, csLat = 0.0, csAlt = 0.0, csAcc = 0.0;

        /* Position extremums */
        double csMinLon = 1e100, csMaxLon = -1e100;
        double csMinLat = 1e100, csMaxLat = -1e100;
        double csMinAlt = 1e100, csMaxAlt = -1e100;

        /* Create query descriptors */
        csTrigger = lp_query_trigger_create    ( csPath.ptRoot, csPath.ptCAMd, csPath.ptCAMm );
        csGeopos  = lp_query_position_create   ( csPath.ptRoot, csPath.ptGPSd, csPath.ptGPSm );
        csOrient  = lp_query_orientation_create( csPath.ptRoot, csPath.ptIMUd, csPath.ptIMUm );

        /* Declare display list begining */
        glNewList( csTag, GL_COMPILE ); {

            /* Update line width */
            glLineWidth( 2.0 );

            /* Begin primitive */
            glBegin( GL_LINES ); {

                /* Loop on camera records */
                for ( csParse = 0; csParse < lp_query_trigger_size( & csTrigger ); csParse ++ ) {

                    /* Query position and orientation by timestamp */
                    lp_query_trigger_byindex( & csTrigger, csParse );
                    lp_query_position( & csGeopos, csTrigger.qrSynch );
                    lp_query_orientation( & csOrient, csTrigger.qrSynch );

                    /* Check query results */
                    if ( ( csGeopos.qrStatus == LP_TRUE ) && ( csOrient.qrStatus == LP_TRUE ) ) {

                        /* Compute flat meters using equatorial radius */
                        csGeopos.qrLongitude *= CS_VIEW_SCENE_RAD2METER; 
                        csGeopos.qrLatitude  *= CS_VIEW_SCENE_RAD2METER;

                        /* Save initial position */
                        if ( csIF == 0 ) {

                            /* Save components */
                            csIX = + csGeopos.qrLongitude;
                            csIY = + csGeopos.qrAltitude;
                            csIZ = + csGeopos.qrLatitude;

                        } csIF = 1;

                        /* Compute cartesian coordinates */
                        csPX = csGeopos.qrLongitude - csIX;
                        csPY = csGeopos.qrAltitude  - csIY;
                        csPZ = csGeopos.qrLatitude  - csIZ;

                        /* Verify previous point memory */
                        if ( csMF != 0 ) { 

                            /* Update color */
                            glColor3f( 0.92941, 0.69412, 0.0 );

                            /* Send position vertex */
                            glVertex3d( csMX, csMY, csMZ );
                            glVertex3d( csPX, csPY, csPZ );

                            /* Update color */
                            glColor3f( 0.94, 0.94, 0.94 );

                            /* Send position vertex */
                            glVertex3d( csPX, csPY - CS_VIEW_SCENE_GRAVITY, csPZ );
                            glVertex3d( csPX, csPY, csPZ );

                            /* Send position vertex */
                            glVertex3d( csMX, csMY - CS_VIEW_SCENE_GRAVITY, csMZ );
                            glVertex3d( csPX, csPY - CS_VIEW_SCENE_GRAVITY, csPZ );

                        } csMF = 1;

                        /* Memorize position */
                        csMX = csPX; csMY = csPY; csMZ = csPZ;

                        /* Compute frame vectors */
                        csXX = csPX + csOrient.qrfxx * CS_VIEW_SCENE_FRAME; csXY = csPY + csOrient.qrfxz * CS_VIEW_SCENE_FRAME; csXZ = csPZ + csOrient.qrfxy * CS_VIEW_SCENE_FRAME;
                        csYX = csPX + csOrient.qrfyx * CS_VIEW_SCENE_FRAME; csYY = csPY + csOrient.qrfyz * CS_VIEW_SCENE_FRAME; csYZ = csPZ + csOrient.qrfyy * CS_VIEW_SCENE_FRAME;
                        csZX = csPX + csOrient.qrfzx * CS_VIEW_SCENE_FRAME; csZY = csPY + csOrient.qrfzz * CS_VIEW_SCENE_FRAME; csZZ = csPZ + csOrient.qrfzy * CS_VIEW_SCENE_FRAME;

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
                        if ( csFF != 0 ) {

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

                        } csFF = 1;

                        /* Memorize frame */
                        csMXX = csXX; csMXY = csXY; csMXZ = csXZ;
                        csMYX = csYX; csMYY = csYY; csMYZ = csYZ;
                        csMZX = csZX; csMZY = csZY; csMZZ = csZZ; 

                        /* Accumulates position */
                        csLon += csPX;
                        csAlt += csPY;
                        csLat += csPZ;

                        /* Update index */
                        csAcc += 1.0;

                        /* Compute extremums */
                        if ( csPX > csMaxLon ) csMaxLon = csPX;
                        if ( csPX < csMinLon ) csMinLon = csPX;
                        if ( csPZ > csMaxLat ) csMaxLat = csPZ;
                        if ( csPZ < csMinLat ) csMinLat = csPZ;
                        if ( csPY > csMaxAlt ) csMaxAlt = csPY;
                        if ( csPY < csMinAlt ) csMinAlt = csPY;

                    }

                }

                /* Update color */
                glColor3f( 0.70, 0.20, 0.10 );

                /* Draw referential frame - x-vector */
                glVertex3f( csMinLon - CS_VIEW_SCENE_BOX, csMinAlt - CS_VIEW_SCENE_BOX, csMinLat - CS_VIEW_SCENE_BOX );
                glVertex3f( csMaxLon + CS_VIEW_SCENE_BOX, csMinAlt - CS_VIEW_SCENE_BOX, csMinLat - CS_VIEW_SCENE_BOX );

                /* Update color */
                glColor3f( 0.20, 0.50, 0.30 );

                /* Draw referential frame - y-vector */
                glVertex3f( csMinLon - CS_VIEW_SCENE_BOX, csMinAlt - CS_VIEW_SCENE_BOX, csMinLat - CS_VIEW_SCENE_BOX );
                glVertex3f( csMinLon - CS_VIEW_SCENE_BOX, csMinAlt - CS_VIEW_SCENE_BOX, csMaxLat + CS_VIEW_SCENE_BOX );

                /* Update color */
                glColor3f( 0.20, 0.30, 0.60 );

                /* Draw referential frame - z-vector */
                glVertex3f( csMinLon - CS_VIEW_SCENE_BOX, csMinAlt - CS_VIEW_SCENE_BOX, csMinLat - CS_VIEW_SCENE_BOX );
                glVertex3f( csMinLon - CS_VIEW_SCENE_BOX, csMaxAlt + CS_VIEW_SCENE_BOX, csMinLat - CS_VIEW_SCENE_BOX );

            /* End primitive */
            } glEnd();

            /* Begin primitive */
            glBegin( GL_QUADS );

                /* Update color */
                glColor3f( 0.90, 0.90, 0.90 );
                
                /* Send vertex */
                glVertex3f( csMinLon - CS_VIEW_SCENE_BOX, csMinAlt - CS_VIEW_SCENE_BOX, csMinLat - CS_VIEW_SCENE_BOX );
                glVertex3f( csMaxLon + CS_VIEW_SCENE_BOX, csMinAlt - CS_VIEW_SCENE_BOX, csMinLat - CS_VIEW_SCENE_BOX );
                glVertex3f( csMaxLon + CS_VIEW_SCENE_BOX, csMinAlt - CS_VIEW_SCENE_BOX, csMaxLat + CS_VIEW_SCENE_BOX );
                glVertex3f( csMinLon - CS_VIEW_SCENE_BOX, csMinAlt - CS_VIEW_SCENE_BOX, csMaxLat + CS_VIEW_SCENE_BOX );

            /* End primitive */
            glEnd();

        /* Declare display list end */
        } glEndList();

        /* Delete queries descriptors */
        lp_query_trigger_delete    ( & csTrigger );
        lp_query_position_delete   ( & csGeopos  );
        lp_query_orientation_delete( & csOrient  );

        /* Reset initial position and assign initial means */
        cs_view_controls_reset( CS_VIEW_CONTROLS_SET, ( csLon / csAcc ), ( csLat / csAcc ), ( csAlt / csAcc ) );

    }

