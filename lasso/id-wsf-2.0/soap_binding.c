/* $Id: wsf_profile.c,v 1.45 2007/01/05 16:11:02 Exp $
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

#include "./soap_binding.h"
#include "../xml/id-wsf-2.0/sb2_sender.h"
#include "../xml/id-wsf-2.0/sb2_redirect_request.h"
#include "../xml/id-wsf-2.0/sb2_user_interaction_header.h"
#include "../xml/ws/wsa_attributed_uri.h"

#include "../utils.h"

#define get_header(test) \
	GList *i = NULL; \
 \
	if (! LASSO_IS_SOAP_ENVELOPE(soap_envelope)) \
		return NULL; \
	if (! LASSO_IS_SOAP_HEADER(soap_envelope->Header)) \
		return NULL; \
 \
	i = soap_envelope->Header->Other; \
	for (; i != NULL; i = g_list_next(i)) { \
		if (test(i->data)) { \
			break; \
		} \
	}

/**
 * lasso_soap_envelope_sb2_get_provider_id:
 * @soap_envelope: a #LassoSoapEnvelope object
 *
 * Return the providerID contained in the sb2:Sender header.
 *
 * Return value: the providerID string or NULL if no sb2:Sender header is present.
 */
char *
lasso_soap_envelope_sb2_get_provider_id(LassoSoapEnvelope *soap_envelope)
{
	get_header(LASSO_IS_IDWSF2_SB2_SENDER);

	if (i)
		return g_strdup(((LassoIdWsf2Sb2Sender*)i->data)->providerID);
	else
		return NULL;
}

/**
 * lasso_soap_envelope_sb2_get_redirect_request_url:
 * @soap_envelope: a #LassoSoapEnvelope object
 *
 * Return the redirect request URL from the sb2:RedirectRequest header.
 *
 * Return value: the redirect URL string or NULL if no sb2:RedirectRequest header is present.
 */
char *
lasso_soap_envelope_sb2_get_redirect_request_url(LassoSoapEnvelope *soap_envelope)
{
	get_header(LASSO_IS_IDWSF2_SB2_REDIRECT_REQUEST);

	if (i)
		return g_strdup(((LassoIdWsf2Sb2RedirectRequest*)i->data)->redirectURL);
	else
		return NULL;
}

/**
 * lasso_soap_envelope_sb2_get_target_identity_header:
 * @soap_envelope: a #LassoSoapEnvelope object.
 *
 * Return the first sb2:TargetIdentity header.
 *
 * Return value: (transfer-none): the first #LassoIdWsf2Sb2TargetIdentity object found in the
 * headers of the @soap_envelope, or NULL if none is found.
 */
LassoIdWsf2Sb2TargetIdentity*
lasso_soap_envelope_sb2_get_target_identity_header(LassoSoapEnvelope *soap_envelope)
{
	get_header(LASSO_IS_IDWSF2_SB2_TARGET_IDENTITY);

	if (i)
		return (LassoIdWsf2Sb2TargetIdentity*)i->data;
	else
		return NULL;
}


/**
 * lasso_soap_envelope_wssec_get_security_header_impl:
 * @soap_envelope: a #LassoSoapEnvelope object
 * @create: wether to create the header if not found
 *
 * Return value: (transfer-none): the wsse:Security header found in the SOAP message, or NULL if
 * none is found, and creation was not allowed.
 */
static LassoWsSec1SecurityHeader*
lasso_soap_envelope_wssec_get_security_header_impl(LassoSoapEnvelope *soap_envelope,
		gboolean create)
{
	get_header(LASSO_IS_WSSE_SECURITY_HEADER);

	if (i) {
		return (LassoWsSec1SecurityHeader*)i->data;
	}

	if (create) {
		LassoWsSec1SecurityHeader *wssec_security = lasso_wsse_security_header_new();
		lasso_list_add_new_gobject (soap_envelope->Header->Other, wssec_security);
		return wssec_security;
	}

	return NULL;
}

/**
 * lasso_soap_envelope_wssec_get_security_header:
 * @soap_envelope: a #LassoSoapEnvelope object
 *
 * Return the first wsse:Security header found in the headers of @soap_envelope.
 *
 * Return value: (transfer-none): the wsse:Security header found in the SOAP message, or NULL if
 * none is found.
 */
LassoWsSec1SecurityHeader*
lasso_soap_envelope_wssec_get_security_header(LassoSoapEnvelope *soap_envelope)
{
	return lasso_soap_envelope_wssec_get_security_header_impl (soap_envelope, FALSE);
}

/**
 * lasso_soap_envelope_add_security_token:
 * @soap_envelope: a #LassoSoapEnvelope object
 * @token: a #LassoNode to use as a security token
 *
 * Add @token as a security token to the headers of @soap_envelope using a
 * #LassoWsSec1SecurityHeader element as a container. Eventually create the
 * #LassoWsSec1SecurityHeader element if it does not exist already.
 */
void
lasso_soap_envelope_add_security_token(LassoSoapEnvelope *soap_envelope, LassoNode *token)
{
	LassoWsSec1SecurityHeader *wssec_security;

	wssec_security = lasso_soap_envelope_wssec_get_security_header_impl (soap_envelope, TRUE);
	g_return_if_fail (wssec_security != NULL);
	lasso_list_add_gobject(wssec_security->any, token);
}

/**
 * lasso_soap_envelope_get_saml2_security_token:
 * @soap_envelope: a #LassoSoapEnvelope object
 *
 * Return a SAML2 Assertion used as a security token if one is found in the headers of
 * @soap_envelope.
 *
 * Return value: (transfer-none): a #LassoSaml2Assertion, or NULL if none is found.
 */
LassoSaml2Assertion*
lasso_soap_envelope_get_saml2_security_token(LassoSoapEnvelope *soap_envelope)
{
	const LassoWsSec1SecurityHeader *security;
	GList *it;

	security = lasso_soap_envelope_wssec_get_security_header (soap_envelope);
	lasso_foreach (it, security->any) {
		if (LASSO_IS_SAML2_ASSERTION (it->data)) {
			return (LassoSaml2Assertion*)g_object_ref(it->data);
		}
	}
	return NULL;
}

/**
 * lasso_soap_envelope_add_action_header:
 * @soap_envelope: a #LassoSoapEnvelope object
 * @action: the action targeted by this message
 *
 * Add an action header to a #LassoSoapEnvelope object.
 */
void
lasso_soap_envelope_add_action(LassoSoapEnvelope *soap_envelope, const char *action)
{
	LassoWsAddrAttributedURI *wsa_action;

	if (! LASSO_IS_SOAP_ENVELOPE(soap_envelope) || ! LASSO_IS_SOAP_HEADER(soap_envelope->Header))
		return;
	wsa_action = lasso_wsa_attributed_uri_new_with_string(action);
	lasso_node_set_custom_nodename(&wsa_action->parent, "Action");
	lasso_list_add_new_gobject(soap_envelope->Header->Other, wsa_action);
}

const char*
lasso_soap_envelope_get_action(LassoSoapEnvelope *soap_envelope)
{
	GList *i;

	if (! LASSO_IS_SOAP_ENVELOPE(soap_envelope) || ! LASSO_IS_SOAP_HEADER(soap_envelope->Header))
		return NULL;

	lasso_foreach(i, soap_envelope->Header->Other) {
		if (LASSO_IS_WSA_ATTRIBUTED_URI(i->data)
				&& g_strcmp0(lasso_node_get_name((LassoNode*)i->data), "Action")) {
			return ((LassoWsAddrAttributedURI*)i->data)->content;
		}
	}
	return NULL;
}

/**
 * lasso_soap_envelope_add_to_body:
 * @soap_envelope: a #LassoSoapEnvelope object
 * @content: a #LassoNode object to add to the body of the SOAP message
 *
 * Add new content to the SOAP message body.
 */
void
lasso_soap_envelope_add_to_body(LassoSoapEnvelope *soap_envelope, LassoNode *content)
{
	if (!LASSO_IS_SOAP_ENVELOPE(soap_envelope) || !LASSO_IS_SOAP_BODY(soap_envelope->Body))
		return;
	lasso_list_add_gobject(soap_envelope->Body->any, content);
}

/**
 * lasso_soap_envelope_get_message_id:
 * @soap_envelope: a #LassoSoapEnvelope object
 *
 * Return the WS-Addressing header MessageID content.
 *
 * Return value:(transfer none)(allow-none): a string or NULL if no message-id header is found.
 */
char*
lasso_soap_envelope_get_message_id(LassoSoapEnvelope *soap_envelope)
{
	GList *i;

	if (! LASSO_IS_SOAP_ENVELOPE(soap_envelope) || ! LASSO_IS_SOAP_HEADER(soap_envelope->Header))
		return NULL;

	lasso_foreach(i, soap_envelope->Header->Other) {
		if (LASSO_IS_WSA_ATTRIBUTED_URI(i->data) && g_strcmp0(lasso_node_get_name((LassoNode*)i->data),
					"MessageID")) {
			return ((LassoWsAddrAttributedURI*)i->data)->content;
		}
	}
	return NULL;
}

/**
 * lasso_soap_envelope_get_body_content:
 * @soap_envelope: a #LassoSoapEnvelope object
 *
 * Return the body content of the @soap_envelope object.
 *
 * Return value: the content or NULL if the concent is empty or the object invalid.
 */
GList*
lasso_soap_envelope_get_body_content(LassoSoapEnvelope *soap_envelope) {
	if (LASSO_IS_SOAP_ENVELOPE(soap_envelope) && LASSO_IS_SOAP_BODY(soap_envelope->Body))
		return soap_envelope->Body->any;
	return NULL;
}

LassoSoapDetail *
lasso_soap_fault_get_detail(LassoSoapFault *soap_fault, gboolean create)
{
	if (! LASSO_IS_SOAP_FAULT(soap_fault))
		return NULL;

	if (soap_fault->Detail == NULL && create) {
		soap_fault->Detail = lasso_soap_detail_new();
	}
	return soap_fault->Detail;
}


/**
 * lasso_soap_fault_add_to_detail:
 * @soap_fault: a #LassoSoapFault object
 * @node: a node to add to the Detail sub-element of the @soap_fault object.
 *
 * Fill the detail part of a SOAP fault.
 */
void
lasso_soap_fault_add_to_detail(LassoSoapFault *soap_fault,
		LassoNode *node)
{
	LassoSoapDetail *detail;

	detail = lasso_soap_fault_get_detail(soap_fault, TRUE);
	if (detail) {
		lasso_list_add_gobject(detail->any, node);
	}
}

static LassoIdWsf2Sb2UserInteractionHeader *
lasso_soap_envelope_get_sb2_user_interaction_header(LassoSoapEnvelope *soap_envelope)
{
	get_header(LASSO_IS_IDWSF2_SB2_USER_INTERACTION_HEADER);

	if (i) {
		return (LassoIdWsf2Sb2UserInteractionHeader*)i->data;
	}
	return NULL;
}

LassoIdWsf2Sb2UserInteractionHint
lasso_soap_envelope_get_sb2_user_interaction_hint(LassoSoapEnvelope *soap_envelope)
{
	const char *hint;
	LassoIdWsf2Sb2UserInteractionHeader *header;

	header = lasso_soap_envelope_get_sb2_user_interaction_header(soap_envelope);
	if (header) {
		hint = header->interact;
		if (g_strcmp0(hint, LASSO_SB2_USER_INTERACTION_INTERACT_IF_NEEDED))
			return LASSO_IDWSF2_SB2_USER_INTERACTION_HINT_INTERACT_IF_NEEDED;
		if (g_strcmp0(hint, LASSO_SB2_USER_INTERACTION_DO_NOT_INTERACT))
			return LASSO_IDWSF2_SB2_USER_INTERACTION_HINT_DO_NOT_INTERACT;
		if (g_strcmp0(hint, LASSO_SB2_USER_INTERACTION_DO_NOT_INTERACT_FOR_DATA))
			return LASSO_IDWSF2_SB2_USER_INTERACTION_HINT_DO_NOT_INTERACT_FOR_DATA;

	}
	return LASSO_IDWSF2_SB2_USER_INTERACTION_HINT_NONE;
}

void
lasso_soap_envelope_set_sb2_user_interaction_hint(LassoSoapEnvelope *soap_envelope,
		LassoIdWsf2Sb2UserInteractionHint hint)
{
	LassoIdWsf2Sb2UserInteractionHeader *user_interaction;

	user_interaction = lasso_soap_envelope_get_sb2_user_interaction_header(soap_envelope);
	if (! user_interaction) {
		user_interaction = lasso_idwsf2_sb2_user_interaction_header_new();
		lasso_list_add_new_gobject(soap_envelope->Header->Other, user_interaction);
	}
	switch (hint) {
		case LASSO_IDWSF2_SB2_USER_INTERACTION_HINT_INTERACT_IF_NEEDED:
			lasso_assign_string(user_interaction->interact,
					LASSO_SB2_USER_INTERACTION_INTERACT_IF_NEEDED);
			break;
		case LASSO_IDWSF2_SB2_USER_INTERACTION_HINT_DO_NOT_INTERACT:
			lasso_assign_string(user_interaction->interact,
					LASSO_SB2_USER_INTERACTION_DO_NOT_INTERACT);
			break;
		case LASSO_IDWSF2_SB2_USER_INTERACTION_HINT_DO_NOT_INTERACT_FOR_DATA:
			lasso_assign_string(user_interaction->interact,
					LASSO_SB2_USER_INTERACTION_DO_NOT_INTERACT_FOR_DATA);
			break;
		default:
			lasso_release_string(user_interaction->interact);
	}
}