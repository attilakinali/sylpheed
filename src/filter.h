/*
 * Sylpheed -- a GTK+ based, lightweight, and fast e-mail client
 * Copyright (C) 1999-2005 Hiroyuki Yamamoto
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __FILTER_H__
#define __FILTER_H__

#include <glib.h>

#include "folder.h"
#include "procmsg.h"
#include "utils.h"

typedef struct _FilterCond	FilterCond;
typedef struct _FilterAction	FilterAction;
typedef struct _FilterRule	FilterRule;
typedef struct _FilterInfo	FilterInfo;

typedef enum
{
	FLT_TIMING_ANY,
	FLT_TIMING_ON_RECEIVE,
	FLT_TIMING_MANUAL
} FilterTiming;

typedef enum
{
	FLT_COND_HEADER,
	FLT_COND_ANY_HEADER,
	FLT_COND_TO_OR_CC,
	FLT_COND_BODY,
	FLT_COND_CMD_TEST,
	FLT_COND_SIZE_GREATER,
	FLT_COND_AGE_GREATER,
	FLT_COND_ACCOUNT
} FilterCondType;

typedef enum
{
	FLT_CONTAIN,
	FLT_EQUAL,
	FLT_REGEX
} FilterMatchType;

typedef enum
{
	FLT_NOT_MATCH	= 1 << 0,
	FLT_CASE_SENS	= 1 << 1
} FilterMatchFlag;

typedef enum
{
	FLT_OR,
	FLT_AND
} FilterBoolOp;

typedef enum
{
	FLT_ACTION_MOVE,
	FLT_ACTION_COPY,
	FLT_ACTION_NOT_RECEIVE,
	FLT_ACTION_DELETE,
	FLT_ACTION_EXEC,
	FLT_ACTION_EXEC_ASYNC,
	FLT_ACTION_MARK,
	FLT_ACTION_COLOR_LABEL,
	FLT_ACTION_MARK_READ,
	FLT_ACTION_FORWARD,
	FLT_ACTION_FORWARD_AS_ATTACHMENT,
	FLT_ACTION_REDIRECT,
	FLT_ACTION_STOP_EVAL,
	FLT_ACTION_NONE
} FilterActionType;

#define FLT_IS_NOT_MATCH(flag)	((flag & FLT_NOT_MATCH) != 0)
#define FLT_IS_CASE_SENS(flag)	((flag & FLT_CASE_SENS) != 0)

struct _FilterCond
{
	FilterCondType type;

	gchar *header_name;

	gchar *str_value;
	gint int_value;

	FilterMatchType match_type;
	FilterMatchFlag match_flag;

	StrFindFunc match_func;
};

struct _FilterAction
{
	FilterActionType type;

	gchar *str_value;
	gint int_value;
};

struct _FilterRule
{
	gchar *name;

	FilterBoolOp bool_op;

	GSList *cond_list;
	GSList *action_list;

	FilterTiming timing;
	gboolean enabled;
};

struct _FilterInfo
{
	PrefsAccount *account;
	MsgFlags flags;

	gboolean actions[FLT_ACTION_NONE];
	GSList *dest_list;
	FolderItem *move_dest;
	gboolean drop_done;
};

gint filter_apply			(GSList			*fltlist,
					 const gchar		*file,
					 FilterInfo		*fltinfo);
gint filter_apply_msginfo		(GSList			*fltlist,
					 MsgInfo		*msginfo,
					 FilterInfo		*fltinfo);

gint filter_action_exec			(FilterRule		*rule,
					 MsgInfo		*msginfo,
					 const gchar		*file,
					 FilterInfo		*fltinfo);

gboolean filter_match_rule		(FilterRule		*rule,
					 MsgInfo		*msginfo,
					 GSList			*hlist,
					 FilterInfo		*fltinfo);

/* read / write config */
GSList *filter_xml_node_to_filter_list	(GNode			*node);
void filter_read_config			(void);
void filter_write_config		(void);

/* for old filterrc */
gchar *filter_get_str			(FilterRule		*rule);
FilterRule *filter_read_str		(const gchar		*str);

FilterRule *filter_rule_new		(const gchar		*name,
					 FilterBoolOp		 bool_op,
					 GSList			*cond_list,
					 GSList			*action_list);
FilterCond *filter_cond_new		(FilterCondType		 type,
					 FilterMatchType	 match_type,
					 FilterMatchFlag	 match_flag,
					 const gchar		*header,
					 const gchar		*body);
FilterAction *filter_action_new		(FilterActionType	 type,
					 const gchar		*str);
FilterInfo *filter_info_new		(void);

void filter_rule_rename_dest_path	(FilterRule		*rule,
					 const gchar		*old_path,
					 const gchar		*new_path);
void filter_rule_delete_action_by_dest_path
					(FilterRule		*rule,
					 const gchar		*path);

void filter_list_rename_path		(const gchar		*old_path,
					 const gchar		*new_path);
void filter_list_delete_path		(const gchar		*path);

void filter_rule_match_type_str_to_enum	(const gchar		*type_str,
					 FilterMatchType	*type,
					 FilterMatchFlag	*flag);

void filter_rule_list_free		(GSList			*fltlist);
void filter_rule_free			(FilterRule		*rule);
void filter_cond_list_free		(GSList			*cond_list);
void filter_action_list_free		(GSList			*action_list);
void filter_info_free			(FilterInfo		*info);

#endif /* __FILTER_H__ */
