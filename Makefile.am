### Root automake file for CLEED ###
# Process this file with automake to produce Makefile.in

SUBDIRS = src
EXTRA_DIST = autogen.sh
data_DATA = examplesdocdir = docdoc_DATA = doc/*.pdfphasedir = phasephase_DATA = data/phase
man_MANS = man/*
if WIN32    data_DATA += data/templates data/win32/icons else    data_DATA += shareendif

APP_NAME=cleed


# Create a deb package
pkg-deb: pkg-src
		- rm -r $(TMP_DIR)
		mkdir -p $(TMP_DIR)
		cp pkg/$(APP_NAME)-$(VERSION).tar.bz2 $(TMP_DIR)/
		tar --directory $(TMP_DIR)/ \
				-xjf $(TMP_DIR)/$(APP_NAME)-$(VERSION).tar.bz2
		cd $(TMP_DIR)/$(APP_NAME)-$(VERSION)/; \
				echo | dh_make --single --copyright gpl -e liam.m.deacon@gmail.com -f \
                        ../$(APP_NAME)-$(VERSION).tar.bz2
		cp install/deb/changelog install/deb/control \
      	install/deb/copyright $(TMP_DIR)/$(APP_NAME)-$(VERSION)/debian/
		cd $(TMP_DIR)/$(APP_NAME)-$(VERSION)/; \
		rm debian/README.Debian debian/*.ex debian/*.EX; \
		./configure; \
		dpkg-buildpackage -rfakeroot; \
		mv ../*.deb $(PWD)/pkg/
		rm -r $(TMP_DIR);


# Create an RPM package
pkg-rpm: pkg-src
	mkdir -p $(HOME)/.rpm/RPMS/i386
	mkdir -p $(HOME)/.rpm/SRPMS
	mkdir -p $(HOME)/.rpm/BUILD
	mkdir -p $(HOME)/.rpm/SOURCES
	mkdir -p $(HOME)/.rpm/tmp
	- rm -r $(HOME)/.rpm/BUILD/$(APP_NAME)-root
	- rm -r $(HOME)/.rpm/RPMS/$(APP_NAME)-*
	- rm -r $(HOME)/.rpm/SRPMS/$(APP_NAME)-*
	- rm -r $(HOME)/.rpm/SOURCES/$(APP_NAME)-*
	cp pkg/$(APP_NAME)-$(VERSION).tar.bz2 $(HOME)/.rpm/SOURCES/
	rpmbuild --quiet --nodeps --define="_topdir $(HOME)/.rpm" \
			-ba install/rpm/gssmp.spec
	mv $(HOME)/.rpm/RPMS/i386/$(APP_NAME)-$(VERSION)-*.i386.rpm pkg/