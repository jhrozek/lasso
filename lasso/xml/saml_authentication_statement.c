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

#include <lasso/xml/saml_authentication_statement.h>

/*
The schema fragment (oasis-sstc-saml-schema-assertion-1.0.xsd):

<element name="AuthenticationStatement" type="saml:AuthenticationStatementType"/>
<complexType name="AuthenticationStatementType">
  <complexContent>
    <extension base="saml:SubjectStatementAbstractType">
      <sequence>
        <element ref="saml:SubjectLocality" minOccurs="0"/>
        <element ref="saml:AuthorityBinding" minOccurs="0" maxOccurs="unbounded"/>
      </sequence>
      <attribute name="AuthenticationMethod" type="anyURI" use="required"/>
      <attribute name="AuthenticationInstant" type="dateTime" use="required"/>
    </extension>
  </complexContent>
</complexType>
*/

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/

#define snippets() \
	LassoSamlAuthenticationStatement *statement = LASSO_SAML_AUTHENTICATION_STATEMENT(node); \
	struct XmlSnippet snippets[] = { \
		{ "SubjectLocality", 'n', (void**)&(statement->SubjectLocality) }, \
		{ "AuthorityBinding", 'n', (void**)&(statement->AuthorityBinding) }, \
		{ NULL, 0, NULL} \
	};

static LassoNodeClass *parent_class = NULL;

static xmlNode*
get_xmlNode(LassoNode *node)
{
	xmlNode *xmlnode;
	snippets();

	xmlnode = parent_class->get_xmlNode(node);
	xmlNodeSetName(xmlnode, "AuthenticationStatement");
	lasso_node_build_xml_with_snippets(xmlnode, snippets);

	if (statement->AuthenticationMethod)
		xmlSetProp(xmlnode, "AuthenticationMethod", statement->AuthenticationMethod);
	if (statement->AuthenticationInstant)
		xmlSetProp(xmlnode, "AuthenticationInstant", statement->AuthenticationInstant);

	return xmlnode;
}

static int
init_from_xml(LassoNode *node, xmlNode *xmlnode)
{
	snippets();

	if (parent_class->init_from_xml(node, xmlnode))
		return -1;
	lasso_node_init_xml_with_snippets(xmlnode, snippets);
	statement->AuthenticationMethod = xmlGetProp(xmlnode, "AuthenticationMethod");
	statement->AuthenticationInstant = xmlGetProp(xmlnode, "AuthenticationInstant");
	return 0;
}
	
/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/

static void
instance_init(LassoSamlAuthenticationStatement *node)
{
	node->SubjectLocality = NULL;
	node->AuthorityBinding = NULL;
	node->AuthenticationMethod = NULL;
	node->AuthenticationInstant = NULL;
}

static void
class_init(LassoSamlAuthenticationStatementClass *klass)
{
	parent_class = g_type_class_peek_parent(klass);
	LASSO_NODE_CLASS(klass)->get_xmlNode = get_xmlNode;
	LASSO_NODE_CLASS(klass)->init_from_xml = init_from_xml;
}

GType
lasso_saml_authentication_statement_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoSamlAuthenticationStatementClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoSamlAuthenticationStatement),
			0,
			(GInstanceInitFunc) instance_init,
		};

		this_type = g_type_register_static(LASSO_TYPE_SAML_SUBJECT_STATEMENT_ABSTRACT,
				"LassoSamlAuthenticationStatement", &this_info, 0);
	}
	return this_type;
}

/**
 * lasso_saml_authentication_statement_new:
 *
 * Creates a new <saml:AuthenticationStatement> node object.
 * 
 * Return value: the new @LassoSamlAuthenticationStatement
 **/
LassoNode*
lasso_saml_authentication_statement_new()
{
	return g_object_new(LASSO_TYPE_SAML_AUTHENTICATION_STATEMENT, NULL);
}

