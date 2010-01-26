/*
 * Lasso - A free implementation of the Liberty Alliance specifications.
 *
 * Copyright (C) 2004-2007 Entr'ouvert
 * http://lasso.entrouvert.org
 *
 * Authors: See AUTHORS file in top-level directory.
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
 *
 */

#include <perl.h>
#include <glib.h>
#include <glib-object.h>
#include <lasso/xml/xml.h>

/*
 * Manipulate a pointer to indicate that an SV is undead.
 * Relies on SV pointers being word-aligned.
 */
#define IS_UNDEAD(x) (PTR2UV(x) & 1)
#define MAKE_UNDEAD(x) INT2PTR(void*, PTR2UV(x) | 1)
#define REVIVE_UNDEAD(x) INT2PTR(void*, PTR2UV(x) & ~1)

/* this code is copied / adapted from libglib-perl */
GHashTable *type_to_package;
GQuark wrapper_quark;

extern int lasso_init();

void
init_perl_lasso() {
	type_to_package = g_hash_table_new_full(g_int_hash, g_int_equal, NULL, g_free);
	wrapper_quark = g_quark_from_static_string("PerlLasso::wrapper");
	lasso_init();
}

static const char *
gperl_object_package_from_type (GType gtype)
{
	gchar* package;
	const gchar* type_name;

	if (!g_type_is_a (gtype, G_TYPE_OBJECT) &&
		!g_type_is_a (gtype, G_TYPE_INTERFACE))
		return NULL;

	package = g_hash_table_lookup(type_to_package, (gconstpointer)gtype);
	if (package)
		return package;

	type_name = g_type_name(gtype);
	if (! type_name)
		return NULL;

	if (strncmp(type_name, "Lasso", 5) != 0)
		return NULL;

	package = g_strconcat("Lasso::", &type_name[5], NULL);
	g_hash_table_insert(type_to_package, (gpointer)gtype, (gpointer)package);

	return package;
}

static void
gobject_destroy_wrapper (SV *obj)
{
#ifdef NOISY
	warn ("gobject_destroy_wrapper (%p)[%d]\n", obj,
			SvREFCNT ((SV*)REVIVE_UNDEAD(obj)));
#endif
	obj = REVIVE_UNDEAD(obj);
	sv_unmagic (obj, PERL_MAGIC_ext);

	/* we might want to optimize away the call to DESTROY here for non-perl classes. */
	SvREFCNT_dec (obj);
}

static HV *
gperl_object_stash_from_type (GType gtype)
{
	const char * package = gperl_object_package_from_type (gtype);
	if (package)
		return gv_stashpv (package, TRUE);
	else
		return NULL;
}

static void
update_wrapper (GObject *object, gpointer obj)
{
#ifdef NOISY
	warn("update_wrapper [%p] (%p)\n", object, obj); */
#endif
        g_object_steal_qdata (object, wrapper_quark);
        g_object_set_qdata_full (object,
                                 wrapper_quark,
                                 obj,
                                 (GDestroyNotify)gobject_destroy_wrapper);
}

SV *
gperl_new_object (GObject * object,
                  gboolean own)
{
	SV *obj;
	SV *sv;

	/* take the easy way out if we can */
	if (!object) {
		return &PL_sv_undef;
	}

	if (!LASSO_IS_NODE (object))
		croak ("object %p is not really a LassoNode", object);

	/* fetch existing wrapper_data */
	obj = (SV *)g_object_get_qdata (object, wrapper_quark);

	if (!obj) {
		/* create the perl object */
		GType gtype = G_OBJECT_TYPE (object);

		HV *stash = gperl_object_stash_from_type (gtype);

		/* We should only get NULL for the stash here if gtype is
		 * neither a GObject nor GInterface.  We filtered out all
		 * non-GObject types a few lines back. */
		g_assert (stash != NULL);

		/*
		 * Create the "object", a hash.
		 *
		 * This does not need to be a HV, the only problem is finding
		 * out what to use, and HV is certainly the way to go for any
		 * built-in objects.
		 */

		/* this increases the combined object's refcount. */
		obj = (SV *)newHV ();
		/* attach magic */
		sv_magic (obj, 0, PERL_MAGIC_ext, (const char *)object, 0);

		/* The SV has a ref to the C object.  If we are to own this
		 * object, then any other references will be taken care of
		 * below in take_ownership */
		g_object_ref (object);

		/* create the wrapper to return, the _noinc decreases the
		 * combined refcount by one. */
		sv = newRV_noinc (obj);

		/* bless into the package */
		sv_bless (sv, stash);

		/* attach it to the gobject */
		update_wrapper (object, obj);
		/* printf("creating new wrapper for [%p] (%p)\n", object, obj); */

		/* the noinc is so that the SV (initially) exists only as long
		 * as the perl code needs it.  When the DESTROY gets called, we
		 * check and see if the SV is the only referer to the C object,
		 * and if so remove both.  Otherwise, the SV will become
		 * "undead," to be either revived or destroyed with the C
		 * object */

#ifdef NOISY
		warn ("gperl_new_object%d %s(%p)[%d] => %s (%p) (NEW)\n", own,
				G_OBJECT_TYPE_NAME (object), object, object->ref_count,
				gperl_object_package_from_type (G_OBJECT_TYPE (object)),
				SvRV (sv));
#endif
	} else {
		/* create the wrapper to return, increases the combined
		 * refcount by one. */

		/* if the SV is undead, revive it */
		if (IS_UNDEAD(obj)) {
			g_object_ref (object);
			obj = REVIVE_UNDEAD(obj);
			update_wrapper (object, obj);
			sv = newRV_noinc (obj);
			/* printf("reviving undead wrapper for [%p] (%p)\n", object, obj); */
		} else {
			/* printf("reusing previous wrapper for %p\n", obj); */
			sv = newRV_inc (obj);
		}
	}

#ifdef NOISY
	warn ("gperl_new_object%d %s(%p)[%d] => %s (%p)[%d] (PRE-OWN)\n", own,
			G_OBJECT_TYPE_NAME (object), object, object->ref_count,
			gperl_object_package_from_type (G_OBJECT_TYPE (object)),
			SvRV (sv), SvREFCNT (SvRV (sv)));
#endif
	if (own)
		g_object_unref(object);

	return sv;
}

GObject *
gperl_get_object (SV * sv)
{
	MAGIC *mg;

	if (!sv || !SvOK(sv) || !SvROK (sv) || !(mg = mg_find (SvRV (sv), PERL_MAGIC_ext)))
		return NULL;
	if (! mg->mg_ptr)
		return NULL;
	if (! G_IS_OBJECT(mg->mg_ptr))
		return NULL;
	return (GObject *) mg->mg_ptr;
}