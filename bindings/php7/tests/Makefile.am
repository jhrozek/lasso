MAINTAINERCLEANFILES = Makefile.in
if PHP7_ENABLED
TESTS_ENVIRONMENT=env "SRCDIR=$(srcdir)/" "PHP7=$(PHP7)"
TESTS = profile_tests.sh binding_tests.sh
endif

EXTRA_DIST = profile_tests.php binding_tests.php profile_tests.sh binding_tests.sh

