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

#include <xmlsec/base64.h>

#include <lasso/xml/lib_authentication_statement.h>
#include <lasso/xml/lib_subject.h>

#include <lasso/id-ff/login.h>
#include <lasso/id-ff/provider.h>


struct _LassoLoginPrivate
{
	char *soap_request_msg;
};

/*****************************************************************************/
/* static methods/functions */
/*****************************************************************************/

/**
 * lasso_login_build_assertion:
 * @login: a #LassoLogin
 * @authenticationMethod: the authentication method.
 * @authenticationInstant: the time at which the authentication took place or NULL.
 * @reauthenticateOnOrAfter: the time at, or after which the service provider
 *     reauthenticates the Principal with the identity provider or NULL.
 * @notBefore: the earliest time instant at which the assertion is valid or NULL.
 * @notOnOrAfter: the time instant at which the assertion has expired or NULL.
 * 
 * Builds an assertion.
 * Assertion is stored in session property. If session property is NULL, a new
 * session is built before.
 * The NameIdentifier of the assertion is stored into nameIdentifier proprerty.
 * If @authenticationInstant is NULL, the current time will be set.
 * Time values must be encoded in UTC.
 *
 * Return value: 0 on success; or a negative value otherwise.
 **/
int
lasso_login_build_assertion(LassoLogin *login,
		const char *authenticationMethod,
		const char *authenticationInstant,
		const char *reauthenticateOnOrAfter,
		const char *notBefore,
		const char *notOnOrAfter)
{
	LassoSamlAssertion *assertion;
	LassoLibAuthenticationStatement *as;
	LassoSamlNameIdentifier *nameIdentifier;
	LassoProfile *profile;
	LassoFederation *federation;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);
	/* federation MAY be NULL */

	profile = LASSO_PROFILE(login);

	federation = g_hash_table_lookup(profile->identity->federations,
			profile->remote_providerID);
	
	/*
	 get RequestID to build Assertion
	 it may be NULL when the Identity provider initiates SSO.
	 in this case, no InResponseTo will be added in assertion
	 (XXX: what does that mean ?  would profile->request also be NULL?)
	 */
	assertion = LASSO_SAML_ASSERTION(lasso_lib_assertion_new_full(
			LASSO_PROVIDER(profile->server)->ProviderID,
			LASSO_SAMLP_REQUEST_ABSTRACT(profile->request)->RequestID,
			profile->remote_providerID, notBefore, notOnOrAfter));

	if (strcmp(login->nameIDPolicy, LASSO_LIB_NAMEID_POLICY_TYPE_ONE_TIME) == 0) {
		/* if NameIDPolicy is 'onetime', don't use a federation */
		nameIdentifier = lasso_saml_name_identifier_new();
		nameIdentifier->content = lasso_build_unique_id(32);
		nameIdentifier->NameQualifier = LASSO_PROVIDER(profile->server)->ProviderID;
		nameIdentifier->Format = LASSO_LIB_NAME_IDENTIFIER_FORMAT_ONE_TIME;

		as = lasso_lib_authentication_statement_new_full(authenticationMethod,
				authenticationInstant, reauthenticateOnOrAfter,
				NULL, nameIdentifier);
		profile->nameIdentifier = nameIdentifier;
	} else {
		as = lasso_lib_authentication_statement_new_full(authenticationMethod,
				authenticationInstant, reauthenticateOnOrAfter,
				federation->remote_nameIdentifier,
				federation->local_nameIdentifier);
	}

	LASSO_SAML_ASSERTION(assertion)->AuthenticationStatement = 
				LASSO_SAML_AUTHENTICATION_STATEMENT(as);

	if (profile->server->certificate) {
		assertion->sign_type = LASSO_SIGNATURE_TYPE_WITHX509;
	} else {
		assertion->sign_type = LASSO_SIGNATURE_TYPE_SIMPLE;
	}
	assertion->sign_method = profile->server->signature_method;
	assertion->private_key_file = g_strdup(profile->server->private_key);
	assertion->certificate_file = g_strdup(profile->server->certificate);

	if (login->protocolProfile == LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST) {
		/* only add assertion if response is an AuthnResponse */
		LASSO_SAMLP_RESPONSE(profile->response)->Assertion =
			LASSO_SAML_ASSERTION(assertion);
	}
	/* store assertion in session object */
	if (profile->session == NULL) {
		profile->session = lasso_session_new();
	}
	if (login->assertion)
		lasso_node_destroy(LASSO_NODE(login->assertion));
	login->assertion = LASSO_SAML_ASSERTION(g_object_ref(assertion));
	lasso_session_add_assertion(profile->session, profile->remote_providerID,
			LASSO_SAML_ASSERTION(g_object_ref(assertion)));
	return 0;
}

/**
 * lasso_login_must_ask_for_consent_private:
 * @login: a #LassoLogin
 * 
 * Evaluates if it is necessary to ask the consent of the Principal. 
 * This method doesn't take the isPassive value into account.
 * 
 * Return value: TRUE or FALSE
 **/
static gboolean
lasso_login_must_ask_for_consent_private(LassoLogin *login)
{
	xmlChar *nameIDPolicy, *consent;
	LassoProfile *profile = LASSO_PROFILE(login);
	LassoFederation *federation = NULL;

	nameIDPolicy = LASSO_LIB_AUTHN_REQUEST(profile->request)->NameIDPolicy;

	if (nameIDPolicy == NULL || strcmp(nameIDPolicy, LASSO_LIB_NAMEID_POLICY_TYPE_NONE) == 0)
		return FALSE;

	if (strcmp(nameIDPolicy, LASSO_LIB_NAMEID_POLICY_TYPE_ONE_TIME) == 0)
		return FALSE;

	if (strcmp(nameIDPolicy, LASSO_LIB_NAMEID_POLICY_TYPE_FEDERATED) != 0 &&
			strcmp(nameIDPolicy, LASSO_LIB_NAMEID_POLICY_TYPE_ANY) != 0) {
		message(G_LOG_LEVEL_CRITICAL, "Unknown NameIDPolicy: %s", nameIDPolicy);
		/* NameIDPolicy is considered empty (None value) if its value is unknown/invalid */
		return TRUE;
	}

	if (profile->identity != NULL) {
		federation = g_hash_table_lookup(profile->identity->federations,
				profile->remote_providerID);
		if (federation)
			return FALSE;
	}

	consent = LASSO_LIB_AUTHN_REQUEST(profile->request)->consent;
	if (consent == NULL)
		return TRUE;

	if (strcmp(consent, LASSO_LIB_CONSENT_OBTAINED) == 0)
		return FALSE;

	if (strcmp(consent, LASSO_LIB_CONSENT_OBTAINED_PRIOR) == 0)
		return FALSE;

	if (strcmp(consent, LASSO_LIB_CONSENT_OBTAINED_CURRENT_IMPLICIT) == 0)
		return FALSE;

	if (strcmp(consent, LASSO_LIB_CONSENT_OBTAINED_CURRENT_EXPLICIT) == 0)
		return FALSE;

	if (strcmp(consent, LASSO_LIB_CONSENT_UNAVAILABLE) == 0)
		return TRUE;

	if (strcmp(consent, LASSO_LIB_CONSENT_INAPPLICABLE) == 0)
		return TRUE;

	message(G_LOG_LEVEL_CRITICAL, "Unknown consent value: %s", consent);
	/* we consider consent as empty if its value is unknown/invalid */
	return TRUE;
}

/**
 * lasso_login_process_federation:
 * @login: a LassoLogin
 * @is_consent_obtained: is user consent obtained ?
 * 
 * Return value: a positive value on success or a negative if an error occurs.
 **/
static gint
lasso_login_process_federation(LassoLogin *login, gboolean is_consent_obtained)
{
	LassoFederation *federation = NULL;
	LassoProfile *profile = LASSO_PROFILE(login);
	char *nameIDPolicy;
	gint ret = 0;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);

	/* verify if identity already exists else create it */
	if (profile->identity == NULL) {
		profile->identity = lasso_identity_new();
	}

	/* get nameIDPolicy in lib:AuthnRequest */
	nameIDPolicy = LASSO_LIB_AUTHN_REQUEST(profile->request)->NameIDPolicy;
	if (nameIDPolicy == NULL)
		nameIDPolicy = LASSO_LIB_NAMEID_POLICY_TYPE_NONE;
	login->nameIDPolicy = g_strdup(nameIDPolicy);

	/* if nameIDPolicy is 'onetime' => nothing to do */
	if (strcmp(nameIDPolicy, LASSO_LIB_NAMEID_POLICY_TYPE_ONE_TIME) == 0) {
		return 0;
	}

	/* search a federation in the identity */
	federation = g_hash_table_lookup(LASSO_PROFILE(login)->identity->federations,
			LASSO_PROFILE(login)->remote_providerID);

	if (strcmp(nameIDPolicy, LASSO_LIB_NAMEID_POLICY_TYPE_NONE) == 0) {
		/* a federation MUST exist */
		if (federation == NULL) {
			/* if protocolProfile is LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST
			 * set StatusCode to FederationDoesNotExist in lib:AuthnResponse
			 */
			if (login->protocolProfile == LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST) {
				lasso_profile_set_response_status(LASSO_PROFILE(login),
						LASSO_LIB_STATUS_CODE_FEDERATION_DOES_NOT_EXIST);
			}
			return LASSO_LOGIN_ERROR_FEDERATION_NOT_FOUND;
		}

		LASSO_PROFILE(login)->nameIdentifier = g_object_ref(
			LASSO_SAML_NAME_IDENTIFIER(federation->local_nameIdentifier));
		return 0;
	}

	if (strcmp(nameIDPolicy, LASSO_LIB_NAMEID_POLICY_TYPE_FEDERATED) != 0 &&
			strcmp(nameIDPolicy, LASSO_LIB_NAMEID_POLICY_TYPE_ANY) != 0) {
		return critical_error(LASSO_LOGIN_ERROR_INVALID_NAMEIDPOLICY, nameIDPolicy);
	}

	/* consent is necessary, it should be obtained via consent attribute
	 * in lib:AuthnRequest or IDP should ask the Principal
	 */
	if (lasso_login_must_ask_for_consent_private(login) && !is_consent_obtained) {
		if (strcmp(nameIDPolicy, LASSO_LIB_NAMEID_POLICY_TYPE_ANY) == 0) {
			/* if the NameIDPolicy element is 'any' and if the policy
			 * for the Principal forbids federation, then evaluation
			 * MAY proceed as if the value was 'onetime'.
			 */
			g_free(login->nameIDPolicy);
			login->nameIDPolicy = g_strdup(LASSO_LIB_NAMEID_POLICY_TYPE_ONE_TIME);
			return 0;
		}
		
		/* if protocolProfile is LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST
		 * set StatusCode to FederationDoesNotExist in lib:AuthnResponse
		 */
		/* FIXME : is it the correct value for the StatusCode ? */
		if (login->protocolProfile == LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST) {
			lasso_profile_set_response_status(LASSO_PROFILE(login),
					LASSO_LIB_STATUS_CODE_FEDERATION_DOES_NOT_EXIST);
		}
		return LASSO_LOGIN_ERROR_CONSENT_NOT_OBTAINED;
	}

	if (federation == NULL) {
		federation = lasso_federation_new(LASSO_PROFILE(login)->remote_providerID);
		lasso_federation_build_local_name_identifier(federation,
				LASSO_PROVIDER(LASSO_PROFILE(login)->server)->ProviderID,
				LASSO_LIB_NAME_IDENTIFIER_FORMAT_FEDERATED,
				NULL);
		lasso_identity_add_federation(LASSO_PROFILE(login)->identity, federation);
	}

	LASSO_PROFILE(login)->nameIdentifier = 
		g_object_ref(LASSO_SAML_NAME_IDENTIFIER(federation->local_nameIdentifier));

	return ret;
}

static gint
lasso_login_process_response_status_and_assertion(LassoLogin *login)
{
	LassoProvider *idp;
	LassoSamlpResponse *response;
	char *status_value;
	int ret = 0;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);

	response = LASSO_SAMLP_RESPONSE(LASSO_PROFILE(login)->response);

	if (response->Status == NULL || ! LASSO_IS_SAMLP_STATUS(response->Status))
		return LASSO_ERROR_UNDEFINED;

	if (response->Status->StatusCode == NULL)
		return LASSO_ERROR_UNDEFINED;

	status_value = response->Status->StatusCode->Value;
	if (status_value == NULL) {
		/* XXX ? was ignored before ? */ 
	}
	if (status_value && strcmp(status_value, LASSO_SAML_STATUS_CODE_SUCCESS) != 0) {
		return LASSO_ERROR_UNDEFINED;
	}

	if (response->Assertion) {
		LassoProfile *profile = LASSO_PROFILE(login);
		idp = g_hash_table_lookup(profile->server->providers, profile->remote_providerID);
		if (idp == NULL)
			return LASSO_ERROR_UNDEFINED;

		/* verify signature */
		/* FIXME detect X509Data ? */
		ret = lasso_node_verify_signature(LASSO_NODE(response->Assertion),
					idp->public_key, idp->ca_cert_chain);
		if (ret < 0)
			return ret;

		/* store NameIdentifier */
		if (response->Assertion->AuthenticationStatement == NULL) {
			return LASSO_ERROR_UNDEFINED;
		}

		profile->nameIdentifier = g_object_ref(LASSO_SAML_SUBJECT_STATEMENT_ABSTRACT(
					response->Assertion->AuthenticationStatement
					)->Subject->NameIdentifier);

		if (LASSO_PROFILE(login)->nameIdentifier == NULL)
			return LASSO_ERROR_UNDEFINED;
	}

	return ret;
}

/*****************************************************************************/
/* public methods */
/*****************************************************************************/

/**
 * lasso_login_accept_sso:
 * @login: a LassoLogin
 * 
 * Gets the assertion of the response and adds it into the session.
 * Builds a federation with the 2 name identifiers of the assertion
 * and adds it into the identity.
 * If the session or the identity are NULL, they are created.
 * 
 * Return value: 0 on success and a negative value otherwise.
 **/
gint
lasso_login_accept_sso(LassoLogin *login)
{
	LassoProfile *profile;
	LassoSamlAssertion *assertion;
	LassoSamlNameIdentifier *ni, *idp_ni = NULL;
	LassoFederation *federation;
	LassoSamlSubjectStatementAbstract *authentication_statement;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);

	profile = LASSO_PROFILE(login);

	if (profile->identity == NULL)
		profile->identity = lasso_identity_new(); 

	if (profile->session == NULL)
		profile->session = lasso_session_new();

	if (profile->response == NULL)
		return LASSO_ERROR_UNDEFINED;

	assertion = LASSO_SAMLP_RESPONSE(profile->response)->Assertion;
	if (assertion == NULL)
		return LASSO_ERROR_UNDEFINED;

	lasso_session_add_assertion(profile->session, profile->remote_providerID,
			g_object_ref(assertion));

	authentication_statement = LASSO_SAML_SUBJECT_STATEMENT_ABSTRACT(
			LASSO_SAMLP_RESPONSE(profile->response
				)->Assertion->AuthenticationStatement);
	ni = authentication_statement->Subject->NameIdentifier;

	if (ni == NULL)
		return LASSO_ERROR_UNDEFINED;

	if (LASSO_IS_LIB_SUBJECT(authentication_statement->Subject)) {
		idp_ni = LASSO_LIB_SUBJECT(
				authentication_statement->Subject)->IDPProvidedNameIdentifier;
	}

	/* create federation, only if nameidentifier format is Federated */
	if (strcmp(ni->Format, LASSO_LIB_NAME_IDENTIFIER_FORMAT_FEDERATED) == 0) {
		federation = lasso_federation_new(LASSO_PROFILE(login)->remote_providerID);
		if (ni != NULL && idp_ni != NULL) {
			federation->local_nameIdentifier = g_object_ref(ni);
			federation->remote_nameIdentifier = g_object_ref(idp_ni);
		} else {
			federation->remote_nameIdentifier = g_object_ref(ni);
		}
		/* add federation in identity */
		lasso_identity_add_federation(LASSO_PROFILE(login)->identity, federation);
	}

	return 0;
}

/**
 * lasso_login_build_artifact_msg:
 * @login: a LassoLogin
 * @http_method: the HTTP method to send the artifact (REDIRECT or POST)
 * 
 * Builds an artifact. Depending of the HTTP method, the data for the sending of
 * the artifact are stored in msg_url (REDIRECT) or msg_url, msg_body and
 * msg_relayState (POST).
 *
 * Return value: 0 on success and a negative value otherwise.
 **/
gint
lasso_login_build_artifact_msg(LassoLogin *login, lassoHttpMethod http_method)
{
	LassoProvider *remote_provider;
	gchar *url;
	xmlSecByte samlArt[42], *b64_samlArt, *relayState;
	xmlChar *identityProviderSuccinctID;
	gint ret = 0;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);

	if (http_method != LASSO_HTTP_METHOD_REDIRECT && http_method != LASSO_HTTP_METHOD_POST) {
		return critical_error(LASSO_PROFILE_ERROR_INVALID_HTTP_METHOD);
	}

	/* ProtocolProfile must be BrwsArt */
	if (login->protocolProfile != LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART) {
		return critical_error(LASSO_PROFILE_ERROR_INVALID_PROTOCOLPROFILE);
	}

	if (LASSO_PROFILE(login)->remote_providerID == NULL)
		return critical_error(LASSO_PROFILE_ERROR_MISSING_REMOTE_PROVIDERID);

	/* build artifact infos */
	remote_provider = g_hash_table_lookup(LASSO_PROFILE(login)->server->providers,
			LASSO_PROFILE(login)->remote_providerID);
	url = lasso_provider_get_metadata_one(remote_provider, "AssertionConsumerServiceURL");
	if (url == NULL) {
		return critical_error(LASSO_PROFILE_ERROR_UNKNOWN_PROFILE_URL);
	}
	identityProviderSuccinctID = lasso_sha1(
			LASSO_PROVIDER(LASSO_PROFILE(login)->server)->ProviderID);

	/* Artifact Format is described in "Binding Profiles", 3.2.2.2. */
	memcpy(samlArt, "\000\003", 2); /* type code */
	memcpy(samlArt+2, identityProviderSuccinctID, 20);
	lasso_build_random_sequence(samlArt+22, 20);

	xmlFree(identityProviderSuccinctID);
	b64_samlArt = xmlSecBase64Encode(samlArt, 42, 0);
	relayState = xmlURIEscapeStr(
			LASSO_LIB_AUTHN_REQUEST(LASSO_PROFILE(login)->request)->RelayState, NULL);

	if (http_method == LASSO_HTTP_METHOD_REDIRECT) {
		if (relayState == NULL) {
			LASSO_PROFILE(login)->msg_url = g_strdup_printf(
					"%s?SAMLart=%s", url, b64_samlArt);
		} else {
			LASSO_PROFILE(login)->msg_url = g_strdup_printf(
					"%s?SAMLart=%s&RelayState=%s", 
					url, b64_samlArt, relayState);
		}
	}

	if (http_method == LASSO_HTTP_METHOD_POST) {
		LASSO_PROFILE(login)->msg_url = g_strdup(url);
		LASSO_PROFILE(login)->msg_body = g_strdup(b64_samlArt);
		if (relayState != NULL) {
			LASSO_PROFILE(login)->msg_relayState = g_strdup(relayState);
		}
	}
	login->assertionArtifact = g_strdup(b64_samlArt);
	xmlFree(url);
	xmlFree(b64_samlArt);
	xmlFree(relayState);

	return ret;
}

/**
 * lasso_login_build_authn_request_msg:
 * @login: a LassoLogin
 * 
 * Builds an authentication request. Depending of the selected HTTP method,
 * the data for the sending of the request are stored in msg_url (GET) or
 * msg_url and msg_body (POST).
 * 
 * Return value: 0 on success and a negative value otherwise.
 **/
gint
lasso_login_build_authn_request_msg(LassoLogin *login)
{
	LassoProvider *provider, *remote_provider;
	char *md_authnRequestsSigned, *url, *query, *lareq, *protocolProfile;
	LassoProviderRole role;
	gboolean must_sign;
	gint ret = 0;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);

	provider = LASSO_PROVIDER(LASSO_PROFILE(login)->server);
	remote_provider = g_hash_table_lookup(LASSO_PROFILE(login)->server->providers,
			LASSO_PROFILE(login)->remote_providerID);
	if (LASSO_IS_PROVIDER(remote_provider) == FALSE) {
		return critical_error(LASSO_SERVER_ERROR_PROVIDER_NOT_FOUND,
				LASSO_PROFILE(login)->remote_providerID);
	}

	protocolProfile = LASSO_LIB_AUTHN_REQUEST(LASSO_PROFILE(login)->request)->ProtocolProfile;
	if (protocolProfile == NULL)
		protocolProfile = LASSO_LIB_PROTOCOL_PROFILE_BRWS_ART;

	role = provider->role;
	provider->role = LASSO_PROVIDER_ROLE_SP; /* we act as an SP for sure here */

	if (lasso_provider_has_protocol_profile(remote_provider,
				LASSO_MD_PROTOCOL_TYPE_SINGLE_SIGN_ON, protocolProfile) == FALSE) {
		provider->role = role;
		return LASSO_PROFILE_ERROR_UNSUPPORTED_PROFILE;
	}

	/* check if authnRequest must be signed */
	md_authnRequestsSigned = lasso_provider_get_metadata_one(provider, "AuthnRequestsSigned");
	must_sign = (md_authnRequestsSigned && strcmp(md_authnRequestsSigned, "true") == 0);
	g_free(md_authnRequestsSigned);
	provider->role = role;

	if (login->http_method == LASSO_HTTP_METHOD_REDIRECT) {
		/* REDIRECT -> query */
		if (must_sign) {
			query = lasso_node_export_to_query(LASSO_PROFILE(login)->request,
					LASSO_PROFILE(login)->server->signature_method,
					LASSO_PROFILE(login)->server->private_key);
		} else {
			query = lasso_node_export_to_query(
					LASSO_PROFILE(login)->request, 0, NULL);
		}
		if (query == NULL) {
			return critical_error(LASSO_PROFILE_ERROR_BUILDING_QUERY_FAILED);
		}

		/* get SingleSignOnServiceURL metadata */
		url = lasso_provider_get_metadata_one(remote_provider, "SingleSignOnServiceURL");
		if (url == NULL) {
			return critical_error(LASSO_PROFILE_ERROR_UNKNOWN_PROFILE_URL);
		}

		LASSO_PROFILE(login)->msg_url = g_strdup_printf("%s?%s", url, query);
		LASSO_PROFILE(login)->msg_body = NULL;
		g_free(query);
		g_free(url);
	}
	if (login->http_method == LASSO_HTTP_METHOD_POST) {
		char *private_key = NULL, *certificate = NULL;
		if (must_sign) {
			private_key = LASSO_PROFILE(login)->server->private_key;
			certificate = LASSO_PROFILE(login)->server->certificate;
		}
		lareq = lasso_node_export_to_base64(LASSO_PROFILE(login)->request,
				private_key, certificate);

		if (lareq == NULL) {
			message(G_LOG_LEVEL_CRITICAL,
					"Failed to export AuthnRequest (Base64 encoded).");
			return -5;
		}

		LASSO_PROFILE(login)->msg_url = lasso_provider_get_metadata_one(
				remote_provider, "SingleSignOnServiceURL");
		LASSO_PROFILE(login)->msg_body = lareq;
	}

	return ret;
}

/**
 * lasso_login_build_authn_response_msg:
 * @login: a LassoLogin
 * 
 * Builds an authentication response. The data for the sending of the response
 * are stored in msg_url and msg_body.
 *
 * Return value: 0 on success and a negative value otherwise.
 **/
gint
lasso_login_build_authn_response_msg(LassoLogin *login)
{
	LassoProvider *remote_provider;
	LassoProfile *profile;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);

	profile = LASSO_PROFILE(login);

	/* ProtocolProfile must be BrwsPost */
	if (login->protocolProfile != LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST) {
		return critical_error(LASSO_PROFILE_ERROR_INVALID_PROTOCOLPROFILE);
	}

	/* Countermeasure: The issuer should sign <lib:AuthnResponse> messages.
	 * (binding and profiles (1.2errata2, page 65) */
	LASSO_SAMLP_RESPONSE_ABSTRACT(profile->response)->sign_type = LASSO_SIGNATURE_TYPE_WITHX509;
	LASSO_SAMLP_RESPONSE_ABSTRACT(profile->response)->sign_method = 
		LASSO_SIGNATURE_METHOD_RSA_SHA1;

	/* build an lib:AuthnResponse base64 encoded */
	profile->msg_body = lasso_node_export_to_base64(profile->response,
			profile->server->private_key, profile->server->certificate);

	remote_provider = g_hash_table_lookup(LASSO_PROFILE(login)->server->providers,
			LASSO_PROFILE(login)->remote_providerID);
	profile->msg_url = lasso_provider_get_metadata_one(
			remote_provider, "AssertionConsumerServiceURL");

	return 0;
}

/**
 * lasso_login_build_request_msg:
 * @login: a LassoLogin
 * 
 * Builds a SOAP request message. The data for the sending of the request
 * are stored in msg_url and msg_body.
 * 
 * Return value: 0 on success and a negative value otherwise.
 **/
gint
lasso_login_build_request_msg(LassoLogin *login)
{
	LassoProvider *remote_provider;
	LassoProfile *profile;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);

	profile = LASSO_PROFILE(login);

	LASSO_PROFILE(login)->msg_body = lasso_node_export_to_soap(profile->request,
			profile->server->private_key, profile->server->certificate);

	remote_provider = g_hash_table_lookup(profile->server->providers,
			profile->remote_providerID);
	if (LASSO_IS_PROVIDER(remote_provider) == FALSE) {
		return critical_error(LASSO_SERVER_ERROR_PROVIDER_NOT_FOUND,
				profile->remote_providerID);
	}
	profile->msg_url = lasso_provider_get_metadata_one(remote_provider, "SoapEndpoint");
	return 0;
}

/**
 * lasso_login_build_response_msg:
 * @login: a LassoLogin
 * @remote_providerID: the providerID of the session provider
 * 
 * Builds a SOAP response message. The data for the sending of the response
 * are stored in msg_body.
 * 
 * Return value: 0 on success or a negative value if an 
 **/
gint
lasso_login_build_response_msg(LassoLogin *login, gchar *remote_providerID)
{
	LassoProvider *remote_provider;
	LassoProfile *profile;
	LassoSamlAssertion *assertion;
	gint ret = 0;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), -1);
	profile = LASSO_PROFILE(login);

	profile->response = lasso_samlp_response_new();
	LASSO_SAMLP_RESPONSE_ABSTRACT(profile->response)->sign_type = LASSO_SIGNATURE_TYPE_WITHX509;
	LASSO_SAMLP_RESPONSE_ABSTRACT(profile->response)->sign_method = 
		LASSO_SIGNATURE_METHOD_RSA_SHA1;

	if (remote_providerID != NULL) {
		profile->remote_providerID = g_strdup(remote_providerID);
		remote_provider = g_hash_table_lookup(profile->server->providers,
				profile->remote_providerID);
		ret = lasso_provider_verify_signature(remote_provider,
				login->private_data->soap_request_msg,
				"RequestID", LASSO_MESSAGE_FORMAT_SOAP);
		g_free(login->private_data->soap_request_msg);
		login->private_data->soap_request_msg = NULL;

		/* changed status code into RequestDenied
		 if signature is invalid or not found
		 if an error occurs during verification */
		if (ret != 0) {
			lasso_profile_set_response_status(profile,
					LASSO_SAML_STATUS_CODE_REQUEST_DENIED);
		}

		if (profile->session) {
			/* get assertion in session and add it in response */
			assertion = lasso_session_get_assertion(profile->session,
					profile->remote_providerID);
			if (assertion) {
				LASSO_SAMLP_RESPONSE(profile->response)->Assertion =
					g_object_ref(assertion);
			} else {
				/* FIXME should this message output by
				 * lasso_session_get_assertion () ? */
				message(G_LOG_LEVEL_CRITICAL, "Assertion not found in session");
			}
		}
	} else {
		lasso_profile_set_response_status(profile, LASSO_SAML_STATUS_CODE_REQUEST_DENIED);
	}

	profile->msg_body = lasso_node_export_to_soap(profile->response,
			profile->server->private_key, profile->server->certificate);

	return ret;
}

/**
 * lasso_login_destroy:
 * @login: a #LassoLogin
 * 
 * Destroys LassoLogin objects created with lasso_login_new() or lasso_login_new_from_dump().
 **/
void
lasso_login_destroy(LassoLogin *login)
{
	lasso_node_destroy(LASSO_NODE(login));
}

gint
lasso_login_init_authn_request(LassoLogin *login, const gchar *remote_providerID,
		lassoHttpMethod http_method)
{
	LassoLibAuthnRequest *request;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);

	if (http_method != LASSO_HTTP_METHOD_REDIRECT && http_method != LASSO_HTTP_METHOD_POST) {
		return critical_error(LASSO_PROFILE_ERROR_INVALID_HTTP_METHOD);
	}

	if (remote_providerID != NULL) {
		LASSO_PROFILE(login)->remote_providerID = g_strdup(remote_providerID);
	} else {
		LASSO_PROFILE(login)->remote_providerID = lasso_server_get_first_providerID(
				LASSO_PROFILE(login)->server);
	}

	login->http_method = http_method;

	/* XXX: should be moved somehow in samlp_request_abstract.c */
	request = lasso_lib_authn_request_new();
	LASSO_SAMLP_REQUEST_ABSTRACT(request)->RequestID = lasso_build_unique_id(32);
	LASSO_SAMLP_REQUEST_ABSTRACT(request)->MajorVersion = LASSO_LIB_MAJOR_VERSION_N;
	LASSO_SAMLP_REQUEST_ABSTRACT(request)->MinorVersion = LASSO_LIB_MINOR_VERSION_N;
	LASSO_SAMLP_REQUEST_ABSTRACT(request)->IssueInstant = lasso_get_current_time();
	request->ProviderID = g_strdup(LASSO_PROVIDER(LASSO_PROFILE(login)->server)->ProviderID);
	request->RelayState = g_strdup(LASSO_PROFILE(login)->msg_relayState);

	if (http_method == LASSO_HTTP_METHOD_POST) {
		LASSO_SAMLP_REQUEST_ABSTRACT(request)->sign_method =
			LASSO_SIGNATURE_METHOD_RSA_SHA1;
		LASSO_SAMLP_REQUEST_ABSTRACT(request)->sign_type = LASSO_SIGNATURE_TYPE_WITHX509;
	}

	LASSO_PROFILE(login)->request = LASSO_NODE(request);

	if (LASSO_PROFILE(login)->request == NULL) {
		return critical_error(LASSO_PROFILE_ERROR_BUILDING_REQUEST_FAILED);
	}

	return 0;
}

gint
lasso_login_init_request(LassoLogin *login, gchar *response_msg,
		lassoHttpMethod response_http_method)
{
	char **query_fields;
	gint ret = 0;
	int i;
	char *artifact_b64 = NULL, *provider_succint_id_b64;
	char provider_succint_id[21];
	char artifact[43];
	LassoSamlpRequestAbstract *request;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);
	g_return_val_if_fail(response_msg != NULL, LASSO_PARAM_ERROR_INVALID_VALUE);

	if (response_http_method != LASSO_HTTP_METHOD_REDIRECT &&
			response_http_method != LASSO_HTTP_METHOD_POST) {
		return critical_error(LASSO_PROFILE_ERROR_INVALID_HTTP_METHOD);
	}

	/* rebuild response (artifact) */
	if (response_http_method == LASSO_HTTP_METHOD_REDIRECT) {
		query_fields = urlencoded_to_strings(response_msg);
		for (i=0; query_fields[i]; i++) {
			if (strncmp(query_fields[i], "SAMLart=", 8) != 0) {
				free(query_fields[i]);
				continue;
			}
			artifact_b64 = g_strdup(query_fields[i]+8);
			free(query_fields[i]);
		}
		free(query_fields);
	}
	if (response_http_method == LASSO_HTTP_METHOD_POST) {
		artifact_b64 = g_strdup(response_msg);
	}

	i = xmlSecBase64Decode(artifact_b64, artifact, 43);
	if (i < 0 || i > 42) {
		g_free(artifact_b64);
		return LASSO_ERROR_UNDEFINED;
	}

	if (artifact[0] != 0 || artifact[1] != 3) { /* wrong type code */
		g_free(artifact_b64);
		return LASSO_ERROR_UNDEFINED;
	}

	memcpy(provider_succint_id, artifact+2, 20);
	provider_succint_id[20] = 0;

	provider_succint_id_b64 = xmlSecBase64Encode(provider_succint_id, 20, 0);

	LASSO_PROFILE(login)->remote_providerID = lasso_server_get_providerID_from_hash(
			LASSO_PROFILE(login)->server, provider_succint_id_b64);
	xmlFree(provider_succint_id_b64);

	request = LASSO_SAMLP_REQUEST_ABSTRACT(lasso_samlp_request_new());
	request->RequestID = lasso_build_unique_id(32);
	request->MajorVersion = LASSO_LIB_MAJOR_VERSION_N;
	request->MinorVersion = LASSO_LIB_MINOR_VERSION_N;
	request->IssueInstant = lasso_get_current_time();

	LASSO_SAMLP_REQUEST(request)->AssertionArtifact = artifact_b64;
	request->sign_type = LASSO_SIGNATURE_TYPE_WITHX509;
	request->sign_method = LASSO_SIGNATURE_METHOD_RSA_SHA1;

	LASSO_PROFILE(login)->request = LASSO_NODE(request);
	
	return ret;
}

/**
 * lasso_login_init_idp_initiated_authn_request:
 * @login: a LassoLogin.
 * @remote_providerID: the providerID of the remote service provider (may be NULL).
 * 
 * It's possible for an identity provider to generate an authentication response without first
 * having received an authentication request. This method must be used in this case.
 *
 * If @remote_providerID is NULL, the providerID of the first provider found in server is used.
 * 
 * Return value: 0 on success and a negative value if an error occurs.
 **/
gint
lasso_login_init_idp_initiated_authn_request(LassoLogin *login,
		const gchar *remote_providerID)
{
	LassoLibAuthnRequest *request;
	gchar *first_providerID;
	gint ret = 0;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);
	/* if remote_providerID is NULL, get first providerID in server */

	/* store providerID of the service provider */
	if (remote_providerID == NULL) {
		first_providerID = lasso_server_get_first_providerID(LASSO_PROFILE(login)->server);
		LASSO_PROFILE(login)->remote_providerID = first_providerID;
	} else {
		LASSO_PROFILE(login)->remote_providerID = g_strdup(remote_providerID);
	}

	/* build self-addressed lib:AuthnRequest */
	request = lasso_lib_authn_request_new();
	/* no RequestID attribute or it would be used in response assertion */
	LASSO_SAMLP_REQUEST_ABSTRACT(request)->MajorVersion = LASSO_LIB_MAJOR_VERSION_N;
	LASSO_SAMLP_REQUEST_ABSTRACT(request)->MinorVersion = LASSO_LIB_MINOR_VERSION_N;
	LASSO_SAMLP_REQUEST_ABSTRACT(request)->IssueInstant = lasso_get_current_time();
	request->ProviderID = g_strdup(LASSO_PROFILE(login)->remote_providerID);
	request->NameIDPolicy = LASSO_LIB_NAMEID_POLICY_TYPE_ANY;

	LASSO_PROFILE(login)->request = LASSO_NODE(request);

	return ret;
}

/**
 * lasso_login_must_ask_for_consent:
 * @login: a #LassoLogin
 * 
 * Evaluates if consent must be asked to the Principal to federate him.
 * 
 * Return value: TRUE or FALSE
 **/
gboolean
lasso_login_must_ask_for_consent(LassoLogin *login)
{
	if (lasso_login_must_ask_for_consent_private(login)) {
		if (LASSO_LIB_AUTHN_REQUEST(LASSO_PROFILE(login)->request)->IsPassive)
			return FALSE;
		return TRUE;
	}
	return FALSE;
}

/**
 * lasso_login_must_authenticate:
 * @login: a #LassoLogin
 * 
 * Verifies if the user must be authenticated or not.
 * 
 * Return value: TRUE or FALSE
 **/
gboolean
lasso_login_must_authenticate(LassoLogin *login)
{
	LassoLibAuthnRequest *request;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);
	
	request = LASSO_LIB_AUTHN_REQUEST(LASSO_PROFILE(login)->request);

	/* get IsPassive and ForceAuthn in AuthnRequest if exists */
	if ((request->ForceAuthn || LASSO_PROFILE(login)->session == NULL) &&
			request->IsPassive == FALSE)
		return TRUE;
	
	if (LASSO_PROFILE(login)->identity == NULL && request->IsPassive &&
			login->protocolProfile == LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST) {
		lasso_profile_set_response_status(LASSO_PROFILE(login),
				LASSO_LIB_STATUS_CODE_NO_PASSIVE);
		return FALSE;
	}

	return FALSE;
}

gint
lasso_login_process_authn_request_msg(LassoLogin *login, const char *authn_request_msg)
{
	LassoProvider *remote_provider;
	gchar *protocolProfile;
	gchar *authnRequestSigned;
	gboolean must_verify_signature = FALSE;
	gint ret = 0;
	LassoLibAuthnRequest *request;
	LassoMessageFormat format;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);

	if (authn_request_msg == NULL) {
		format = 0;
		if (LASSO_PROFILE(login)->request == NULL) {
			return critical_error(LASSO_PROFILE_ERROR_MISSING_REQUEST);
		}

		/* LibAuthnRequest already set by lasso_login_init_idp_initiated_authn_request() */
		request = LASSO_LIB_AUTHN_REQUEST(LASSO_PROFILE(login)->request);
		
		/* verify that NameIDPolicy is 'any' */
		if (request->NameIDPolicy == NULL)
			return LASSO_LOGIN_ERROR_INVALID_NAMEIDPOLICY;
			
		if (strcmp(request->NameIDPolicy, LASSO_LIB_NAMEID_POLICY_TYPE_ANY) != 0)
			return LASSO_LOGIN_ERROR_INVALID_NAMEIDPOLICY;
	} else {
		request = lasso_lib_authn_request_new();
		format = lasso_node_init_from_message(LASSO_NODE(request), authn_request_msg);
		if (format == LASSO_MESSAGE_FORMAT_UNKNOWN ||
				format == LASSO_MESSAGE_FORMAT_ERROR) {
			return critical_error(LASSO_PROFILE_ERROR_INVALID_MSG);
		}
		
		LASSO_PROFILE(login)->request = LASSO_NODE(request);
	}


	/* get ProtocolProfile in lib:AuthnRequest */
	protocolProfile = LASSO_LIB_AUTHN_REQUEST(LASSO_PROFILE(login)->request)->ProtocolProfile;
	if (protocolProfile == NULL ||
			strcmp(protocolProfile, LASSO_LIB_PROTOCOL_PROFILE_BRWS_ART) == 0) {
		protocolProfile = LASSO_LIB_PROTOCOL_PROFILE_BRWS_ART;
		login->protocolProfile = LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART;
	} else if (strcmp(protocolProfile, LASSO_LIB_PROTOCOL_PROFILE_BRWS_POST) == 0) {
		protocolProfile = LASSO_LIB_PROTOCOL_PROFILE_BRWS_POST;
		login->protocolProfile = LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST;
	} else {
		return critical_error(LASSO_PROFILE_ERROR_INVALID_PROTOCOLPROFILE);
	}

	/* check if requested single sign on protocol profile is supported */
	LASSO_PROVIDER(LASSO_PROFILE(login)->server)->role = LASSO_PROVIDER_ROLE_IDP;
	if (lasso_provider_has_protocol_profile(
				LASSO_PROVIDER(LASSO_PROFILE(login)->server),
				LASSO_MD_PROTOCOL_TYPE_SINGLE_SIGN_ON,
				protocolProfile) == FALSE) {
		return critical_error(LASSO_PROFILE_ERROR_UNSUPPORTED_PROFILE);
	}

	/* get remote ProviderID */
	LASSO_PROFILE(login)->remote_providerID = g_strdup(
			LASSO_LIB_AUTHN_REQUEST(LASSO_PROFILE(login)->request)->ProviderID);

	/* Check authnRequest signature. */
	if (authn_request_msg != NULL) {
		remote_provider = g_hash_table_lookup(LASSO_PROFILE(login)->server->providers,
			LASSO_PROFILE(login)->remote_providerID);
		if (remote_provider != NULL) {
			/* Is authnRequest signed ? */
			authnRequestSigned = lasso_provider_get_metadata_one(
					remote_provider, "AuthnRequestsSigned");
			if (authnRequestSigned != NULL) {
				must_verify_signature = strcmp(authnRequestSigned, "true") == 0;
				g_free(authnRequestSigned);
			} else {
				/* AuthnRequestsSigned element is required */
				message(G_LOG_LEVEL_CRITICAL, "XXX");
				return LASSO_ERROR_UNDEFINED;
			}
		} else {
			return critical_error(LASSO_SERVER_ERROR_PROVIDER_NOT_FOUND,
					LASSO_PROFILE(login)->remote_providerID);
		}

		/* verify request signature */
		if (must_verify_signature) {
			ret = lasso_provider_verify_signature(remote_provider,
					authn_request_msg, "RequestID", format);
			LASSO_PROFILE(login)->signature_status = ret;
		}
	}

	return ret;
}

gint
lasso_login_process_authn_response_msg(LassoLogin *login, gchar *authn_response_msg)
{
	gint ret1 = 0, ret2 = 0;
	LassoMessageFormat format;
	LassoProvider *remote_provider;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);
	g_return_val_if_fail(authn_response_msg != NULL, LASSO_PARAM_ERROR_INVALID_VALUE);
	
	if (LASSO_PROFILE(login)->response)
		lasso_node_destroy(LASSO_PROFILE(login)->response);

	LASSO_PROFILE(login)->response = lasso_lib_authn_response_new(NULL, NULL);
	format = lasso_node_init_from_message(LASSO_PROFILE(login)->response, authn_response_msg);
	if (format == LASSO_MESSAGE_FORMAT_UNKNOWN || format == LASSO_MESSAGE_FORMAT_ERROR) {
		return critical_error(LASSO_PROFILE_ERROR_INVALID_MSG);
	}

	LASSO_PROFILE(login)->remote_providerID = g_strdup(
			LASSO_LIB_AUTHN_RESPONSE(LASSO_PROFILE(login)->response)->ProviderID);

	if (LASSO_PROFILE(login)->remote_providerID == NULL) {
		ret1 = critical_error(LASSO_SERVER_ERROR_PROVIDER_NOT_FOUND);
	}

	remote_provider = g_hash_table_lookup(LASSO_PROFILE(login)->server->providers,
			LASSO_PROFILE(login)->remote_providerID);

	LASSO_PROFILE(login)->msg_relayState = g_strdup(LASSO_LIB_AUTHN_RESPONSE(
			LASSO_PROFILE(login)->response)->RelayState);

	LASSO_PROFILE(login)->signature_status = lasso_provider_verify_signature(
			remote_provider, authn_response_msg, "ResponseID", format);
	ret2 = lasso_login_process_response_status_and_assertion(login);

	/* XXX: and what about signature_status ?  Shouldn't it return error on
	 * failure ? */
	return ret2 == 0 ? ret1 : ret2;
}

gint
lasso_login_process_request_msg(LassoLogin *login, gchar *request_msg)
{
	gint ret = 0;
	LassoProfile *profile = LASSO_PROFILE(login);

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);
	g_return_val_if_fail(request_msg != NULL, LASSO_PARAM_ERROR_INVALID_VALUE);

	/* rebuild samlp:Request with request_msg */
	profile->request = lasso_node_new_from_soap(request_msg);
	if (profile->request == NULL) {
		return critical_error(LASSO_PROFILE_ERROR_INVALID_MSG);
	}
	/* get AssertionArtifact */
	login->assertionArtifact = g_strdup(
			LASSO_SAMLP_REQUEST(profile->request)->AssertionArtifact);

	/* Keep a copy of request msg so signature can be verified when we get
	 * the providerId in lasso_login_build_response_msg()
	 */
	login->private_data->soap_request_msg = g_strdup(request_msg);

	return ret;
}

gint
lasso_login_process_response_msg(LassoLogin *login, gchar *response_msg)
{
	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);
	g_return_val_if_fail(response_msg != NULL, LASSO_PARAM_ERROR_INVALID_VALUE);

	/* rebuild samlp:Response with response_msg */
	LASSO_PROFILE(login)->response = lasso_node_new_from_soap(response_msg);
	if (! LASSO_IS_SAMLP_RESPONSE(LASSO_PROFILE(login)->response) ) {
		lasso_node_destroy(LASSO_PROFILE(login)->response);
		LASSO_PROFILE(login)->response = NULL;
		return critical_error(LASSO_PROFILE_ERROR_INVALID_MSG);
	}

	return lasso_login_process_response_status_and_assertion(login);
}

/*****************************************************************************/
/* private methods                                                           */
/*****************************************************************************/

static struct XmlSnippet schema_snippets[] = {
	{ "AssertionArtifact", SNIPPET_CONTENT, G_STRUCT_OFFSET(LassoLogin, assertionArtifact) },
	{ "NameIDPolicy", SNIPPET_CONTENT, G_STRUCT_OFFSET(LassoLogin, nameIDPolicy) },
	{ "Assertion", SNIPPET_NODE_IN_CHILD, G_STRUCT_OFFSET(LassoLogin, assertion) },
	{ NULL, 0, 0}
};

static LassoNodeClass *parent_class = NULL;

static xmlNode*
get_xmlNode(LassoNode *node, gboolean lasso_dump)
{
	xmlNode *xmlnode;
	LassoLogin *login = LASSO_LOGIN(node);

	xmlnode = parent_class->get_xmlNode(node, lasso_dump);
	xmlSetProp(xmlnode, "LoginDumpVersion", "2");

	if (login->protocolProfile == LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART)
		xmlNewTextChild(xmlnode, NULL, "ProtocolProfile", "Artifact");
	if (login->protocolProfile == LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST)
		xmlNewTextChild(xmlnode, NULL, "ProtocolProfile", "POST");

	return xmlnode;
}

static int
init_from_xml(LassoNode *node, xmlNode *xmlnode)
{
	LassoLogin *login = LASSO_LOGIN(node);
	xmlNode *t;
	int rc;

	rc = parent_class->init_from_xml(node, xmlnode);
	if (rc) return rc;

	t = xmlnode->children;
	while (t) {
		if (t->type != XML_ELEMENT_NODE) {
			t = t->next;
			continue;
		}
		if (strcmp(t->name, "ProtocolProfile") == 0) {
			char *s;
			s = xmlNodeGetContent(t);
			if (strcmp(s, "Artifact") == 0)
				login->protocolProfile = LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART;
			if (strcmp(s, "POST") == 0)
				login->protocolProfile = LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST;
			xmlFree(s);
		}
		t = t->next;
	}
	return 0;
}


/*****************************************************************************/
/* overridden parent class methods                                           */
/*****************************************************************************/

static void
dispose(GObject *object)
{
	LassoLogin *login = LASSO_LOGIN(object);
	g_free(login->private_data->soap_request_msg);
	G_OBJECT_CLASS(parent_class)->dispose(object);
}

static void
finalize(GObject *object)
{  
	LassoLogin *login = LASSO_LOGIN(object);
	g_free(login->private_data);
	G_OBJECT_CLASS(parent_class)->finalize(object);
}

/*****************************************************************************/
/* instance and class init functions */
/*****************************************************************************/

static void
instance_init(LassoLogin *login)
{
	login->private_data = g_new(LassoLoginPrivate, 1);
	login->private_data->soap_request_msg = NULL;

	login->protocolProfile = 0;
	login->assertionArtifact = NULL;
	login->nameIDPolicy = NULL;
	login->http_method = 0;
}

static void
class_init(LassoLoginClass *klass)
{
	LassoNodeClass *nclass = LASSO_NODE_CLASS(klass);

	parent_class = g_type_class_peek_parent(klass);
	nclass->get_xmlNode = get_xmlNode;
	nclass->init_from_xml = init_from_xml;
	nclass->node_data = g_new0(LassoNodeClassData, 1);
	lasso_node_class_set_nodename(nclass, "Login");
	lasso_node_class_add_snippets(nclass, schema_snippets);

	G_OBJECT_CLASS(klass)->dispose = dispose;
	G_OBJECT_CLASS(klass)->finalize = finalize;
}

GType
lasso_login_get_type()
{
	static GType this_type = 0;

	if (!this_type) {
		static const GTypeInfo this_info = {
			sizeof(LassoLoginClass),
			NULL,
			NULL,
			(GClassInitFunc) class_init,
			NULL,
			NULL,
			sizeof(LassoLogin),
			0,
			(GInstanceInitFunc) instance_init,
		};

		this_type = g_type_register_static(LASSO_TYPE_PROFILE,
				"LassoLogin", &this_info, 0);
	}
	return this_type;
}

/**
 * lasso_login_new
 * @server: the #LassoServer
 *
 * Creates a new #LassoLogin.
 *
 * Return value: a newly created #LassoLogin object; or NULL if an error
 *     occured
 **/
LassoLogin*
lasso_login_new(LassoServer *server)
{
	LassoLogin *login = NULL;

	g_return_val_if_fail(LASSO_IS_SERVER(server), NULL);

	login = g_object_new(LASSO_TYPE_LOGIN, NULL);
	LASSO_PROFILE(login)->server = server;

	return login;
}

/**
 * lasso_login_new_from_dump:
 * @server: the #LassoServer
 * @dump: XML login dump
 *
 * Restores the @dump to a new #LassoLogin.
 *
 * Return value: a newly created #LassoLogin; or NULL if an error occured.
 **/
LassoLogin*
lasso_login_new_from_dump(LassoServer *server, const gchar *dump)
{
	LassoLogin *login;
	xmlDoc *doc;

	login = g_object_new(LASSO_TYPE_LOGIN, NULL);
	doc = xmlParseMemory(dump, strlen(dump));
	init_from_xml(LASSO_NODE(login), xmlDocGetRootElement(doc)); 
	LASSO_PROFILE(login)->server = server;

	return login;
}

/**
 * lasso_login_dump:
 * @login: a #LassoLogin
 *
 * Dumps @login content to an XML string.
 *
 * Return value: the dump string.  It must be freed by the caller.
 **/
gchar*
lasso_login_dump(LassoLogin *login)
{
	return lasso_node_dump(LASSO_NODE(login), NULL, 1);
}


int
lasso_login_validate_request_msg(LassoLogin *login, gboolean authentication_result,
		gboolean is_consent_obtained)
{
	LassoProfile *profile;
	gint ret = 0;

	g_return_val_if_fail(LASSO_IS_LOGIN(login), LASSO_PARAM_ERROR_BAD_TYPE_OR_NULL_OBJ);

	profile = LASSO_PROFILE(login);

	/* create LibAuthnResponse */
	profile->response = lasso_lib_authn_response_new(
			LASSO_PROVIDER(profile->server)->ProviderID,
			LASSO_LIB_AUTHN_REQUEST(profile->request));

	/* modify AuthnResponse StatusCode if user authentication is not OK */
	if (authentication_result == FALSE) {
		lasso_profile_set_response_status(profile,
				LASSO_SAML_STATUS_CODE_REQUEST_DENIED);
		return LASSO_LOGIN_ERROR_REQUEST_DENIED;
	}

	/* if signature is not OK => modify AuthnResponse StatusCode */
	if (profile->signature_status == LASSO_DS_ERROR_INVALID_SIGNATURE) {
		lasso_profile_set_response_status(profile,
				LASSO_LIB_STATUS_CODE_INVALID_SIGNATURE);
		return LASSO_LOGIN_ERROR_INVALID_SIGNATURE;
	}

	if (profile->signature_status == LASSO_DS_ERROR_SIGNATURE_NOT_FOUND) {
		/* Unsigned AuthnRequest */
		lasso_profile_set_response_status(profile,
				LASSO_LIB_STATUS_CODE_UNSIGNED_AUTHN_REQUEST);
		return LASSO_LOGIN_ERROR_UNSIGNED_AUTHN_REQUEST;
	}

	if (profile->signature_status == 0 && authentication_result == TRUE) {
		/* process federation */
		ret = lasso_login_process_federation(login, is_consent_obtained);
		if (ret < 0)
			return ret;

		/* XXX: what should be done if ret was > 0 ?  I would return
		 * that code */
	}

	lasso_profile_set_response_status(profile, LASSO_SAML_STATUS_CODE_SUCCESS);

	return ret;
}

