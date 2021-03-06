/* $Id: is_select.c,v 1.0 2005/10/14 15:17:55 fpeters Exp $
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
#include "is_select.h"
#include "idwsf2_strings.h"

/**
 * SECTION:is_select
 * @short_description: &lt;is:Select&gt;
 *
 * <figure><title>Schema fragment for is:Select</title>
 * <programlisting><![CDATA[
 *
 * <xs:complexType name="SelectType">
 *   <xs:complexContent>
 *     <xs:extension base="InquiryElementType">
 *       <xs:sequence>
 *         <xs:element name="Item" minOccurs="2" maxOccurs="unbounded">
 *           <xs:complexType>
 *             <xs:sequence>
 *               <xs:element ref="Hint" minOccurs="0"/>
 *             </xs:sequence>
 *             <xs:attribute name="label" type="xs:string" use="optional"/>
 *             <xs:attribute name="value" type="xs:NMTOKEN" use="required"/>
 *           </xs:complexType>
 * ]]></programlisting>
 * </figure>
 */

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/


static struct XmlSnippet schema_snippets[] = {
	{ "Item", SNIPPET_LIST_NODES,
		G_STRUCT_OFFSET(LassoIdWsf2IsSelect, Item), NULL, NULL, NULL},
	{NULL, 0, 0, NULL, NULL, NULL}
};

static LassoNodeClass *parent_class = NULL;


/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/


static void
class_init(LassoIdWsf2IsSelectClass *klass, void *unused G_GNUC_UNUSED)
{
	LassoNodeClass *nclass = LASSO_NODE_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);
	nclass->node_data = g_new0(LassoNodeClassData, 1);
	lasso_node_class_set_nodename(nclass, "Select");
	lasso_node_class_set_ns(nclass, LASSO_IDWSF2_IS_HREF, LASSO_IDWSF2_IS_PREFIX);
	lasso_node_class_add_snippets(nclass, schema_snippets);
}

GType
lasso_idwsf2_is_select_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoIdWsf2IsSelectClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoIdWsf2IsSelect),
			0,
			NULL,
			NULL
		};

		this_type = g_type_register_static(LASSO_TYPE_IDWSF2_IS_INQUIRY_ELEMENT,
				"LassoIdWsf2IsSelect", &this_info, 0);
	}
	return this_type;
}

/**
 * lasso_idwsf2_is_select_new:
 *
 * Creates a new #LassoIdWsf2IsSelect object.
 *
 * Return value: a newly created #LassoIdWsf2IsSelect object
 **/
LassoIdWsf2IsSelect*
lasso_idwsf2_is_select_new()
{
	return g_object_new(LASSO_TYPE_IDWSF2_IS_SELECT, NULL);
}
