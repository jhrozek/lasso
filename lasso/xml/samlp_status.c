/* $Id$
 *
 * Lasso - A free implementation of the Liberty Alliance specifications.
 *
 * Copyright (C) 2004 Entr'ouvert
 * http://lasso.entrouvert.org
 * 
 * Authors: Nicolas Clapies <nclapies@entrouvert.com>
 *          Valery Febvre <vfebvre@easter-eggs.com>
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

#include <lasso/xml/samlp_status.h>

/*
 * Schema fragment (oasis-sstc-saml-schema-protocol-1.0.xsd):
 * 
 * <element name="Status" type="samlp:StatusType"/>
 * <complexType name="StatusType">
 *   <sequence>
 *     <element ref="samlp:StatusCode"/>
 *     <element ref="samlp:StatusMessage" minOccurs="0" maxOccurs="1"/>
 *     <element ref="samlp:StatusDetail" minOccurs="0"/>
 *   </sequence>
 * </complexType>
 * 
 * <element name="StatusMessage" type="string"/>
 */

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/

static struct XmlSnippet schema_snippets[] = {
	{ "StatusCode", SNIPPET_NODE, G_STRUCT_OFFSET(LassoSamlpStatus, StatusCode) },
	{ "StatusMessage", SNIPPET_CONTENT, G_STRUCT_OFFSET(LassoSamlpStatus, StatusMessage) },
	{ NULL, 0, 0}
};

/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/


static void
instance_init(LassoSamlpStatus *node)
{
	node->StatusCode = NULL;
	node->StatusMessage = NULL;
}

static void
class_init(LassoSamlpStatusClass *klass)
{
	LassoNodeClass *nclass = LASSO_NODE_CLASS(klass);

	nclass->node_data = g_new0(LassoNodeClassData, 1);
	lasso_node_class_set_nodename(nclass, "Status");
	lasso_node_class_set_ns(nclass, LASSO_SAML_PROTOCOL_HREF, LASSO_SAML_PROTOCOL_PREFIX);
	lasso_node_class_add_snippets(nclass, schema_snippets);
}

GType
lasso_samlp_status_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoSamlpStatusClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoSamlpStatus),
			0,
			(GInstanceInitFunc) instance_init,
		};

		this_type = g_type_register_static(LASSO_TYPE_NODE,
				"LassoSamlpStatus", &this_info, 0);
	}
	return this_type;
}

LassoSamlpStatus*
lasso_samlp_status_new()
{
	return g_object_new(LASSO_TYPE_SAMLP_STATUS, NULL);
}

