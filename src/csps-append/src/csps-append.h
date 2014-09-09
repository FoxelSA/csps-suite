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

    /*! \file   csps-append.h
     *  \author Nils Hamel (n.hamel@foxel.ch)
     *   
     *  Software main header
     */

    /*! \mainpage csps-audit
     *
     *  Front-end suite for CSPS library.
     */

/* 
    Header - Include guard
 */

    # ifndef __CS_APPEND__
    # define __CS_APPEND__

/* 
    Header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/* 
    Header - Includes
 */

    # include <stdio.h>
    # include <stdlib.h>
    # include <time.h>
    # include <string.h>
    # include <dirent.h>
    # include <sys/stat.h>
    # include <csps-all.h>

/* 
    Header - Preprocessor definitions
 */

    /* Standard help */
    # define CS_HELP "Usage summary :\n"           \
    "  csps-append [Arguments] [Parameters] ...\n" \
    "Short arguments and parameters summary :\n"   \
    "  -p Course master directory\n"               \
    "csps-append - csps-suite\n"                   \
    "Copyright (c) 2013-2014 FOXEL SA\n"

    /* Define standard types */
    # define CS_NULL            0
    # define CS_STRING          1
    # define CS_CHAR            2
    # define CS_SHORT           3
    # define CS_INT             4
    # define CS_LONG            5
    # define CS_LLONG           6
    # define CS_UCHAR           7
    # define CS_USHORT          8
    # define CS_UINT            9
    # define CS_ULONG           10
    # define CS_ULLONG          11
    # define CS_FLOAT           12
    # define CS_DOUBLE          13

    /* Define standard output */
    # define CS_OUT             stdout

    /* Define boolean variables */
    # define CS_FALSE           0
    # define CS_TRUE            1

    /* Define directory structure */
    # define CS_PATH_PATTERN    ".log-"
    # define CS_PATH_RAW        "/mov/1"
    # define CS_PATH_SEGMENT    "segment"
    # define CS_PATH_STREAMS    "streams"
    # define CS_PATH_DEVICES    "devices"
    # define CS_PATH_EYESIS4    "eyesis4pi"
    # define CS_PATH_LOGFILE    "fpga-log.bin"

    /* Define descriptors stack size */
    # define CS_STACK_SIZE      1024

    /* Define transfer buffer size */
    # define CS_BUFFER_SIZE     8192

/* 
    Header - Preprocessor macros
 */

/* 
    Header - Typedefs
 */

/* 
    Header - Structures
 */

    /*! \struct cs_Descriptor_struct
     *  \brief Raw log file descriptor
     *
     *  This structure stores the necessary information to perform
     *  contigous file detection and appending.
     *
     *  \var cs_Descriptor_struct::dsName
     *  Stores raw log file path
     *  \var cs_Descriptor_struct::dsFlag
     *  Appended flag - True if file already appended
     *  \var cs_Descriptor_struct::dsFirst
     *  Raw log file first IMU timestamp
     *  \var cs_Descriptor_struct::dsLast
     *  Raw log file last IMU timestamp
     */ 

    typedef struct cs_Descriptor_struct {

        /* File name */
        char dsName[256];

        /* Descriptor flag */
        int dsFlag;

        /* Timestamp boundaries */
        lp_Time_t dsFirst;
        lp_Time_t dsLast;

    } cs_Descriptor;

    /* I know ! Screw you ! */
    typedef struct dirent DIRENT;

/* 
    Header - Function prototypes
 */

    /*! \brief Software main function
     *  
     *  The main function calls the analysis and appending procedure in
     *  order to perform contigous log detection and appending.
     *  
     *  \param argc Standard main parameter
     *  \param argv Standard main parameter
     */

    int main ( int argc, char ** argv );

    /*! \brief Raw logs analysis
     *  
     *  This function performs an analysis on raw log file
     *  available in the master directory. It creates the
     *  the descriptors stack that stores information on raw
     *  logs in order to perform appending of contigous logs.
     *
     *  The descriptor stack size is limited according to
     *  CS_STACK_SIZE constant.
     *  
     *  \param csPath Master directory
     *  \param csStack Descriptors stack to fill up
     *  \return Returns created stack size
     */

    int cs_append_create ( char * csPath, cs_Descriptor * csDescriptor );

    /*! \brief Contigous logs appending
     *  
     *  This function performs the appending of contigous raw log
     *  files that are provided by the descriptor stack.
     *  
     *  \param csPath Master directory
     *  \param csDescriptors Filled descriptors stack
     *  \param csStack Size of the filled descriptors stack
     */

    void cs_append_append ( char * csPath, cs_Descriptor * csDescriptors, int csStack );

    /*! \brief Appending coprocess
     *  
     *  This function append the content of the file described by the
     *  descriptor into the csHandle stream. The output stream has to 
     *  be already open.
     *  
     *  \param csDescriptor Descriptor of raw log to append
     *  \param csHandle Stream in which appending is performed
     *  \return Returns the last IMU timestamp stored by the provided descriptor
     */     

    lp_Time_t cs_append_push( cs_Descriptor * csDescriptor, FILE * csHandle );

    /*! \brief Search agrument position in argv
     *  
     *  This function search in the argv string array the position of the argument
     *  defined through ltag/stag and return the index of the corresponding parameter
     *  in argv.
     *  
     *  \param argc Standard main parameter
     *  \param argv Standard main parameter
     *  \param ltag Long-form argument string (--argument)
     *  \param stag Short-form argument string (-a)
     *  \return Index of parameter in argv
     */

    int  cs_stda ( int argc, char ** argv, const char * const ltag, const char * const stag );

    /*! \brief Parameter reader in argv
     *  
     *  This function interpret the parameter in the desired type and return it through
     *  the param variable. The argi variable is typically set using stda function. If
     *  argi is set to CS_NULL, the function does nothing.
     *  
     *  \param argi Index of the parameter in argv
     *  \param argv Standard main parameter
     *  \param param Pointer to the variable that recieve the parameter
     *  \param type Type to use for parameter interpretation
     */

    void cs_stdp ( int argi, char ** argv, void * param, int type );

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

