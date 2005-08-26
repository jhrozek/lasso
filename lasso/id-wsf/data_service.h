/* $Id$ 
 *
 * Lasso - A free implementation of the Liberty Alliance specifications.
 *
 * Copyright (C) 2004, 2005 Entr'ouvert
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
 */

#ifndef __LASSO_DATA_SERVICE_H__
#define __LASSO_DATA_SERVICE_H__

#ifdef __cplusplus
extern "C" {

#endif /* __cplusplus */ 

#include <lasso/id-wsf/wsf_profile.h>
#include <lasso/xml/disco_resource_id.h>
#include <lasso/xml/disco_encrypted_resource_id.h>
#include <lasso/xml/dst_data.h>
#include <lasso/xml/dst_modification.h>
#include <lasso/xml/dst_query_item.h>
#include <lasso/xml/disco_resource_offering.h>
#include <lasso/xml/xml.h>

#define LASSO_TYPE_PROFILE_SERVICE (lasso_data_service_get_type())
#define LASSO_DATA_SERVICE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), \
       LASSO_TYPE_PROFILE_SERVICE, LassoDataService))
#define LASSO_DATA_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), \
       LASSO_TYPE_PROFILE_SERVICE, LassoDataServiceClass))
#define LASSO_IS_PROFILE_SERVICE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), \
       LASSO_TYPE_PROFILE_SERVICE))
#define LASSO_IS_PROFILE_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
       LASSO_TYPE_PROFILE_SERVICE))
#define LASSO_DATA_SERVICE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), \
       LASSO_TYPE_PROFILE_SERVICE, LassoDataServiceClass)) 

typedef struct _LassoDataService LassoDataService;
typedef struct _LassoDataServiceClass LassoDataServiceClass;
typedef struct _LassoDataServicePrivate LassoDataServicePrivate;

struct _LassoDataService {
	LassoWsfProfile parent;

	/*< public >*/
	LassoDiscoResourceID *resource_id;
	LassoDiscoEncryptedResourceID *encrypted_resource_id;
	xmlNode *resource_data;

	/*< private >*/
	LassoDataServicePrivate *private_data;
};

struct _LassoDataServiceClass {
	LassoWsfProfileClass parent;
};


LASSO_EXPORT GType lasso_data_service_get_type(void);

LASSO_EXPORT LassoDataService* lasso_data_service_new(LassoServer *server);

LASSO_EXPORT LassoDataService* lasso_data_service_new_full(LassoServer *server,
		LassoDiscoResourceOffering *offering);

LASSO_EXPORT gint lasso_data_service_init_query(LassoDataService *service,
		const char *select, const char *item_id);

LASSO_EXPORT LassoDstQueryItem* lasso_data_service_add_query_item(LassoDataService *service,
		const char *select, const char *item_id);

LASSO_EXPORT gint lasso_data_service_process_query_msg(LassoDataService *service,
		const char *message);

LASSO_EXPORT gint lasso_data_service_build_modify_response_msg(LassoDataService *service);

LASSO_EXPORT gint lasso_data_service_build_response_msg(LassoDataService *service);

LASSO_EXPORT gint lasso_data_service_process_query_response_msg(LassoDataService *service,
		const char *message);

LASSO_EXPORT xmlNode* lasso_data_service_get_answer(LassoDataService *service,
		const char *select);

LASSO_EXPORT xmlNode* lasso_data_service_get_answer_for_item_id(LassoDataService *service,
		const char *item_id);

LASSO_EXPORT  gint lasso_data_service_init_modify(LassoDataService *service,
	const gchar *select, xmlNode *newData);

LASSO_EXPORT LassoDstModification* lasso_data_service_add_modification(
	LassoDataService *service, const gchar *select);

LASSO_EXPORT gint lasso_data_service_process_modify_msg(LassoDataService *service,
	const gchar *soap_msg);

LASSO_EXPORT gint lasso_data_service_process_modify_response_msg(LassoDataService *service,
	const gchar *soap_msg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LASSO_DATA_SERVICE_H__ */