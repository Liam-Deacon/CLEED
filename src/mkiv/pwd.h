/*  pwd.h - Try to approximate UN*X's getuser...() functions under MS-DOS.
   Copyright (C) 1990 by Thorsten Ohl, td12@ddagsi3.bitnet

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   $Header: /cvs/Darwin/apache_mod_php/php/win32/pwd.h,v 1.1.1.2 2000/09/07 00:06:44 wsanchez Exp $
 */

/* This 'implementation' is conjectured from the use of this functions in
   the RCS and BASH distributions.  Of course these functions don't do too
   much useful things under MS-DOS, but using them avoids many "#ifdef
   MSDOS" in ported UN*X code ...  */
/*
   #define WIN32_LEAN_AND_MEAN
   #include <windows.h>
 */
#ifndef PWD_H
#define PWD_H
#if 0
/* This is taken care of in Windows-NT/config.h.  */
typedef int uid_t;
#endif

struct passwd {
	char *pw_name;				/* user name */
	char *pw_passwd;			/* encrypted password */
	int pw_uid;					/* user uid */
	int pw_gid;					/* user gid */
	char *pw_comment;			/* comment */
	char *pw_gecos;				/* Honeywell login info */
	char *pw_dir;				/* home directory */
	char *pw_shell;				/* default shell */
};

extern struct passwd *getpwuid(int);
extern struct passwd *getpwnam(char *name);
extern char *getlogin(void);
#endif
/*
 * Local Variables:
 * mode:C
 * ChangeLog:ChangeLog
 * compile-command:make
 * End:
 */