/* $Id$
 *
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LASSO_SAMLP2_EXTENSIONS_H__
#define __LASSO_SAMLP2_EXTENSIONS_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "../xml.h"

#define LASSO_TYPE_SAMLP2_EXTENSIONS (lasso_samlp2_extensions_get_type())
#define LASSO_SAMLP2_EXTENSIONS(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), LASSO_TYPE_SAMLP2_EXTENSIONS, \
				LassoSamlp2Extensions))
#define LASSO_SAMLP2_EXTENSIONS_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass), LASSO_TYPE_SAMLP2_EXTENSIONS, \
				LassoSamlp2ExtensionsClass))
#define LASSO_IS_SAMLP2_EXTENSIONS(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj), LASSO_TYPE_SAMLP2_EXTENSIONS))
#define LASSO_IS_SAMLP2_EXTENSIONS_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), LASSO_TYPE_SAMLP2_EXTENSIONS))
#define LASSO_SAMLP2_EXTENSIONS_GET_CLASS(o) \
	(G_TYPE_INSTANCE_GET_CLASS ((o), LASSO_TYPE_SAMLP2_EXTENSIONS, \
				LassoSamlp2ExtensionsClass))

typedef struct _LassoSamlp2Extensions LassoSamlp2Extensions;
typedef struct _LassoSamlp2ExtensionsClass LassoSamlp2ExtensionsClass;


struct _LassoSamlp2Extensions {
	LassoNode parent;

	/*< public >*/
};


struct _LassoSamlp2ExtensionsClass {
	LassoNodeClass parent;
};

LASSO_EXPORT GType lasso_samlp2_extensions_get_type(void);
LASSO_EXPORT LassoNode* lasso_samlp2_extensions_new(void);
LASSO_EXPORT GList* lasso_samlp2_extensions_get_any(LassoSamlp2Extensions *extensions);
LASSO_EXPORT void lasso_samlp2_extensions_set_any(LassoSamlp2Extensions *extensions, GList *any);
LASSO_EXPORT GHashTable* lasso_samlp2_extensions_get_attributes(LassoSamlp2Extensions *extensions);
LASSO_EXPORT void lasso_samlp2_extensions_set_attributes(LassoSamlp2Extensions *extensions, GHashTable *attributes);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LASSO_SAMLP2_EXTENSIONS_H__ */
