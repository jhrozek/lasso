/*
 * ColdFusionLasso -- ColdFusion bindings for Lasso library
 *
 * Copyright (C) 2004 Entr'ouvert
 * http://lasso.entrouvert.org
 *
 * Authors: Emmanuel Raviart <eraviart@entrouvert.com>
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


/*
 * Simple wrapper for JLasso, to ease its use by ColdFusion applications.
 *
 * To use it:
 * $ javac -classpath ../../lasso.jar CFLasso.java
 * Edit ColdFusion file bin/jvm.config:
 *   - Add libjlasso.so directory to java.library.path variable.
 *   - Add classes directory to java.class.path variable.
 */


public class CFLasso {
    protected com.entrouvert.lasso.Server getServer() {
        com.entrouvert.lasso.Server server = new com.entrouvert.lasso.Server(
            "/home/manou/projects/lasso/lasso-devel/tests/data/sp1-la/metadata.xml'",
	    null, //"/home/manou/projects/lasso/lasso-devel/tests/data/sp1-la/public-key.pem",
            "/home/manou/projects/lasso/lasso-devel/tests/data/sp1-la/private-key-raw.pem",
            "/home/manou/projects/lasso/lasso-devel/tests/data/sp1-la/certificate.pem",
            com.entrouvert.lasso.lasso.signatureMethodRsaSha1);
        server.addProvider(
            "/home/manou/projects/lasso/lasso-devel/tests/data/idp1-la/metadata.xml",
            "/home/manou/projects/lasso/lasso-devel/tests/data/idp1-la/public-key.pem",
            "/home/manou/projects/lasso/lasso-devel/tests/data/ca1-la/certificate.pem");
	return server;
    }

    public String login(String relayState) {
	com.entrouvert.lasso.AuthnRequest authnRequest;
	com.entrouvert.lasso.Login login;
	com.entrouvert.lasso.Server server;
	String authnRequestUrl;

	// com.entrouvert.lasso.lasso.init();

	server = getServer();
        login = new com.entrouvert.lasso.Login(server);
        login.initAuthnRequest(com.entrouvert.lasso.lasso.httpMethodRedirect);
	authnRequest = login.getAuthnRequest();
        authnRequest.setIsPassive(false);
        authnRequest.setNameIdPolicy(com.entrouvert.lasso.lasso.libNameIdPolicyTypeFederated);
        authnRequest.setConsent(com.entrouvert.lasso.lasso.libConsentObtained);
	if (relayState != null)
	    authnRequest.setRelayState(relayState);
        login.buildAuthnRequestMsg("https://sp1/metadata");
        authnRequestUrl = login.getMsgUrl();

	// com.entrouvert.lasso.lasso.shutdown();

	return authnRequestUrl;
    }

    static public void main(String [] args) {
	CFLasso lasso = new CFLasso();
	String ssoUrl = lasso.login(null);
	System.out.print("Identity provider single sign-on URL = ");
	System.out.println(ssoUrl);
    }
}
