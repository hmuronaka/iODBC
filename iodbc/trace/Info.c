/*
 *  Info.c
 *
 *  $Id$
 *
 *  Decode the SQLGetInfo responses and dump them to the trace log
 *
 *  The iODBC driver manager.
 *
 *  Copyright (C) 1996-2006 by OpenLink Software <iodbc@openlinksw.com>
 *  All Rights Reserved.
 *
 *  This software is released under the terms of either of the following
 *  licenses:
 *
 *      - GNU Library General Public License (see LICENSE.LGPL)
 *      - The BSD License (see LICENSE.BSD).
 *
 *  Note that the only valid version of the LGPL license as far as this
 *  project is concerned is the original GNU Library General Public License
 *  Version 2, dated June 1991.
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
 *  License as published by the Free Software Foundation; only
 *  Version 2 of the License dated June 1991.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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


/*
 *  Internal macros to decode the various GetInfo return values
 */
#define I_STR(T)	case T: \
			  infoname = #T; \
			  if (output) \
			    goto print_string; \
			  break

#define I_INT16(T)	case T: \
			  infoname = #T; \
			  if (output) \
			    goto print_int16; \
			  break

#define I_INT32(T)	case T: \
			  infoname = #T; \
			  if (output) \
			    goto print_int32; \
			  break

#define I_MASK(T)	case T: \
			  infoname = #T; \
			  mask = _mask_##T; \
			  elem = NUM_ELEM(_mask_##T); \
			  if (output) \
			    goto print_mask; \
			  break

#define I_MASK1(T, O)	case T: \
			  infoname = #T; \
			  mask = _mask_##O; \
			  elem = NUM_ELEM(_mask_##O); \
			  if (output) \
			    goto print_mask; \
			  break

#define I_SVAL(T)	case T: \
			  infoname = #T; \
			  mask = _mask_##T; \
			  elem = NUM_ELEM(_mask_##T); \
			  if (output) \
			    goto print_svalue; \
			  break

#define I_SVAL1(T,O)	case T: \
			  infoname = #T; \
			  mask = _mask_##O; \
			  elem = NUM_ELEM(_mask_##O); \
			  if (output) \
			    goto print_svalue; \
			  break


#define MASK(X)  	char * _mask_##X[]
#define NUM_ELEM(X)	(sizeof(X) / sizeof(X[0]))


/*
 *  Various bitmask strings
 */
static
MASK (SQL_AGGREGATE_FUNCTIONS) =
{
  "SQL_AF_UNKNOWN",
  "SQL_AF_AVG",
  "SQL_AF_COUNT",
  "SQL_AF_MAX",
  "SQL_AF_MIN",
  "SQL_AF_SUM"
  "SQL_AF_DISTINCT",
  "SQL_AF_ALL"
};


static
MASK (SQL_ALTER_DOMAIN) =
{
  "SQL_AD_CONSTRAINT_UNKNOWN",
  "SQL_AD_CONSTRAINT_NAME_DEFINITION",
  "SQL_AD_ADD_DOMAIN_CONSTRAINT",
  "SQL_AD_DROP_DOMAIN_CONSTRAINT",
  "SQL_AD_ADD_DOMAIN_DEFAULT",
  "SQL_AD_DROP_DOMAIN_DEFAULT",
  "SQL_AD_ADD_CONSTRAINT_INITIALLY_DEFERRED",
  "SQL_AD_ADD_CONSTRAINT_INITIALLY_IMMEDIATE",
  "SQL_AD_ADD_CONSTRAINT_DEFERRABLE",
  "SQL_AD_ADD_CONSTRAINT_NON_DEFERRABLE"
};


static
MASK (SQL_ALTER_TABLE) =
{
  "SQL_AT_UNKNOWN",
  "SQL_AT_ADD_COLUMN",
  "SQL_AT_DROP_COLUMN",
  "SQL_AT_UNKNOWN_BIT_3",
  "SQL_AT_ADD_CONSTRAINT",
  "SQL_AT_UNKNOWN_BIT_5",
  "SQL_AT_ADD_COLUMN_SINGLE",
  "SQL_AT_ADD_COLUMN_DEFAULT",
  "SQL_AT_ADD_COLUMN_COLLATION",
  "SQL_AT_SET_COLUMN_DEFAULT",
  "SQL_AT_DROP_COLUMN_DEFAULT",
  "SQL_AT_DROP_COLUMN_CASCADE",
  "SQL_AT_DROP_COLUMN_RESTRICT",
  "SQL_AT_ADD_TABLE_CONSTRAINT",
  "SQL_AT_DROP_TABLE_CONSTRAINT_CASCADE",
  "SQL_AT_DROP_TABLE_CONSTRAINT_RESTRICT",
  "SQL_AT_CONSTRAINT_NAME_DEFINITION",
  "SQL_AT_CONSTRAINT_INITIALLY_DEFERRED",
  "SQL_AT_CONSTRAINT_INITIALLY_IMMEDIATE",
  "SQL_AT_CONSTRAINT_DEFERRABLE",
  "SQL_AT_CONSTRAINT_NON_DEFERRABLE"
};


static
MASK (SQL_ASYNC_MODE) =
{
  "SQL_AM_NONE",
  "SQL_AM_CONNECTION",
  "SQL_AM_STATEMENT"
};


static
MASK (SQL_BATCH_ROW_COUNT) =
{
  "SQL_BRC_UNKNOWN",
  "SQL_BRC_PROCEDURES",
  "SQL_BRC_EXPLICIT",
  "SQL_BRC_ROLLED_UP"
};


static
MASK (SQL_BATCH_SUPPORT) =
{
  "SQL_BS_UNKNOWN",
  "SQL_BS_SELECT_EXPLICIT",
  "SQL_BS_ROW_COUNT_EXPLICIT",
  "SQL_BS_SELECT_PROC",
  "SQL_BS_ROW_COUNT_PROC"
};


static
MASK (SQL_BOOKMARK_PERSISTENCE) =
{
  "SQL_BP_UNKNOWN",
  "SQL_BP_CLOSE",
  "SQL_BP_DELETE",
  "SQL_BP_DROP",
  "SQL_BP_TRANSACTION",
  "SQL_BP_UPDATE",
  "SQL_BP_OTHER_HSTMT",
  "SQL_BP_SCROLL"
};


static
MASK (SQL_CATALOG_LOCATION) =
{
  "SQL_CL_UNKNOWN",
  "SQL_CL_START",
  "SQL_CL_END"
};


static
MASK (SQL_CATALOG_USAGE) =
{
  "SQL_CU_UNKNOWN",
  "SQL_CU_DML_STATEMENTS",
  "SQL_CU_PROCEDURE_INVOCATION",
  "SQL_CU_TABLE_DEFINITION",
  "SQL_CU_INDEX_DEFINITION",
  "SQL_CU_PRIVILEGE_DEFINITION"
};


static
MASK (SQL_CONCAT_NULL_BEHAVIOR) =
{
  "SQL_CB_NULL",
  "SQL_CB_NON_NULL"
};


static
MASK (CONVERT) =
{
  "SQL_CVT_UNKNOWN",
  "SQL_CVT_CHAR",
  "SQL_CVT_NUMERIC",
  "SQL_CVT_DECIMAL",
  "SQL_CVT_INTEGER",
  "SQL_CVT_SMALLINT",
  "SQL_CVT_FLOAT",
  "SQL_CVT_REAL",
  "SQL_CVT_DOUBLE",
  "SQL_CVT_VARCHAR"
  "SQL_CVT_LONGVARCHAR",
  "SQL_CVT_BINARY",
  "SQL_CVT_VARBINARY",
  "SQL_CVT_BIT",
  "SQL_CVT_TINYINT",
  "SQL_CVT_BIGINT",
  "SQL_CVT_DATE",
  "SQL_CVT_TIME",
  "SQL_CVT_TIMESTAMP",
  "SQL_CVT_LONGVARBINARY",
  "SQL_CVT_INTERVAL_YEAR_MONTH",
  "SQL_CVT_INTERVAL_DAY_TIME",
  "SQL_CVT_WCHAR",
  "SQL_CVT_WLONGVARCHAR",
  "SQL_CVT_WVARCHAR"
};


static
MASK (SQL_CONVERT_FUNCTIONS) =
{
  "SQL_FN_CVT_UNKNOWN",
  "SQL_FN_CVT_CONVERT",
  "SQL_FN_CVT_CAST"
};


static
MASK (SQL_CORRELATION_NAME) =
{
  "SQL_CN_NONE",
  "SQL_CN_DIFFERENT",
  "SQL_CN_ANY"
};


static
MASK (SQL_CREATE_ASSERTION) =
{
  "SQL_CA_UNKNOWN",
  "SQL_CA_CREATE_ASSERTION",
  "SQL_CA_UNKNOWN_BIT 2",
  "SQL_CA_UNKNOWN_BIT 3",
  "SQL_CA_UNKNOWN_BIT 4",
  "SQL_CA_CONSTRAINT_INITIALLY_DEFERRED",
  "SQL_CA_CONSTRAINT_INITIALLY_IMMEDIATE",
  "SQL_CA_CONSTRAINT_DEFERRABLE",
  "SQL_CA_CONSTRAINT_NON_DEFERRABLE"
};


static
MASK (SQL_CREATE_CHARACTER_SET) =
{
  "SQL_CSS_UNKNOWN",
  "SQL_CSS_CREATE_CHARACTER_SET",
  "SQL_CSS_COLLATE_CLAUSE",
  "SQL_CSS_LIMITED_COLLATION"
};


static
MASK (SQL_CREATE_COLLATION) =
{
  "SQL_CCOL_UNKNOWN",
  "SQL_CCOL_CREATE_COLLATION"
};


static
MASK (SQL_CREATE_DOMAIN) =
{
  "SQL_CDO_UNKNOWN",
  "SQL_CDO_CREATE_DOMAIN",
  "SQL_CDO_DEFAULT",
  "SQL_CDO_CONSTRAINT",
  "SQL_CDO_COLLATION",
  "SQL_CDO_CONSTRAINT_NAME_DEFINITION",
  "SQL_CDO_CONSTRAINT_INITIALLY_DEFERRED",
  "SQL_CDO_CONSTRAINT_INITIALLY_IMMEDIATE",
  "SQL_CDO_CONSTRAINT_DEFERRABLE",
  "SQL_CDO_CONSTRAINT_NON_DEFERRABLE"
};


static
MASK (SQL_CREATE_SCHEMA) =
{
  "SQL_CS_UNKNOWN",
  "SQL_CS_CREATE_SCHEMA",
  "SQL_CS_AUTHORIZATION",
  "SQL_CS_DEFAULT_CHARACTER_SET"
};


static
MASK (SQL_CREATE_TABLE) =
{
  "SQL_CT_UNKNOWN",
  "SQL_CT_CREATE_TABLE",
  "SQL_CT_COMMIT_PRESERVE",
  "SQL_CT_COMMIT_DELETE",
  "SQL_CT_GLOBAL_TEMPORARY",
  "SQL_CT_LOCAL_TEMPORARY",
  "SQL_CT_CONSTRAINT_INITIALLY_DEFERRED",
  "SQL_CT_CONSTRAINT_INITIALLY_IMMEDIATE",
  "SQL_CT_CONSTRAINT_DEFERRABLE",
  "SQL_CT_CONSTRAINT_NON_DEFERRABLE",
  "SQL_CT_COLUMN_CONSTRAINT",
  "SQL_CT_COLUMN_DEFAULT",
  "SQL_CT_COLUMN_COLLATION",
  "SQL_CT_TABLE_CONSTRAINT",
  "SQL_CT_CONSTRAINT_NAME_DEFINITION"
};


static
MASK (SQL_CREATE_TRANSLATION) =
{
  "SQL_CTR_UNKNOWN",
  "SQL_CTR_CREATE_TRANSLATION"
};


static
MASK (SQL_CREATE_VIEW) =
{
  "SQL_CV_UNKNOWN",
  "SQL_CV_CREATE_VIEW",
  "SQL_CV_CHECK_OPTION",
  "SQL_CV_CASCADED",
  "SQL_CV_LOCAL"
};


static
MASK (CURSOR_BEHAVIOR) =
{
  "SQL_CB_DELETE",
  "SQL_CB_CLOSE",
  "SQL_CB_PRESERVE"
};

static
MASK (SQL_CURSOR_SENSITIVITY) =
{
  "SQL_UNSPECIFIED",
  "SQL_INSENSITIVE",
  "SQL_SENSITIVE"
};


static
MASK (SQL_DATETIME_LITERALS) =
{
  "SQL_DL_SQL92_DATE",
  "SQL_DL_SQL92_TIME",
  "SQL_DL_SQL92_TIMESTAMP",
  "SQL_DL_SQL92_INTERVAL_YEAR",
  "SQL_DL_SQL92_INTERVAL_MONTH",
  "SQL_DL_SQL92_INTERVAL_DAY",
  "SQL_DL_SQL92_INTERVAL_HOUR",
  "SQL_DL_SQL92_INTERVAL_MINUTE",
  "SQL_DL_SQL92_INTERVAL_SECOND",
  "SQL_DL_SQL92_INTERVAL_YEAR_TO_MONTH",
  "SQL_DL_SQL92_INTERVAL_DAY_TO_HOUR",
  "SQL_DL_SQL92_INTERVAL_DAY_TO_MINUTE",
  "SQL_DL_SQL92_INTERVAL_DAY_TO_SECOND",
  "SQL_DL_SQL92_INTERVAL_HOUR_TO_MINUTE",
  "SQL_DL_SQL92_INTERVAL_HOUR_TO_SECOND",
  "SQL_DL_SQL92_INTERVAL_MINUTE_TO_SECOND"
};


static
MASK (SQL_DDL_INDEX) =
{
  "SQL_DI_UNKNOWN",
  "SQL_DI_CREATE_INDEX",
  "SQL_DI_DROP_INDEX"
};


static
MASK (SQL_DROP_ASSERTION) =
{
  "SQL_DA_UNKNOWN",
  "SQL_DA_DROP_ASSERTION"
};


static
MASK (SQL_DROP_CHARACTER_SET) =
{
  "SQL_DCS_UNKNOWN",
  "SQL_DCS_DROP_CHARACTER_SET"
};


static
MASK (SQL_DROP_COLLATION) =
{
  "SQL_DC_UNKNOWN",
  "SQL_DC_DROP_COLLATION"
};


static
MASK (SQL_DROP_DOMAIN) =
{
  "SQL_DD_UNKNOWN",
  "SQL_DD_DROP_DOMAIN",
  "SQL_DD_RESTRICT",
  "SQL_DD_CASCASE"
};


static
MASK (SQL_DROP_SCHEMA) =
{
  "SQL_DS_UNKNOWN",
  "SQL_DS_DROP_SCHEMA",
  "SQL_DS_RESTRICT",
  "SQL_DS_CASCADE"
};


static
MASK (SQL_DROP_TABLE) =
{
  "SQL_DT_UNKNOWN",
  "SQL_DT_DROP_TABLE",
  "SQL_DT_RESTRICT",
  "SQL_DT_CASCADE"
};


static
MASK (SQL_DROP_TRANSLATION) =
{
  "SQL_DTR_UNKNOWN",
  "SQL_DTR_DROP_TRANSLATION"
};


static
MASK (SQL_DROP_VIEW) =
{
  "SQL_DV_UNKNOWN",
  "SQL_DV_DROP_TABLE",
  "SQL_DV_RESTRICT",
  "SQL_DV_CASCADE"
};


static
MASK (CURSOR_ATTRIBUTES1) =
{
  "SQL_CA1_UNKNOWN",
  "SQL_CA1_NEXT",
  "SQL_CA1_ABSOLUTE",
  "SQL_CA1_RELATIVE",
  "SQL_CA1_BOOKMARK",
  "SQL_CA1_UNKNOWN_BIT 5",
  "SQL_CA1_UNKNOWN_BIT 6",
  "SQL_CA1_LOCK_NO_CHANGE",
  "SQL_CA1_LOCK_EXCLUSIVE",
  "SQL_CA1_LOCK_UNLOCK",
  "SQL_CA1_POS_POSITION",
  "SQL_CA1_POS_UPDATE",
  "SQL_CA1_POS_DELETE",
  "SQL_CA1_POS_REFRESH",
  "SQL_CA1_POSITIONED_UPDATE",
  "SQL_CA1_POSITIONED_DELETE",
  "SQL_CA1_SELECT_FOR_UPDATE",
  "SQL_CA1_BULK_ADD",
  "SQL_CA1_BULK_UPDATE_BY_BOOKMARK",
  "SQL_CA1_BULK_DELETE_BY_BOOKMARK",
  "SQL_CA1_BULK_FETCH_BY_BOOKMARK"
};


static
MASK (CURSOR_ATTRIBUTES2) =
{
  "SQL_CA2_UNKNOWN",
  "SQL_CA2_READ_ONLY_CONCURRENCY",
  "SQL_CA2_LOCK_CONCURRENCY",
  "SQL_CA2_OPT_ROWVER_CONCURRENCY",
  "SQL_CA2_OPT_VALUES_CONCURRENCY",
  "SQL_CA2_SENSITIVITY_ADDITIONS",
  "SQL_CA2_SENSITIVITY_DELETIONS",
  "SQL_CA2_SENSITIVITY_UPDATES",
  "SQL_CA2_MAX_ROWS_SELECT",
  "SQL_CA2_MAX_ROWS_INSERT",
  "SQL_CA2_MAX_ROWS_DELETE",
  "SQL_CA2_MAX_ROWS_UPDATE",
  "SQL_CA2_MAX_ROWS_CATALOG",
  "SQL_CA2_CRC_EXACT",
  "SQL_CA2_CRC_APPROXIMATE",
  "SQL_CA2_SIMULATE_NON_UNIQUE",
  "SQL_CA2_SIMULATE_TRY_UNIQUE",
  "SQL_CA2_SIMULATE_UNIQUE"
};


static
MASK (SQL_INDEX_KEYWORDS) =
{
  "SQL_IK_NONE",
  "SQL_IK_ASC",
  "SQL_IK_DESC"
};


static
MASK (SQL_INFO_SCHEMA_VIEWS) =
{
  "SQL_ISV_UNKNOWN",
  "SQL_ISV_ASSERTIONS",
  "SQL_ISV_CHARACTER_SETS",
  "SQL_ISV_CHECK_CONSTRAINTS",
  "SQL_ISV_COLLATIONS",
  "SQL_ISV_COLUMN_DOMAIN_USAGE",
  "SQL_ISV_COLUMN_PRIVILEGES",
  "SQL_ISV_COLUMNS",
  "SQL_ISV_CONSTRAINT_COLUMN_USAGE",
  "SQL_ISV_CONSTRAINT_TABLE_USAGE",
  "SQL_ISV_DOMAIN_CONSTRAINTS",
  "SQL_ISV_DOMAINS",
  "SQL_ISV_KEY_COLUMN_USAGE",
  "SQL_ISV_REFERENTIAL_CONSTRAINTS",
  "SQL_ISV_SCHEMATA",
  "SQL_ISV_SQL_LANGUAGES",
  "SQL_ISV_TABLE_CONSTRAINTS",
  "SQL_ISV_TABLE_PRIVILEGES",
  "SQL_ISV_TABLES",
  "SQL_ISV_TRANSLATIONS",
  "SQL_ISV_USAGE_PRIVILEGES",
  "SQL_ISV_VIEW_COLUMN_USAGE",
  "SQL_ISV_VIEW_TABLE_USAGE",
  "SQL_ISV_VIEWS"
};


static
MASK (SQL_INSERT_STATEMENT) =
{
  "SQL_IS_UNKNOWN",
  "SQL_IS_INSERT_LITERALS",
  "SQL_IS_INSERT_SEARCHED",
  "SQL_IS_SELECT_INTO"
};


static
MASK (SQL_DTC_TRANSITION_COST) =
{
  "SQL_DTC_UNKNOWN",
  "SQL_DTC_ENLIST_EXPENSIVE",
  "SQL_DTC_UNENLIST_EXPENSIVE"
};


static
MASK (TXN_ISOLATION) =
{
  "SQL_TXN_UNKNOWN",
  "SQL_TXN_READ_UNCOMMITTED",
  "SQL_TXN_READ_COMMITTED",
  "SQL_TXN_REPEATABLE_READ",
  "SQL_TXN_SERIALIZABLE",
  "SQL_TXN_VERSIONING"
};


static
MASK (SQL_FETCH_DIRECTION) =
{
  "SQL_FD_FETCH_UNKNOWN",
  "SQL_FD_FETCH_NEXT",
  "SQL_FD_FETCH_FIRST",
  "SQL_FD_FETCH_LAST",
  "SQL_FD_FETCH_PRIOR",
  "SQL_FD_FETCH_ABSOLUTE",
  "SQL_FD_FETCH_RELATIVE",
  "SQL_FD_FETCH_RESUME",
  "SQL_FD_FETCH_BOOKMARK"
};


static
MASK (SQL_FILE_USAGE) =
{
  "SQL_FILE_NOT_SUPPORTED",
  "SQL_FILE_TABLE",
  "SQL_FILE_QUALIFIER"
};


static
MASK (SQL_GETDATA_EXTENSIONS) =
{
  "SQL_GD_UNKNOWN",
  "SQL_GD_ANY_COLUMN",
  "SQL_GD_ANY_ORDER",
  "SQL_GD_BLOCK",
  "SQL_GD_BOUND"
};


static
MASK (SQL_GROUP_BY) =
{
  "SQL_GB_NOT_SUPPORTED",
  "SQL_GB_GROUP_BY_EQUALS_SELECT",
  "SQL_GB_GROUP_BY_CONTAINS_SELECT",
  "SQL_GB_NO_RELATION"
};


static
MASK (IDENTIFIER_CASE) =
{
  "SQL_IC_UNKNOWN",
  "SQL_IC_UPPER",
  "SQL_IC_LOWER",
  "SQL_IC_SENSITIVE",
  "SQL_IC_MIXED"
};


static
MASK (SQL_LOCK_TYPES) =
{
  "SQL_LCK_UNKNOWN",
  "SQL_LCK_NO_CHANGE",
  "SQL_LCK_EXCLUSIVE",
  "SQL_LCK_UNLOCK"
};


static
MASK (SQL_NON_NULLABLE_COLUMNS) =
{
  "SQL_NNC_NULL",
  "SQL_NNC_NON_NULL"
};


static
MASK (SQL_NULL_COLLATION) =
{
  "SQL_NC_HIGH",
  "SQL_NC_LOW",
  "SQL_NC_START"
  "SQL_NC_END",
};


static
MASK (SQL_NUMERIC_FUNCTIONS) =
{
  "SQL_FN_UNKNOWN",
  "SQL_FN_NUM_ABS",
  "SQL_FN_NUM_ACOS",
  "SQL_FN_NUM_ASIN",
  "SQL_FN_NUM_ATAN",
  "SQL_FN_NUM_ATAN2",
  "SQL_FN_NUM_CEILING",
  "SQL_FN_NUM_COS",
  "SQL_FN_NUM_COT",
  "SQL_FN_NUM_EXP",
  "SQL_FN_NUM_FLOOR",
  "SQL_FN_NUM_LOG",
  "SQL_FN_NUM_MOD",
  "SQL_FN_NUM_SIGN",
  "SQL_FN_NUM_SIN",
  "SQL_FN_NUM_SQRT",
  "SQL_FN_NUM_TAN",
  "SQL_FN_NUM_PI",
  "SQL_FN_NUM_RAND",
  "SQL_FN_NUM_DEGREES",
  "SQL_FN_NUM_LOG10",
  "SQL_FN_NUM_POWER",
  "SQL_FN_NUM_RADIANS",
  "SQL_FN_NUM_ROUND",
  "SQL_FN_NUM_TRUNCATE"
};


static
MASK (SQL_ODBC_API_CONFORMANCE) =
{
  "SQL_OAC_NONE",
  "SQL_OAC_LEVEL1",
  "SQL_OAC_LEVEL2"
};


static
MASK (SQL_ODBC_INTERFACE_CONFORMANCE) =
{
  "SQL_OIC_UNKNOWN",
  "SQL_OAC_CORE",
  "SQL_OAC_LEVEL1",
  "SQL_OAC_LEVEL2"
};


static
MASK (SQL_ODBC_SAG_CLI_CONFORMANCE) =
{
  "SQL_OSCC_NOT_COMPLIANT",
  "SQL_OSCC_COMPLIANT"
};


static
MASK (SQL_ODBC_SQL_CONFORMANCE) =
{
  "SQL_OSC_MINIMUM",
  "SQL_OSC_CORE",
  "SQL_OSC_EXTENDED"
};


static
MASK (SQL_OJ_CAPABILITIES) =
{
  "SQL_OJ_UNKNOWN",
  "SQL_OJ_LEFT",
  "SQL_OJ_RIGHT",
  "SQL_OJ_FULL",
  "SQL_OJ_NESTED",
  "SQL_OJ_NOT_ORDERED",
  "SQL_OJ_INNER",
  "SQL_OJ_ALL_COMPARISON_OPS"
};


#if (ODBCVER < 0x0300)
static
MASK (SQL_OWNER_USAGE) =
{
  "SQL_OU_UNKNOWN",
  "SQL_OU_DML_STATEMENTS",
  "SQL_OU_PROCEDURE_INVOCATION",
  "SQL_OU_TABLE_DEFINITION",
  "SQL_OU_INDEX_DEFINITION",
  "SQL_OU_PRIVILEGE_DEFINITION"
};
#endif


static
MASK (SQL_PARAM_ARRAY_ROW_COUNTS) =
{
  "SQL_PARC_UNKNOWN",
  "SQL_PARC_BATCH",
  "SQL_PARC_NOBATCH"
};


static
MASK (SQL_PARAM_ARRAY_SELECTS) =
{
  "SQL_PAS_UNKNOWN",
  "SQL_PAS_BATCH",
  "SQL_PAS_NO_BATCH",
  "SQL_PAS_NO_SELECT"
};


static
MASK (SQL_POSITIONED_STATEMENTS) =
{
  "SQL_PS_UNKNOWN",
  "SQL_PS_POSITIONED_DELETE",
  "SQL_PS_POSITIONED_UPDATE",
  "SQL_PS_SELECT_FOR_UPDATE"
};


static
MASK (SQL_POS_OPERATIONS) =
{
  "SQL_POS_UNKNOWN",
  "SQL_POS_POSITION",
  "SQL_POS_REFRESH",
  "SQL_POS_UPDATE",
  "SQL_POS_DELETE",
  "SQL_POS_ADD"
};


#if (ODBCVER < 0x0300)
static
MASK (SQL_QUALIFIER_LOCATION) =
{
  "SQL_QL_UNKNOWN",
  "SQL_QL_START",
  "SQL_QL_END"
};
#endif


#if (ODBCVER < 0x0300)
static
MASK (SQL_QUALIFIER_USAGE) =
{
  "SQL_QU_UNKNOWN",
  "SQL_QU_DML_STATEMENTS",
  "SQL_QU_PROCEDURE_INVOCATION",
  "SQL_QU_TABLE_DEFINITION",
  "SQL_QU_INDEX_DEFINITION",
  "SQL_QU_PRIVILEGE_DEFINITION"
};
#endif


static
MASK (SQL_SCHEMA_USAGE) =
{
  "SQL_SU_UNKNOWN",
  "SQL_SU_DML_STATEMENTS",
  "SQL_SU_PROCEDURE_INVOCATION",
  "SQL_SU_TABLE_DEFINITION",
  "SQL_SU_INDEX_DEFINITION",
  "SQL_SU_PRIVILEGE_DEFINITION"
};


static
MASK (SQL_SCROLL_CONCURRENCY) =
{
  "SQL_SCCO_UNKNOWN",
  "SQL_SCCO_READ_ONLY",
  "SQL_SCCO_LOCK",
  "SQL_SCCO_OPT_ROWVER",
  "SQL_SCCO_OPT_VALUES"
};


static
MASK (SQL_SCROLL_OPTIONS) =
{
  "SQL_SO_UNKNOWN",
  "SQL_SO_FORWARD_ONLY",
  "SQL_SO_KEYSET_DRIVEN",
  "SQL_SO_DYNAMIC",
  "SQL_SO_MIXED",
  "SQL_SO_STATIC"
};


static
MASK (SQL_SQL_CONFORMANCE) =
{
  "SQL_SC_UINKNOWN",
  "SQL_SC_SQL92_ENTRY",
  "SQL_SC_FIPS127_2_TRANSITIONAL",
  "SQL_SC_SQL92_INTERMEDIATE",
  "SQL_SC_SQL92_FULL"
};


static
MASK (SQL_SQL92_DATETIME_FUNCTIONS) =
{
  "SQL_SDF_UNKNOWN",
  "SQL_SDF_CURRENT_DATE",
  "SQL_SDF_CURRENT_TIME",
  "SQL_SDF_CURRENT_TIMESTAMP"
};


static
MASK (SQL_SQL92_FOREIGN_KEY_DELETE_RULE) =
{
  "SQL_SFKD_UNKNOWN",
  "SQL_SFKD_CASCADE",
  "SQL_SFKD_NO_ACTION",
  "SQL_SFKD_SET_DEFAULT",
  "SQL_SFKD_SET_NULL"
};


static
MASK (SQL_SQL92_FOREIGN_KEY_UPDATE_RULE) =
{
  "SQL_SFKU_UNKNOWN",
  "SQL_SFKU_CASCADE",
  "SQL_SFKU_NO_ACTION",
  "SQL_SFKU_SET_DEFAULT",
  "SQL_SFKU_SET_NULL"
};


static
MASK (SQL_SQL92_GRANT) =
{
  "SQL_SG_UNKNOWN",
  "SQL_SG_USAGE_ON_DOMAIN",
  "SQL_SG_USAGE_ON_CHARACTER_SET",
  "SQL_SG_USAGE_ON_COLLATION",
  "SQL_SG_USAGE_ON_TRANSLATION",
  "SQL_SG_WITH_GRANT_OPTION",
  "SQL_SG_DELETE_TABLE",
  "SQL_SG_INSERT_TABLE",
  "SQL_SG_INSERT_COLUMN",
  "SQL_SG_REFERENCES_TABLE",
  "SQL_SG_REFERENCES_COLUMN",
  "SQL_SG_SELECT_TABLE",
  "SQL_SG_UPDATE_TABLE",
  "SQL_SG_UPDATE_COLUMN"
};


static
MASK (SQL_SQL92_NUMERIC_VALUE_FUNCTIONS) =
{
  "SQL_SNVF_UNKNOWN",
  "SQL_SNVF_BIT_LENGTH",
  "SQL_SNVF_CHAR_LENGTH",
  "SQL_SNVF_CHARACTER_LENGTH",
  "SQL_SNVF_EXTRACT",
  "SQL_SNVF_OCTET_LENGTH",
  "SQL_SNVF_POSITION"
};


static
MASK (SQL_SQL92_PREDICATES) =
{
  "SQL_SP_UNKNOWN",
  "SQL_SP_EXISTS",
  "SQL_SP_ISNOTNULL",
  "SQL_SP_ISNULL",
  "SQL_SP_MATCH_FULL",
  "SQL_SP_MATCH_PARTIAL",
  "SQL_SP_MATCH_UNIQUE_FULL",
  "SQL_SP_MATCH_UNIQUE_PARTIAL",
  "SQL_SP_OVERLAPS",
  "SQL_SP_UNIQUE",
  "SQL_SP_LIKE",
  "SQL_SP_IN",
  "SQL_SP_BETWEEN",
  "SQL_SP_COMPARISON",
  "SQL_SP_QUANTIFIED_COMPARISON"
};


static
MASK (SQL_SQL92_RELATIONAL_JOIN_OPERATORS) =
{
  "SQL_SRJO_UNKOWN",
  "SQL_SRJO_CORRESPONDING_CLAUSE",
  "SQL_SRJO_CROSS_JOIN",
  "SQL_SRJO_EXCEPT_JOIN",
  "SQL_SRJO_FULL_OUTER_JOIN",
  "SQL_SRJO_INNER_JOIN",
  "SQL_SRJO_INTERSECT_JOIN",
  "SQL_SRJO_LEFT_OUTER_JOIN",
  "SQL_SRJO_NATURAL_JOIN",
  "SQL_SRJO_RIGHT_OUTER_JOIN",
  "SQL_SRJO_UNION_JOIN"
};


static
MASK (SQL_SQL92_REVOKE) =
{
  "SQL_SR_UNKNOWN",
  "SQL_SR_USAGE_ON_DOMAIN",
  "SQL_SR_USAGE_ON_CHARACTER_SET",
  "SQL_SR_USAGE_ON_COLLATION",
  "SQL_SR_USAGE_ON_TRANSLATION",
  "SQL_SR_GRANT_OPTION_FOR",
  "SQL_SR_CASCADE",
  "SQL_SR_RESTRICT",
  "SQL_SR_DELETE_TABLE",
  "SQL_SR_INSERT_TABLE",
  "SQL_SR_INSERT_COLUMN",
  "SQL_SR_REFERENCES_TABLE",
  "SQL_SR_REFERENCES_COLUMN",
  "SQL_SR_SELECT_TABLE",
  "SQL_SR_UPDATE_TABLE",
  "SQL_SR_UPDATE_COLUMN"
};


static
MASK (SQL_SQL92_ROW_VALUE_CONSTRUCTOR) =
{
  "SQL_SRVC_UNKOWN",
  "SQL_SRVC_VALUE_EXPRESSION",
  "SQL_SRVC_NULL",
  "SQL_SRVC_DEFAULT",
  "SQL_SRVC_ROW_SUBQUERY"
};


static
MASK (SQL_SQL92_STRING_FUNCTIONS) =
{
  "SQL_SSF_UNKNOWN",
  "SQL_SSF_CONVERT",
  "SQL_SSF_LOWER",
  "SQL_SSF_UPPER",
  "SQL_SSF_SUBSTRING",
  "SQL_SSF_TRANSLATE",
  "SQL_SSF_TRIM_BOTH",
  "SQL_SSF_TRIM_LEADING",
  "SQL_SSF_TRIM_TRAILING"
};


static
MASK (SQL_SQL92_VALUE_EXPRESSIONS) =
{
  "SQL_SVE_UNKNOWN",
  "SQL_SVE_CASE",
  "SQL_SVE_CAST",
  "SQL_SVE_COALESCE",
  "SQL_SVE_NULLIF"
};


static
MASK (SQL_STANDARD_CLI_CONFORMANCE) =
{
  "SQL_SCC_UNKNOWN",
  "SQL_SCC_XOPEN_CLI_VERSION1",
  "SQL_SCC_ISO92_CLI",
};


static
MASK (SQL_STATIC_SENSITIVITY) =
{
  "SQL_SS_UNKNOWN",
  "SQL_SS_ADDITIONS",
  "SQL_SS_DELETIONS",
  "SQL_SS_UPDATES"
};


static
MASK (SQL_STRING_FUNCTIONS) =
{
  "SQL_FN_STR_UNKNOWN",
  "SQL_FN_STR_CONCAT",
  "SQL_FN_STR_INSERT",
  "SQL_FN_STR_LEFT",
  "SQL_FN_STR_LTRIM",
  "SQL_FN_STR_LENGTH",
  "SQL_FN_STR_LOCATE",
  "SQL_FN_STR_LCASE",
  "SQL_FN_STR_REPEAT",
  "SQL_FN_STR_REPLACE",
  "SQL_FN_STR_RIGHT",
  "SQL_FN_STR_RTRIM",
  "SQL_FN_STR_SUBSTRING",
  "SQL_FN_STR_UCASE ",
  "SQL_FN_STR_ASCII",
  "SQL_FN_STR_CHAR",
  "SQL_FN_STR_DIFFERENCE",
  "SQL_FN_STR_LOCATE_2",
  "SQL_FN_STR_SOUNDEX",
  "SQL_FN_STR_SPACE",
  "SQL_FN_BIT_LENGTH",
  "SQL_FN_STR_CHAR_LENGTH",
  "SQL_FN_STR_CARACTER_LENGTH",
  "SQL_FN_STR_OCTET_LENGTH",
  "SQL_FN_STR_POSITION"
};


static
MASK (SQL_SUBQUERIES) =
{
  "SQL_SQ_UNKNOWN",
  "SQL_SQ_COMPARISON",
  "SQL_SQ_EXISTS",
  "SQL_SQ_IN",
  "SQL_SQ_QUANTIFIED",
  "SQL_SQ_CORRELATED_SUBQUERIES"
};


static
MASK (SQL_SYSTEM_FUNCTIONS) =
{
  "SQL_FN_SYS_UNKNOWN",
  "SQL_FN_SYS_USERNAME",
  "SQL_FN_SYS_DBNAME",
  "SQL_FN_SYS_IFNULL"
};


static
MASK (TIMEDATE_INTERVALS) =
{
  "SQL_FN_TSI_UNKNOWN",
  "SQL_FN_TSI_FRAC_SECOND",
  "SQL_FN_TSI_SECOND",
  "SQL_FN_TSI_MINUTE",
  "SQL_FN_TSI_HOUR",
  "SQL_FN_TSI_DAY",
  "SQL_FN_TSI_WEEK",
  "SQL_FN_TSI_MONTH",
  "SQL_FN_TSI_QUARTER",
  "SQL_FN_TSI_YEAR "
};


static
MASK (SQL_TIMEDATE_FUNCTIONS) =
{
  "SQL_FN_TD_UNKNOWN",
  "SQL_FN_TD_NOW",
  "SQL_FN_TD_CURDATE",
  "SQL_FN_TD_DAYOFMONTH",
  "SQL_FN_TD_DAYOFWEEK",
  "SQL_FN_TD_DAYOFYEAR",
  "SQL_FN_TD_MONTH",
  "SQL_FN_TD_QUARTER",
  "SQL_FN_TD_WEEK",
  "SQL_FN_TD_YEAR",
  "SQL_FN_TD_CURTIME",
  "SQL_FN_TD_HOUR",
  "SQL_FN_TD_MINUTE",
  "SQL_FN_TD_SECOND",
  "SQL_FN_TD_TIMESTAMPADD",
  "SQL_FN_TD_TIMESTAMPDIFF",
  "SQL_FN_TD_DAYNAME",
  "SQL_FN_TD_MONTHNAME",
  "SQL_FN_TD_CURRENT_DATE",
  "SQL_FN_TD_CURRENT_TIME",
  "SQL_FN_TD_CURRENT_TIMESTAMP",
  "SQL_FN_TD_EXTRACT"
};


static
MASK (SQL_TXN_CAPABLE) =
{
  "SQL_TC_NONE",
  "SQL_TC_DML",
  "SQL_TC_ALL",
  "SQL_TC_DDL_COMMIT",
  "SQL_TC_DDL_IGNORE",
};


static
MASK (SQL_UNION) =
{
  "SQL_U_UNION_UNKNOWN",
  "SQL_U_UNION",
  "SQL_U_UNION_ALL"
};



/*
 *  Decode the various GetInfo return values and print them into the trace log
 */
static void
_trace_getinfo (
  SQLUSMALLINT		  fInfoType,
  SQLPOINTER		  rgbInfoValue,
  SQLSMALLINT		  cbInfoValueMax,
  SQLSMALLINT    	* pcbInfoValue,
  int			  output,
  char			  waMode)
{
  char *infoname;
  char **mask;
  int elem;
  int i;

  cbInfoValueMax = cbInfoValueMax;	/*UNUSED*/
  pcbInfoValue = pcbInfoValue;		/*UNUSED*/

  /*
   *  If the pointer is NULL, we have no information to decode, so
   *  we just print the generic details.
   */
  if (!rgbInfoValue)
    output = 0;

  switch (fInfoType)
    {

      /*
       *  ODBC 1.0
       */
      I_STR (SQL_ACCESSIBLE_TABLES);

      I_STR (SQL_ACCESSIBLE_PROCEDURES);

#if (ODBCVER < 0x0300)
      I_INT16 (SQL_ACTIVE_CONNECTIONS);	/* 3.0: SQL_MAX_DRIVER_CONNECTIONS */
#endif

#if (ODBCVER < 0x0300)
      I_INT16 (SQL_ACTIVE_STATEMENTS);  /* 3.0: SQL_MAX_CONCURRENT_ACTIVITIES */
#endif

      I_SVAL (SQL_CONCAT_NULL_BEHAVIOR);

      I_MASK (SQL_CONVERT_FUNCTIONS);

      I_MASK1 (SQL_CONVERT_BIGINT, CONVERT);
      I_MASK1 (SQL_CONVERT_BINARY, CONVERT);
      I_MASK1 (SQL_CONVERT_BIT, CONVERT);
      I_MASK1 (SQL_CONVERT_CHAR, CONVERT);
      I_MASK1 (SQL_CONVERT_DATE, CONVERT);
      I_MASK1 (SQL_CONVERT_DECIMAL, CONVERT);
      I_MASK1 (SQL_CONVERT_DOUBLE, CONVERT);
      I_MASK1 (SQL_CONVERT_FLOAT, CONVERT);
      I_MASK1 (SQL_CONVERT_INTEGER, CONVERT);
      I_MASK1 (SQL_CONVERT_LONGVARBINARY, CONVERT);
      I_MASK1 (SQL_CONVERT_LONGVARCHAR, CONVERT);
      I_MASK1 (SQL_CONVERT_NUMERIC, CONVERT);
      I_MASK1 (SQL_CONVERT_REAL, CONVERT);
      I_MASK1 (SQL_CONVERT_SMALLINT, CONVERT);
      I_MASK1 (SQL_CONVERT_TIME, CONVERT);
      I_MASK1 (SQL_CONVERT_TIMESTAMP, CONVERT);
      I_MASK1 (SQL_CONVERT_TINYINT, CONVERT);
      I_MASK1 (SQL_CONVERT_VARBINARY, CONVERT);
      I_MASK1 (SQL_CONVERT_VARCHAR, CONVERT);

      I_SVAL1 (SQL_CURSOR_COMMIT_BEHAVIOR, CURSOR_BEHAVIOR);

      I_SVAL1 (SQL_CURSOR_ROLLBACK_BEHAVIOR, CURSOR_BEHAVIOR);

      I_STR (SQL_DATA_SOURCE_NAME);

      I_STR (SQL_DATA_SOURCE_READ_ONLY);

      I_STR (SQL_DATABASE_NAME);

      I_STR (SQL_DBMS_NAME);

      I_STR (SQL_DBMS_VER);

      I_MASK1 (SQL_DEFAULT_TXN_ISOLATION, TXN_ISOLATION);

      I_INT32 (SQL_DRIVER_HDBC);

      I_INT32 (SQL_DRIVER_HENV);

      I_INT32 (SQL_DRIVER_HSTMT);

      I_STR (SQL_DRIVER_NAME);

      I_STR (SQL_DRIVER_VER);

      I_STR (SQL_EXPRESSIONS_IN_ORDERBY);

      I_MASK (SQL_FETCH_DIRECTION);

      I_SVAL1 (SQL_IDENTIFIER_CASE, IDENTIFIER_CASE);

      I_STR (SQL_IDENTIFIER_QUOTE_CHAR);

      I_INT16 (SQL_MAX_COLUMN_NAME_LEN);

      I_INT16 (SQL_MAX_CURSOR_NAME_LEN);

#if (ODBCVER < 0x0300)
      I_INT16 (SQL_MAX_OWNER_NAME_LEN); /* 3.0: SQL_MAX_SCHEMA_NAME_LEN */
#endif

      I_INT16 (SQL_MAX_PROCEDURE_NAME_LEN);

#if (ODBCVER < 0x0300)
      I_INT16 (SQL_MAX_QUALIFIER_NAME_LEN); /* 3.0: SQL_MAX_CATALOG_NAME_LEN */
#endif

      I_INT16 (SQL_MAX_TABLE_NAME_LEN);

      I_STR (SQL_MULT_RESULT_SETS);

      I_STR (SQL_MULTIPLE_ACTIVE_TXN);

      I_MASK (SQL_NUMERIC_FUNCTIONS);

      I_SVAL (SQL_ODBC_API_CONFORMANCE);

      I_SVAL (SQL_ODBC_SAG_CLI_CONFORMANCE);

      I_SVAL (SQL_ODBC_SQL_CONFORMANCE);

      I_STR (SQL_ODBC_VER);

#if (ODBCVER < 0x0300)
      I_STR (SQL_ODBC_SQL_OPT_IEF); /* 3.0: SQL_INTEGRITY */
#endif

#if (ODBCVER < 0x0300)
      I_STR (SQL_OWNER_TERM); /* 3.0: SQL_SCHEMA_TERM */
#endif

      I_STR (SQL_OUTER_JOINS);

      I_STR (SQL_PROCEDURE_TERM);

      I_STR (SQL_PROCEDURES);

#if (ODBCVER < 0x0300)
      I_STR (SQL_QUALIFIER_NAME_SEPARATOR); /* 3.0: SQL_CATALOG_NAME_SEPARATOR */
#endif

#if (ODBCVER < 0x0300)
      I_STR (SQL_QUALIFIER_TERM); /* 3.0: SQL_CATALOG_TERM */
#endif

      I_STR (SQL_ROW_UPDATES);

      I_MASK (SQL_SCROLL_CONCURRENCY);

      I_MASK (SQL_SCROLL_OPTIONS);

      I_STR (SQL_SEARCH_PATTERN_ESCAPE);

      I_STR (SQL_SERVER_NAME);

      I_MASK (SQL_STRING_FUNCTIONS);

      I_MASK (SQL_SYSTEM_FUNCTIONS);

      I_STR (SQL_TABLE_TERM);

      I_MASK (SQL_TIMEDATE_FUNCTIONS);

      I_SVAL (SQL_TXN_CAPABLE);

      I_MASK1 (SQL_TXN_ISOLATION_OPTION, TXN_ISOLATION);

      I_STR (SQL_USER_NAME);


      /*
       * ODBC 1.0 Additions
       */
      I_SVAL (SQL_CORRELATION_NAME);

      I_SVAL (SQL_NON_NULLABLE_COLUMNS);


      /*
       *  ODBC 2.0 Additions
       */
      I_MASK (SQL_ALTER_TABLE);

      I_MASK (SQL_BOOKMARK_PERSISTENCE);

      I_STR (SQL_COLUMN_ALIAS);

      I_INT32 (SQL_DRIVER_HLIB);

      I_STR (SQL_DRIVER_ODBC_VER);

      I_MASK (SQL_GETDATA_EXTENSIONS);

      I_SVAL (SQL_GROUP_BY);

      I_SVAL (SQL_FILE_USAGE);

      I_STR (SQL_KEYWORDS);

      I_STR (SQL_LIKE_ESCAPE_CLAUSE);

      I_MASK (SQL_LOCK_TYPES);

      I_INT32 (SQL_MAX_BINARY_LITERAL_LEN);

      I_INT32 (SQL_MAX_CHAR_LITERAL_LEN);

      I_INT16 (SQL_MAX_COLUMNS_IN_GROUP_BY);

      I_INT16 (SQL_MAX_COLUMNS_IN_INDEX);

      I_INT16 (SQL_MAX_COLUMNS_IN_ORDER_BY);

      I_INT16 (SQL_MAX_COLUMNS_IN_SELECT);

      I_INT16 (SQL_MAX_COLUMNS_IN_TABLE);

      I_INT32 (SQL_MAX_INDEX_SIZE);

      I_STR (SQL_MAX_ROW_SIZE_INCLUDES_LONG);

      I_INT32 (SQL_MAX_ROW_SIZE);

      I_INT32 (SQL_MAX_STATEMENT_LEN);

      I_INT16 (SQL_MAX_TABLES_IN_SELECT);

      I_INT16 (SQL_MAX_USER_NAME_LEN);

      I_STR (SQL_NEED_LONG_DATA_LEN);

      I_SVAL (SQL_NULL_COLLATION);

      I_STR (SQL_ORDER_BY_COLUMNS_IN_SELECT);

#if (ODBCVER < 0x0300)
      I_MASK (SQL_OWNER_USAGE);  /* 3.0: SQL_SCHEMA_USAGE */
#endif

      I_MASK (SQL_OJ_CAPABILITIES);

      I_MASK (SQL_POS_OPERATIONS);

      I_MASK (SQL_POSITIONED_STATEMENTS);

#if (ODBCVER < 0x0300)
      I_SVAL (SQL_QUALIFIER_LOCATION); /* 3.0: SQL_CATALOG_LOCATION */
#endif

#if (ODBCVER < 0x0300)
      I_MASK (SQL_QUALIFIER_USAGE); /* 3.0: SQL_CATALOG_USAGE */
#endif

      I_SVAL1 (SQL_QUOTED_IDENTIFIER_CASE, IDENTIFIER_CASE);

      I_STR (SQL_SPECIAL_CHARACTERS);

      I_MASK (SQL_STATIC_SENSITIVITY);

      I_MASK (SQL_SUBQUERIES);

      I_MASK1 (SQL_TIMEDATE_ADD_INTERVALS, TIMEDATE_INTERVALS);

      I_MASK1 (SQL_TIMEDATE_DIFF_INTERVALS, TIMEDATE_INTERVALS);

      I_MASK (SQL_UNION);


      /*
       *  ODBC 3.0
       */
#if (ODBCVER >= 0x0300)
      I_INT16 (SQL_ACTIVE_ENVIRONMENTS);

      I_MASK (SQL_AGGREGATE_FUNCTIONS);

      I_MASK (SQL_ALTER_DOMAIN);

      I_SVAL (SQL_ASYNC_MODE);

      I_MASK (SQL_BATCH_ROW_COUNT);

      I_MASK (SQL_BATCH_SUPPORT);

      I_SVAL (SQL_CATALOG_LOCATION);

      I_STR (SQL_CATALOG_NAME);

      I_STR (SQL_CATALOG_NAME_SEPARATOR);

      I_STR (SQL_CATALOG_TERM);

      I_MASK (SQL_CATALOG_USAGE);

      I_STR (SQL_COLLATION_SEQ);

      I_MASK1 (SQL_CONVERT_INTERVAL_YEAR_MONTH, CONVERT);

      I_MASK1 (SQL_CONVERT_INTERVAL_DAY_TIME, CONVERT);

      I_MASK1 (SQL_CONVERT_WCHAR, CONVERT);

      I_MASK1 (SQL_CONVERT_WLONGVARCHAR, CONVERT);

      I_MASK1 (SQL_CONVERT_WVARCHAR, CONVERT);

      I_MASK (SQL_CREATE_ASSERTION);

      I_MASK (SQL_CREATE_CHARACTER_SET);

      I_MASK (SQL_CREATE_COLLATION);

      I_MASK (SQL_CREATE_DOMAIN);

      I_MASK (SQL_CREATE_SCHEMA);

      I_MASK (SQL_CREATE_TABLE);

      I_MASK (SQL_CREATE_TRANSLATION);

      I_MASK (SQL_CREATE_VIEW);

      I_SVAL (SQL_CURSOR_SENSITIVITY);

      I_MASK (SQL_DATETIME_LITERALS);

      I_MASK (SQL_DDL_INDEX);

      I_STR (SQL_DESCRIBE_PARAMETER);

      I_STR (SQL_DM_VER);

      I_MASK (SQL_DTC_TRANSITION_COST);

      I_MASK (SQL_DROP_ASSERTION);

      I_MASK (SQL_DROP_CHARACTER_SET);

      I_MASK (SQL_DROP_COLLATION);

      I_MASK (SQL_DROP_DOMAIN);

      I_MASK (SQL_DROP_SCHEMA);

      I_MASK (SQL_DROP_TABLE);

      I_MASK (SQL_DROP_TRANSLATION);

      I_MASK (SQL_DROP_VIEW);

      I_MASK1 (SQL_DYNAMIC_CURSOR_ATTRIBUTES1, CURSOR_ATTRIBUTES1);

      I_MASK1 (SQL_DYNAMIC_CURSOR_ATTRIBUTES2, CURSOR_ATTRIBUTES2);

      I_MASK1 (SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1, CURSOR_ATTRIBUTES1);

      I_MASK1 (SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2, CURSOR_ATTRIBUTES2);

      I_MASK (SQL_INDEX_KEYWORDS);

      I_MASK (SQL_INFO_SCHEMA_VIEWS);

      I_MASK (SQL_INSERT_STATEMENT);

      I_STR (SQL_INTEGRITY);

      I_MASK1 (SQL_KEYSET_CURSOR_ATTRIBUTES1, CURSOR_ATTRIBUTES1);

      I_MASK1 (SQL_KEYSET_CURSOR_ATTRIBUTES2, CURSOR_ATTRIBUTES2);

      I_INT32 (SQL_MAX_ASYNC_CONCURRENT_STATEMENTS);

      I_INT16 (SQL_MAX_CATALOG_NAME_LEN);

      I_INT16 (SQL_MAX_CONCURRENT_ACTIVITIES);

      I_INT16 (SQL_MAX_DRIVER_CONNECTIONS);

      I_INT16 (SQL_MAX_IDENTIFIER_LEN);

      I_INT16 (SQL_MAX_SCHEMA_NAME_LEN);

      I_SVAL (SQL_ODBC_INTERFACE_CONFORMANCE);

      I_SVAL (SQL_PARAM_ARRAY_ROW_COUNTS);

      I_SVAL (SQL_PARAM_ARRAY_SELECTS);

      I_STR (SQL_SCHEMA_TERM);

      I_MASK (SQL_SCHEMA_USAGE);

      I_SVAL (SQL_SQL_CONFORMANCE);

      I_MASK (SQL_SQL92_DATETIME_FUNCTIONS);

      I_MASK (SQL_SQL92_FOREIGN_KEY_DELETE_RULE);

      I_MASK (SQL_SQL92_FOREIGN_KEY_UPDATE_RULE);

      I_MASK (SQL_SQL92_GRANT);

      I_MASK (SQL_SQL92_NUMERIC_VALUE_FUNCTIONS);

      I_MASK (SQL_SQL92_PREDICATES);

      I_MASK (SQL_SQL92_RELATIONAL_JOIN_OPERATORS);

      I_MASK (SQL_SQL92_REVOKE);

      I_MASK (SQL_SQL92_ROW_VALUE_CONSTRUCTOR);

      I_MASK (SQL_SQL92_STRING_FUNCTIONS);

      I_MASK (SQL_SQL92_VALUE_EXPRESSIONS);

      I_MASK (SQL_STANDARD_CLI_CONFORMANCE);

      I_MASK1 (SQL_STATIC_CURSOR_ATTRIBUTES1, CURSOR_ATTRIBUTES1);

      I_MASK1 (SQL_STATIC_CURSOR_ATTRIBUTES2, CURSOR_ATTRIBUTES2);

      I_STR (SQL_XOPEN_CLI_YEAR);
#endif

    default:
      infoname = "unknown or driver specific";
      break;
    }


  /*
   *  If we arrive here, just print the generic pointer information
   */
  trace_emit ("\t\t%-15.15s   %d (%s)\n",
      "SQLUSMALLINT", fInfoType, infoname);
  if (rgbInfoValue)
    trace_emit ("\t\t%-15.15s   %p\n", "SQLPOINTER", rgbInfoValue);
  else
    trace_emit ("\t\t%-15.15s   0x0\n", "SQLPOINTER");
  goto print_end;


print_int16:
  trace_emit ("\t\t%-15.15s   %d (%s)\n",
      "SQLUSMALLINT", fInfoType, infoname);
  trace_emit ("\t\t%-15.15s   %p (%ld)\n",
      "SQLPOINTER", rgbInfoValue, (long) *((short *) rgbInfoValue));
  goto print_end;


print_int32:
  trace_emit ("\t\t%-15.15s   %d (%s)\n",
  	"SQLUSMALLINT", fInfoType, infoname);
  trace_emit ("\t\t%-15.15s   %p (%ld)\n",
	"SQLPOINTER", rgbInfoValue, (long) *((int *) rgbInfoValue));
  goto print_end;


print_string:
  trace_emit ("\t\t%-15.15s   %d (%s)\n",
  	"SQLUSMALLINT", fInfoType, infoname);
  trace_emit ("\t\t%-15.15s   %p\n",
  	"SQLPOINTER", rgbInfoValue);
  if (waMode == 'A')
    trace_emit_string ((SQLCHAR *) rgbInfoValue, pcbInfoValue ? *pcbInfoValue : SQL_NTS, 0);
  else
    {
      SQLCHAR *str_u8 = dm_SQL_W2A ((SQLWCHAR *) rgbInfoValue, pcbInfoValue ? *pcbInfoValue : SQL_NTS);
      trace_emit_string (str_u8, SQL_NTS, 1);
      free (str_u8);
    }
  goto print_end;


print_mask:
  trace_emit ("\t\t%-15.15s   %d (%s)\n",
  	"SQLUSMALLINT", fInfoType, infoname);
  trace_emit ("\t\t%-15.15s   %p (0x%lX)\n",
	"SQLPOINTER", rgbInfoValue,
	(unsigned long) *((unsigned int *) rgbInfoValue));

  if (*(int *) rgbInfoValue == 0)
    trace_emit ("\t\t\t\t  | %-40.40s |\n", mask[0]);
  else
    {
      register unsigned int val = *(unsigned int *) rgbInfoValue;

      for (i = 1; i < 32; i++)
	{
	  if (val & (1 << (i - 1)))
	    {
	      if (i < elem)
		trace_emit ("\t\t\t\t  | %-40.40s |\n", mask[i]);
	      else
		trace_emit ("\t\t\t\t  | %-40.40s |\n", "UNKNOWN");
	    }
	}
    }
  goto print_end;


print_svalue:
  i = *((short *) rgbInfoValue);
  trace_emit ("\t\t%-15.15s   %d (%s)\n",
	"SQLUSMALLINT", fInfoType, infoname);
  trace_emit ("\t\t%-15.15s   %p (%ld)\n",
	"SQLPOINTER", rgbInfoValue, (long) *((SQLSMALLINT *) rgbInfoValue));
  trace_emit ("\t\t\t\t  | %-40.40s |\n",
  	(i < elem) ? mask[i] : "UNKNOWN");
  goto print_end;


  /*
   *  All done
   */
print_end:
  return;
}


void
trace_SQLGetInfo (int trace_leave, int retcode,
  SQLHDBC		  hdbc,
  SQLUSMALLINT		  fInfoType,
  SQLPOINTER		  rgbInfoValue,
  SQLSMALLINT		  cbInfoValueMax,
  SQLSMALLINT    	* pcbInfoValue)
{
  /* Trace function */
  _trace_print_function (en_GetInfo, trace_leave, retcode);

  /* Trace Arguments */
  _trace_handle (SQL_HANDLE_DBC, hdbc);
  _trace_getinfo (fInfoType, rgbInfoValue, cbInfoValueMax, pcbInfoValue,
  	TRACE_OUTPUT_SUCCESS, 'A');
  _trace_smallint (cbInfoValueMax);
  _trace_smallint_p (pcbInfoValue, TRACE_OUTPUT_SUCCESS);
}


#if ODBCVER >= 0x0300
void
trace_SQLGetInfoW (int trace_leave, int retcode,
  SQLHDBC		  hdbc,
  SQLUSMALLINT		  fInfoType,
  SQLPOINTER		  rgbInfoValue,
  SQLSMALLINT		  cbInfoValueMax,
  SQLSMALLINT    	* pcbInfoValue)
{
  /* Trace function */
  _trace_print_function (en_GetInfoW, trace_leave, retcode);

  /* Trace Arguments */
  _trace_handle (SQL_HANDLE_DBC, hdbc);
  _trace_getinfo (fInfoType, rgbInfoValue, cbInfoValueMax, pcbInfoValue,
  	TRACE_OUTPUT_SUCCESS, 'W');
  _trace_smallint (cbInfoValueMax);
  _trace_smallint_p (pcbInfoValue, TRACE_OUTPUT_SUCCESS);
}
#endif
