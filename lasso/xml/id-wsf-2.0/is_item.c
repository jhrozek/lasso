/* $Id: is_item.c,v 1.0 2005/10/14 15:17:55 fpeters Exp $
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
#include "is_item.h"
#include "idwsf2_strings.h"

/**
 * SECTION:is_item
 * @short_description: &lt;is:Item&gt;
 *
 * <figure><title>Schema fragment for is:Item</title>
 * <programlisting><![CDATA[
 *
 * <xs:element name="Item" minOccurs="2" maxOccurs="unbounded">
 *   <xs:complexType>
 *     <xs:sequence>
 *       <xs:element ref="Hint" minOccurs="0"/>
 *     </xs:sequence>
 *     <xs:attribute name="label" type="xs:string" use="optional"/>
 *     <xs:attribute name="value" type="xs:NMTOKEN" use="required"/>
 *   </xs:complexType>
 * </xs:element>
 * ]]></programlisting>
 * </figure>
 */

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/


static struct XmlSnippet schema_snippets[] = {
	{ "Hint", SNIPPET_CONTENT,
		G_STRUCT_OFFSET(LassoIdWsf2IsItem, Hint), NULL, NULL, NULL},
	{ "label", SNIPPET_ATTRIBUTE | SNIPPET_OPTIONAL,
		G_STRUCT_OFFSET(LassoIdWsf2IsItem, label), NULL, NULL, NULL},
	{ "value", SNIPPET_ATTRIBUTE,
		G_STRUCT_OFFSET(LassoIdWsf2IsItem, value), NULL, NULL, NULL},
	{NULL, 0, 0, NULL, NULL, NULL}
};

static LassoNodeClass *parent_class = NULL;


/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/


static void
class_init(LassoIdWsf2IsItemClass *klass, void *unused G_GNUC_UNUSED)
{
	LassoNodeClass *nclass = LASSO_NODE_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);
	nclass->node_data = g_new0(LassoNodeClassData, 1);
	lasso_node_class_set_nodename(nclass, "Item");
	lasso_node_class_set_ns(nclass, LASSO_IDWSF2_IS_HREF, LASSO_IDWSF2_IS_PREFIX);
	lasso_node_class_add_snippets(nclass, schema_snippets);
}

GType
lasso_idwsf2_is_item_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoIdWsf2IsItemClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoIdWsf2IsItem),
			0,
			NULL,
			NULL
		};

		this_type = g_type_register_static(LASSO_TYPE_NODE,
				"LassoIdWsf2IsItem", &this_info, 0);
	}
	return this_type;
}

/**
 * lasso_idwsf2_is_item_new:
 *
 * Creates a new #LassoIdWsf2IsItem object.
 *
 * Return value: a newly created #LassoIdWsf2IsItem object
 **/
LassoIdWsf2IsItem*
lasso_idwsf2_is_item_new()
{
	return g_object_new(LASSO_TYPE_IDWSF2_IS_ITEM, NULL);
}
