/* -*- Mode: c; c-basic-offset: 8 -*-
 *
 * $Id$
 *
 * SWIG bindings for Lasso Library
 *
 * Copyright (C) 2004 Entr'ouvert
 * http://lasso.entrouvert.org
 *
 * Authors: Romain Chantereau <rchantereau@entrouvert.com>
 *          Emmanuel Raviart <eraviart@entrouvert.com>
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


%module Lasso


%include exception.i       
%include typemaps.i


%{

#if defined(SWIGRUBY) || defined (PHP_VERSION)
/* Ruby and PHP pollute the #define space with these names */
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef PACKAGE_BUGREPORT
#endif

#include <lasso/lasso_config.h>
#include <lasso/lasso.h>
#include <lasso/xml/lib_assertion.h>


/* 
 * Thanks to the patch in this Debian bug for the solution
 * to the crash inside vsnprintf on some architectures.
 *
 * "reuse of args inside the while(1) loop is in violation of the
 * specs and only happens to work by accident on other systems."
 *
 * http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=104325 
 */

#ifndef va_copy
#ifdef __va_copy
#define va_copy(dest,src) __va_copy(dest,src)
#else
#define va_copy(dest,src) (dest) = (src)
#endif
#endif

%}

#ifdef SWIGJAVA
#if SWIG_VERSION >= 0x010322
  %include "enumsimple.swg"
#endif
%pragma(java) jniclasscode=%{
  static {
    try {
        // Load a library whose "core" name is "jlasso".
        // Operating system specific stuff will be added to make an
        // actual filename from this: Under Unix this will become
	// libjlasso.so while under Windows it will likely become
	// something like jlasso.dll.
        System.loadLibrary("jlasso");
    } catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. \n" + e);
      System.exit(1);
    }
    // Initialize Lasso.
    init();
  }
%}
#else

#ifdef SWIGPYTHON
%{
	PyObject *lassoError;
	PyObject *LASSO_WARNING;
%}

%init %{
	lassoError = PyErr_NewException("_lasso.Error", NULL, NULL);
	Py_INCREF(lassoError);
	PyModule_AddObject(m, "Error", lassoError);

	LASSO_WARNING = PyErr_NewException("_lasso.Warning", lassoError, NULL);
	Py_INCREF(LASSO_WARNING);
	PyModule_AddObject(m, "Warning", LASSO_WARNING);
	
	lasso_init();
%}

%pythoncode %{
Error = _lasso.Error
Warning = _lasso.Warning
%}

#else
/* Apache fails when lasso_init is called too early in PHP binding. */
/* FIXME: To investigate. */
#ifndef SWIGPHP4
%init %{
	lasso_init();
%}
#endif
#endif
#endif


/***********************************************************************
 ***********************************************************************
 * Common
 ***********************************************************************
 ***********************************************************************/


#if defined(SWIGPYTHON)
%typemap(in,parse="z") char *, char [ANY] "";
#endif


#if defined(SWIGPHP4)
%{
/* ZVAL_STRING segfault when s is null */
#undef ZVAL_STRING
#define ZVAL_STRING(z, s, duplicate) {	\
		char *__s=(s);					\
        if (__s) {                      \
		  (z)->value.str.len = strlen(__s);	\
		  (z)->value.str.val = (duplicate?estrndup(__s, (z)->value.str.len):__s);	\
		} else {                        \
		  (z)->value.str.len = 0;	    \
		  (z)->value.str.val = empty_string; \
		}                               \
		(z)->type = IS_STRING;	        \
	}
%}
#endif

/* GLib types */

#define gboolean bool
%{
#define bool int
#define false 0
#define true 1
%}
#define gchar char
#define gint int
#define gpointer void*
#define GPtrArray void

/* Functions */

#ifndef SWIGPHP4
%rename(init) lasso_init;
#endif
int lasso_init(void);

#ifndef SWIGPHP4
%rename(shutdown) lasso_shutdown;
#endif
int lasso_shutdown(void);

/* Utilities */

%{

void add_key_to_array(char *key, gpointer pointer, GPtrArray *array)
{
        g_ptr_array_add(array, g_strdup(key));
}

gpointer get_object(gpointer value)
{
	return value == NULL ? NULL : g_object_ref(value);
}

void set_object(gpointer *pointer, gpointer value)
{
	if (*pointer != NULL)
		g_object_unref(*pointer);
	*pointer = value == NULL ? NULL : g_object_ref(value);
}

void set_string(char **pointer, char *value)
{
	if (*pointer != NULL)
		free(*pointer);
	*pointer = value == NULL ? NULL : strdup(value);
}

%}


/***********************************************************************
 * Constants
 ***********************************************************************/


/* Version number */
#ifndef SWIGPHP4
%rename(VERSION_MAJOR) LASSO_VERSION_MAJOR;
%rename(VERSION_MINOR) LASSO_VERSION_MINOR;
%rename(VERSION_SUBMINOR) LASSO_VERSION_SUBMINOR;
#endif
/* Useless because some lines before, we explicitly tell to include lasso_config
 * in the generated wrap C source code.
 * #define LASSO_VERSION_MAJOR 0
#define LASSO_VERSION_MINOR 4
#define LASSO_VERSION_SUBMINOR 0*/


/* HttpMethod */
#ifndef SWIGPHP4
%rename(httpMethodAny) LASSO_HTTP_METHOD_ANY;
%rename(httpMethodIdpInitiated) LASSO_HTTP_METHOD_IDP_INITIATED;
%rename(httpMethodGet) LASSO_HTTP_METHOD_GET;
%rename(httpMethodPost) LASSO_HTTP_METHOD_POST;
%rename(httpMethodRedirect) LASSO_HTTP_METHOD_REDIRECT;
%rename(httpMethodSoap) LASSO_HTTP_METHOD_SOAP;
#endif
typedef enum {
	LASSO_HTTP_METHOD_NONE = -1,
	LASSO_HTTP_METHOD_ANY,
	LASSO_HTTP_METHOD_IDP_INITIATED,
	LASSO_HTTP_METHOD_GET,
	LASSO_HTTP_METHOD_POST,
	LASSO_HTTP_METHOD_REDIRECT,
	LASSO_HTTP_METHOD_SOAP
} lassoHttpMethod;

/* Consent */
#ifndef SWIGPHP4
%rename(libConsentObtained) LASSO_LIB_CONSENT_OBTAINED;
%rename(libConsentObtainedPrior) LASSO_LIB_CONSENT_OBTAINED_PRIOR;
%rename(libConsentObtainedCurrentImplicit) LASSO_LIB_CONSENT_OBTAINED_CURRENT_IMPLICIT;
%rename(libConsentObtainedCurrentExplicit) LASSO_LIB_CONSENT_OBTAINED_CURRENT_EXPLICIT;
%rename(libConsentUnavailable) LASSO_LIB_CONSENT_UNAVAILABLE;
%rename(libConsentInapplicable) LASSO_LIB_CONSENT_INAPPLICABLE;
#endif
#define LASSO_LIB_CONSENT_OBTAINED "urn:liberty:consent:obtained"
#define LASSO_LIB_CONSENT_OBTAINED_PRIOR "urn:liberty:consent:obtained:prior"
#define LASSO_LIB_CONSENT_OBTAINED_CURRENT_IMPLICIT "urn:liberty:consent:obtained:current:implicit"
#define LASSO_LIB_CONSENT_OBTAINED_CURRENT_EXPLICIT "urn:liberty:consent:obtained:current:explicit"
#define LASSO_LIB_CONSENT_UNAVAILABLE "urn:liberty:consent:unavailable"
#define LASSO_LIB_CONSENT_INAPPLICABLE "urn:liberty:consent:inapplicable"

/* NameIdPolicyType */
#ifndef SWIGPHP4
%rename(libNameIdPolicyTypeNone) LASSO_LIB_NAMEID_POLICY_TYPE_NONE;
%rename(libNameIdPolicyTypeOneTime) LASSO_LIB_NAMEID_POLICY_TYPE_ONE_TIME;
%rename(libNameIdPolicyTypeFederated) LASSO_LIB_NAMEID_POLICY_TYPE_FEDERATED;
%rename(libNameIdPolicyTypeAny) LASSO_LIB_NAMEID_POLICY_TYPE_ANY;
#endif
#define LASSO_LIB_NAMEID_POLICY_TYPE_NONE "none"
#define LASSO_LIB_NAMEID_POLICY_TYPE_ONE_TIME "onetime"
#define LASSO_LIB_NAMEID_POLICY_TYPE_FEDERATED "federated"
#define LASSO_LIB_NAMEID_POLICY_TYPE_ANY "any"

/* ProtocolProfile */
#ifndef SWIGPHP4
%rename(libProtocolProfileBrwsArt) LASSO_LIB_PROTOCOL_PROFILE_BRWS_ART;
%rename(libProtocolProfileBrwsPost) LASSO_LIB_PROTOCOL_PROFILE_BRWS_POST;
%rename(libProtocolProfileFedTermIdpHttp) LASSO_LIB_PROTOCOL_PROFILE_FED_TERM_IDP_HTTP;
%rename(libProtocolProfileFedTermIdpSoap) LASSO_LIB_PROTOCOL_PROFILE_FED_TERM_IDP_SOAP;
%rename(libProtocolProfileFedTermSpHttp) LASSO_LIB_PROTOCOL_PROFILE_FED_TERM_SP_HTTP;
%rename(libProtocolProfileFedTermSpSoap) LASSO_LIB_PROTOCOL_PROFILE_FED_TERM_SP_SOAP;
%rename(libProtocolProfileRniIdpHttp) LASSO_LIB_PROTOCOL_PROFILE_RNI_IDP_HTTP;
%rename(libProtocolProfileRniIdpSoap) LASSO_LIB_PROTOCOL_PROFILE_RNI_IDP_SOAP;
%rename(libProtocolProfileRniSpHttp) LASSO_LIB_PROTOCOL_PROFILE_RNI_SP_HTTP;
%rename(libProtocolProfileRniSpSoap) LASSO_LIB_PROTOCOL_PROFILE_RNI_SP_SOAP;
%rename(libProtocolProfileSloIdpHttp) LASSO_LIB_PROTOCOL_PROFILE_SLO_IDP_HTTP;
%rename(libProtocolProfileSloIdpSoap) LASSO_LIB_PROTOCOL_PROFILE_SLO_IDP_SOAP;
%rename(libProtocolProfileSloSpHttp) LASSO_LIB_PROTOCOL_PROFILE_SLO_SP_HTTP;
%rename(libProtocolProfileSloSpSoap) LASSO_LIB_PROTOCOL_PROFILE_SLO_SP_SOAP;
#endif
#define LASSO_LIB_PROTOCOL_PROFILE_BRWS_ART "http://projectliberty.org/profiles/brws-art"
#define LASSO_LIB_PROTOCOL_PROFILE_BRWS_POST "http://projectliberty.org/profiles/brws-post"
#define LASSO_LIB_PROTOCOL_PROFILE_FED_TERM_IDP_HTTP "http://projectliberty.org/profiles/fedterm-idp-http"
#define LASSO_LIB_PROTOCOL_PROFILE_FED_TERM_IDP_SOAP "http://projectliberty.org/profiles/fedterm-idp-soap"
#define LASSO_LIB_PROTOCOL_PROFILE_FED_TERM_SP_HTTP "http://projectliberty.org/profiles/fedterm-sp-http"
#define LASSO_LIB_PROTOCOL_PROFILE_FED_TERM_SP_SOAP "http://projectliberty.org/profiles/fedterm-sp-soap"
#define LASSO_LIB_PROTOCOL_PROFILE_RNI_IDP_HTTP "http://projectliberty.org/profiles/rni-idp-http"
#define LASSO_LIB_PROTOCOL_PROFILE_RNI_IDP_SOAP "http://projectliberty.org/profiles/rni-idp-soap"
#define LASSO_LIB_PROTOCOL_PROFILE_RNI_SP_HTTP "http://projectliberty.org/profiles/rni-sp-http"
#define LASSO_LIB_PROTOCOL_PROFILE_RNI_SP_SOAP "http://projectliberty.org/profiles/rni-sp-soap"
#define LASSO_LIB_PROTOCOL_PROFILE_SLO_IDP_HTTP "http://projectliberty.org/profiles/slo-idp-http"
#define LASSO_LIB_PROTOCOL_PROFILE_SLO_IDP_SOAP "http://projectliberty.org/profiles/slo-idp-soap"
#define LASSO_LIB_PROTOCOL_PROFILE_SLO_SP_HTTP "http://projectliberty.org/profiles/slo-sp-http"
#define LASSO_LIB_PROTOCOL_PROFILE_SLO_SP_SOAP "http://projectliberty.org/profiles/slo-sp-soap"

/* LoginProtocolProfile */
#ifndef SWIGPHP4
%rename(loginProtocolProfileBrwsArt) LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART;
%rename(loginProtocolProfileBrwsPost) LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST;
#endif
typedef enum {
	LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_ART = 1,
	LASSO_LOGIN_PROTOCOL_PROFILE_BRWS_POST,
} lassoLoginProtocolProfile;

/* MessageType */
#ifndef SWIGPHP4
%rename(messageTypeNone) LASSO_MESSAGE_TYPE_NONE;
%rename(messageTypeAuthnRequest) LASSO_MESSAGE_TYPE_AUTHN_REQUEST;
%rename(messageTypeAuthnResponse) LASSO_MESSAGE_TYPE_AUTHN_RESPONSE;
%rename(messageTypeRequest) LASSO_MESSAGE_TYPE_REQUEST;
%rename(messageTypeResponse) LASSO_MESSAGE_TYPE_RESPONSE;
%rename(messageTypeArtifact) LASSO_MESSAGE_TYPE_ARTIFACT;
#endif
typedef enum {
	LASSO_MESSAGE_TYPE_NONE = 0,
	LASSO_MESSAGE_TYPE_AUTHN_REQUEST,
	LASSO_MESSAGE_TYPE_AUTHN_RESPONSE,
	LASSO_MESSAGE_TYPE_REQUEST,
	LASSO_MESSAGE_TYPE_RESPONSE,
	LASSO_MESSAGE_TYPE_ARTIFACT
} lassoMessageType;

/* ProviderRole */
#ifndef SWIGPHP4
%rename(providerRoleNone) LASSO_PROVIDER_ROLE_NONE;
%rename(providerRoleSp) LASSO_PROVIDER_ROLE_SP;
%rename(providerRoleIdp) LASSO_PROVIDER_ROLE_IDP;
#endif
typedef enum {
	LASSO_PROVIDER_ROLE_NONE = 0,
	LASSO_PROVIDER_ROLE_SP,
	LASSO_PROVIDER_ROLE_IDP
} LassoProviderRole;

/* RequestType */
#ifndef SWIGPHP4
%rename(requestTypeInvalid) LASSO_REQUEST_TYPE_INVALID;
%rename(requestTypeLogin) LASSO_REQUEST_TYPE_LOGIN;
%rename(requestTypeLogout) LASSO_REQUEST_TYPE_LOGOUT;
%rename(requestTypeDefederation) LASSO_REQUEST_TYPE_DEFEDERATION;
%rename(requestTypeNameRegistration) LASSO_REQUEST_TYPE_NAME_REGISTRATION;
%rename(requestTypeNameIdentifierMapping) LASSO_REQUEST_TYPE_NAME_IDENTIFIER_MAPPING;
%rename(requestTypeLecp) LASSO_REQUEST_TYPE_LECP;
#endif
typedef enum {
	LASSO_REQUEST_TYPE_INVALID = 0,
	LASSO_REQUEST_TYPE_LOGIN = 1,
	LASSO_REQUEST_TYPE_LOGOUT = 2,
	LASSO_REQUEST_TYPE_DEFEDERATION = 3,
	LASSO_REQUEST_TYPE_NAME_REGISTRATION = 4,
	LASSO_REQUEST_TYPE_NAME_IDENTIFIER_MAPPING = 5,
	LASSO_REQUEST_TYPE_LECP = 6
} lassoRequestType;

/* SamlAuthenticationMethod */
#ifndef SWIGPHP4
%rename(samlAuthenticationMethodPassword) LASSO_SAML_AUTHENTICATION_METHOD_PASSWORD;
%rename(samlAuthenticationMethodKerberos) LASSO_SAML_AUTHENTICATION_METHOD_KERBEROS;
%rename(samlAuthenticationMethodSecureRemotePassword) LASSO_SAML_AUTHENTICATION_METHOD_SECURE_REMOTE_PASSWORD;
%rename(samlAuthenticationMethodHardwareToken) LASSO_SAML_AUTHENTICATION_METHOD_HARDWARE_TOKEN;
%rename(samlAuthenticationMethodSmartcardPki) LASSO_SAML_AUTHENTICATION_METHOD_SMARTCARD_PKI;
%rename(samlAuthenticationMethodSoftwarePki) LASSO_SAML_AUTHENTICATION_METHOD_SOFTWARE_PKI;
%rename(samlAuthenticationMethodPgp) LASSO_SAML_AUTHENTICATION_METHOD_PGP;
%rename(samlAuthenticationMethodSpki) LASSO_SAML_AUTHENTICATION_METHODS_PKI;
%rename(samlAuthenticationMethodXkms) LASSO_SAML_AUTHENTICATION_METHOD_XKMS;
%rename(samlAuthenticationMethodXmlDsig) LASSO_SAML_AUTHENTICATION_METHOD_XMLD_SIG;
%rename(samlAuthenticationMethodUnspecified) LASSO_SAML_AUTHENTICATION_METHOD_UNSPECIFIED;
#endif
#define LASSO_SAML_AUTHENTICATION_METHOD_PASSWORD "urn:oasis:names:tc:SAML:1.0:am:password"
#define LASSO_SAML_AUTHENTICATION_METHOD_KERBEROS "urn:ietf:rfc:1510"
#define LASSO_SAML_AUTHENTICATION_METHOD_SECURE_REMOTE_PASSWORD "urn:ietf:rfc:2945"
#define LASSO_SAML_AUTHENTICATION_METHOD_HARDWARE_TOKEN "urn:oasis:names:tc:SAML:1.0:am:HardwareToken"
#define LASSO_SAML_AUTHENTICATION_METHOD_SMARTCARD_PKI "urn:ietf:rfc:2246"
#define LASSO_SAML_AUTHENTICATION_METHOD_SOFTWARE_PKI "urn:oasis:names:tc:SAML:1.0:am:X509-PKI"
#define LASSO_SAML_AUTHENTICATION_METHOD_PGP "urn:oasis:names:tc:SAML:1.0:am:PGP"
#define LASSO_SAML_AUTHENTICATION_METHODS_PKI "urn:oasis:names:tc:SAML:1.0:am:SPKI"
#define LASSO_SAML_AUTHENTICATION_METHOD_XKMS "urn:oasis:names:tc:SAML:1.0:am:XKMS"
#define LASSO_SAML_AUTHENTICATION_METHOD_XMLD_SIG "urn:ietf:rfc:3075"
#define LASSO_SAML_AUTHENTICATION_METHOD_UNSPECIFIED "urn:oasis:names:tc:SAML:1.0:am:unspecified"

/* SignatureMethod */
#ifndef SWIGPHP4
%rename(signatureMethodRsaSha1) LASSO_SIGNATURE_METHOD_RSA_SHA1;
%rename(signatureMethodDsaSha1) LASSO_SIGNATURE_METHOD_DSA_SHA1;
#endif
typedef enum {
	LASSO_SIGNATURE_METHOD_RSA_SHA1 = 1,
	LASSO_SIGNATURE_METHOD_DSA_SHA1
} lassoSignatureMethod;


/***********************************************************************
 * Errors
 ***********************************************************************/


/* undefined */
#ifndef SWIGPHP4
%rename(ERROR_UNDEFINED) LASSO_ERROR_UNDEFINED;
#endif

/* generic XML */
#ifndef SWIGPHP4
%rename(XML_ERROR_NODE_NOT_FOUND) LASSO_XML_ERROR_NODE_NOT_FOUND;
%rename(XML_ERROR_NODE_CONTENT_NOT_FOUND) LASSO_XML_ERROR_NODE_CONTENT_NOT_FOUND;
%rename(XML_ERROR_ATTR_NOT_FOUND) LASSO_XML_ERROR_ATTR_NOT_FOUND;
%rename(XML_ERROR_ATTR_VALUE_NOT_FOUND) LASSO_XML_ERROR_ATTR_VALUE_NOT_FOUND;
#endif

/* XMLDSig */
#ifndef SWIGPHP4
%rename(DS_ERROR_SIGNATURE_NOT_FOUND) LASSO_DS_ERROR_SIGNATURE_NOT_FOUND;
%rename(DS_ERROR_INVALID_SIGNATURE) LASSO_DS_ERROR_INVALID_SIGNATURE;
%rename(DS_ERROR_SIGNATURE_TMPL_CREATION_FAILED) LASSO_DS_ERROR_SIGNATURE_TMPL_CREATION_FAILED;
%rename(DS_ERROR_CONTEXT_CREATION_FAILED) LASSO_DS_ERROR_CONTEXT_CREATION_FAILED;
%rename(DS_ERROR_PUBLIC_KEY_LOAD_FAILED) LASSO_DS_ERROR_PUBLIC_KEY_LOAD_FAILED;
%rename(DS_ERROR_PRIVATE_KEY_LOAD_FAILED) LASSO_DS_ERROR_PRIVATE_KEY_LOAD_FAILED;
%rename(DS_ERROR_CERTIFICATE_LOAD_FAILED) LASSO_DS_ERROR_CERTIFICATE_LOAD_FAILED;
%rename(DS_ERROR_SIGNATURE_FAILED) LASSO_DS_ERROR_SIGNATURE_FAILED;
%rename(DS_ERROR_KEYS_MNGR_CREATION_FAILED) LASSO_DS_ERROR_KEYS_MNGR_CREATION_FAILED;
%rename(DS_ERROR_KEYS_MNGR_INIT_FAILED) LASSO_DS_ERROR_KEYS_MNGR_INIT_FAILED;
%rename(DS_ERROR_SIGNATURE_VERIFICATION_FAILED) LASSO_DS_ERROR_SIGNATURE_VERIFICATION_FAILED;
%rename(DS_ERROR_CA_CERT_CHAIN_LOAD_FAILED) LASSO_DS_ERROR_CA_CERT_CHAIN_LOAD_FAILED;
%rename(DS_ERROR_INVALID_SIGALG) LASSO_DS_ERROR_INVALID_SIGALG;
%rename(DS_ERROR_DIGEST_COMPUTE_FAILED) LASSO_DS_ERROR_DIGEST_COMPUTE_FAILED;
#endif

/* Server */
#ifndef SWIGPHP4
%rename(SERVER_ERROR_PROVIDER_NOT_FOUND) LASSO_SERVER_ERROR_PROVIDER_NOT_FOUND;
%rename(SERVER_ERROR_ADD_PROVIDER_FAILED) LASSO_SERVER_ERROR_ADD_PROVIDER_FAILED;
#endif

/* Single Logout */
#ifndef SWIGPHP4
%rename(LOGOUT_ERROR_UNSUPPORTED_PROFILE) LASSO_LOGOUT_ERROR_UNSUPPORTED_PROFILE;
#endif

/* Profile */
#ifndef SWIGPHP4
%rename(PROFILE_ERROR_INVALID_QUERY) LASSO_PROFILE_ERROR_INVALID_QUERY;
%rename(PROFILE_ERROR_INVALID_POST_MSG) LASSO_PROFILE_ERROR_INVALID_POST_MSG;
%rename(PROFILE_ERROR_INVALID_SOAP_MSG) LASSO_PROFILE_ERROR_INVALID_SOAP_MSG;
%rename(PROFILE_ERROR_MISSING_REQUEST) LASSO_PROFILE_ERROR_MISSING_REQUEST;
%rename(PROFILE_ERROR_INVALID_HTTP_METHOD) LASSO_PROFILE_ERROR_INVALID_HTTP_METHOD;
%rename(PROFILE_ERROR_INVALID_PROTOCOLPROFILE) LASSO_PROFILE_ERROR_INVALID_PROTOCOLPROFILE;
%rename(PROFILE_ERROR_INVALID_MSG) LASSO_PROFILE_ERROR_INVALID_MSG;
%rename(PROFILE_ERROR_MISSING_REMOTE_PROVIDERID) LASSO_PROFILE_ERROR_MISSING_REMOTE_PROVIDERID;
%rename(PROFILE_ERROR_UNSUPPORTED_PROFILE) LASSO_PROFILE_ERROR_UNSUPPORTED_PROFILE;
%rename(PROFILE_ERROR_UNKNOWN_PROFILE_URL) LASSO_PROFILE_ERROR_UNKNOWN_PROFILE_URL;
%rename(PROFILE_ERROR_IDENTITY_NOT_FOUND) LASSO_PROFILE_ERROR_IDENTITY_NOT_FOUND;
%rename(PROFILE_ERROR_FEDERATION_NOT_FOUND) LASSO_PROFILE_ERROR_FEDERATION_NOT_FOUND;
%rename(PROFILE_ERROR_NAME_IDENTIFIER_NOT_FOUND) LASSO_PROFILE_ERROR_NAME_IDENTIFIER_NOT_FOUND;
%rename(PROFILE_ERROR_BUILDING_QUERY_FAILED) LASSO_PROFILE_ERROR_BUILDING_QUERY_FAILED;
%rename(PROFILE_ERROR_BUILDING_REQUEST_FAILED) LASSO_PROFILE_ERROR_BUILDING_REQUEST_FAILED;
%rename(PROFILE_ERROR_BUILDING_MESSAGE_FAILED) LASSO_PROFILE_ERROR_BUILDING_MESSAGE_FAILED;
%rename(PROFILE_ERROR_BUILDING_RESPONSE_FAILED) LASSO_PROFILE_ERROR_BUILDING_RESPONSE_FAILED;
%rename(PROFILE_ERROR_SESSION_NOT_FOUND) LASSO_PROFILE_ERROR_SESSION_NOT_FOUND;
%rename(PROFILE_ERROR_BAD_IDENTITY_DUMP) LASSO_PROFILE_ERROR_BAD_IDENTITY_DUMP;
%rename(PROFILE_ERROR_BAD_SESSION_DUMP) LASSO_PROFILE_ERROR_BAD_SESSION_DUMP;
#endif

/* functions/methods parameters checking */
#ifndef SWIGPHP4
%rename(PARAM_ERROR_BADTYPE_OR_NULL_OBJ) LASSO_PARAM_ERROR_BADTYPE_OR_NULL_OBJ;
%rename(PARAM_ERROR_INVALID_VALUE) LASSO_PARAM_ERROR_INVALID_VALUE;
%rename(PARAM_ERROR_ERR_CHECK_FAILED) LASSO_PARAM_ERROR_ERR_CHECK_FAILED;
#endif

/* Single Sign-On */
#ifndef SWIGPHP4
%rename(LOGIN_ERROR_FEDERATION_NOT_FOUND) LASSO_LOGIN_ERROR_FEDERATION_NOT_FOUND;
%rename(LOGIN_ERROR_CONSENT_NOT_OBTAINED) LASSO_LOGIN_ERROR_CONSENT_NOT_OBTAINED;
%rename(LASSO_LOGIN_ERROR_INVALID_NAMEIDPOLICY) LASSO_LOGIN_ERROR_INVALID_NAMEIDPOLICY;
%rename(LOGIN_ERROR_REQUEST_DENIE) LASSO_LOGIN_ERROR_REQUEST_DENIE;
%rename(LOGIN_ERROR_INVALID_SIGNATURE) LASSO_LOGIN_ERROR_INVALID_SIGNATURE;
%rename(LOGIN_ERROR_UNSIGNED_AUTHN_REQUEST) LASSO_LOGIN_ERROR_UNSIGNED_AUTHN_REQUEST;
%rename(LOGIN_ERROR_STATUS_NOT_SUCCESS) LASSO_LOGIN_ERROR_STATUS_NOT_SUCCESS;
#endif

/* Federation Termination Notification */
#ifndef SWIGPHP4
%rename(DEFEDERATION_ERROR_MISSING_NAME_IDENTIFIER) LASSO_DEFEDERATION_ERROR_MISSING_NAME_IDENTIFIER;
#endif

#ifndef SWIGPHP4
%rename(strerror) lasso_strerror;
#endif
%ignore lasso_strerror;

%include "../lasso/xml/errors.h"
%{
#include <lasso/xml/errors.h>
%}


/***********************************************************************
 * Exceptions Generation From Lasso Error Codes
 ***********************************************************************/


#ifdef SWIGPYTHON

%{

void lasso_exception(int errorCode) {
	PyObject *errorTuple;

	if (errorCode > 0) {
		errorTuple = Py_BuildValue("(is)", errorCode, "Lasso Warning");
		PyErr_SetObject(LASSO_WARNING, errorTuple);
		Py_DECREF(errorTuple);
	}
	else {
		errorTuple = Py_BuildValue("(is)", errorCode, "Lasso Error");
		PyErr_SetObject(lassoError, errorTuple);
		Py_DECREF(errorTuple);
	}
}

%}

%define THROW_ERROR
%exception {
	int errorCode;
	errorCode = $action
	if (errorCode) {
		lasso_exception(errorCode);
		SWIG_fail;
	}
}
%enddef

#else

%{

void build_exception_msg(int errorCode, char *errorMsg) {
	if (errorCode > 0)
		sprintf(errorMsg, "%d / Lasso Warning", errorCode);
	else
		sprintf(errorMsg, "%d / Lasso Error", errorCode);
}

%}

%define THROW_ERROR
%exception {
	int errorCode;
	errorCode = $action
	if (errorCode) {
		char errorMsg[256];
		build_exception_msg(errorCode, errorMsg);
		SWIG_exception(SWIG_UnknownError, errorMsg);
	}
}
%enddef

#endif

%define END_THROW_ERROR
%exception;
%enddef


/***********************************************************************
 * StringArray
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(StringArray) LassoStringArray;
#endif
%{
typedef GPtrArray LassoStringArray;
%}
typedef struct {
	%extend {
		/* Constructor, Destructor & Static Methods */

		LassoStringArray();

		~LassoStringArray();

		/* Methods */

		void append(char *item) {
			if (item != NULL)
				item = g_strdup(item);
			g_ptr_array_add(self, item);
		}

		GPtrArray *cast() {
			return self;
		}

		static LassoStringArray *frompointer(GPtrArray *stringArray) {
			return (LassoStringArray *) stringArray;
		}

#if defined(SWIGPYTHON)
		%rename(__getitem__) getitem;
#endif
		%newobject getitem;
		%exception getitem {
			if (arg2 < 0 || arg2 >= arg1->len) {
				char errorMsg[256];
				sprintf(errorMsg, "%d", arg2);
				SWIG_exception(SWIG_IndexError, errorMsg);
			}
			$action
		}
		char *getitem(int index) {
			return g_ptr_array_index(self, index);
		}
		%exception getitem;

#if defined(SWIGPYTHON)
		%rename(__len__) length;
#endif
		int length() {
			return self->len;
		}

#if defined(SWIGPYTHON)
		%rename(__setitem__) setitem;
#endif
		%exception setitem {
			if (arg2 < 0 || arg2 >= arg1->len) {
				char errorMsg[256];
				sprintf(errorMsg, "%d", arg2);
				SWIG_exception(SWIG_IndexError, errorMsg);
			}
			$action
		}
		void setitem(int index, char *item) {
			char **itemPtr = (char **) &g_ptr_array_index(self, index);
			if (*itemPtr != NULL)
				free(*itemPtr);
			if (item == NULL)
				*itemPtr = NULL;
			else
				*itemPtr = g_strdup(item);
		}
		%exception setitem;
	}
} LassoStringArray;

%{

/* Constructors, destructors & static methods implementations */

#define new_LassoStringArray g_ptr_array_new
#define delete_LassoStringArray(self) g_ptr_array_free(self, true)

%}


/***********************************************************************
 ***********************************************************************
 * XML Elements without namespace
 ***********************************************************************
 ***********************************************************************/


/***********************************************************************
 * Node
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(Node) LassoNode;
#endif
typedef struct {
} LassoNode;
%extend LassoNode {
	/* Constructor, Destructor & Static Methods */

	LassoNode();

	~LassoNode();

	/* Methods */

	%newobject dump;
	char *dump(char *encoding = NULL, int format = 1);
}

%{

/* Constructors, destructors & static methods implementations */

#define new_LassoNode lasso_node_new
#define delete_LassoNode lasso_node_destroy

/* Methods implementations */

#define LassoNode_dump(self, encoding, format) lasso_node_dump(LASSO_NODE(self), encoding, format)

%}


/***********************************************************************
 ***********************************************************************
 * XML Elements in SAML Namespace
 ***********************************************************************
 ***********************************************************************/


/***********************************************************************
 * saml:Assertion
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(SamlAssertion) LassoSamlAssertion;
#endif
typedef struct {
	/* Attributes */

	char *AssertionID;
	char *Issuer;
	char *IssueInstant;
	int MajorVersion;
	int MinorVersion;

	char *certificate_file;
	char *private_key_file;
	lassoSignatureType sign_type;
	lassoSignatureMethod sign_method;
} LassoSamlAssertion;
%extend LassoSamlAssertion {
	/* Attributes */

	// FIXME: LassoSamlConditions *Conditions;
	// FIXME: LassoSamlAdvice *Advice;
	// FIXME: LassoSamlStatement *Statement;
	// FIXME: LassoSamlSubjectStatement *SubjectStatement;
	// FIXME: LassoSamlAuthenticationStatement *AuthenticationStatement;
	// FIXME: LassoSamlAuthorizationDecisionsStatement *AuthorizationDecisionStatement;
	// FIXME: LassoSamlAttributeStatement *AttributeStatement;

	/* Constructor, Destructor & Static Methods */

	LassoSamlAssertion();

	~LassoSamlAssertion();

	/* Methods inherited from LassoNode */

	%newobject dump;
	char *dump(char *encoding = NULL, int format = 1);
}

%{

/* Constructors, destructors & static methods implementations */

#define new_LassoSamlAssertion lasso_saml_assertion_new
#define delete_LassoSamlAssertion(self) lasso_node_destroy(LASSO_NODE(self))

/* Implementations of methods inherited from LassoNode */

#define LassoSamlAssertion_dump(self, encoding, format) lasso_node_dump(LASSO_NODE(self), encoding, format)

%}


/***********************************************************************
 * saml:NameIdentifier
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(SamlNameIdentifier) LassoSamlNameIdentifier;
#endif
typedef struct {
	/* Attributes */

	char *NameQualifier;
	char *Format;
	char *content;
} LassoSamlNameIdentifier;
%extend LassoSamlNameIdentifier {
	/* Constructor, Destructor & Static Methods */

	LassoSamlNameIdentifier();

	~LassoSamlNameIdentifier();

	/* Methods inherited from LassoNode */

	%newobject dump;
	char *dump(char *encoding = NULL, int format = 1);
}

%{

/* Constructors, destructors & static methods implementations */

#define new_LassoSamlNameIdentifier lasso_saml_name_identifier_new
#define delete_LassoSamlNameIdentifier(self) lasso_node_destroy(LASSO_NODE(self))

/* Implementations of methods inherited from LassoNode */

#define LassoSamlNameIdentifier_dump(self, encoding, format) lasso_node_dump(LASSO_NODE(self), encoding, format)

%}


/***********************************************************************
 ***********************************************************************
 * XML Elements in SAMLP Namespace
 ***********************************************************************
 ***********************************************************************/


/***********************************************************************
 * samlp:Request
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(SamlpRequest) LassoSamlpRequest;
#endif
typedef struct {
	/* Attributes */

	char *AssertionArtifact;
} LassoSamlpRequest;
%extend LassoSamlpRequest {
	/* Constructor, Destructor & Static Methods */

	LassoSamlpRequest();

	~LassoSamlpRequest();

	/* Methods inherited from LassoNode */

	%newobject dump;
	char *dump(char *encoding = NULL, int format = 1);
}

%{

/* Constructors, destructors & static methods implementations */

#define new_LassoSamlpRequest lasso_samlp_request_new
#define delete_LassoSamlpRequest(self) lasso_node_destroy(LASSO_NODE(self))

/* Implementations of methods inherited from LassoNode */

#define LassoSamlpRequest_dump(self, encoding, format) lasso_node_dump(LASSO_NODE(self), encoding, format)

%}


/***********************************************************************
 * samlp:Response
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(SamlpResponse) LassoSamlpResponse;
#endif
typedef struct {
} LassoSamlpResponse;
%extend LassoSamlpResponse {
	/* Attributes */

	// FIXME: LassoSamlAssertion *Assertion;
	LassoSamlpStatus *Status;

	/* Constructor, Destructor & Static Methods */

	LassoSamlpResponse();

	~LassoSamlpResponse();

	/* Methods inherited from LassoNode */

	%newobject dump;
	char *dump(char *encoding = NULL, int format = 1);
}

%{

/* Attributes Implementations */

/* Status */
#define LassoSamlpResponse_get_Status(self) get_object((self)->Status)
#define LassoSamlpResponse_Status_get(self) get_object((self)->Status)
#define LassoSamlpResponse_set_Status(self, value) set_object((gpointer *) &(self)->Status, (value))
#define LassoSamlpResponse_Status_set(self, value) set_object((gpointer *) &(self)->Status, (value))

/* Constructors, destructors & static methods implementations */

#define new_LassoSamlpResponse lasso_samlp_response_new
#define delete_LassoSamlpResponse(self) lasso_node_destroy(LASSO_NODE(self))

/* Implementations of methods inherited from LassoNode */

#define LassoSamlpResponse_dump(self, encoding, format) lasso_node_dump(LASSO_NODE(self), encoding, format)

%}


/***********************************************************************
 * samlp:Status
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(SamlpStatus) LassoSamlpStatus;
#endif
typedef struct {
	/* Attributes */

	char *StatusMessage;
} LassoSamlpStatus;
%extend LassoSamlpStatus {
	/* Attributes */

	LassoSamlpStatusCode *StatusCode;

	/* Constructor, Destructor & Static Methods */

	LassoSamlpStatus();

	~LassoSamlpStatus();

	/* Methods inherited from LassoNode */

	%newobject dump;
	char *dump(char *encoding = NULL, int format = 1);
}

%{

/* Attributes Implementations */

/* StatusCode */
#define LassoSamlpStatus_get_StatusCode(self) get_object((self)->StatusCode)
#define LassoSamlpStatus_StatusCode_get(self) get_object((self)->StatusCode)
#define LassoSamlpStatus_set_StatusCode(self, value) set_object((gpointer *) &(self)->StatusCode, (value))
#define LassoSamlpStatus_StatusCode_set(self, value) set_object((gpointer *) &(self)->StatusCode, (value))

/* Constructors, destructors & static methods implementations */

#define new_LassoSamlpStatus lasso_samlp_status_new
#define delete_LassoSamlpStatus(self) lasso_node_destroy(LASSO_NODE(self))

/* Implementations of methods inherited from LassoNode */

#define LassoSamlpStatus_dump(self, encoding, format) lasso_node_dump(LASSO_NODE(self), encoding, format)

%}


/***********************************************************************
 * samlp:StatusCode
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(SamlpStatusCode) LassoSamlpStatusCode;
#endif
typedef struct {
	/* Attributes */

	char *Value;
} LassoSamlpStatusCode;
%extend LassoSamlpStatusCode {
	/* Attributes */

	LassoSamlpStatusCode *StatusCode;

	/* Constructor, Destructor & Static Methods */

	LassoSamlpStatusCode();

	~LassoSamlpStatusCode();

	/* Methods inherited from LassoNode */

	%newobject dump;
	char *dump(char *encoding = NULL, int format = 1);
}

%{

/* Attributes Implementations */

/* StatusCode */
#define LassoSamlpStatusCode_get_StatusCode(self) get_object((self)->StatusCode)
#define LassoSamlpStatusCode_StatusCode_get(self) get_object((self)->StatusCode)
#define LassoSamlpStatusCode_set_StatusCode(self, value) set_object((gpointer *) &(self)->StatusCode, (value))
#define LassoSamlpStatusCode_StatusCode_set(self, value) set_object((gpointer *) &(self)->StatusCode, (value))

/* Constructors, destructors & static methods implementations */

#define new_LassoSamlpStatusCode lasso_samlp_status_code_new
#define delete_LassoSamlpStatusCode(self) lasso_node_destroy(LASSO_NODE(self))

/* Implementations of methods inherited from LassoNode */

#define LassoSamlpStatusCode_dump(self, encoding, format) lasso_node_dump(LASSO_NODE(self), encoding, format)

%}


/***********************************************************************
 ***********************************************************************
 * XML Elements in Liberty Namespace
 ***********************************************************************
 ***********************************************************************/


/***********************************************************************
 * lib:Assertion
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(LibAssertion) LassoLibAssertion;
#endif
typedef struct {
} LassoLibAssertion;
%extend LassoLibAssertion {
	/* Attributes inherited from SamlAssertion */

/* 	char *AssertionID; */
/* 	char *Issuer; */
/* 	char *IssueInstant; */
/* 	int MajorVersion; */
/* 	int MinorVersion; */

/* 	// FIXME: LassoSamlConditions *Conditions; */
/* 	// FIXME: LassoSamlAdvice *Advice; */
/* 	// FIXME: LassoSamlStatement *Statement; */
/* 	// FIXME: LassoSamlSubjectStatement *SubjectStatement; */
/* 	// FIXME: LassoSamlAuthenticationStatement *AuthenticationStatement; */
/* 	// FIXME: LassoSamlAuthorizationDecisionsStatement *AuthorizationDecisionStatement; */
/* 	// FIXME: LassoSamlAttributeStatement *AttributeStatement; */

/* 	char *certificate_file; */
/* 	char *private_key_file; */
/* 	lassoSignatureType sign_type; */
/* 	lassoSignatureMethod sign_method; */

	/* Constructor, Destructor & Static Methods */

	LassoLibAssertion(char *issuer, char *requestId, char *audience,
			  char *notBefore, char *notOnOrAfter);

	~LassoLibAssertion();

	/* Methods inherited from LassoNode */

	%newobject dump;
	char *dump(char *encoding = NULL, int format = 1);
}

%{

/* Implementations of methods inherited from SamlAssertion */

/* /\* AssertionID *\/ */
/* #define LassoLibAssertion_get_AssertionID(self) get_object((self)->AssertionID) */
/* #define LassoLibAssertion_AssertionID_get(self) get_object((self)->AssertionID) */
/* #define LassoLibAssertion_set_AssertionID(self, value) set_object((gpointer *) &(self)->AssertionID, (value)) */
/* #define LassoLibAssertion_AssertionID_set(self, value) set_object((gpointer *) &(self)->AssertionID, (value)) */

/* Constructors, destructors & static methods implementations */

#define new_LassoLibAssertion lasso_lib_assertion_new_full
#define delete_LassoLibAssertion(self) lasso_node_destroy(LASSO_NODE(self))

/* Implementations of methods inherited from LassoNode */

#define LassoLibAssertion_dump(self, encoding, format) lasso_node_dump(LASSO_NODE(self), encoding, format)

%}


/***********************************************************************
 * lib:AuthnRequest
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(LibAuthnRequest) LassoLibAuthnRequest;
#endif
%nodefault LassoLibAuthnRequest;
typedef struct {
	%extend {
		/* XXX shouldn't need all of this now */
		/* Attributes from LassoLibAuthnRequest */

		char *affiliationId;
		char *assertionConsumerServiceId;
		char *consent;
		LassoStringArray *extension;
		gboolean forceAuthn;
		gboolean isPassive;
		char *nameIdPolicy;
		char *protocolProfile;
		char *providerId;
		char *relayState;
	}
} LassoLibAuthnRequest;

%{

/* Attributes Implementations */

/* affiliationId */
#define LassoLibAuthnRequest_get_affiliationId LassoLibAuthnRequest_affiliationId_get
char *LassoLibAuthnRequest_affiliationId_get(LassoLibAuthnRequest *self) {
	return NULL; /* FIXME */
}
#define LassoLibAuthnRequest_set_affiliationId(self, value) set_string(&(self)->AffiliationID, (value))
#define LassoLibAuthnRequest_affiliationId_set(self, value) set_string(&(self)->AffiliationID, (value))

/* assertionConsumerServiceId */
#define LassoLibAuthnRequest_get_assertionConsumerServiceId LassoLibAuthnRequest_assertionConsumerServiceId_get
char *LassoLibAuthnRequest_assertionConsumerServiceId_get(LassoLibAuthnRequest *self) {
	return NULL; /* FIXME */
}
#define LassoLibAuthnRequest_set_assertionConsumerServiceId(self, value) set_string(&(self)->AssertionConsumerServiceID, (value))
#define LassoLibAuthnRequest_assertionConsumerServiceId_set(self, value) set_string(&(self)->AssertionConsumerServiceID, (value))

/* consent */
#define LassoLibAuthnRequest_get_consent LassoLibAuthnRequest_consent_get
char *LassoLibAuthnRequest_consent_get(LassoLibAuthnRequest *self) {
	return NULL; /* FIXME */
}
#define LassoLibAuthnRequest_set_consent(self, value) set_string(&(self)->consent, (value))
#define LassoLibAuthnRequest_consent_set(self, value) set_string(&(self)->consent, (value))

/* extension */
#define LassoLibAuthnRequest_get_extension LassoLibAuthnRequest_extension_get
LassoStringArray *LassoLibAuthnRequest_extension_get(LassoLibAuthnRequest *self) {
	return NULL; /* FIXME */
}

static void free_xml_list_elem(xmlNode *xmlnode, gpointer unused)
{
	xmlFreeNode(xmlnode);
}

#define LassoLibAuthnRequest_set_extension LassoLibAuthnRequest_extension_set
void LassoLibAuthnRequest_extension_set(LassoLibAuthnRequest *self, LassoStringArray *extension) {
	if (self->Extension != NULL) {
		g_list_foreach(self->Extension, (GFunc) free_xml_list_elem, NULL);
		g_list_free(self->Extension);
	}
	if (extension == NULL)
		self->Extension = NULL;
	else {
		int index;
		for (index = 0; index < extension->len; index ++) {
			xmlDoc *doc;
			xmlNode *node;
			doc = xmlReadDoc(g_ptr_array_index(extension, index), NULL, NULL,
					XML_PARSE_NONET);
			if (doc == NULL)
				continue;
			node = xmlDocGetRootElement(doc);
			if (node != NULL) {
				xmlNode *extensionNode;
				xmlNs *libertyNamespace;
				extensionNode = xmlNewNode(NULL, "Extension");
				libertyNamespace = xmlNewNs(extensionNode, LASSO_LIB_HREF,
						LASSO_LIB_PREFIX);
				xmlSetNs(extensionNode, libertyNamespace);
				xmlAddChild(extensionNode, xmlCopyNode(node, 1));
				self->Extension = g_list_append(self->Extension, extensionNode);
						
			}
			xmlFreeDoc(doc);
		}
	}
}

/* forceAuthn */
#define LassoLibAuthnRequest_get_forceAuthn LassoLibAuthnRequest_forceAuthn_get
gboolean LassoLibAuthnRequest_forceAuthn_get(LassoLibAuthnRequest *self) {
	return 0; /* FIXME */
}
#define LassoLibAuthnRequest_set_forceAuthn LassoLibAuthnRequest_forceAuthn_set
void LassoLibAuthnRequest_forceAuthn_set(LassoLibAuthnRequest *self, gboolean forceAuthn) {
	 self->ForceAuthn = forceAuthn;
}

/* isPassive */
#define LassoLibAuthnRequest_get_isPassive LassoLibAuthnRequest_isPassive_get
gboolean LassoLibAuthnRequest_isPassive_get(LassoLibAuthnRequest *self) {
	return self->IsPassive;
}
#define LassoLibAuthnRequest_set_isPassive LassoLibAuthnRequest_isPassive_set
void LassoLibAuthnRequest_isPassive_set(LassoLibAuthnRequest *self, gboolean isPassive) {
	self->IsPassive = isPassive;
}

/* nameIdPolicy */
#define LassoLibAuthnRequest_get_nameIdPolicy(self) (self)->NameIDPolicy
#define LassoLibAuthnRequest_nameIdPolicy_get(self) (self)->NameIDPolicy
#define LassoLibAuthnRequest_set_nameIdPolicy(self, value) set_string(&(self)->NameIDPolicy, (value))
#define LassoLibAuthnRequest_nameIdPolicy_set(self, value) set_string(&(self)->NameIDPolicy, (value))

/* protocolProfile */
#define LassoLibAuthnRequest_get_protocolProfile(self) (self)->ProtocolProfile
#define LassoLibAuthnRequest_protocolProfile_get(self) (self)->ProtocolProfile
#define LassoLibAuthnRequest_set_protocolProfile(self, value) set_string(&(self)->ProtocolProfile, (value))
#define LassoLibAuthnRequest_protocolProfile_set(self, value) set_string(&(self)->ProtocolProfile, (value))

/* providerId */
#define LassoLibAuthnRequest_get_providerId(self) (self)->ProviderID
#define LassoLibAuthnRequest_providerId_get(self) (self)->ProviderID
#define LassoLibAuthnRequest_set_providerId(self, value) set_string(&(self)->ProviderID, (value))
#define LassoLibAuthnRequest_providerId_set(self, value) set_string(&(self)->ProviderID, (value))

/* relayState */
#define LassoLibAuthnRequest_get_relayState(self) (self)->RelayState
#define LassoLibAuthnRequest_relayState_get(self) (self)->RelayState
#define LassoLibAuthnRequest_set_relayState(self, value) set_string(&(self)->RelayState, (value))
#define LassoLibAuthnRequest_relayState_set(self, value) set_string(&(self)->RelayState, (value))

%}


/***********************************************************************
 * lib:AuthnResponse
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(LibAuthnResponse) LassoLibAuthnResponse;
#endif
%nodefault LassoLibAuthnResponse;
typedef struct {
	%extend {
		/* Attributes inherited from LassoSamlpResponse */
		LassoSamlpStatus *Status;
		// FIXME: LassoSamlAssertion *Assertion;
	}
} LassoLibAuthnResponse;

%{

/* Attributes inherited from LassoSamlpResponse implementations */

/* Status */
#define LassoLibAuthnResponse_get_Status(self) get_object(LASSO_SAMLP_RESPONSE(self)->Status)
#define LassoLibAuthnResponse_Status_get(self) get_object(LASSO_SAMLP_RESPONSE(self)->Status)
#define LassoLibAuthnResponse_set_Status(self, value) set_object((gpointer *) &LASSO_SAMLP_RESPONSE(self)->Status, (value))
#define LassoLibAuthnResponse_Status_set(self, value) set_object((gpointer *) &LASSO_SAMLP_RESPONSE(self)->Status, (value))

%}


/***********************************************************************
 * lib:FederationTerminationNotification
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(LibFederationTerminationNotification) LassoLibFederationTerminationNotification;
#endif
%nodefault LassoLibFederationTerminationNotification;
typedef struct {
	/* FIXME: Add a relayState when Lasso supports it. */
} LassoLibFederationTerminationNotification;


/***********************************************************************
 * lib:LogoutRequest
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(LibLogoutRequest) LassoLibLogoutRequest;
#endif
%nodefault LassoLibLogoutRequest;
typedef struct {
	%extend {
		/* Attributes */
		char *relayState;
	}
} LassoLibLogoutRequest;

%{

/* Attributes */

/* relayState */
#define LassoLibLogoutRequest_get_relayState(self) (self)->RelayState
#define LassoLibLogoutRequest_relayState_get(self) (self)->RelayState
#define LassoLibLogoutRequest_set_relayState(self, value) set_string(&(self)->RelayState, (value))
#define LassoLibLogoutRequest_relayState_set(self, value) set_string(&(self)->RelayState, (value))

%}


/***********************************************************************
 * lib:LogoutResponse
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(LibLogoutResponse) LassoLibLogoutResponse;
#endif
%nodefault LassoLibLogoutResponse;
typedef struct {
	%extend {
		/* Attributes inherited from LassoLibStatusResponse */
		LassoSamlpStatus *Status;
		// FIXME: LassoSamlAssertion *Assertion;
	}
} LassoLibLogoutResponse;

%{

/* Attributes inherited from LassoLibStatusResponse implementations */

/* Status */
#define LassoLibLogoutResponse_get_Status(self) get_object(LASSO_LIB_STATUS_RESPONSE(self)->Status)
#define LassoLibLogoutResponse_Status_get(self) get_object(LASSO_LIB_STATUS_RESPONSE(self)->Status)
#define LassoLibLogoutResponse_set_Status(self, value) set_object((gpointer *) &LASSO_LIB_STATUS_RESPONSE(self)->Status, (value))
#define LassoLibLogoutResponse_Status_set(self, value) set_object((gpointer *) &LASSO_LIB_STATUS_RESPONSE(self)->Status, (value))

%}


/***********************************************************************
 * lib:RegisterNameIdentifierRequest
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(LibRegisterNameIdentifierRequest) LassoLibRegisterNameIdentifierRequest;
#endif
%nodefault LassoLibRegisterNameIdentifierRequest;
typedef struct {
	%extend {
		/* Attributes inherited from LassoLibRegisterNameIdentifierRequest */

		char *relayState;
	}
} LassoLibRegisterNameIdentifierRequest;

%{

/* Attributes Implementations */

/* relayState */
#define LassoLibRegisterNameIdentifierRequest_get_relayState(self) (self)->RelayState
#define LassoLibRegisterNameIdentifierRequest_relayState_get(self) (self)->RelayState
#define LassoLibRegisterNameIdentifierRequest_set_relayState(self, value) set_string(&(self)->RelayState, (value))
#define LassoLibRegisterNameIdentifierRequest_relayState_set(self, value) set_string(&(self)->RelayState, (value))

%}


/***********************************************************************
 * lib:RegisterNameIdentifierResponse
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(LibRegisterNameIdentifierResponse) LassoLibRegisterNameIdentifierResponse;
#endif
%nodefault LassoLibRegisterNameIdentifierResponse;
typedef struct {
} LassoLibRegisterNameIdentifierResponse;


/***********************************************************************
 * lib:StatusResponse
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(LibStatusResponse) LassoLibStatusResponse;
#endif
%nodefault LassoLibStatusResponse;
typedef struct {
	// FIXME: GList *Extension;
	// FIXME: char *ProviderID;
	LassoSamlpStatus *Status;
	// FIXME: char *RelayState;
} LassoLibStatusResponse;


/***********************************************************************
 ***********************************************************************
 * ID-FF
 ***********************************************************************
 ***********************************************************************/


/***********************************************************************
 * lasso:Provider
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(Provider) LassoProvider;
#endif
%nodefault LassoProvider;
typedef struct {
	/* XXX
	%immutable metadata;
	LassoNode *metadata;
	*/

	%immutable role;
	LassoProviderRole role;

	%extend {
		/* Attributes */
		%immutable providerId;
		%newobject providerId_get;
		char *providerId;
	}
} LassoProvider;

%{

/* Attributes implementations */

/* providerId */
#define LassoProvider_get_providerId(self) (self)->ProviderID
#define LassoProvider_providerId_get(self) (self)->ProviderID

%}


/***********************************************************************
 * lasso:Server
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(Server) LassoServer;
#endif
typedef struct {
	%extend {
		/* Attributes */

		%immutable providerId;
		char *providerId;

		%immutable providerIds;
		%newobject providerIds_get;
		LassoStringArray *providerIds;

		/* Constructor, destructor & static methods */

		LassoServer(char *metadata = NULL, char *privateKey = NULL,
			    char *secretKey = NULL, char *certificate = NULL);

		~LassoServer();

		%newobject newFromDump;
		static LassoServer *newFromDump(char *dump);

		/* Methods */

	        THROW_ERROR
		void addProvider(LassoProviderRole role, char *metadata, char *publicKey = NULL,
				 char *caCertChain = NULL);
		END_THROW_ERROR

		%newobject dump;
		char *dump();

		LassoProvider *getProvider(char *providerId);
	}
} LassoServer;

%{

/* Attributes implementations */

/* providerId */
#define LassoServer_get_providerId(self) LASSO_PROVIDER(self)->ProviderID
#define LassoServer_providerId_get(self) LASSO_PROVIDER(self)->ProviderID

/* providerIds */
#define LassoServer_get_providerIds LassoServer_providerIds_get
LassoStringArray *LassoServer_providerIds_get(LassoServer *self) {
	GPtrArray *providerIds = g_ptr_array_sized_new(g_hash_table_size(self->providers));
	g_hash_table_foreach(self->providers, (GHFunc) add_key_to_array, providerIds);
	return providerIds;
}

/* Constructors, destructors & static methods implementations */

#define new_LassoServer lasso_server_new
#define delete_LassoServer lasso_server_destroy
#ifdef PHP_VERSION
#define LassoServer_newFromDump lasso_server_new_from_dump
#else
#define Server_newFromDump lasso_server_new_from_dump
#endif

/* Methods implementations */

#define LassoServer_addProvider lasso_server_add_provider
#define LassoServer_dump lasso_server_dump
#define LassoServer_getProvider lasso_server_get_provider

%}


/***********************************************************************
 * lasso:Identity
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(Identity) LassoIdentity;
#endif
typedef struct {
	%extend {
		/* Attributes */

		%immutable isDirty;
		gboolean isDirty;

		%immutable providerIds;
		%newobject providerIds_get;
		LassoStringArray *providerIds;

		/* Constructor, Destructor & Static Methods */

		LassoIdentity();

		~LassoIdentity();

		%newobject newFromDump;
		static LassoIdentity *newFromDump(char *dump);

		/* Methods */

		%newobject dump;
		char *dump();
	}
} LassoIdentity;

%{

/* Attributes implementations */

/* isDirty */
#define LassoIdentity_get_isDirty(self) (self)->is_dirty
#define LassoIdentity_isDirty_get(self) (self)->is_dirty

/* providerIds */
#define LassoIdentity_get_providerIds LassoIdentity_providerIds_get
LassoStringArray *LassoIdentity_providerIds_get(LassoIdentity *self) {
	GPtrArray *providerIds = g_ptr_array_sized_new(g_hash_table_size(self->federations));
	g_hash_table_foreach(self->federations, (GHFunc) add_key_to_array, providerIds);
	return providerIds;
}


/* Constructors, destructors & static methods implementations */

#define new_LassoIdentity lasso_identity_new
#define delete_LassoIdentity lasso_identity_destroy
#ifdef PHP_VERSION
#define LassoIdentity_newFromDump lasso_identity_new_from_dump
#else
#define Identity_newFromDump lasso_identity_new_from_dump
#endif

/* Methods implementations */

#define LassoIdentity_dump lasso_identity_dump

%}


/***********************************************************************
 * lasso:Session
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(Session) LassoSession;
#endif
typedef struct {
	%extend {
		/* Attributes */

		%immutable isDirty;
		gboolean isDirty;

		%immutable providerIds;
		%newobject providerIds_get;
		LassoStringArray *providerIds;

		/* Constructor, destructor & static methods */

		LassoSession();

		~LassoSession();

		%newobject newFromDump;
		static LassoSession *newFromDump(char *dump);

		/* Methods */

		%newobject dump;
		char *dump();
	}
} LassoSession;

%{

/* Attributes implementations */

/* isDirty */
#define LassoSession_get_isDirty(self) (self)->is_dirty
#define LassoSession_isDirty_get(self) (self)->is_dirty

/* providerIds */
#define LassoSession_get_providerIds LassoSession_providerIds_get
LassoStringArray *LassoSession_providerIds_get(LassoSession *self) {
	GPtrArray *providerIds = g_ptr_array_sized_new(g_hash_table_size(self->assertions));
	g_hash_table_foreach(self->assertions, (GHFunc) add_key_to_array, providerIds);
	return providerIds;
}

/* Constructors, destructors & static methods implementations */

#define new_LassoSession lasso_session_new
#define delete_LassoSession lasso_session_destroy
#ifdef PHP_VERSION
#define LassoSession_newFromDump lasso_session_new_from_dump
#else
#define Session_newFromDump lasso_session_new_from_dump
#endif

/* Methods implementations */

#define LassoSession_dump lasso_session_dump

%}


/***********************************************************************
 * lasso:Profile
 ***********************************************************************/


/* Functions */

#ifdef SWIGPHP4
%rename(lasso_getRequestTypeFromSoapMsg) lasso_profile_get_request_type_from_soap_msg;
#else
%rename(getRequestTypeFromSoapMsg) lasso_profile_get_request_type_from_soap_msg;
#endif
lassoRequestType lasso_profile_get_request_type_from_soap_msg(char *soap);

#ifdef SWIGPHP4
%rename(lasso_isLibertyQuery) lasso_profile_is_liberty_query;
#else
%rename(isLibertyQuery) lasso_profile_is_liberty_query;
#endif
gboolean lasso_profile_is_liberty_query(char *query);


/***********************************************************************
 * lasso:Defederation
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(Defederation) LassoDefederation;
#endif
typedef struct {
} LassoDefederation;
%extend LassoDefederation {
	/* Attributes inherited from LassoProfile */

	%newobject identity_get;
	LassoIdentity *identity;

	%immutable isIdentityDirty;
	gboolean isIdentityDirty;

	%immutable isSessionDirty;
	gboolean isSessionDirty;

	%immutable msgBody;
	char *msgBody;

	%immutable msgRelayState;
	char *msgRelayState;

	%immutable msgUrl;
	char *msgUrl;

	LassoSamlNameIdentifier *nameIdentifier;

	%newobject remoteProviderId_get;
	char *remoteProviderId;

	%immutable request;
	LassoLibFederationTerminationNotification *request;

	%newobject session_get;
	LassoSession *session;

	/* Constructor, Destructor & Static Methods */

	LassoDefederation(LassoServer *server);

	~LassoDefederation();

	/* Methods inherited from LassoProfile */

        THROW_ERROR
	void setIdentityFromDump(char *dump);
	END_THROW_ERROR

	THROW_ERROR
	void setSessionFromDump(char *dump);
	END_THROW_ERROR

	/* Methods */

	THROW_ERROR
	void buildNotificationMsg();
	END_THROW_ERROR

	THROW_ERROR
	void initNotification(char *remoteProviderId = NULL,
			      lassoHttpMethod httpMethod = LASSO_HTTP_METHOD_ANY);
	END_THROW_ERROR

	THROW_ERROR
	void processNotificationMsg(char *notificationMsg);
	END_THROW_ERROR

	THROW_ERROR
	void validateNotification();
	END_THROW_ERROR
}

%{

/* Attributes inherited from LassoProfile implementations */

/* identity */
#define LassoDefederation_get_identity(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoDefederation_identity_get(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoDefederation_set_identity(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))
#define LassoDefederation_identity_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))

/* isIdentityDirty */
#define LassoDefederation_get_isIdentityDirty(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))
#define LassoDefederation_isIdentityDirty_get(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))

/* isSessionDirty */
#define LassoDefederation_get_isSessionDirty(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))
#define LassoDefederation_isSessionDirty_get(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))

/* msgBody */
#define LassoDefederation_get_msgBody(self) LASSO_PROFILE(self)->msg_body
#define LassoDefederation_msgBody_get(self) LASSO_PROFILE(self)->msg_body

/* msgRelayState */
#define LassoDefederation_get_msgRelayState(self) LASSO_PROFILE(self)->msg_relayState
#define LassoDefederation_msgRelayState_get(self) LASSO_PROFILE(self)->msg_relayState

/* msgUrl */
#define LassoDefederation_get_msgUrl(self) LASSO_PROFILE(self)->msg_url
#define LassoDefederation_msgUrl_get(self) LASSO_PROFILE(self)->msg_url

/* nameIdentifier */
#define LassoDefederation_get_nameIdentifier(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoDefederation_nameIdentifier_get(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoDefederation_set_nameIdentifier(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))
#define LassoDefederation_nameIdentifier_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))

/* remoteProviderId */
#define LassoDefederation_get_remoteProviderId(self) LASSO_PROFILE(self)->remote_providerID
#define LassoDefederation_remoteProviderId_get(self) LASSO_PROFILE(self)->remote_providerID
#define LassoDefederation_set_remoteProviderId(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))
#define LassoDefederation_remoteProviderId_set(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))

/* request */
#define LassoDefederation_get_request(self) LASSO_LIB_FEDERATION_TERMINATION_NOTIFICATION(LASSO_PROFILE(self)->request)
#define LassoDefederation_request_get(self) LASSO_LIB_FEDERATION_TERMINATION_NOTIFICATION(LASSO_PROFILE(self)->request)

/* responseStatus */
#define LassoDefederation_get_responseStatus(self) NULL /* FIXME: no set */
#define LassoDefederation_responseStatus_get(self) NULL /* FIXME: no set */
#define LassoDefederation_set_responseStatus(self, value) lasso_profile_set_response_status(LASSO_PROFILE(self), value)
#define LassoDefederation_responseStatus_set(self, value) lasso_profile_set_response_status(LASSO_PROFILE(self), value)

/* session */
#define LassoDefederation_get_session(self) lasso_profile_get_session(LASSO_PROFILE(self))
#define LassoDefederation_session_get(self) lasso_profile_get_session(LASSO_PROFILE(self))
#define LassoDefederation_set_session(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))
#define LassoDefederation_session_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))

/* Constructors, destructors & static methods implementations */

#define new_LassoDefederation lasso_defederation_new
#define delete_LassoDefederation lasso_defederation_destroy

/* Methods inherited from LassoProfile implementations */

int LassoDefederation_setIdentityFromDump(LassoDefederation *self, char *dump) {
	return lasso_profile_set_identity_from_dump(LASSO_PROFILE(self), dump);
}

int LassoDefederation_setSessionFromDump(LassoDefederation *self, char *dump) {
	return lasso_profile_set_session_from_dump(LASSO_PROFILE(self), dump);
}

/* Methods implementations */

#define LassoDefederation_buildNotificationMsg lasso_defederation_build_notification_msg
#define LassoDefederation_initNotification lasso_defederation_init_notification
#define LassoDefederation_processNotificationMsg lasso_defederation_process_notification_msg
#define LassoDefederation_validateNotification lasso_defederation_validate_notification

%}


/***********************************************************************
 * lasso:Login
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(Login) LassoLogin;
#endif
typedef struct {
	%immutable assertionArtifact;
	char *assertionArtifact;

	%immutable protocolProfile;
	lassoLoginProtocolProfile protocolProfile;

	%extend {
		/* Attributes inherited from LassoProfile */

		%immutable authnRequest;
		LassoLibAuthnRequest *authnRequest;

		%immutable authnResponse;
		LassoLibAuthnResponse *authnResponse;

		%newobject identity_get;
		LassoIdentity *identity;

		%immutable isIdentityDirty;
		gboolean isIdentityDirty;

		%immutable isSessionDirty;
		gboolean isSessionDirty;

		%immutable msgBody;
		char *msgBody;

		%immutable msgRelayState;
		char *msgRelayState;

		%immutable msgUrl;
		char *msgUrl;

		LassoSamlNameIdentifier *nameIdentifier;

		%newobject remoteProviderId_get;
		char *remoteProviderId;

		%immutable request;
		LassoSamlpRequest *request;

		%immutable response;
		LassoSamlpResponse *response;

		char *responseStatus;

		%newobject session_get;
		LassoSession *session;

		/* Constructor, Destructor & Static Methods */

		LassoLogin(LassoServer *server);

		~LassoLogin();

		%newobject newFromDump;
		static LassoLogin *newFromDump(LassoServer *server, char *dump);

		/* Methods inherited from LassoProfile */

	        THROW_ERROR
		void setIdentityFromDump(char *dump);
		END_THROW_ERROR

		THROW_ERROR
		void setSessionFromDump(char *dump);
		END_THROW_ERROR

		/* Methods */

		THROW_ERROR
		void acceptSso();
		END_THROW_ERROR

		THROW_ERROR
		void buildArtifactMsg(lassoHttpMethod httpMethod);
		END_THROW_ERROR

		THROW_ERROR
		int buildAssertion(char *authenticationMethod, char *authenticationInstant,
				char *reauthenticateOnOrAfter,
				char *notBefore, char *notOnOrAfter);
		END_THROW_ERROR

		THROW_ERROR
		void buildAuthnRequestMsg();
		END_THROW_ERROR

		THROW_ERROR
		void buildAuthnResponseMsg();
		END_THROW_ERROR

		THROW_ERROR
		void buildRequestMsg();
		END_THROW_ERROR

		THROW_ERROR
		void buildResponseMsg(char *remoteProviderId);
		END_THROW_ERROR

		%newobject dump;
		char *dump();

		THROW_ERROR
		void initAuthnRequest(char *remoteProviderId = NULL,
				 lassoHttpMethod httpMethod = LASSO_HTTP_METHOD_REDIRECT);
		END_THROW_ERROR

		THROW_ERROR
		void initRequest(char *responseMsg,
				 lassoHttpMethod httpMethod = LASSO_HTTP_METHOD_REDIRECT);
		END_THROW_ERROR

		THROW_ERROR
		void initIdpInitiatedAuthnRequest(char *remoteProviderID = NULL);
		END_THROW_ERROR

		gboolean mustAskForConsent();

		gboolean mustAuthenticate();

		THROW_ERROR
		void processAuthnRequestMsg(char *authnrequestMsg);
		END_THROW_ERROR

		THROW_ERROR
		void processAuthnResponseMsg(char *authnResponseMsg);
		END_THROW_ERROR

		THROW_ERROR
		void processRequestMsg(char *requestMsg);
		END_THROW_ERROR

		THROW_ERROR
		void processResponseMsg(char *responseMsg);
		END_THROW_ERROR

		THROW_ERROR
		int validateRequestMsg(gboolean authenticationResult, gboolean isConsentObtained);
		END_THROW_ERROR

	}
} LassoLogin;

%{

/* Attributes inherited from LassoProfile implementations */

/* authnRequest */
#define LassoLogin_get_authnRequest LassoLogin_authnRequest_get
LassoLibAuthnRequest *LassoLogin_authnRequest_get(LassoLogin *self) {
	LassoProfile *profile = LASSO_PROFILE(self);
	if (LASSO_IS_LIB_AUTHN_REQUEST(profile->request))
		return LASSO_LIB_AUTHN_REQUEST(profile->request);
	return NULL;
}

/* authnResponse */
#define LassoLogin_get_authnResponse LassoLogin_authnResponse_get
LassoLibAuthnResponse *LassoLogin_authnResponse_get(LassoLogin *self) {
	LassoProfile *profile = LASSO_PROFILE(self);
	if (LASSO_IS_LIB_AUTHN_RESPONSE(profile->response))
		return LASSO_LIB_AUTHN_RESPONSE(profile->response);
	return NULL;
}

/* identity */
#define LassoLogin_get_identity(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoLogin_identity_get(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoLogin_set_identity(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))
#define LassoLogin_identity_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))

/* isIdentityDirty */
#define LassoLogin_get_isIdentityDirty(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))
#define LassoLogin_isIdentityDirty_get(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))

/* isSessionDirty */
#define LassoLogin_get_isSessionDirty(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))
#define LassoLogin_isSessionDirty_get(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))

/* msgBody */
#define LassoLogin_get_msgBody(self) LASSO_PROFILE(self)->msg_body
#define LassoLogin_msgBody_get(self) LASSO_PROFILE(self)->msg_body

/* msgRelayState */
#define LassoLogin_get_msgRelayState(self) LASSO_PROFILE(self)->msg_relayState
#define LassoLogin_msgRelayState_get(self) LASSO_PROFILE(self)->msg_relayState

/* msgUrl */
#define LassoLogin_get_msgUrl(self) LASSO_PROFILE(self)->msg_url
#define LassoLogin_msgUrl_get(self) LASSO_PROFILE(self)->msg_url

/* nameIdentifier */
#define LassoLogin_get_nameIdentifier(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoLogin_nameIdentifier_get(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoLogin_set_nameIdentifier(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))
#define LassoLogin_nameIdentifier_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))

/* remoteProviderId */
#define LassoLogin_get_remoteProviderId(self) LASSO_PROFILE(self)->remote_providerID
#define LassoLogin_remoteProviderId_get(self) LASSO_PROFILE(self)->remote_providerID
#define LassoLogin_set_remoteProviderId(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))
#define LassoLogin_remoteProviderId_set(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))

/* request */
#define LassoLogin_get_request LassoLogin_request_get
LassoSamlpRequest *LassoLogin_request_get(LassoLogin *self) {
	LassoProfile *profile = LASSO_PROFILE(self);
	if (LASSO_IS_SAMLP_REQUEST(profile->request))
		return LASSO_SAMLP_REQUEST(profile->request);
	return NULL;
}

/* response */
#define LassoLogin_get_response LassoLogin_response_get
LassoSamlpResponse *LassoLogin_response_get(LassoLogin *self) {
	LassoProfile *profile = LASSO_PROFILE(self);
	if (LASSO_IS_SAMLP_RESPONSE(profile->response))
		return LASSO_SAMLP_RESPONSE(profile->response);
	return NULL;
}

/* responseStatus */
#define LassoLogin_get_responseStatus(self) NULL /* FIXME: no set */
#define LassoLogin_responseStatus_get(self) NULL /* FIXME: no set */
#define LassoLogin_set_responseStatus(self, value) lasso_profile_set_response_status(LASSO_PROFILE(self), value)
#define LassoLogin_responseStatus_set(self, value) lasso_profile_set_response_status(LASSO_PROFILE(self), value)

/* session */
#define LassoLogin_get_session(self) lasso_profile_get_session(LASSO_PROFILE(self))
#define LassoLogin_session_get(self) lasso_profile_get_session(LASSO_PROFILE(self))
#define LassoLogin_set_session(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))
#define LassoLogin_session_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))

/* Constructors, destructors & static methods implementations */

#define new_LassoLogin lasso_login_new
#define delete_LassoLogin lasso_login_destroy
#ifdef PHP_VERSION
#define LassoLogin_newFromDump lasso_login_new_from_dump
#else
#define Login_newFromDump lasso_login_new_from_dump
#endif

/* Methods inherited from LassoProfile implementations */

int LassoLogin_setIdentityFromDump(LassoLogin *self, char *dump) {
	return lasso_profile_set_identity_from_dump(LASSO_PROFILE(self), dump);
}

int LassoLogin_setSessionFromDump(LassoLogin *self, char *dump) {
	return lasso_profile_set_session_from_dump(LASSO_PROFILE(self), dump);
}

/* Methods implementations */

#define LassoLogin_acceptSso lasso_login_accept_sso
#define LassoLogin_buildAssertion lasso_login_build_assertion
#define LassoLogin_buildArtifactMsg lasso_login_build_artifact_msg
#define LassoLogin_buildAuthnRequestMsg lasso_login_build_authn_request_msg
#define LassoLogin_buildAuthnResponseMsg lasso_login_build_authn_response_msg
#define LassoLogin_buildRequestMsg lasso_login_build_request_msg
#define LassoLogin_buildResponseMsg lasso_login_build_response_msg
#define LassoLogin_dump lasso_login_dump
#define LassoLogin_initAuthnRequest lasso_login_init_authn_request
#define LassoLogin_initRequest lasso_login_init_request
#define LassoLogin_initIdpInitiatedAuthnRequest lasso_login_init_idp_initiated_authn_request
#define LassoLogin_mustAskForConsent lasso_login_must_ask_for_consent
#define LassoLogin_mustAuthenticate lasso_login_must_authenticate
#define LassoLogin_processAuthnRequestMsg lasso_login_process_authn_request_msg
#define LassoLogin_processAuthnResponseMsg lasso_login_process_authn_response_msg
#define LassoLogin_processRequestMsg lasso_login_process_request_msg
#define LassoLogin_processResponseMsg lasso_login_process_response_msg
#define LassoLogin_validateRequestMsg lasso_login_validate_request_msg

%}


/***********************************************************************
 * lasso:Logout
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(Logout) LassoLogout;
#endif
typedef struct {
	%extend {
		/* Attributes inherited from LassoProfile */

		%newobject identity_get;
		LassoIdentity *identity;

		%immutable isIdentityDirty;
		gboolean isIdentityDirty;

		%immutable isSessionDirty;
		gboolean isSessionDirty;

		%immutable msgBody;
		char *msgBody;

		%immutable msgRelayState;
		char *msgRelayState;

		%immutable msgUrl;
		char *msgUrl;

		LassoSamlNameIdentifier *nameIdentifier;

		%newobject remoteProviderId_get;
		char *remoteProviderId;

		%immutable request;
		LassoLibLogoutRequest *request;

		%immutable response;
		LassoLibLogoutResponse *response;

		char *responseStatus;

		%newobject session_get;
		LassoSession *session;

		/* Constructor, Destructor & Static Methods */

		LassoLogout(LassoServer *server);

		~LassoLogout();

		%newobject newFromDump;
		static LassoLogout *newFromDump(LassoServer *server, char *dump);

		/* Methods inherited from LassoProfile */

	        THROW_ERROR
		void setIdentityFromDump(char *dump);
		END_THROW_ERROR

		THROW_ERROR
		void setSessionFromDump(char *dump);
		END_THROW_ERROR

		/* Methods */

		THROW_ERROR
		void buildRequestMsg();
		END_THROW_ERROR

		THROW_ERROR
		void buildResponseMsg();
		END_THROW_ERROR

		%newobject dump;
		char *dump();

		%newobject getNextProviderId;
		char *getNextProviderId();

		THROW_ERROR
		void initRequest(char *remoteProviderId = NULL,
				 lassoHttpMethod httpMethod = LASSO_HTTP_METHOD_ANY);
		END_THROW_ERROR

		THROW_ERROR
		void processRequestMsg(char *requestMsg);
		END_THROW_ERROR

		THROW_ERROR
		void processResponseMsg(char *responseMsg);
		END_THROW_ERROR

		THROW_ERROR
		void resetProviderIdIndex();
		END_THROW_ERROR

		THROW_ERROR
		void validateRequest();
		END_THROW_ERROR
	}
} LassoLogout;

%{

/* Attributes inherited from LassoProfile implementations */

/* identity */
#define LassoLogout_get_identity(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoLogout_identity_get(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoLogout_set_identity(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))
#define LassoLogout_identity_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))

/* isIdentityDirty */
#define LassoLogout_get_isIdentityDirty(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))
#define LassoLogout_isIdentityDirty_get(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))

/* isSessionDirty */
#define LassoLogout_get_isSessionDirty(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))
#define LassoLogout_isSessionDirty_get(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))

/* msgBody */
#define LassoLogout_get_msgBody(self) LASSO_PROFILE(self)->msg_body
#define LassoLogout_msgBody_get(self) LASSO_PROFILE(self)->msg_body

/* msgRelayState */
#define LassoLogout_get_msgRelayState(self) LASSO_PROFILE(self)->msg_relayState
#define LassoLogout_msgRelayState_get(self) LASSO_PROFILE(self)->msg_relayState

/* msgUrl */
#define LassoLogout_get_msgUrl(self) LASSO_PROFILE(self)->msg_url
#define LassoLogout_msgUrl_get(self) LASSO_PROFILE(self)->msg_url

/* nameIdentifier */
#define LassoLogout_get_nameIdentifier(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoLogout_nameIdentifier_get(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoLogout_set_nameIdentifier(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))
#define LassoLogout_nameIdentifier_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))

/* remoteProviderId */
#define LassoLogout_get_remoteProviderId(self) LASSO_PROFILE(self)->remote_providerID
#define LassoLogout_remoteProviderId_get(self) LASSO_PROFILE(self)->remote_providerID
#define LassoLogout_set_remoteProviderId(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))
#define LassoLogout_remoteProviderId_set(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))

/* request */
#define LassoLogout_get_request(self) LASSO_LIB_LOGOUT_REQUEST(LASSO_PROFILE(self)->request)
#define LassoLogout_request_get(self) LASSO_LIB_LOGOUT_REQUEST(LASSO_PROFILE(self)->request)

/* response */
#define LassoLogout_get_response(self) LASSO_LIB_LOGOUT_RESPONSE(LASSO_PROFILE(self)->response)
#define LassoLogout_response_get(self) LASSO_LIB_LOGOUT_RESPONSE(LASSO_PROFILE(self)->response)

/* responseStatus */
#define LassoLogout_get_responseStatus(self) NULL /* FIXME: no set */
#define LassoLogout_responseStatus_get(self) NULL /* FIXME: no set */
#define LassoLogout_set_responseStatus(self, value) lasso_profile_set_response_status(LASSO_PROFILE(self), value)
#define LassoLogout_responseStatus_set(self, value) lasso_profile_set_response_status(LASSO_PROFILE(self), value)

/* session */
#define LassoLogout_get_session(self) lasso_profile_get_session(LASSO_PROFILE(self))
#define LassoLogout_session_get(self) lasso_profile_get_session(LASSO_PROFILE(self))
#define LassoLogout_set_session(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))
#define LassoLogout_session_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))

/* Constructors, destructors & static methods implementations */

#define new_LassoLogout lasso_logout_new
#define delete_LassoLogout lasso_logout_destroy
#ifdef PHP_VERSION
#define LassoLogout_newFromDump lasso_logout_new_from_dump
#else
#define Logout_newFromDump lasso_logout_new_from_dump
#endif

/* Methods inherited from LassoProfile implementations */

int LassoLogout_setIdentityFromDump(LassoLogout *self, char *dump) {
	return lasso_profile_set_identity_from_dump(LASSO_PROFILE(self), dump);
}

int LassoLogout_setSessionFromDump(LassoLogout *self, char *dump) {
	return lasso_profile_set_session_from_dump(LASSO_PROFILE(self), dump);
}

/* Methods implementations */

#define LassoLogout_buildRequestMsg lasso_logout_build_request_msg
#define LassoLogout_buildResponseMsg lasso_logout_build_response_msg
#define LassoLogout_dump lasso_logout_dump
#define LassoLogout_getNextProviderId lasso_logout_get_next_providerID
#define LassoLogout_initRequest lasso_logout_init_request
#define LassoLogout_processRequestMsg lasso_logout_process_request_msg
#define LassoLogout_processResponseMsg lasso_logout_process_response_msg
#define LassoLogout_resetProviderIdIndex lasso_logout_reset_providerID_index
#define LassoLogout_validateRequest lasso_logout_validate_request

%}


/***********************************************************************
 * lasso:LECP
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(Lecp) LassoLecp;
#endif
typedef struct {
	%extend {
		/* Attributes inherited from LassoProfile */

		%immutable authnRequest;
		LassoLibAuthnRequest *authnRequest;

		%immutable authnResponse;
		LassoLibAuthnResponse *authnResponse;

		%newobject identity_get;
		LassoIdentity *identity;

		%immutable isIdentityDirty;
		gboolean isIdentityDirty;

		%immutable isSessionDirty;
		gboolean isSessionDirty;

		%immutable msgBody;
		char *msgBody;

		%immutable msgRelayState;
		char *msgRelayState;

		%immutable msgUrl;
		char *msgUrl;

		LassoSamlNameIdentifier *nameIdentifier;

		%newobject remoteProviderId_get;
		char *remoteProviderId;

		%immutable request;
		LassoSamlpRequest *request;

		%immutable response;
		LassoSamlpResponse *response;

		char *responseStatus;

		%newobject session_get;
		LassoSession *session;

		/* Constructor, Destructor & Static Methods */

		LassoLecp(LassoServer *server);

		~LassoLecp();

		/* Methods inherited from LassoProfile */

	        THROW_ERROR
		void setIdentityFromDump(char *dump);
		END_THROW_ERROR

		THROW_ERROR
		void setSessionFromDump(char *dump);
		END_THROW_ERROR

		/* Methods inherited from LassoLogin */

		THROW_ERROR
		int buildAssertion(char *authenticationMethod, char *authenticationInstant,
				char *reauthenticateOnOrAfter,
				char *notBefore, char *notOnOrAfter);
		END_THROW_ERROR

		THROW_ERROR
		int validateRequestMsg(gboolean authenticationResult, gboolean isConsentObtained);
		END_THROW_ERROR

		/* Methods */

		THROW_ERROR
		void buildAuthnRequestEnvelopeMsg();
		END_THROW_ERROR

		THROW_ERROR
		void buildAuthnRequestMsg();
		END_THROW_ERROR

		THROW_ERROR
		void buildAuthnResponseEnvelopeMsg();
		END_THROW_ERROR

		THROW_ERROR
		void buildAuthnResponseMsg();
		END_THROW_ERROR

		THROW_ERROR
		void initAuthnRequest(char *remoteProviderId = NULL);
		END_THROW_ERROR

		THROW_ERROR
		void processAuthnRequestEnvelopeMsg(char *requestMsg);
		END_THROW_ERROR

		THROW_ERROR
		void processAuthnRequestMsg(char *authnRequestMsg);
		END_THROW_ERROR

		THROW_ERROR
		void processAuthnResponseEnvelopeMsg(char *responseMsg);
		END_THROW_ERROR

	}
} LassoLecp;

%{

/* Attributes inherited from LassoProfile implementations */

/* authnRequest */
#define LassoLecp_get_authnRequest LassoLecp_authnRequest_get
LassoLibAuthnRequest *LassoLecp_authnRequest_get(LassoLecp *self) {
	LassoProfile *profile = LASSO_PROFILE(self);
	if (LASSO_IS_LIB_AUTHN_REQUEST(profile->request))
		return LASSO_LIB_AUTHN_REQUEST(profile->request);
	return NULL;
}

/* authnResponse */
#define LassoLecp_get_authnResponse LassoLecp_authnResponse_get
LassoLibAuthnResponse *LassoLecp_authnResponse_get(LassoLecp *self) {
	LassoProfile *profile = LASSO_PROFILE(self);
	if (LASSO_IS_LIB_AUTHN_RESPONSE(profile->response))
		return LASSO_LIB_AUTHN_RESPONSE(profile->response);
	return NULL;
}

/* identity */
#define LassoLecp_get_identity(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoLecp_identity_get(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoLecp_set_identity(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))
#define LassoLecp_identity_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))

/* isIdentityDirty */
#define LassoLecp_get_isIdentityDirty(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))
#define LassoLecp_isIdentityDirty_get(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))

/* isSessionDirty */
#define LassoLecp_get_isSessionDirty(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))
#define LassoLecp_isSessionDirty_get(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))

/* msgBody */
#define LassoLecp_get_msgBody(self) LASSO_PROFILE(self)->msg_body
#define LassoLecp_msgBody_get(self) LASSO_PROFILE(self)->msg_body

/* msgRelayState */
#define LassoLecp_get_msgRelayState(self) LASSO_PROFILE(self)->msg_relayState
#define LassoLecp_msgRelayState_get(self) LASSO_PROFILE(self)->msg_relayState

/* msgUrl */
#define LassoLecp_get_msgUrl(self) LASSO_PROFILE(self)->msg_url
#define LassoLecp_msgUrl_get(self) LASSO_PROFILE(self)->msg_url

/* nameIdentifier */
#define LassoLecp_get_nameIdentifier(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoLecp_nameIdentifier_get(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoLecp_set_nameIdentifier(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))
#define LassoLecp_nameIdentifier_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))

/* remoteProviderId */
#define LassoLecp_get_remoteProviderId(self) LASSO_PROFILE(self)->remote_providerID
#define LassoLecp_remoteProviderId_get(self) LASSO_PROFILE(self)->remote_providerID
#define LassoLecp_set_remoteProviderId(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))
#define LassoLecp_remoteProviderId_set(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))

/* request */
#define LassoLecp_get_request LassoLecp_request_get
LassoSamlpRequest *LassoLecp_request_get(LassoLecp *self) {
	LassoProfile *profile = LASSO_PROFILE(self);
	if (LASSO_IS_SAMLP_REQUEST(profile->request))
		return LASSO_SAMLP_REQUEST(profile->request);
	return NULL;
}

/* response */
#define LassoLecp_get_response LassoLecp_response_get
LassoSamlpResponse *LassoLecp_response_get(LassoLecp *self) {
	LassoProfile *profile = LASSO_PROFILE(self);
	if (LASSO_IS_SAMLP_RESPONSE(profile->response))
		return LASSO_SAMLP_RESPONSE(profile->response);
	return NULL;
}

/* responseStatus */
#define LassoLecp_get_responseStatus(self) NULL /* FIXME: no set */
#define LassoLecp_responseStatus_get(self) NULL /* FIXME: no set */
#define LassoLecp_set_responseStatus(self, value) lasso_profile_set_response_status(LASSO_PROFILE(self), value)
#define LassoLecp_responseStatus_set(self, value) lasso_profile_set_response_status(LASSO_PROFILE(self), value)

/* session */
#define LassoLecp_get_session(self) lasso_profile_get_session(LASSO_PROFILE(self))
#define LassoLecp_session_get(self) lasso_profile_get_session(LASSO_PROFILE(self))
#define LassoLecp_set_session(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))
#define LassoLecp_session_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))

/* Constructors, destructors & static methods implementations */

#define new_LassoLecp lasso_lecp_new
#define delete_LassoLecp lasso_lecp_destroy

/* Methods inherited from LassoProfile implementations */

int LassoLecp_setIdentityFromDump(LassoLecp *self, char *dump) {
	return lasso_profile_set_identity_from_dump(LASSO_PROFILE(self), dump);
}

int LassoLecp_setSessionFromDump(LassoLecp *self, char *dump) {
	return lasso_profile_set_session_from_dump(LASSO_PROFILE(self), dump);
}

/* Methods inherited from LassoLogin implementations */

int LassoLecp_buildAssertion(LassoLecp *self, char *authenticationMethod,
		char *authenticationInstant, char *reauthenticateOnOrAfter, char *notBefore,
		char *notOnOrAfter) {
	return lasso_login_build_assertion(LASSO_LOGIN(self), authenticationMethod,
			authenticationInstant, reauthenticateOnOrAfter, notBefore, notOnOrAfter);
}

int LassoLecp_validateRequestMsg(LassoLecp *self, gboolean authenticationResult,
		gboolean isConsentObtained) {
	return lasso_login_validate_request_msg(LASSO_LOGIN(self), authenticationResult,
			isConsentObtained);
}

/* Methods implementations */

#define LassoLecp_buildAuthnRequestEnvelopeMsg lasso_lecp_build_authn_request_envelope_msg
#define LassoLecp_buildAuthnRequestMsg lasso_lecp_build_authn_request_msg
#define LassoLecp_buildAuthnResponseEnvelopeMsg lasso_lecp_build_authn_response_envelope_msg
#define LassoLecp_buildAuthnResponseMsg lasso_lecp_build_authn_response_msg
#define LassoLecp_initAuthnRequest lasso_lecp_init_authn_request
#define LassoLecp_processAuthnRequestEnvelopeMsg lasso_lecp_process_authn_request_envelope_msg
#define LassoLecp_processAuthnRequestMsg lasso_lecp_process_authn_request_msg
#define LassoLecp_processAuthnResponseEnvelopeMsg lasso_lecp_process_authn_response_envelope_msg

%}

/***********************************************************************
 * lasso:NameIdentifierMapping
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(NameIdentifierMapping) LassoNameIdentifierMapping;
#endif
typedef struct {
	%immutable targetNameIdentifier;
	char *targetNameIdentifier;

	%extend {
		/* Attributes inherited from LassoProfile */

		%newobject identity_get;
		LassoIdentity *identity;

		%immutable isIdentityDirty;
		gboolean isIdentityDirty;

		%immutable isSessionDirty;
		gboolean isSessionDirty;

		%immutable msgBody;
		char *msgBody;

		%immutable msgUrl;
		char *msgUrl;

		LassoSamlNameIdentifier *nameIdentifier;

		%newobject remoteProviderId_get;
		char *remoteProviderId;

		%newobject session_get;
		LassoSession *session;

		/* Constructor, Destructor & Static Methods */

		LassoNameIdentifierMapping(LassoServer *server);

		~LassoNameIdentifierMapping();

		/* Methods inherited from LassoProfile */

	        THROW_ERROR
		void setIdentityFromDump(char *dump);
		END_THROW_ERROR

		THROW_ERROR
		void setSessionFromDump(char *dump);
		END_THROW_ERROR

		/* Methods */

		THROW_ERROR
		void buildRequestMsg();
		END_THROW_ERROR

		THROW_ERROR
		void buildResponseMsg();
		END_THROW_ERROR

		%newobject dump;
		char *dump();

		THROW_ERROR
		void initRequest(char *targetNamespace, char *remoteProviderId = NULL);
		END_THROW_ERROR

		THROW_ERROR
		void processRequestMsg(char *requestMsg);
		END_THROW_ERROR

		THROW_ERROR
		void processResponseMsg(char *responseMsg);
		END_THROW_ERROR

		THROW_ERROR
		void validateRequest();
		END_THROW_ERROR
	}
} LassoNameIdentifierMapping;

%{

/* Attributes inherited from LassoProfile implementations */

/* identity */
#define LassoNameIdentifierMapping_get_identity(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoNameIdentifierMapping_identity_get(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoNameIdentifierMapping_set_identity(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))
#define LassoNameIdentifierMapping_identity_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))

/* isIdentityDirty */
#define LassoNameIdentifierMapping_get_isIdentityDirty(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))
#define LassoNameIdentifierMapping_isIdentityDirty_get(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))

/* isSessionDirty */
#define LassoNameIdentifierMapping_get_isSessionDirty(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))
#define LassoNameIdentifierMapping_isSessionDirty_get(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))

/* msgBody */
#define LassoNameIdentifierMapping_get_msgBody(self) LASSO_PROFILE(self)->msg_body
#define LassoNameIdentifierMapping_msgBody_get(self) LASSO_PROFILE(self)->msg_body

/* msgRelayState */
#define LassoNameIdentifierMapping_get_msgRelayState(self) LASSO_PROFILE(self)->msg_relayState
#define LassoNameIdentifierMapping_msgRelayState_get(self) LASSO_PROFILE(self)->msg_relayState

/* msgUrl */
#define LassoNameIdentifierMapping_get_msgUrl(self) LASSO_PROFILE(self)->msg_url
#define LassoNameIdentifierMapping_msgUrl_get(self) LASSO_PROFILE(self)->msg_url

/* nameIdentifier */
#define LassoNameIdentifierMapping_get_nameIdentifier(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoNameIdentifierMapping_nameIdentifier_get(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoNameIdentifierMapping_set_nameIdentifier(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))
#define LassoNameIdentifierMapping_nameIdentifier_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))

/* remoteProviderId */
#define LassoNameIdentifierMapping_get_remoteProviderId(self) LASSO_PROFILE(self)->remote_providerID
#define LassoNameIdentifierMapping_remoteProviderId_get(self) LASSO_PROFILE(self)->remote_providerID
#define LassoNameIdentifierMapping_set_remoteProviderId(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))
#define LassoNameIdentifierMapping_remoteProviderId_set(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))

/* session */
#define LassoNameIdentifierMapping_get_session LassoNameIdentifierMapping_session_get
LassoSession *LassoNameIdentifierMapping_session_get(LassoNameIdentifierMapping *self) {
	return lasso_profile_get_session(LASSO_PROFILE(self));
}
#define LassoNameIdentifierMapping_set_session(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))
#define LassoNameIdentifierMapping_session_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))

/* Constructors, destructors & static methods implementations */

#define new_LassoNameIdentifierMapping lasso_name_identifier_mapping_new
#define delete_LassoNameIdentifierMapping lasso_name_identifier_mapping_destroy

/* Methods inherited from LassoProfile implementations */

int LassoNameIdentifierMapping_setIdentityFromDump(LassoNameIdentifierMapping *self, char *dump) {
	return lasso_profile_set_identity_from_dump(LASSO_PROFILE(self), dump);
}

int LassoNameIdentifierMapping_setSessionFromDump(LassoNameIdentifierMapping *self, char *dump) {
	return lasso_profile_set_session_from_dump(LASSO_PROFILE(self), dump);
}

/* Methods implementations */

#define LassoNameIdentifierMapping_buildRequestMsg lasso_name_identifier_mapping_build_request_msg
#define LassoNameIdentifierMapping_buildResponseMsg lasso_name_identifier_mapping_build_response_msg
#define LassoNameIdentifierMapping_dump lasso_name_identifier_mapping_dump
#define LassoNameIdentifierMapping_initRequest lasso_name_identifier_mapping_init_request
#define LassoNameIdentifierMapping_processRequestMsg lasso_name_identifier_mapping_process_request_msg
#define LassoNameIdentifierMapping_processResponseMsg lasso_name_identifier_mapping_process_response_msg
#define LassoNameIdentifierMapping_validateRequest lasso_name_identifier_mapping_validate_request

%}


/***********************************************************************
 * lasso:NameRegistration
 ***********************************************************************/


#ifndef SWIGPHP4
%rename(NameRegistration) LassoNameRegistration;
#endif
typedef struct {
	%extend {
		/* Attributes inherited from LassoProfile */

		%newobject identity_get;
		LassoIdentity *identity;

		%immutable isIdentityDirty;
		gboolean isIdentityDirty;

		%immutable isSessionDirty;
		gboolean isSessionDirty;

		%immutable msgBody;
		char *msgBody;

		%immutable msgRelayState;
		char *msgRelayState;

		%immutable msgUrl;
		char *msgUrl;

		LassoSamlNameIdentifier *nameIdentifier;

		%newobject remoteProviderId_get;
		char *remoteProviderId;

		%immutable request;
		LassoLibRegisterNameIdentifierRequest *request;

		%immutable response;
		LassoLibRegisterNameIdentifierResponse *response;

		%newobject session_get;
		LassoSession *session;

		/* Attributes */

		LassoSamlNameIdentifier *oldNameIdentifier;

		/* Constructor, Destructor & Static Methods */

		LassoNameRegistration(LassoServer *server);

		~LassoNameRegistration();

		%newobject newFromDump;
		static LassoNameRegistration *newFromDump(LassoServer *server, char *dump);

		/* Methods inherited from LassoProfile */

	        THROW_ERROR
		void setIdentityFromDump(char *dump);
		END_THROW_ERROR

		THROW_ERROR
		void setSessionFromDump(char *dump);
		END_THROW_ERROR

		/* Methods */

		THROW_ERROR
		void buildRequestMsg();
		END_THROW_ERROR

		THROW_ERROR
		void buildResponseMsg();
		END_THROW_ERROR

		%newobject dump;
		char *dump();

		THROW_ERROR
		void initRequest(char *remoteProviderId,
				lassoHttpMethod httpMethod = LASSO_HTTP_METHOD_ANY);
		END_THROW_ERROR

		THROW_ERROR
		void processRequestMsg(char *requestMsg);
		END_THROW_ERROR

		THROW_ERROR
		void processResponseMsg(char *responseMsg);
		END_THROW_ERROR

		THROW_ERROR
		void validateRequest();
		END_THROW_ERROR
	}
} LassoNameRegistration;

%{

/* Attributes inherited from LassoProfile implementations */

/* identity */
#define LassoNameRegistration_get_identity(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoNameRegistration_identity_get(self) lasso_profile_get_identity(LASSO_PROFILE(self))
#define LassoNameRegistration_set_identity(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))
#define LassoNameRegistration_identity_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->identity, (value))

/* isIdentityDirty */
#define LassoNameRegistration_get_isIdentityDirty(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))
#define LassoNameRegistration_isIdentityDirty_get(self) lasso_profile_is_identity_dirty(LASSO_PROFILE(self))

/* isSessionDirty */
#define LassoNameRegistration_get_isSessionDirty(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))
#define LassoNameRegistration_isSessionDirty_get(self) lasso_profile_is_session_dirty(LASSO_PROFILE(self))

/* msgBody */
#define LassoNameRegistration_get_msgBody(self) LASSO_PROFILE(self)->msg_body
#define LassoNameRegistration_msgBody_get(self) LASSO_PROFILE(self)->msg_body

/* msgRelayState */
#define LassoNameRegistration_get_msgRelayState(self) LASSO_PROFILE(self)->msg_relayState
#define LassoNameRegistration_msgRelayState_get(self) LASSO_PROFILE(self)->msg_relayState

/* msgUrl */
#define LassoNameRegistration_get_msgUrl(self) LASSO_PROFILE(self)->msg_url
#define LassoNameRegistration_msgUrl_get(self) LASSO_PROFILE(self)->msg_url

/* nameIdentifier */
#define LassoNameRegistration_get_nameIdentifier(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoNameRegistration_nameIdentifier_get(self) get_object(LASSO_PROFILE(self)->nameIdentifier)
#define LassoNameRegistration_set_nameIdentifier(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))
#define LassoNameRegistration_nameIdentifier_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->nameIdentifier, (value))

/* remoteProviderId */
#define LassoNameRegistration_get_remoteProviderId(self) LASSO_PROFILE(self)->remote_providerID
#define LassoNameRegistration_remoteProviderId_get(self) LASSO_PROFILE(self)->remote_providerID
#define LassoNameRegistration_set_remoteProviderId(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))
#define LassoNameRegistration_remoteProviderId_set(self, value) set_string(&LASSO_PROFILE(self)->remote_providerID, (value))

/* request */
#define LassoNameRegistration_get_request(self) LASSO_LIB_REGISTER_NAME_IDENTIFIER_REQUEST(LASSO_PROFILE(self)->request)
#define LassoNameRegistration_request_get(self) LASSO_LIB_REGISTER_NAME_IDENTIFIER_REQUEST(LASSO_PROFILE(self)->request)

/* response */
#define LassoNameRegistration_get_response(self) LASSO_LIB_REGISTER_NAME_IDENTIFIER_RESPONSE(LASSO_PROFILE(self)->response)
#define LassoNameRegistration_response_get(self) LASSO_LIB_REGISTER_NAME_IDENTIFIER_RESPONSE(LASSO_PROFILE(self)->response)

/* session */
#define LassoNameRegistration_get_session(self) lasso_profile_get_session(LASSO_PROFILE(self))
#define LassoNameRegistration_session_get(self) lasso_profile_get_session(LASSO_PROFILE(self))
#define LassoNameRegistration_set_session(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))
#define LassoNameRegistration_session_set(self, value) set_object((gpointer *) &LASSO_PROFILE(self)->session, (value))

/* Attributes implementations */

/* oldNameIdentifier */
#define LassoNameRegistration_get_oldNameIdentifier(self) get_object((self)->oldNameIdentifier)
#define LassoNameRegistration_oldNameIdentifier_get(self) get_object((self)->oldNameIdentifier)
#define LassoNameRegistration_set_oldNameIdentifier(self, value) set_object((gpointer *) &(self)->oldNameIdentifier, (value))
#define LassoNameRegistration_oldNameIdentifier_set(self, value) set_object((gpointer *) &(self)->oldNameIdentifier, (value))

/* Constructors, destructors & static methods implementations */

#define new_LassoNameRegistration lasso_name_registration_new
#define delete_LassoNameRegistration lasso_name_registration_destroy
#ifdef PHP_VERSION
#define LassoNameRegistration_newFromDump lasso_name_registration_new_from_dump
#else
#define NameRegistration_newFromDump lasso_name_registration_new_from_dump
#endif

/* Methods inherited from LassoProfile implementations */

int LassoNameRegistration_setIdentityFromDump(LassoNameRegistration *self, char *dump) {
	return lasso_profile_set_identity_from_dump(LASSO_PROFILE(self), dump);
}

int LassoNameRegistration_setSessionFromDump(LassoNameRegistration *self, char *dump) {
	return lasso_profile_set_session_from_dump(LASSO_PROFILE(self), dump);
}

/* Methods implementations */

#define LassoNameRegistration_buildRequestMsg lasso_name_registration_build_request_msg
#define LassoNameRegistration_buildResponseMsg lasso_name_registration_build_response_msg
#define LassoNameRegistration_dump lasso_name_registration_dump
#define LassoNameRegistration_initRequest lasso_name_registration_init_request
#define LassoNameRegistration_processRequestMsg lasso_name_registration_process_request_msg
#define LassoNameRegistration_processResponseMsg lasso_name_registration_process_response_msg
#define LassoNameRegistration_validateRequest lasso_name_registration_validate_request

%}

%include Lasso-wsf.i

