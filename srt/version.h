/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2018 Haivision Systems Inc.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 */

/*****************************************************************************
written by
   Haivision Systems Inc.
 *****************************************************************************/

#ifndef INC__SRT_VERSION_H
#define INC__SRT_VERSION_H

// To construct version value
#define SRT_MAKE_VERSION(major, minor, patch) \
   ((patch) + ((minor)*0x100) + ((major)*0x10000))
#define SRT_MAKE_VERSION_VALUE SRT_MAKE_VERSION

#define SRT_VERSION_MAJOR @SRT_VERSION_MAJOR@
#define SRT_VERSION_MINOR @SRT_VERSION_MINOR@
#define SRT_VERSION_PATCH @SRT_VERSION_PATCH@
//#cmakedefine SRT_VERSION_BUILD @APPVEYOR_BUILD_NUMBER_STRING@

#define SRT_VERSION_STRING "@SRT_VERSION@"
#define SRT_VERSION_VALUE \
   SRT_MAKE_VERSION_VALUE( \
      SRT_VERSION_MAJOR, SRT_VERSION_MINOR, SRT_VERSION_PATCH )

#endif // INC__SRT_VERSION_H
