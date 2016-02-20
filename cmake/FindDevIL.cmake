#.rst:
# FindDevIL
# ---------
#
#
#
# This module locates the developer's image library.
# http://openil.sourceforge.net/
#
# This module sets:
#
# ::
#
#    IL_LIBRARIES -   the name of the IL library. These include the full path to
#                     the core DevIL library. This one has to be linked into the
#                     application.
#    ILU_LIBRARIES -  the name of the ILU library. Again, the full path. This
#                     library is for filters and effects, not actual loading. It
#                     doesn't have to be linked if the functionality it provides
#                     is not used.
#    ILUT_LIBRARIES - the name of the ILUT library. Full path. This part of the
#                     library interfaces with OpenGL. It is not strictly needed
#                     in applications.
#    IL_INCLUDE_DIR - where to find the il.h, ilu.h and ilut.h files.
#    IL_FOUND -       this is set to TRUE if all the above variables were set.
#                     This will be set to false if ILU or ILUT are not found,
#                     even if they are not needed. In most systems, if one
#                     library is found all the others are as well. That's the
#                     way the DevIL developers release it.

#=============================================================================
# CMake - Cross Platform Makefile Generator
# Copyright 2000-2015 Kitware, Inc.
# Copyright 2000-2011 Insight Software Consortium
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the names of Kitware, Inc., the Insight Software Consortium,
#   nor the names of their contributors may be used to endorse or promote
#   products derived from this software without specific prior written
#   permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ------------------------------------------------------------------------------
#
# The above copyright and license notice applies to distributions of
# CMake in source and binary form.  Some source files contain additional
# notices of original copyright by their contributors; see each source
# for details.  Third-party software packages supplied with CMake under
# compatible licenses provide their own copyright notices documented in
# corresponding subdirectories.
#
# ------------------------------------------------------------------------------
#
# CMake was initially developed by Kitware with the following sponsorship:
#
#  * National Library of Medicine at the National Institutes of Health
#    as part of the Insight Segmentation and Registration Toolkit (ITK).
#
#  * US National Labs (Los Alamos, Livermore, Sandia) ASC Parallel
#    Visualization Initiative.
#
#  * National Alliance for Medical Image Computing (NAMIC) is funded by the
#    National Institutes of Health through the NIH Roadmap for Medical Research,
#    Grant U54 EB005149.
#
#  * Kitware, Inc.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# TODO: Add version support.
# Tested under Linux and Windows (MSVC)

include(FindPackageHandleStandardArgs)

find_path(IL_INCLUDE_DIR IL/il.h
  PATH_SUFFIXES include
  DOC "The path to the directory that contains IL/il.h"
)

#message("IL_INCLUDE_DIR is ${IL_INCLUDE_DIR}")

find_library(IL_LIBRARIES
  NAMES IL DEVIL
  PATH_SUFFIXES lib64 lib lib32
  DOC "The file that corresponds to the base il library."
)

#message("IL_LIBRARIES is ${IL_LIBRARIES}")

find_library(ILUT_LIBRARIES
  NAMES ILUT
  PATH_SUFFIXES lib64 lib lib32
  DOC "The file that corresponds to the il (system?) utility library."
)

#message("ILUT_LIBRARIES is ${ILUT_LIBRARIES}")

find_library(ILU_LIBRARIES
  NAMES ILU
  PATH_SUFFIXES lib64 lib lib32
  DOC "The file that corresponds to the il utility library."
)

#message("ILU_LIBRARIES is ${ILU_LIBRARIES}")

FIND_PACKAGE_HANDLE_STANDARD_ARGS(IL DEFAULT_MSG
                                  IL_LIBRARIES ILU_LIBRARIES
                                  ILUT_LIBRARIES IL_INCLUDE_DIR)
