// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef SEEN_SYS_H
#define SEEN_SYS_H

/*
 * System abstraction utility routines
 *
 * Authors:
 *   Jon A. Cruz <jon@joncruz.org>
 *
 * Copyright (C) 2004-2005 Authors
 *
 * Released under GNU GPL v2+, read the file 'COPYING' for more information.
 */

#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <glib.h>
#include <glibmm/spawn.h>
#include <string>
#include <vector>

/*#####################
## U T I L I T Y
#####################*/

namespace Inkscape::IO {

void dump_fopen_call( char const *utf8name, char const *id );

FILE *fopen_utf8name( char const *utf8name, char const *mode );

int mkdir_utf8name( char const *utf8name );

bool file_test( char const *utf8name, GFileTest test );

bool file_directory_exists( char const *utf8name );

bool file_is_writable( char const *utf8name);

GDir *dir_open(gchar const *utf8name, guint flags, GError **error);

gchar *dir_read_utf8name(GDir *dir);

gchar* locale_to_utf8_fallback( const gchar *opsysstring,
				gssize len,
				gsize *bytes_read,
				gsize *bytes_written,
				GError **error );

Glib::ustring sanitizeString(char const *str);

void spawn_async_with_pipes (const std::string& working_directory,
                             const std::vector<std::string>& argv,
                             Glib::SpawnFlags flags,
                             const sigc::slot<void ()>& child_setup,
                             Glib::Pid* child_pid,
                             int* standard_input,
                             int* standard_output,
                             int* standard_error);

Glib::ustring get_file_extension(Glib::ustring path);

} // namespace Inkscape::IO

#endif // SEEN_SYS_H
