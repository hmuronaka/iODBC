/*
 *  ColumnPrivileges.c
 *
 *  $Id$
 *
 *  SQLColumnPrivileges trace functions
 *
 *  The iODBC driver manager.
 *  
 *  Copyright (C) 1996-2003 by OpenLink Software <iodbc@openlinksw.com>
 *  All Rights Reserved.
 *
 *  This software is released under the terms of either of the following
 *  licenses:
 *
 *      - GNU Library General Public License (see LICENSE.LGPL) 
 *      - The BSD License (see LICENSE.BSD).
 *
 *  While not mandated by the BSD license, any patches you make to the
 *  iODBC source code may be contributed back into the iODBC project
 *  at your discretion. Contributions will benefit the Open Source and
 *  Data Access community as a whole. Submissions may be made at:
 *
 *      http://www.iodbc.org
 *
 *
 *  GNU Library Generic Public License Version 2
 *  ============================================
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 *  The BSD License
 *  ===============
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *  3. Neither the name of OpenLink Software Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL OPENLINK OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "trace.h"


void
trace_SQLColumnPrivileges (int trace_leave, int retcode,
  SQLHSTMT		  hstmt,
  SQLCHAR  		* szTableQualifier,
  SQLSMALLINT		  cbTableQualifier,
  SQLCHAR  		* szTableOwner,
  SQLSMALLINT		  cbTableOwner,
  SQLCHAR  		* szTableName,
  SQLSMALLINT		  cbTableName,
  SQLCHAR  		* szColumnName,
  SQLSMALLINT		  cbColumnName)
{
  /* Trace function */
  _trace_print_function (en_ColumnPrivileges, trace_leave, retcode);

  /* Trace Arguments */
  _trace_handle (SQL_HANDLE_STMT, hstmt);
  _trace_string (szTableQualifier, cbTableQualifier, NULL, TRACE_INPUT);
  _trace_stringlen ("SQLSMALLINT", cbTableQualifier);
  _trace_string (szTableOwner, cbTableOwner, NULL, TRACE_INPUT);
  _trace_stringlen ("SQLSMALLINT", cbTableOwner);
  _trace_string (szTableName, cbTableName, NULL, TRACE_INPUT);
  _trace_stringlen ("SQLSMALLINT", cbTableName);
  _trace_string (szColumnName, cbColumnName, NULL, TRACE_INPUT);
  _trace_stringlen ("SQLSMALLINT", cbColumnName);
}


void
trace_SQLColumnPrivilegesW (int trace_leave, int retcode,
  SQLHSTMT		  hstmt,
  SQLWCHAR  		* szTableQualifier,
  SQLSMALLINT		  cbTableQualifier,
  SQLWCHAR  		* szTableOwner,
  SQLSMALLINT		  cbTableOwner,
  SQLWCHAR  		* szTableName,
  SQLSMALLINT		  cbTableName,
  SQLWCHAR  		* szColumnName,
  SQLSMALLINT		  cbColumnName)
{
  /* Trace function */
  _trace_print_function (en_ColumnPrivilegesW, trace_leave, retcode);

  /* Trace Arguments */
  _trace_handle (SQL_HANDLE_STMT, hstmt);
  _trace_string_w (szTableQualifier, cbTableQualifier, NULL, TRACE_INPUT);
  _trace_stringlen ("SQLSMALLINT", cbTableQualifier);
  _trace_string_w (szTableOwner, cbTableOwner, NULL, TRACE_INPUT);
  _trace_stringlen ("SQLSMALLINT", cbTableOwner);
  _trace_string_w (szTableName, cbTableName, NULL, TRACE_INPUT);
  _trace_stringlen ("SQLSMALLINT", cbTableName);
  _trace_string_w (szColumnName, cbColumnName, NULL, TRACE_INPUT);
  _trace_stringlen ("SQLSMALLINT", cbColumnName);
}