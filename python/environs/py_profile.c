/* $Id$ 
 *
 * PyLasso -- Python bindings for Lasso library
 *
 * Copyright (C) 2004 Entr'ouvert
 * http://lasso.labs.libre-entreprise.org
 * 
 * Authors: Valery Febvre <vfebvre@easter-eggs.com>
 *          Nicolas Clapies <nclapies@entrouvert.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "../lassomod.h"
#include "py_profile.h"
#include "py_identity.h"
#include "py_session.h"
#include "py_server.h"

PyObject *LassoProfile_wrap(LassoProfile *ctx) {
  PyObject *ret;

  if (ctx == NULL) {
    Py_INCREF(Py_None);
    return (Py_None);
  }
  ret = PyCObject_FromVoidPtrAndDesc((void *) ctx,
                                     (char *) "LassoProfile *", NULL);
  return (ret);
}

/******************************************************************************/

PyObject *profile_get_request_type_from_soap_msg(PyObject *self, PyObject *args) {
  gchar *soap_buffer;
  gint   type;

  if (CheckArgs(args, "S:profile_get_request_type_from_soap_msg")) {
    if(!PyArg_ParseTuple(args, (char *) "s:profile_get_request_type_from_soap_msg",
			 &soap_buffer))
      return NULL;
  }
  else return NULL;

  type = lasso_profile_get_request_type_from_soap_msg(soap_buffer);

  return(int_wrap(type));
}

/******************************************************************************/

PyObject *profile_new(PyObject *self, PyObject *args) {
  PyObject *server_obj, *identity_obj, *session_obj;
  LassoProfile *ctx;
  LassoIdentity *identity = NULL;
  LassoSession *session = NULL;

  if (CheckArgs(args, "Ooo:profile_new")) {
    if(!PyArg_ParseTuple(args, (char *) "O|OO:profile_new",
			 &server_obj, &identity_obj, &session_obj))
      return NULL;
  }
  else return NULL;

  if (identity_obj != Py_None) {
    identity = LassoIdentity_get(identity_obj);
  }
  if (session_obj != Py_None) {
    session = LassoSession_get(session_obj);
  }
  ctx = lasso_profile_new(LassoServer_get(server_obj),
			  identity, session);

  return (LassoProfile_wrap(ctx));
}

PyObject *profile_get_identity(PyObject *self, PyObject *args) {
  PyObject *ctx_obj;
  LassoIdentity *identity;

  if (CheckArgs(args, "O:profile_get_identity")) {
    if(!PyArg_ParseTuple(args, (char *) "O:profile_get_identity",
			 &ctx_obj))
      return NULL;
  }
  else return NULL;

  identity = lasso_profile_get_identity(LassoProfile_get(ctx_obj));

  return(LassoIdentity_wrap(identity));
}

PyObject *profile_get_session(PyObject *self, PyObject *args) {
  PyObject *ctx_obj;
  LassoSession *session;

  if (CheckArgs(args, "O:profile_get_session")) {
    if(!PyArg_ParseTuple(args, (char *) "O:profile_get_session",
			 &ctx_obj))
      return NULL;
  }
  else return NULL;

  session = lasso_profile_get_session(LassoProfile_get(ctx_obj));

  return(LassoSession_wrap(session));
}

PyObject *profile_is_identity_dirty(PyObject *self, PyObject *args) {
  PyObject *ctx_obj;
  int ret;

  if (CheckArgs(args, "O:profile_is_identity_dirty")) {
    if(!PyArg_ParseTuple(args, (char *) "O:profile_is_identity_dirty",
			 &ctx_obj))
      return NULL;
  }
  else return NULL;

  ret = lasso_profile_is_identity_dirty(LassoProfile_get(ctx_obj));

  return(int_wrap(ret));
}

PyObject *profile_is_session_dirty(PyObject *self, PyObject *args) {
  PyObject *ctx_obj;
  int ret;

  if (CheckArgs(args, "O:profile_is_session_dirty")) {
    if(!PyArg_ParseTuple(args, (char *) "O:profile_is_session_dirty",
			 &ctx_obj))
      return NULL;
  }
  else return NULL;

  ret = lasso_profile_is_session_dirty(LassoProfile_get(ctx_obj));

  return(int_wrap(ret));
}

PyObject *profile_set_identity(PyObject *self, PyObject *args) {
  PyObject *ctx_obj, *identity_obj;
  gint ret;

  if (CheckArgs(args, "OO:profile_set_identity")) {
    if(!PyArg_ParseTuple(args, (char *) "OO:profile_set_identity",
			 &ctx_obj, &identity_obj))
      return NULL;
  }
  else return NULL;

  ret = lasso_profile_set_identity(LassoProfile_get(ctx_obj),
				   LassoIdentity_get(identity_obj));

  return(int_wrap(ret));
}

PyObject *profile_set_identity_from_dump(PyObject *self, PyObject *args) {
  PyObject *ctx_obj;
  gchar *dump;
  gint   ret;

  if (CheckArgs(args, "OS:profile_set_identity_from_dump")) {
    if(!PyArg_ParseTuple(args, (char *) "Os:profile_set_identity_from_dump",
			 &ctx_obj, &dump))
      return NULL;
  }
  else return NULL;

  ret = lasso_profile_set_identity_from_dump(LassoProfile_get(ctx_obj),
					     dump);

  return(int_wrap(ret));
}

PyObject *profile_set_session(PyObject *self, PyObject *args) {
  PyObject *ctx_obj, *session_obj;
  gint ret;

  if (CheckArgs(args, "OO:profile_set_session")) {
    if(!PyArg_ParseTuple(args, (char *) "OO:profile_set_session",
			 &ctx_obj, &session_obj))
      return NULL;
  }
  else return NULL;

  ret = lasso_profile_set_session(LassoProfile_get(ctx_obj),
				  LassoSession_get(session_obj));

  return(int_wrap(ret));
}

PyObject *profile_set_session_from_dump(PyObject *self, PyObject *args) {
  PyObject *ctx_obj;
  gchar *dump;
  gint   ret;

  if (CheckArgs(args, "OS:profile_set_session_from_dump")) {
    if(!PyArg_ParseTuple(args, (char *) "Os:profile_set_session_from_dump",
			 &ctx_obj, &dump))
      return NULL;
  }
  else return NULL;

  ret = lasso_profile_set_session_from_dump(LassoProfile_get(ctx_obj),
					    dump);

  return(int_wrap(ret));
}
