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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "../private.h"
#include "samlp2_response.h"
#include "saml2_assertion.h"
#include "saml2_encrypted_element.h"

/**
 * SECTION:samlp2_response
 * @short_description: &lt;samlp2:Response&gt;
 *
 * <figure><title>Schema fragment for samlp2:Response</title>
 * <programlisting><![CDATA[
 *
 * <complexType name="ResponseType">
 *   <complexContent>
 *     <extension base="samlp:StatusResponseType">
 *       <choice minOccurs="0" maxOccurs="unbounded">
 *         <element ref="saml:Assertion"/>
 *         <element ref="saml:EncryptedAssertion"/>
 *       </choice>
 *     </extension>
 *   </complexContent>
 * </complexType>
 * ]]></programlisting>
 * </figure>
 */

extern LassoNode* lasso_assertion_encrypt(LassoSaml2Assertion *assertion);

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/

static struct XmlSnippet schema_snippets[] = {
	{ "Assertion", SNIPPET_LIST_NODES,
		G_STRUCT_OFFSET(LassoSamlp2Response, Assertion), NULL, NULL, NULL},
	{ "EncryptedAssertion", SNIPPET_LIST_NODES,
		G_STRUCT_OFFSET(LassoSamlp2Response, EncryptedAssertion), NULL, NULL, NULL},
	{NULL, 0, 0, NULL, NULL, NULL}
};

static LassoNodeClass *parent_class = NULL;

static xmlNode*
get_xmlNode(LassoNode *node, gboolean lasso_dump)
{
	LassoSamlp2Response *response = LASSO_SAMLP2_RESPONSE(node);
	GList *assertions, *assertions_copy;
	LassoNode *encrypted_element = NULL;
	xmlNode *result;


	/* Encrypt Assertions for messages but not for dumps */
	if (lasso_dump == FALSE) {
		assertions_copy = g_list_copy(response->Assertion);
		for (assertions = response->Assertion;
				assertions != NULL; assertions = g_list_next(assertions)) {
			encrypted_element = lasso_assertion_encrypt(assertions->data);
			if (encrypted_element != NULL) {
				/* use EncryptedAssertion */
				response->EncryptedAssertion = g_list_append(
					response->EncryptedAssertion, encrypted_element);
				/* and remove original unencrypted from Assertion */
				response->Assertion = g_list_remove(response->Assertion,
					assertions->data);
			}
		}
	}

	result = parent_class->get_xmlNode(node, lasso_dump);

	if (lasso_dump == FALSE) {
		g_list_free(response->Assertion);
		response->Assertion = assertions_copy;
		for (assertions = response->EncryptedAssertion; assertions != NULL;
				assertions = g_list_next(assertions)) {
			lasso_node_destroy(assertions->data);
		}
		g_list_free(response->EncryptedAssertion);
		response->EncryptedAssertion = NULL;
	}

	return result;
}

/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/


static void
class_init(LassoSamlp2ResponseClass *klass)
{
	LassoNodeClass *nclass = LASSO_NODE_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);
	nclass->get_xmlNode = get_xmlNode;
	nclass->node_data = g_new0(LassoNodeClassData, 1);
	nclass->node_data->keep_xmlnode = TRUE;
	lasso_node_class_set_nodename(nclass, "Response");
	lasso_node_class_set_ns(nclass, LASSO_SAML2_PROTOCOL_HREF, LASSO_SAML2_PROTOCOL_PREFIX);
	lasso_node_class_add_snippets(nclass, schema_snippets);
}

GType
lasso_samlp2_response_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoSamlp2ResponseClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoSamlp2Response),
			0,
			NULL,
			NULL
		};

		this_type = g_type_register_static(LASSO_TYPE_SAMLP2_STATUS_RESPONSE,
				"LassoSamlp2Response", &this_info, 0);
	}
	return this_type;
}

/**
 * lasso_samlp2_response_new:
 *
 * Creates a new #LassoSamlp2Response object.
 *
 * Return value: a newly created #LassoSamlp2Response object
 **/
LassoNode*
lasso_samlp2_response_new()
{
	return g_object_new(LASSO_TYPE_SAMLP2_RESPONSE, NULL);
}
