####
####	Makefile for Ted
####	================
####
####	No explicit 'configure' is needed as it is done by the 'make' process
####	for the various targets.
####
####	Targets for external use are:
####	-----------------------------
####
####	compile:	Just build an executable in the 'Ted' directory.
####	private:	Install the Ted executable and the most essential
####			files for yourself only.
####	package:	Build a package. Must sometimes be run as root.
####			Use fakeroot to avoid using root privileges.
####	sysvpkg:	Build a Unix system V (Solaris) package. On 
####			OpenSolaris this can be run through fakeroot.
####	deb:		Build a Debian package. This is NOT the official 
####			Debian distribution but just a way to produce a 
####			Debian package. (Must be built with fakeroot)
####	deb-dependencies: Install the dependencied for debian. Must be run 
####			as root.
####	pet:		Build a Puppy Linux package. Only works if you first
####			download and install the devx_<version>.sfs  file as
####			an extra punishment after the puppy installation.
####	slackpkg:	Build a Slackware Linux installer package. This 
####			also applies for Vector Linux, where the build 
####			can be called through fakeroot.
####	freebsdpkg:	Build a FreeBSD package. (I tried to do something 
####			similar for OpenBSD but I failed to understand the 
####			rather hermetic documentation and procedures.)
####	tedPackage/arch-PKGBUILD: Building blocks for an arch linux package.
####			Refer to the instructions in tedPackage/arch-PKGBUILD.in
####			for details
####	install:	Install the package. Must be run as root.
####	clean:		Cleanup rubbish from previous attempts.
####
####	compile.shared:	Compile, do not try to link statically.
####	package.shared:	Build a package, using the dynamically linked
####			executable.
####	install.shared:	Install the package. Must be run as root,
####			or another user with sufficient privileges to 
####			create the directories and files.
####			Installs the package just made, does not force
####			the package to be statically linked. This target
####			only exists for convenience.
####
####	Actively supported 'configure' options are:
####	-------------------------------------------
####
####    Please NOTE that you have to 'make clean' after you have 
####    changed the CONFIGURE_OPTIONS.
####
####		--prefix		Use another place than /usr
####					for installation. E.G.
####					--prefix=/opt/Ted or --prefix=/usr/local
####					On OSX with brew use --prefix=/usr/local
####		--with-GTK		Use the Gtk+ gui toolkit. (The
####					default)
####		--with-MOTIF		Use the Motif gui toolkit. (Preferable
####					on traditional UNIX systems like
####					Solaris, AIX, HP/UX)
####		--without-FONTCONFIG	In older systems, Fontconfig sometimes
####					is in a state that makes it better
####					not to use it. This makes it possible
####					to avoid it even if the software is 
####					found on the machine.
####		--without-XFT		In older systems, Xft sometimes
####					is in a state that makes it better
####					not to use it. This makes it possible
####					to avoid it even if the software is 
####					found on the machine.
####		--without-LIBICONV	In some cases detecting libiconv is 
####					such a nuisance that it is easier to 
####					avoid it. Without libiconv, Ted can 
####					only handle US and western European
####					documents.
####
####
####	P.S.	To port to Compaq OpenVMS, use the descrip.mms file
####		It contains an explanatory note as well.
####
####	P.S.	I admit that this makefile is more like a shell script.
####

CONFIGURE_OPTIONS=--with-GTK

compile:	Ted/Ted		\
		Ted/Ted.static
	:
	: Static executable Ted/Ted.static ready
	: *   To make an installation package
	:     you can now run 'make package'. Depending on the platform, 
	:     it might be that this must be done AS ROOT. Try to 
	:     use 'fakeroot' if possible. Otherwise fall back to 'sudo'
	: *   To install Ted for yourself only
	:     you can now run 'make private'

compile.shared:	Ted/Ted
	:
	: Dynamic executable Ted/Ted ready
	: *   To make an installation package
	:     you can now run 'make package.shared' AS ROOT
	: *   To install Ted for yourself only
	:     you can now run 'make private'

####
####	Make library directory.
####

lib:
	mkdir -p lib

####
####	Build ted libraries
####

tedlibs: 	lib/appUtil.a		\
		lib/textEncoding.a	\
		lib/utilPs.a		\
		lib/bitmap.a		\
		lib/docFont.a		\
		lib/drawMeta.a		\
		lib/docBase.a		\
		lib/docBuf.a		\
		lib/ind.a		\
		lib/docRtf.a		\
		lib/docEdit.a		\
		lib/docLayout.a		\
		lib/docHtml.a		\
		lib/guiBase.a		\
		lib/appFrame.a		\
		lib/appTools.a		\
		lib/tedResource.a	\
		lib/tedTools.a

####
AUT_DEP= Makefile
AUT_CFG= appUtil/appUtilConfig.h

lib/appUtil.a: $(AUT_CFG) lib
	cd appUtil && $(MAKE)

$(AUT_CFG): \
	appUtil/makefile.in appUtil/appUtilConfig.h.in $(AUT_DEP)
	cd appUtil && ./configure $(CONFIGURE_OPTIONS)

####
TXE_DEP= \
	$(AUT_CFG) \
	Makefile
TXE_CFG= textEncoding/textEncodingConfig.h

lib/textEncoding.a: $(TXE_CFG) lib
	cd textEncoding && $(MAKE)

$(TXE_CFG): \
	textEncoding/makefile.in textEncoding/textEncodingConfig.h.in $(TXE_DEP)
	cd textEncoding && ./configure $(CONFIGURE_OPTIONS)

####
UPS_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	Makefile
UPS_CFG= utilPs/utilPsConfig.h

lib/utilPs.a: $(UPS_CFG) lib
	cd utilPs && $(MAKE)

$(UPS_CFG): utilPs/makefile.in utilPs/utilPsConfig.h.in $(UPS_DEP)
	cd utilPs && ./configure $(CONFIGURE_OPTIONS)

####
DFT_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(UPS_CFG) \
	Makefile
DFT_CFG= docFont/docFontConfig.h

lib/docFont.a: $(DFT_CFG) lib
	cd docFont && $(MAKE)

$(DFT_CFG): docFont/makefile.in docFont/docFontConfig.h.in $(DFT_DEP)
	cd docFont && ./configure $(CONFIGURE_OPTIONS)

####
IND_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	\
	$(DFT_CFG) $(UPS_CFG) \
	Makefile
IND_CFG= ind/indConfig.h

lib/ind.a: $(IND_CFG) lib
	cd ind && $(MAKE)

$(IND_CFG): ind/makefile.in ind/indConfig.h.in $(IND_DEP)
	cd ind && ./configure $(CONFIGURE_OPTIONS)

####
BIM_DEP= \
	$(AUT_CFG) $(UPSE_CFG) \
	\
	$(TXE_CFG) \
	Makefile
BIM_CFG= bitmap/bitmapConfig.h

lib/bitmap.a: $(BIM_CFG) lib
	cd bitmap && $(MAKE)

$(BIM_CFG): bitmap/makefile.in bitmap/bitmapConfig.h.in $(BIM_DEP)
	cd bitmap && ./configure $(CONFIGURE_OPTIONS)

####
DRM_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) \
	\
	$(IND_CFG) \
	Makefile
DRM_CFG= drawMeta/drawMetaConfig.h

lib/drawMeta.a: $(DRM_CFG) lib
	cd drawMeta && $(MAKE)

$(DRM_CFG): drawMeta/makefile.in drawMeta/drawMetaConfig.h.in $(DRM_DEP)
	cd drawMeta && ./configure $(CONFIGURE_OPTIONS)

####
DBA_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) $(DRM_CFG) \
	\
	$(IND_CFG) \
	Makefile
DBA_CFG= docBase/docBaseConfig.h

lib/docBase.a: $(DBA_CFG) lib
	cd docBase && $(MAKE)

$(DBA_CFG): docBase/makefile.in docBase/docBaseConfig.h.in $(DBA_DEP)
	cd docBase && ./configure $(CONFIGURE_OPTIONS)

####
DBU_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(DBA_CFG) \
	\
	$(BIM_CFG) $(IND_CFG) \
	Makefile
DBU_CFG= docBuf/docBufConfig.h

lib/docBuf.a: $(DBU_CFG) lib
	cd docBuf && $(MAKE)

$(DBU_CFG): docBuf/makefile.in docBuf/docBufConfig.h.in $(DBU_DEP)
	cd docBuf && ./configure $(CONFIGURE_OPTIONS)

####
DRT_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) \
	$(DBA_CFG) $(DBU_CFG) \
	\
	$(IND_CFG) \
	Makefile
DRT_CFG= docRtf/docRtfConfig.h

lib/docRtf.a: $(DRT_CFG) lib
	cd docRtf && $(MAKE)

$(DRT_CFG): docRtf/makefile.in docRtf/docRtfConfig.h.in $(DRT_DEP)
	cd docRtf && ./configure $(CONFIGURE_OPTIONS)

####
DED_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) \
	$(DBA_CFG) $(DBU_CFG) \
	$(DRT_CFG) \
	\
	$(IND_CFG) \
	Makefile
DED_CFG= docEdit/docEditConfig.h

lib/docEdit.a: $(DED_CFG) lib
	cd docEdit && $(MAKE)

$(DED_CFG): docEdit/makefile.in docEdit/docEditConfig.h.in $(DED_DEP)
	cd docEdit && ./configure $(CONFIGURE_OPTIONS)

####
DLY_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) $(DRM_CFG) \
	$(DBA_CFG) $(DBU_CFG) \
	$(DED_CFG) \
	\
	$(IND_CFG) \
	Makefile
DLY_CFG= docLayout/docLayoutConfig.h

lib/docLayout.a: $(DLY_CFG) lib
	cd docLayout && $(MAKE)

$(DLY_CFG): docLayout/makefile.in docLayout/docLayoutConfig.h.in $(DLY_DEP)
	cd docLayout && ./configure $(CONFIGURE_OPTIONS)

####
DHT_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) $(DRM_CFG) \
	$(DBA_CFG) $(DBU_CFG) \
	$(DLY_CFG) $(DED_CFG) \
	\
	$(IND_CFG) \
	Makefile
DHT_CFG= docHtml/docHtmlConfig.h

lib/docHtml.a: $(DHT_CFG) lib
	cd docHtml && $(MAKE)

$(DHT_CFG): docHtml/makefile.in docHtml/docHtmlConfig.h.in $(DHT_DEP)
	cd docHtml && ./configure $(CONFIGURE_OPTIONS)

####
GUB_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) $(DRM_CFG) \
	$(IND_CFG) \
	\
	$(DBA_CFG) $(DBU_CFG) \
	$(DLY_CFG) $(DED_CFG) \
	Makefile
GUB_CFG= guiBase/guiBaseConfig.h

lib/guiBase.a: $(GUB_CFG) lib
	cd guiBase && $(MAKE)

$(GUB_CFG): guiBase/makefile.in guiBase/guiBaseConfig.h.in $(GUB_DEP)
	cd guiBase && ./configure $(CONFIGURE_OPTIONS)

####
APF_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) $(DRM_CFG) \
	$(IND_CFG) $(GUB_CFG) \
	\
	$(DBA_CFG) $(DBU_CFG) \
	$(DLY_CFG) $(DED_CFG) \
	Makefile
APF_CFG= appFrame/appFrameConfig.h

lib/appFrame.a: $(APF_CFG) lib
	cd appFrame && $(MAKE)

$(APF_CFG): appFrame/makefile.in appFrame/appFrameConfig.h.in $(APF_DEP)
	cd appFrame && ./configure $(CONFIGURE_OPTIONS)

####
APT_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) $(DRM_CFG) \
	$(IND_CFG) $(GUB_CFG) \
	$(APF_CFG) \
	$(DBA_CFG) $(DBU_CFG) \
	$(DLY_CFG) $(DED_CFG) \
	Makefile
APT_CFG= appTools/appToolsConfig.h

lib/appTools.a: $(APT_CFG) lib
	cd appTools && $(MAKE)

$(APT_CFG): appTools/makefile.in appTools/appToolsConfig.h.in $(APT_DEP)
	cd appTools && ./configure $(CONFIGURE_OPTIONS)

####
TER_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) $(DRM_CFG) \
	$(IND_CFG) $(GUB_CFG) \
	$(APF_CFG) $(APT_DEB) \
	$(DBA_CFG) $(DBU_CFG) \
	$(DLY_CFG) $(DED_CFG) \
	Makefile
TER_CFG= tedResource/tedResourceConfig.h

lib/tedResource.a: $(TER_CFG) lib
	cd tedResource && $(MAKE)

$(TER_CFG): tedResource/makefile.in tedResource/tedResourceConfig.h.in $(TER_DEP)
	cd tedResource && ./configure $(CONFIGURE_OPTIONS)

####
TET_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) $(DRM_CFG) \
	$(IND_CFG) $(GUB_CFG) \
	$(DBA_CFG) $(DBU_CFG) \
	$(DLY_CFG) $(DED_CFG) \
	$(APF_CFG) $(APT_CFG) \
	$(TER_CFG) \
	tedTools/tedToolsConfig.h
TET_CFG= tedTools/tedToolsConfig.h

lib/tedTools.a: $(TET_CFG) lib
	cd tedTools && $(MAKE)

$(TET_CFG): tedTools/makefile.in tedTools/tedToolsConfig.h.in $(TET_DEP)
	cd tedTools && ./configure $(CONFIGURE_OPTIONS)

####
####	Compile and link Ted
####
TED_DEP= \
	$(AUT_CFG) $(TXE_CFG) \
	$(DFT_CFG) $(UPS_CFG) \
	$(BIM_CFG) $(DRM_CFG) \
	$(IND_CFG) $(GUB_CFG) \
	$(DBA_CFG) $(DBU_CFG) \
	$(DLY_CFG) $(DED_CFG) \
	$(DRT_CFG) $(DHT_CFG) \
	$(APF_CFG) $(APT_CFG) \
	$(TER_CFG) \
	Makefile
TED_CFG= Ted/tedConfig.h

Ted/Ted.static: tedlibs $(TED_CFG) Ted/Ted
	cd Ted && $(MAKE) Ted.static

Ted/Ted: tedlibs $(TED_CFG)
	cd Ted && $(MAKE)

$(TED_CFG): Ted/makefile.in Ted/tedConfig.h.in $(TED_DEP)
	cd Ted && ./configure $(CONFIGURE_OPTIONS)

####
####	Make a ready to install package
####	Must be run as root (Or with fakeroot)
####

package: compile tedPackage/makefile
	cd tedPackage && $(MAKE)
	:
	: Package ready.
	: To install Ted, you can now run 'make install' AS ROOT

package.shared: compile.shared tedPackage/makefile
	cd tedPackage && $(MAKE) package.shared
	:
	: Dynamically linked package ready.
	: To install Ted, you can now run 'make install' AS ROOT

tedPackage/makefile: tedPackage/makefile.in Makefile
	cd tedPackage && ./configure $(CONFIGURE_OPTIONS)

sysvpkg: compile tedPackage/makefile
	cd tedPackage && $(MAKE) $@
	:
	: UNIX system V Package ready.
	@echo : To install Ted run '(AS ROOT)': pkgadd -d tedPackage/*.pkg

deb-dependencies:
	apt-get install `grep '^Build-Depends:' tedPackage/debian-control.in | sed -e 's/^Build-Depends://' -e 's/, */ /g'`

deb: compile tedPackage/makefile
	cd tedPackage && $(MAKE) $@
	:
	: Debian Package ready.
	@echo : To install Ted use: sudo dpkg -i tedPackage/*.deb

pet: compile tedPackage/makefile
	cd tedPackage && $(MAKE) $@
	:
	: Puppy Package ready.
	@echo : To install Ted use: "cd tedPackage && petget `basename tedPackage/*.pet`"

slackpkg: compile tedPackage/makefile
	cd tedPackage && $(MAKE) $@
	:
	: Slackware Linux Package ready.
	@echo : To install Ted use: sudo installpkg tedPackage/*-1mdd.tgz

freebsdpkg: compile tedPackage/makefile
	cd tedPackage && $(MAKE) $@
	:
	: FreeBSD Package ready.
	@echo : To install Ted run '(AS ROOT)': pkg_add tedPackage/*.freebsdpkg.tgz

tedPackage/arch-PKGBUILD: tedPackage/makefile
	cd tedPackage && $(MAKE) arch-PKGBUILD

####
####	Install Ted from the package just built
####

install: package
	( cd tedPackage && $(MAKE) install && $(MAKE) postinstall )

install.shared: package.shared
	( cd tedPackage && $(MAKE) install && $(MAKE) postinstall )

####
####	Install Ted for yourself
####

private: compile tedPackage/makefile
	( cd tedPackage && $(MAKE) private )

####
####	Cleanup
####

clean:
	rm -fr	lib
	rm -fr	*/config.cache */config.log */config.status */confdefs.h autom4te.cache
	rm -fr	*/autom4te.cache
	rm -f	*/*Config.h
	rm -f	*/makefile
	rm -f	*/*.o
	rm -f	Ted/Ted Ted/Ted.static
	rm -fr	tedPackage/scratch
	rm -f	tedPackage/*.gz
	rm -f	tedPackage/*.lsm
	rm -f	tedPackage/*.deb
	rm -f	tedPackage/*.pkg
	rm -f	tedPackage/*.xz
	rm -f	tedPackage/*.pet
	rm -f	tedPackage/README
	rm -f	tedPackage/arch-PKGBUILD
	rm -f	tedPackage/package
	rm -f	tedPackage/package.shared

