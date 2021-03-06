/*
  Copyright (c) 2007-2010 Gluster, Inc. <http://www.gluster.com>
  This file is part of GlusterFS.

  GlusterFS is free software; you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as published
  by the Free Software Foundation; either version 3 of the License,
  or (at your option) any later version.

  GlusterFS is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.
*/

#ifndef _CONFIG_H
#define _CONFIG_H
#include "config.h"
#endif

#include <fnmatch.h>
#include "authenticate.h"

auth_result_t gf_auth (dict_t *input_params, dict_t *config_params)
{
        auth_result_t  result  = AUTH_DONT_CARE;
        int      ret           = 0;
        data_t  *allow_user    = NULL;
        data_t  *username_data = NULL;
        data_t  *passwd_data   = NULL;
        data_t  *password_data = NULL;
        char    *username      = NULL;
        char    *password      = NULL;
        char    *brick_name    = NULL;
        char    *searchstr     = NULL;
        char    *username_str  = NULL;
        char    *tmp           = NULL;
        char    *username_cpy  = NULL;

        username_data = dict_get (input_params, "username");
        if (!username_data) {
                gf_log ("auth/login", GF_LOG_DEBUG,
                        "username not found, returning DONT-CARE");
                goto out;
        }

        username = data_to_str (username_data);

        password_data = dict_get (input_params, "password");
        if (!password_data) {
                gf_log ("auth/login", GF_LOG_WARNING,
                        "password not found, returning DONT-CARE");
                goto out;
        }

        password = data_to_str (password_data);

        brick_name = data_to_str (dict_get (input_params, "remote-subvolume"));
        if (!brick_name) {
                gf_log ("auth/login", GF_LOG_ERROR,
                        "remote-subvolume not specified");
                result = AUTH_REJECT;
                goto out;
        }

        ret = gf_asprintf (&searchstr, "auth.login.%s.allow", brick_name);
        if (-1 == ret) {
                gf_log ("auth/login", GF_LOG_WARNING,
                        "asprintf failed while setting search string, "
                        "returning DONT-CARE");
                goto out;
        }

        allow_user = dict_get (config_params, searchstr);
        GF_FREE (searchstr);

        if (allow_user) {
                username_cpy = gf_strdup (allow_user->data);
                if (!username_cpy)
                        goto out;

                username_str = strtok_r (username_cpy, " ,", &tmp);

                while (username_str) {
                        if (!fnmatch (username_str, username, 0)) {
                                ret = gf_asprintf (&searchstr,
                                                   "auth.login.%s.password",
                                                   username);
                                if (-1 == ret) {
                                        gf_log ("auth/login", GF_LOG_WARNING,
                                                "asprintf failed while setting search string");
                                        goto out;
                                }
                                passwd_data = dict_get (config_params, searchstr);
                                GF_FREE (searchstr);

                                if (!passwd_data) {
                                        gf_log ("auth/login", GF_LOG_ERROR,
                                                "wrong username/password combination");
                                        result = AUTH_REJECT;
                                        goto out;
                                }

                                result = !((strcmp (data_to_str (passwd_data),
                                                    password)) ?
                                           AUTH_ACCEPT :
                                           AUTH_REJECT);
                                if (result == AUTH_REJECT)
                                        gf_log ("auth/login", GF_LOG_ERROR,
                                                "wrong password for user %s",
                                                username);

                                break;
                        }
                        username_str = strtok_r (NULL, " ,", &tmp);
                }
        }

out:
        if (username_cpy)
                GF_FREE (username_cpy);

        return result;
}

struct volume_options options[] = {
        { .key   = {"auth.login.*.allow"},
          .type  = GF_OPTION_TYPE_ANY
        },
        { .key   = {"auth.login.*.password"},
          .type  = GF_OPTION_TYPE_ANY
        },
        { .key = {NULL} }
};
