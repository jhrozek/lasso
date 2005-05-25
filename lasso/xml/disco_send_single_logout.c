/* $Id$ 
 *
 * Lasso - A free implementation of the Liberty Alliance specifications.
 *
 * Copyright (C) 2004, 2005 Entr'ouvert
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

#include <lasso/xml/disco_send_single_logout.h>

/*
 * Schema fragments (liberty-idwsf-disco-svc-1.0-errata-v1.0.xsd):
 * 
 * <xs: complexType name="DirectiveType">
 *  <xs: attribute name="descriptionIDRefs" type="xs:IDREFS" use="optional"/>
 * </xs: complexType>
 * <xs: element name="SendSingleLogOut" type="disco: DirectiveType"/>
 *
 */ 

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/

static struct XmlSnippet schema_snippets[] = {
	{ "descriptionIDRefs",SNIPPET_ATTRIBUTE,
	  G_STRUCT_OFFSET(LassoDiscoSendSingleLogout, descriptionIDRefs) },
	{ NULL, 0, 0}
};

/*****************************************************************************/
/* instance and class init functions                                         */
/*****************************************************************************/

static void
instance_init(LassoDiscoSendSingleLogout *node)
{
	node->descriptionIDRefs = NULL;
}

static void
class_init(LassoDiscoSendSingleLogoutClass *klass)
{
	LassoNodeClass *nclass = LASSO_NODE_CLASS(klass);

	nclass->node_data = g_new0(LassoNodeClassData, 1);
	lasso_node_class_set_nodename(nclass, "SendSingleLogOut");
	lasso_node_class_set_ns(nclass, LASSO_DISCO_HREF, LASSO_DISCO_PREFIX);
	lasso_node_class_add_snippets(nclass, schema_snippets);
}

GType
lasso_disco_send_single_logout_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof (LassoDiscoSendSingleLogoutClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoDiscoSendSingleLogout),
			0,
			(GInstanceInitFunc) instance_init,
		};

		this_type = g_type_register_static(LASSO_TYPE_NODE,
				"LassoDiscoSendSingleLogout", &this_info, 0);
	}
	return this_type;
}

LassoDiscoSendSingleLogout*
lasso_disco_send_single_logout_new()
{
	LassoDiscoSendSingleLogout *node;

	node = g_object_new(LASSO_TYPE_DISCO_SEND_SINGLE_LOGOUT, NULL);

	return node;
}

LassoDiscoSendSingleLogout*
lasso_disco_send_single_logout_new_from_message(const gchar *message)
{
	LassoDiscoSendSingleLogout *node;

	g_return_val_if_fail(message != NULL, NULL);

	node = g_object_new(LASSO_TYPE_DISCO_SEND_SINGLE_LOGOUT, NULL);
	lasso_node_init_from_message(LASSO_NODE(node), message);

	return node;
}