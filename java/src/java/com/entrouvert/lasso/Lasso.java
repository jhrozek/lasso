/*
 * JLasso -- Java bindings for Lasso library
 *
 * Copyright (C) 2004 Entr'ouvert
 * http://lasso.entrouvert.org
 *
 * Authors: Benjamin Poussin <poussin@codelutin.com>
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

package com.entrouvert.lasso;

public class Lasso { // Lasso

    static {
        System.loadLibrary("jlasso");
    }

    static final public int httpMethodGet = 1;
    static final public int httpMethodPost = 2;
    static final public int httpMethodRedirect = 3;
    static final public int httpMethodSoap = 4;

    /* Consent */
    static final public String libConsentObtained = "urn:liberty:consent:obtained";
    static final public String libConsentUnavailable = "urn:liberty:consent:unavailable";
    static final public String libConsentInapplicable = "urn:liberty:consent:inapplicable";

    /* NameIDPolicyType */
    static final public String libNameIdPolicyTypeNone = "none";
    static final public String libNameIdPolicyTypeOneTime = "onetime";
    static final public String libNameIdPolicyTypeFederated = "federated";
    static final public String libNameIdPolicyTypeAny = "any";

    static final public int signatureMethodRsaSha1 = 1;
    static final public int signatureMethodDsaSha1 = 2;

    native static public int init();
    native static public int shutdown();

} // Lasso

