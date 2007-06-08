/* $Id: wsa_endpoint_reference.c,v 1.0 2005/10/14 15:17:55 fpeters Exp $ 
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "wsa_endpoint_reference.h"

/*
 * Schema fragment (ws-addr.xsd):
 *
 * <xs:complexType name="EndpointReferenceType" mixed="false">
 *   <xs:sequence>
 *     <xs:element name="Address" type="tns:AttributedURIType"/>
 *     <xs:element name="ReferenceParameters" type="tns:ReferenceParametersType" minOccurs="0"/>
 *     <xs:element ref="tns:Metadata" minOccurs="0"/>
 *     <xs:any namespace="##other" processContents="lax" minOccurs="0" maxOccurs="unbounded"/>
 *   </xs:sequence>
 *   <xs:anyAttribute namespace="##other" processContents="lax"/>
 * </xs:complexType>
 */

/*****************************************************************************/
/* public methods                                                            */
/*****************************************************************************/

void
lasso_wsa_endpoint_reference_destroy(LassoWsAddrEndpointReference *epr)
{
	lasso_node_destroy(LASSO_NODE(epr));
}

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/


static struct XmlSnippet schema_snippets[] = {
	{ "Address", SNIPPET_NODE,
		G_STRUCT_OFFSET(LassoWsAddrEndpointReference, Address),
		"LassoWsAddrAttributedURI" },
	{ "ReferenceParameters", SNIPPET_NODE,
		G_STRUCT_OFFSET(LassoWsAddrEndpointReference, ReferenceParameters),
		"LassoWsAddrReferenceParameters" },
	{ "Metadata", SNIPPET_NODE,
		G_STRUCT_OFFSET(LassoWsAddrEndpointReference, Metadata),
		"LassoWsAddrMetadata" },
	{ "any", SNIPPET_ATTRIBUTE | SNIPPET_ANY,
		G_STRUCT_OFFSET(LassoWsAddrEndpointReference, attributes) },
	{NULL, 0, 0}
};

static LassoNodeClass *parent_class = NULL;


/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/

static void
instance_init(LassoWsAddrEndpointReference *node)
{
	node->Address = NULL;
	node->ReferenceParameters = NULL;
	node->Metadata = NULL;
}

static void
class_init(LassoWsAddrEndpointReferenceClass *klass)
{
	LassoNodeClass *nclass = LASSO_NODE_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);
	nclass->node_data = g_new0(LassoNodeClassData, 1);
	lasso_node_class_set_nodename(nclass, "EndpointReference"); 
	lasso_node_class_set_ns(nclass, LASSO_WSA_HREF, LASSO_WSA_PREFIX);
	lasso_node_class_add_snippets(nclass, schema_snippets);
}

GType
lasso_wsa_endpoint_reference_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoWsAddrEndpointReferenceClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoWsAddrEndpointReference),
			0,
			(GInstanceInitFunc) instance_init,
		};

		this_type = g_type_register_static(LASSO_TYPE_NODE,
				"LassoWsAddrEndpointReference", &this_info, 0);
	}
	return this_type;
}

/**
 * lasso_wsa_endpoint_reference_new:
 *
 * Creates a new #LassoWsAddrEndpointReference object.
 *
 * Return value: a newly created #LassoWsAddrEndpointReference object
 **/
LassoWsAddrEndpointReference*
lasso_wsa_endpoint_reference_new()
{
	return g_object_new(LASSO_TYPE_WSA_ENDPOINT_REFERENCE, NULL);
}
