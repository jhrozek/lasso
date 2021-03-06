/* $Id: dstref_query_item.c,v 1.0 2005/10/14 15:17:55 fpeters Exp $
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

#include "../private.h"
#include "dstref_query_item.h"
#include "idwsf2_strings.h"

/**
 * SECTION:dstref_query_item
 * @short_description: &lt;dstref:QueryItem&gt;
 *
 * <figure><title>Schema fragment for dstref:QueryItem</title>
 * <programlisting><![CDATA[
 *
 * <xs:complexType name="QueryItemType">
 *   <xs:complexContent>
 *     <xs:extension base="dstref:ResultQueryType">
 *       <xs:attributeGroup ref="dst:PaginationAttributeGroup"/>
 *     </xs:extension>
 *   </xs:complexContent>
 * </xs:complexType>
 * ]]></programlisting>
 * </figure>
 */

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/


static struct XmlSnippet schema_snippets[] = {
	{ "count", SNIPPET_ATTRIBUTE | SNIPPET_INTEGER | SNIPPET_OPTIONAL_NEG,
		G_STRUCT_OFFSET(LassoIdWsf2DstRefQueryItem, count), NULL, NULL, NULL},
	{ "offset", SNIPPET_ATTRIBUTE | SNIPPET_INTEGER | SNIPPET_OPTIONAL_NEG,
		G_STRUCT_OFFSET(LassoIdWsf2DstRefQueryItem, offset), NULL, NULL, NULL},
	{ "setID", SNIPPET_ATTRIBUTE | SNIPPET_OPTIONAL,
		G_STRUCT_OFFSET(LassoIdWsf2DstRefQueryItem, setID), NULL, NULL, NULL},
	{ "setReq", SNIPPET_ATTRIBUTE,
		G_STRUCT_OFFSET(LassoIdWsf2DstRefQueryItem, setReq), NULL, NULL, NULL},
	{NULL, 0, 0, NULL, NULL, NULL}
};

static LassoNodeClass *parent_class = NULL;


/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/

static void
instance_init(LassoIdWsf2DstRefQueryItem *node)
{
	node->count = -1;
	node->offset = -1;
}

static void
class_init(LassoIdWsf2DstRefQueryItemClass *klass, void *unused G_GNUC_UNUSED)
{
	LassoNodeClass *nclass = LASSO_NODE_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);
	nclass->node_data = g_new0(LassoNodeClassData, 1);
	lasso_node_class_set_nodename(nclass, "QueryItem");
	lasso_node_class_set_ns(nclass, LASSO_IDWSF2_DSTREF_HREF, LASSO_IDWSF2_DSTREF_PREFIX);
	lasso_node_class_add_snippets(nclass, schema_snippets);
}

GType
lasso_idwsf2_dstref_query_item_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoIdWsf2DstRefQueryItemClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoIdWsf2DstRefQueryItem),
			0,
			(GInstanceInitFunc) instance_init,
			NULL
		};

		this_type = g_type_register_static(LASSO_TYPE_IDWSF2_DSTREF_RESULT_QUERY,
				"LassoIdWsf2DstRefQueryItem", &this_info, 0);
	}
	return this_type;
}

/**
 * lasso_idwsf2_dstref_query_item_new:
 *
 * Creates a new #LassoIdWsf2DstRefQueryItem object.
 *
 * Return value: a newly created #LassoIdWsf2DstRefQueryItem object
 **/
LassoIdWsf2DstRefQueryItem*
lasso_idwsf2_dstref_query_item_new()
{
	return g_object_new(LASSO_TYPE_IDWSF2_DSTREF_QUERY_ITEM, NULL);
}


LassoIdWsf2DstRefQueryItem*
lasso_idwsf2_dstref_query_item_new_full(const gchar *item_xpath, const gchar *item_id)
{
	LassoIdWsf2DstRefResultQuery *item_result_query = LASSO_IDWSF2_DSTREF_RESULT_QUERY(
		lasso_idwsf2_dstref_query_item_new());
	LassoIdWsf2DstResultQueryBase *item_result_query_base = LASSO_IDWSF2_DST_RESULT_QUERY_BASE(
		item_result_query);

	item_result_query->Select = g_strdup(item_xpath);
	item_result_query_base->itemID = g_strdup(item_id);

	return LASSO_IDWSF2_DSTREF_QUERY_ITEM(item_result_query);
}
