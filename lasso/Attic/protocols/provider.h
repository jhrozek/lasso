/* $Id$ 
 *
 * Lasso - A free implementation of the Liberty Alliance specifications.
 *
 * Copyright (C) 2004 Entr'ouvert
 * http://lasso.entrouvert.org
 * 
 * Author: Valery Febvre <vfebvre@easter-eggs.com>
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

#ifndef __LASSO_PROVIDER_H__
#define __LASSO_PROVIDER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 

#include <lasso/xml/xml.h>

#define LASSO_TYPE_PROVIDER (lasso_provider_get_type())
#define LASSO_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), LASSO_TYPE_PROVIDER, LassoProvider))
#define LASSO_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LASSO_TYPE_PROVIDER, LassoProviderClass))
#define LASSO_IS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), LASSO_TYPE_PROVIDER))
#define LASSP_IS_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LASSO_TYPE_PROVIDER))
#define LASSO_PROVIDER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), LASSO_TYPE_PROVIDER, LassoProviderClass)) 

typedef struct _LassoProvider LassoProvider;
typedef struct _LassoProviderClass LassoProviderClass;

struct _LassoProvider {
  LassoNode parent;

  LassoNode *metadata;

  gchar *public_key;
  gchar *certificate;

  /*< private >*/
};

struct _LassoProviderClass {
  LassoNodeClass parent;
};

LASSO_EXPORT GType          lasso_provider_get_type                         (void);
LASSO_EXPORT LassoProvider* lasso_provider_new                              (gchar       *metadata,
									     const gchar *public_key,
									     const gchar *certificate);

LASSO_EXPORT xmlChar       *lasso_provider_get_providerID                   (LassoProvider *provider);

LASSO_EXPORT xmlChar       *lasso_provider_get_singleSignOnProtocolProfile  (LassoProvider *provider);
LASSO_EXPORT xmlChar       *lasso_provider_get_singleSignOnServiceUrl       (LassoProvider *provider);

LASSO_EXPORT xmlChar       *lasso_provider_get_singleLogoutProtocolProfile  (LassoProvider *provider);
LASSO_EXPORT xmlChar       *lasso_provider_get_singleLogoutServiceUrl       (LassoProvider *provider);
LASSO_EXPORT xmlChar       *lasso_provider_get_singleLogoutServiceReturnUrl (LassoProvider *provider);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LASSO_PROVIDER_H__ */
