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

#ifndef __LASSO_XMLSEC_SOAP_H__
#define __LASSO_XMLSEC_SOAP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libxml/tree.h>

#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/errors.h>


/** Replacement for xmlsec/soap.h */

#define xmlSecSoap11Ns ((xmlChar*)"http://schemas.xmlsoap.org/soap/envelope/")
#define xmlSecSoap12Ns ((xmlChar*)"http://www.w3.org/2003/05/soap-envelope")

static inline xmlNodePtr
xmlSecSoap11GetHeader(xmlNodePtr envNode) {
    xmlNodePtr cur;

    xmlSecAssert2(envNode != NULL, NULL);

    /* optional Header node is first */
    cur = xmlSecGetNextElementNode(envNode->children);
    if((cur != NULL) && xmlSecCheckNodeName(cur, xmlSecNodeHeader, xmlSecSoap11Ns)) {
        return(cur);
    }

    return(NULL);
}

static inline xmlNodePtr
xmlSecSoap11GetBody(xmlNodePtr envNode) {
    xmlNodePtr cur;

    xmlSecAssert2(envNode != NULL, NULL);

    /* optional Header node first */
    cur = xmlSecGetNextElementNode(envNode->children);
    if((cur != NULL) && xmlSecCheckNodeName(cur, xmlSecNodeHeader, xmlSecSoap11Ns)) {
        cur = xmlSecGetNextElementNode(cur->next);
    }

    /* Body node is next */
    if((cur == NULL) || !xmlSecCheckNodeName(cur, xmlSecNodeBody, xmlSecSoap11Ns)) {
        xmlSecError(XMLSEC_ERRORS_HERE,
                    NULL,
                    xmlSecErrorsSafeString(xmlSecNodeBody),
                    XMLSEC_ERRORS_R_NODE_NOT_FOUND,
                    XMLSEC_ERRORS_NO_MESSAGE);
        return(NULL);
    }

    return(cur);
}

static inline xmlNodePtr
xmlSecSoap12GetBody(xmlNodePtr envNode) {
    xmlNodePtr cur;

    xmlSecAssert2(envNode != NULL, NULL);

    /* optional Header node first */
    cur = xmlSecGetNextElementNode(envNode->children);
    if((cur != NULL) && xmlSecCheckNodeName(cur, xmlSecNodeHeader, xmlSecSoap12Ns)) {
        cur = xmlSecGetNextElementNode(cur->next);
    }

    /* Body node is next */
    if((cur == NULL) || !xmlSecCheckNodeName(cur, xmlSecNodeBody, xmlSecSoap12Ns)) {
        xmlSecError(XMLSEC_ERRORS_HERE,
                    NULL,
                    xmlSecErrorsSafeString(xmlSecNodeBody),
                    XMLSEC_ERRORS_R_NODE_NOT_FOUND,
                    XMLSEC_ERRORS_NO_MESSAGE);
        return(NULL);
    }

    return(cur);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LASSO_XMLSEC_SOAP_H__ */
