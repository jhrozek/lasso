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

#include "../private.h"
#include "samlp2_extensions.h"

/**
 * SECTION:samlp2_extensions
 * @short_description: &lt;samlp2:Extensions&gt;
 *
 * <figure><title>Schema fragment for samlp2:Extensions</title>
 * <programlisting><![CDATA[
 *
 * <complexType name="ExtensionsType">
 *   <sequence>
 *     <any namespace="##other" processContents="lax" maxOccurs="unbounded"/>
 *   </sequence>
 * </complexType>
 * ]]></programlisting>
 * </figure>
 *
 * To specify the content of this object you must attach an <type>xmlNode</type> to it using
 * lasso_node_set_original_xmlnode(). The content (attributes, children and namespaces) of this node
 * will be copied to the result node returned by calls to lasso_node_get_xmlNode().
 *
 */

typedef struct _LassoSamlp2ExtensionsPrivate LassoSamlp2ExtensionsPrivate;

struct _LassoSamlp2ExtensionsPrivate {
	GList *any;
	GHashTable *attributes;
};

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/


static struct XmlSnippet schema_snippets[] = {
	{"", SNIPPET_LIST_XMLNODES | SNIPPET_ANY | SNIPPET_PRIVATE,
		G_STRUCT_OFFSET(LassoSamlp2ExtensionsPrivate, any), NULL, NULL, NULL},
	{"", SNIPPET_ATTRIBUTE | SNIPPET_ANY | SNIPPET_PRIVATE,
		G_STRUCT_OFFSET(LassoSamlp2ExtensionsPrivate, attributes), NULL, NULL, NULL},
	{NULL, 0, 0, NULL, NULL, NULL}
};

static LassoNodeClass *parent_class = NULL;

#define GET_PRIVATE(x) G_TYPE_INSTANCE_GET_PRIVATE(x, \
		LASSO_TYPE_SAMLP2_EXTENSIONS, LassoSamlp2ExtensionsPrivate)

/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/

static void
class_init(LassoSamlp2ExtensionsClass *klass, void *unused G_GNUC_UNUSED)
{
	LassoNodeClass *nclass = LASSO_NODE_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);
	nclass->node_data = g_new0(LassoNodeClassData, 1);
	nclass->node_data->keep_xmlnode = TRUE;
	lasso_node_class_set_nodename(nclass, "Extensions");
	lasso_node_class_set_ns(nclass, LASSO_SAML2_PROTOCOL_HREF, LASSO_SAML2_PROTOCOL_PREFIX);
	lasso_node_class_add_snippets(nclass, schema_snippets);
	g_type_class_add_private(G_OBJECT_CLASS(klass), sizeof(LassoSamlp2ExtensionsPrivate));
}

GType
lasso_samlp2_extensions_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoSamlp2ExtensionsClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoSamlp2Extensions),
			0,
			NULL,
			NULL
		};

		this_type = g_type_register_static(LASSO_TYPE_NODE,
				"LassoSamlp2Extensions", &this_info, 0);
	}
	return this_type;
}

/**
 * lasso_samlp2_extensions_new:
 *
 * Creates a new #LassoSamlp2Extensions object.
 *
 * Return value: a newly created #LassoSamlp2Extensions object
 **/
LassoNode*
lasso_samlp2_extensions_new()
{
	return g_object_new(LASSO_TYPE_SAMLP2_EXTENSIONS, NULL);
}

/**
 * lasso_samlp2_extensions_get_any:
 *
 * Return the list of contained XML nodes.
 *
 * Return value:(element-type xmlNode)(transfer none): a #GList of xmlNode.
 */
GList*
lasso_samlp2_extensions_get_any(LassoSamlp2Extensions *extensions)
{
	LassoSamlp2ExtensionsPrivate *pv = NULL;

	pv = GET_PRIVATE(extensions);

	return pv->any;
}

/**
 * lasso_samlp2_extensions_set_any:
 * @any:(allow-none)(element-type xmlNode)(transfer none): a list of xmlNode.
 *
 * Set the list of contained XML nodes.
 *
 */
void
lasso_samlp2_extensions_set_any(LassoSamlp2Extensions *extensions, GList *any)
{
	LassoSamlp2ExtensionsPrivate *pv = NULL;

	pv = GET_PRIVATE(extensions);

	lasso_assign_list_of_xml_node(pv->any, any);
}

/**
 * lasso_samlp2_extensions_get_attributes:
 *
 * Return the list of contained XML nodes.
 *
 * Return value:(element-type xmlNode)(transfer none): a #GList of xmlNode.
 */
GHashTable*
lasso_samlp2_extensions_get_attributes(LassoSamlp2Extensions *extensions)
{
	LassoSamlp2ExtensionsPrivate *pv = NULL;

	pv = GET_PRIVATE(extensions);

	return pv->attributes;
}

/**
 * lasso_samlp2_extensions_set_attributes:
 * @attributes:(element-type char* char*): table of attributes.
 *
 * Set the table of XML attributes.
 */
void
lasso_samlp2_extensions_set_attributes(LassoSamlp2Extensions *extensions, GHashTable *attributes)
{
	LassoSamlp2ExtensionsPrivate *pv = NULL;

	pv = GET_PRIVATE(extensions);

	lasso_assign_table_of_attributes(pv->attributes, attributes);
}
