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

#include "private.h"
#include "saml_name_identifier.h"
#include <libxml/uri.h>

/**
 * SECTION:saml_name_identifier
 * @short_description: &lt;saml:NameIdentifier&gt;
 *
 * <figure><title>Schema fragment for saml:NameIdentifier</title>
 * <programlisting><![CDATA[
 *
 * <element name="NameIdentifier" type="saml:NameIdentifierType"/>
 * <complexType name="NameIdentifierType">
 *   <simpleContent>
 *     <extension base="string">
 *       <attribute name="NameQualifier" type="string" use="optional"/>
 *       <attribute name="Format" type="anyURI" use="optional"/>
 *     </extension>
 *   </simpleContent>
 * </complexType>
 * ]]></programlisting>
 * </figure>
 */

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/

static struct XmlSnippet schema_snippets[] = {
	{ "NameQualifier", SNIPPET_ATTRIBUTE,
		G_STRUCT_OFFSET(LassoSamlNameIdentifier, NameQualifier), NULL, NULL, NULL},
	{ "Format", SNIPPET_ATTRIBUTE, G_STRUCT_OFFSET(LassoSamlNameIdentifier, Format), NULL, NULL, NULL},
	{ "content", SNIPPET_TEXT_CHILD, G_STRUCT_OFFSET(LassoSamlNameIdentifier, content), NULL, NULL, NULL},
	{NULL, 0, 0, NULL, NULL, NULL}
};

/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/

static void
class_init(LassoSamlNameIdentifierClass *klass, void *unused G_GNUC_UNUSED)
{
	LassoNodeClass *nclass = LASSO_NODE_CLASS(klass);

	nclass->node_data = g_new0(LassoNodeClassData, 1);
	lasso_node_class_set_nodename(nclass, "NameIdentifier");
	lasso_node_class_set_ns(nclass, LASSO_SAML_ASSERTION_HREF, LASSO_SAML_ASSERTION_PREFIX);
	lasso_node_class_add_snippets(nclass, schema_snippets);
}

GType
lasso_saml_name_identifier_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoSamlNameIdentifierClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoSamlNameIdentifier),
			0,
			NULL,
			NULL
		};

		this_type = g_type_register_static(LASSO_TYPE_NODE,
				"LassoSamlNameIdentifier", &this_info, 0);
	}
	return this_type;
}

gboolean
lasso_saml_name_identifier_equals(LassoSamlNameIdentifier *a,
		LassoSamlNameIdentifier *b) {
	if (a == NULL || b == NULL)
		return FALSE;

	if (! LASSO_IS_SAML_NAME_IDENTIFIER(a) && ! LASSO_IS_SAML_NAME_IDENTIFIER(b)) {
		return FALSE;
	}
	return lasso_strisequal(a->NameQualifier, b->NameQualifier)
		&& lasso_strisequal(a->Format, b->Format)
		&& lasso_strisequal(a->content, b->content);
}

/**
 * lasso_saml_name_identifier_new:
 *
 * Creates a new #LassoSamlNameIdentifier object.
 *
 * Return value: a newly created #LassoSamlNameIdentifier object
 **/
LassoSamlNameIdentifier*
lasso_saml_name_identifier_new()
{
	return g_object_new(LASSO_TYPE_SAML_NAME_IDENTIFIER, NULL);
}


LassoSamlNameIdentifier*
lasso_saml_name_identifier_new_from_xmlNode(xmlNode *xmlnode)
{
	LassoNode *node;

	node = g_object_new(LASSO_TYPE_SAML_NAME_IDENTIFIER, NULL);
	lasso_node_init_from_xml(node, xmlnode);
	return LASSO_SAML_NAME_IDENTIFIER(node);
}
