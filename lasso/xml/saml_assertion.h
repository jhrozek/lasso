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

#ifndef __LASSO_SAML_ASSERTION_H__
#define __LASSO_SAML_ASSERTION_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 

#include <lasso/xml/xml.h>
#include <lasso/xml/saml_advice.h>
#include <lasso/xml/saml_authentication_statement.h>
#include <lasso/xml/saml_conditions.h>
#include <lasso/xml/saml_statement_abstract.h>
#include <lasso/xml/saml_subject_statement.h>

#define LASSO_TYPE_SAML_ASSERTION (lasso_saml_assertion_get_type())
#define LASSO_SAML_ASSERTION(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), LASSO_TYPE_SAML_ASSERTION, LassoSamlAssertion))
#define LASSO_SAML_ASSERTION_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass), LASSO_TYPE_SAML_ASSERTION, LassoSamlAssertionClass))
#define LASSO_IS_SAML_ASSERTION(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj), LASSO_TYPE_SAML_ASSERTION))
#define LASSO_IS_SAML_ASSERTION_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), LASSO_TYPE_SAML_ASSERTION))
#define LASSO_SAML_ASSERTION_GET_CLASS(o) \
	(G_TYPE_INSTANCE_GET_CLASS ((o), LASSO_TYPE_SAML_ASSERTION, LassoSamlAssertionClass)) 

typedef struct _LassoSamlAssertion LassoSamlAssertion;
typedef struct _LassoSamlAssertionClass LassoSamlAssertionClass;

struct _LassoSamlAssertion {
	LassoNode parent;

	/* <element ref="saml:Conditions" minOccurs="0"/> */
	LassoSamlConditions *Conditions;
	/* <element ref="saml:Advice" minOccurs="0"/> */
	LassoSamlAdvice *Advice;
#if 0 /* missing from lasso */
	LassoSamlStatement *Statement;
#endif
	LassoSamlSubjectStatement *SubjectStatement;
	LassoSamlAuthenticationStatement *AuthenticationStatement;
#if 0
	LassoAuthorizationDecisionsStatement *AuthorizationDecisionStatement;
	LassoAttributeStatement *AttributeStatement;
#endif

	int MajorVersion;
	int MinorVersion;
	char *AssertionID;
	char *Issuer;
	char *IssueInstant;
};

struct _LassoSamlAssertionClass {
	LassoNodeClass parent;
};

LASSO_EXPORT GType lasso_saml_assertion_get_type(void);
LASSO_EXPORT LassoNode* lasso_saml_assertion_new(void);

LASSO_EXPORT gint lasso_saml_assertion_set_signature(LassoSamlAssertion *node,
		gint                 sign_method,
		const xmlChar       *private_key_file,
		const xmlChar       *certificate_file);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LASSO_SAML_ASSERTION_H__ */
