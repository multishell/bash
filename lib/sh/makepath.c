/* makepath.c - glue PATH and DIR together into a full pathname. */

/* Copyright (C) 1987, 1989, 1991 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   Bash is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License along
   with Bash; see the file COPYING.  If not, write to the Free Software
   Foundation, 59 Temple Place, Suite 330, Boston, MA 02111 USA. */

#include <config.h>

#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "bashansi.h"
#include "shell.h"

#include <tilde/tilde.h>

#ifndef NULL
#  define NULL 0
#endif

/* MAKE SURE THESE AGREE WITH ../../externs.h. */

#ifndef MP_DOTILDE
#  define MP_DOTILDE	0x01
#  define MP_DOCWD	0x02
#  define MP_RMDOT	0x04
#endif

extern char *get_working_directory __P((char *));

/* Take PATH, an element from, e.g., $CDPATH, and DIR, a directory name,
   and paste them together into PATH/DIR.  Tilde expansion is performed on
   PATH if (flags & MP_DOTILDE) is non-zero.  If PATH is NULL or the empty
   string, it is converted to the current directory.  A full pathname is
   used if (flags & MP_DOCWD) is non-zero, otherwise `./' is used.  If
   (flags & MP_RMDOT) is non-zero, any `./' is removed from the beginning
   of DIR. */

#define MAKEDOT() \
  do { \
    xpath = xmalloc (2); \
    xpath[0] = '.'; \
    xpath[1] = '\0'; \
    pathlen = 1; \
  } while (0)

char *
sh_makepath (path, dir, flags)
     char *path, *dir;
     int flags;
{
  int dirlen, pathlen;
  char *ret, *xpath;

  if (path == 0 || *path == '\0')
    {
      if (flags & MP_DOCWD)
	{
	  xpath = get_working_directory ("sh_makepath");
	  if (xpath == 0)
	    {
	      ret = get_string_value ("PWD");
	      if (ret)
		xpath = savestring (ret);
	    }
	  if (xpath == 0)
	    MAKEDOT();
	  else
	    pathlen = strlen (xpath);
	}
      else
	MAKEDOT();
    }
  else
    {
      xpath = ((flags & MP_DOTILDE) && *path == '~') ? bash_tilde_expand (path) : path;
      pathlen = strlen (xpath);
    }

  dirlen = strlen (dir);
  if ((flags & MP_RMDOT) && dir[0] == '.' && dir[1] == '/')
    {
      dir += 2;
      dirlen -= 2;
    }

  ret = xmalloc (2 + dirlen + pathlen);
  strcpy (ret, xpath);
  if (xpath[pathlen - 1] != '/')
    {
      ret[pathlen++] = '/';
      ret[pathlen] = '\0';
    }
  strcpy (ret + pathlen, dir);
  if (xpath != path)
    free (xpath);
  return (ret);
}
