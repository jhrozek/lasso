#! /usr/bin/env python
# -*- coding: UTF-8 -*-


# Python unit tests for Lasso library
# By: Frederic Peters <fpeters@entrouvert.com>
#     Emmanuel Raviart <eraviart@entrouvert.com>
#
# Copyright (C) 2004 Entr'ouvert
# http://lasso.entrouvert.org
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


import unittest
import sys

if not '..' in sys.path:
    sys.path.insert(0, '..')
if not '../.libs' in sys.path:
    sys.path.insert(0, '../.libs')

import lasso


class LoginTestCase(unittest.TestCase):
    pass


class LogoutTestCase(unittest.TestCase):
    def test01(self):
        """SP logout without session and indentity; testing init_request."""

        lassoServer = lasso.Server(
            '../../tests/data/sp1-la/metadata.xml',
            None, # '../../tests/data/sp1-la/public-key.pem' is no more used
            '../../tests/data/sp1-la/private-key-raw.pem',
            '../../tests/data/sp1-la/certificate.pem',
            lasso.signatureMethodRsaSha1)
        lassoServer.add_provider(
            '../../tests/data/idp1-la/metadata.xml',
            '../../tests/data/idp1-la/public-key.pem',
            '../../tests/data/ca1-la/certificate.pem')
        logout = lasso.Logout(lassoServer, lasso.providerTypeSp)
        try:
            logout.init_request()
        except lasso.Error, error:
            if error.code != -1:
                raise
        else:
            self.fail('logout.init_request without having set identity before should fail')

    def test02(self):
        """IDP logout without session and identity; testing logout.get_next_providerID."""

        lassoServer = lasso.Server(
            '../../tests/data/idp1-la/metadata.xml',
            None, # '../../tests/data/idp1-la/public-key.pem' is no more used
            '../../tests/data/idp1-la/private-key-raw.pem',
            '../../tests/data/idp1-la/certificate.pem',
            lasso.signatureMethodRsaSha1)
        lassoServer.add_provider(
            '../../tests/data/sp1-la/metadata.xml',
            '../../tests/data/sp1-la/public-key.pem',
            '../../tests/data/ca1-la/certificate.pem')
        logout = lasso.Logout(lassoServer, lasso.providerTypeIdp)
        self.failIf(logout.get_next_providerID())


suite1 = unittest.makeSuite(LoginTestCase, 'test')
suite2 = unittest.makeSuite(LogoutTestCase, 'test')

allTests = unittest.TestSuite((suite1, suite2))

if __name__ == '__main__':
    sys.exit(not unittest.TextTestRunner(verbosity = 2).run(allTests).wasSuccessful())

