/* $Id$
 *
 * Lasso - A free implementation of the Samlerty Alliance specifications.
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

#include <lasso/xml/saml_advice.h>
#include <lasso/xml/saml_assertion.h>

/*
 * Schema fragment (oasis-sstc-saml-schema-assertion-1.0.xsd):
 * 
 * <element name="Advice" type="saml:AdviceType"/>
 * <complexType name="AdviceType">
 *   <choice minOccurs="0" maxOccurs="unbounded">
 *     <element ref="saml:AssertionIDReference"/>
 *     <element ref="saml:Assertion"/>
 *     <any namespace="##other" processContents="lax"/>
 *   </choice>
 * </complexType>
 * 
 * <element name="AssertionIDReference" type="saml:IDReferenceType"/>
 * <simpleType name="IDReferenceType">
 *   <restriction base="string"/>
 * </simpleType>
 */

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/

static struct XmlSnippet schema_snippets[] = {
	{ "AssertionIDReference", SNIPPET_CONTENT,
		G_STRUCT_OFFSET(LassoSamlAdvice, AssertionIDReference) },
	{ "Assertion", SNIPPET_NODE, G_STRUCT_OFFSET(LassoSamlAdvice, Assertion) },
	{ NULL, 0, 0}
};

/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/

static void
instance_init(LassoSamlAdvice *node)
{
	node->AssertionIDReference = NULL;
	node->Assertion = NULL;
}

static void
class_init(LassoSamlAdviceClass *klass)
{
	LassoNodeClass *nclass = LASSO_NODE_CLASS(klass);

	nclass->node_data = g_new0(LassoNodeClassData, 1);
	lasso_node_class_set_nodename(nclass, "Advice");
	lasso_node_class_set_ns(nclass, LASSO_SAML_ASSERTION_HREF, LASSO_SAML_ASSERTION_PREFIX);
	lasso_node_class_add_snippets(nclass, schema_snippets);
}

GType
lasso_saml_advice_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoSamlAdviceClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoSamlAdvice),
			0,
			(GInstanceInitFunc) instance_init,
		};

		this_type = g_type_register_static(LASSO_TYPE_NODE,
				"LassoSamlAdvice", &this_info, 0);
	}
	return this_type;
}

/**
 * lasso_saml_advice_new:
 * 
 * Creates a new <saml:Advice> node object.
 *
 * The <Advice> element contains any additional information that the issuer
 * wishes to provide. This information MAY be ignored by applications without
 * affecting either the semantics or the validity of the assertion.
 * The <Advice> element contains a mixture of zero or more <Assertion>
 * elements, <AssertionIDReference> elements and elements in other namespaces,
 * with lax schema validation in effect for these other elements.
 * Following are some potential uses of the <Advice> element:
 *
 * - Include evidence supporting the assertion claims to be cited, either
 * directly (through incorporating the claims) or indirectly (by reference to
 * the supporting assertions).
 *
 * - State a proof of the assertion claims.
 *
 * - Specify the timing and distribution points for updates to the assertion.
 * 
 * Return value: the new @LassoSamlAdvice
 **/
LassoNode*
lasso_saml_advice_new()
{
	return g_object_new(LASSO_TYPE_SAML_ADVICE, NULL);
}

