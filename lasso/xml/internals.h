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

#ifndef __LASSO_INTERNALS_H__
#define __LASSO_INTERNALS_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
	SNIPPET_NODE,
	SNIPPET_CONTENT,
	SNIPPET_NAME_IDENTIFIER,
	SNIPPET_ATTRIBUTE,
	SNIPPET_LIST_NODES,
	SNIPPET_LIST_CONTENT,
} SnippetType;

struct XmlSnippet {
	char *name;
	char type;
	void **value;
};

void init_xml_with_snippets(xmlNode *node, struct XmlSnippet *snippets);
void build_xml_with_snippets(xmlNode *node, struct XmlSnippet *snippets);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LASSO_INTERNALS_H__ */
