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

#include <lasso/xml/lib_authn_response_envelope.h>



/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/

#define snippets() \
	LassoLibAuthnResponseEnvelope *env = LASSO_LIB_AUTHN_RESPONSE_ENVELOPE(node); \
	struct XmlSnippet snippets[] = { \
		/* Extension */ \
		{ "AuthnResponse", 'n', (void**)&(env->AuthnResponse) }, \
		{ "AssertionConsumerServiceURL", 'c', \
			(void**)&(env->AssertionConsumerServiceURL) }, \
		{ NULL, 0, NULL} \
	};

static LassoNodeClass *parent_class = NULL;

static xmlNode*
get_xmlNode(LassoNode *node)
{
	xmlNode *xmlnode;
	snippets();

	xmlnode = xmlNewNode(NULL, "AuthnResponseEnvelope");
	xmlSetNs(xmlnode, xmlNewNs(xmlnode, LASSO_LIB_HREF, LASSO_LIB_PREFIX));
	lasso_node_build_xml_with_snippets(xmlnode, snippets);

	return xmlnode;
}

static int
init_from_xml(LassoNode *node, xmlNode *xmlnode)
{
	snippets();

	if (parent_class->init_from_xml(node, xmlnode))
		return 1;
	lasso_node_init_xml_with_snippets(xmlnode, snippets);
	return 0;
}


/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/

static void
instance_init(LassoLibAuthnResponseEnvelope *node)
{
	node->Extension = NULL;
	node->AuthnResponse = NULL;
	node->AssertionConsumerServiceURL = NULL;
}

static void
class_init(LassoLibAuthnResponseEnvelopeClass *klass)
{	
	parent_class = g_type_class_peek_parent(klass);
	LASSO_NODE_CLASS(klass)->get_xmlNode = get_xmlNode;
	LASSO_NODE_CLASS(klass)->init_from_xml = init_from_xml;
}

GType
lasso_lib_authn_response_envelope_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoLibAuthnResponseEnvelopeClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoLibAuthnResponseEnvelope),
			0,
			(GInstanceInitFunc) instance_init,
		};

		this_type = g_type_register_static(LASSO_TYPE_NODE,
				"LassoLibAuthnResponseEnvelope", &this_info, 0);
	}
	return this_type;
}

LassoLibAuthnResponseEnvelope*
lasso_lib_authn_response_envelope_new(LassoLibAuthnResponse *response,
		char *assertionConsumerServiceURL)
{
	LassoLibAuthnResponseEnvelope *envelope;

	envelope = g_object_new(LASSO_TYPE_LIB_AUTHN_RESPONSE_ENVELOPE, NULL);
	if (response) {
		envelope->AuthnResponse = response;
		envelope->AssertionConsumerServiceURL = g_strdup(assertionConsumerServiceURL);
	}

	return envelope;
}

