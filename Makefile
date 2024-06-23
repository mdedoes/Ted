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
		lib/utilGeo.a		\
		lib/textEncoding.a	\
		lib/utilPs.a		\
		lib/bitmap.a		\
		lib/docFont.a		\
		lib/drawMeta.a		\
		lib/docBase.a		\
		lib/docField.a		\
		lib/docBuf.a		\
		lib/ind.a		\
		lib/docRtf.a		\
		lib/docEdit.a		\
		lib/docLayout.a		\
		lib/docDraw.a		\
		lib/docHtml.a		\
		lib/guiBase.a		\
		lib/appFrame.a		\
		lib/appTools.a		\
		lib/tedResource.a	\
		lib/tedTools.a

####    Generated piece that enforces sequential 'configure' 
####    invocations
#### appUtil
CFG_appUtil=appUtil/appUtilConfig.h
DEP_appUtil= $(CFG_appUtil)

$(CFG_appUtil): appUtil/makefile.in $(DEP_appUtil) Makefile
	 cd appUtil && ./configure $(CONFIGURE_OPTIONS)

lib/appUtil.a: $(CFG_appUtil) lib
	 cd appUtil && $(MAKE)

#### utilGeo
CFG_utilGeo=utilGeo/utilGeoConfig.h
DEP_utilGeo= $(DEP_appUtil) $(CFG_utilGeo)

$(CFG_utilGeo): utilGeo/makefile.in $(DEP_utilGeo) Makefile
	 cd utilGeo && ./configure $(CONFIGURE_OPTIONS)

lib/utilGeo.a: $(CFG_utilGeo) lib
	 cd utilGeo && $(MAKE)

#### textEncoding
CFG_textEncoding=textEncoding/textEncodingConfig.h
DEP_textEncoding= $(DEP_appUtil) $(DEP_utilGeo) $(CFG_textEncoding)

$(CFG_textEncoding): textEncoding/makefile.in $(DEP_textEncoding) Makefile
	 cd textEncoding && ./configure $(CONFIGURE_OPTIONS)

lib/textEncoding.a: $(CFG_textEncoding) lib
	 cd textEncoding && $(MAKE)

#### utilPs
CFG_utilPs=utilPs/utilPsConfig.h
DEP_utilPs= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(CFG_utilPs)

$(CFG_utilPs): utilPs/makefile.in $(DEP_utilPs) Makefile
	 cd utilPs && ./configure $(CONFIGURE_OPTIONS)

lib/utilPs.a: $(CFG_utilPs) lib
	 cd utilPs && $(MAKE)

#### bitmap
CFG_bitmap=bitmap/bitmapConfig.h
DEP_bitmap= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(CFG_bitmap)

$(CFG_bitmap): bitmap/makefile.in $(DEP_bitmap) Makefile
	 cd bitmap && ./configure $(CONFIGURE_OPTIONS)

lib/bitmap.a: $(CFG_bitmap) lib
	 cd bitmap && $(MAKE)

#### docFont
CFG_docFont=docFont/docFontConfig.h
DEP_docFont= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(CFG_docFont)

$(CFG_docFont): docFont/makefile.in $(DEP_docFont) Makefile
	 cd docFont && ./configure $(CONFIGURE_OPTIONS)

lib/docFont.a: $(CFG_docFont) lib
	 cd docFont && $(MAKE)

#### drawMeta
CFG_drawMeta=drawMeta/drawMetaConfig.h
DEP_drawMeta= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(CFG_drawMeta)

$(CFG_drawMeta): drawMeta/makefile.in $(DEP_drawMeta) Makefile
	 cd drawMeta && ./configure $(CONFIGURE_OPTIONS)

lib/drawMeta.a: $(CFG_drawMeta) lib
	 cd drawMeta && $(MAKE)

#### docBase
CFG_docBase=docBase/docBaseConfig.h
DEP_docBase= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(CFG_docBase)

$(CFG_docBase): docBase/makefile.in $(DEP_docBase) Makefile
	 cd docBase && ./configure $(CONFIGURE_OPTIONS)

lib/docBase.a: $(CFG_docBase) lib
	 cd docBase && $(MAKE)

#### docField
CFG_docField=docField/docFieldConfig.h
DEP_docField= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(CFG_docField)

$(CFG_docField): docField/makefile.in $(DEP_docField) Makefile
	 cd docField && ./configure $(CONFIGURE_OPTIONS)

lib/docField.a: $(CFG_docField) lib
	 cd docField && $(MAKE)

#### docBuf
CFG_docBuf=docBuf/docBufConfig.h
DEP_docBuf= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(CFG_docBuf)

$(CFG_docBuf): docBuf/makefile.in $(DEP_docBuf) Makefile
	 cd docBuf && ./configure $(CONFIGURE_OPTIONS)

lib/docBuf.a: $(CFG_docBuf) lib
	 cd docBuf && $(MAKE)

#### docRtf
CFG_docRtf=docRtf/docRtfConfig.h
DEP_docRtf= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(CFG_docRtf)

$(CFG_docRtf): docRtf/makefile.in $(DEP_docRtf) Makefile
	 cd docRtf && ./configure $(CONFIGURE_OPTIONS)

lib/docRtf.a: $(CFG_docRtf) lib
	 cd docRtf && $(MAKE)

#### ind
CFG_ind=ind/indConfig.h
DEP_ind= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(DEP_docRtf) $(CFG_ind)

$(CFG_ind): ind/makefile.in $(DEP_ind) Makefile
	 cd ind && ./configure $(CONFIGURE_OPTIONS)

lib/ind.a: $(CFG_ind) lib
	 cd ind && $(MAKE)

#### docEdit
CFG_docEdit=docEdit/docEditConfig.h
DEP_docEdit= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(DEP_docRtf) $(DEP_ind) $(CFG_docEdit)

$(CFG_docEdit): docEdit/makefile.in $(DEP_docEdit) Makefile
	 cd docEdit && ./configure $(CONFIGURE_OPTIONS)

lib/docEdit.a: $(CFG_docEdit) lib
	 cd docEdit && $(MAKE)

#### guiBase
CFG_guiBase=guiBase/guiBaseConfig.h
DEP_guiBase= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(DEP_docRtf) $(DEP_ind) $(DEP_docEdit) $(CFG_guiBase)

$(CFG_guiBase): guiBase/makefile.in $(DEP_guiBase) Makefile
	 cd guiBase && ./configure $(CONFIGURE_OPTIONS)

lib/guiBase.a: $(CFG_guiBase) lib
	 cd guiBase && $(MAKE)

#### docLayout
CFG_docLayout=docLayout/docLayoutConfig.h
DEP_docLayout= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(DEP_docRtf) $(DEP_ind) $(DEP_docEdit) $(DEP_guiBase) $(CFG_docLayout)

$(CFG_docLayout): docLayout/makefile.in $(DEP_docLayout) Makefile
	 cd docLayout && ./configure $(CONFIGURE_OPTIONS)

lib/docLayout.a: $(CFG_docLayout) lib
	 cd docLayout && $(MAKE)

#### docDraw
CFG_docDraw=docDraw/docDrawConfig.h
DEP_docDraw= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(DEP_docRtf) $(DEP_ind) $(DEP_docEdit) $(DEP_docLayout) $(DEP_guiBase) $(CFG_docDraw)

$(CFG_docDraw): docDraw/makefile.in $(DEP_docDraw) Makefile
	 cd docDraw && ./configure $(CONFIGURE_OPTIONS)

lib/docDraw.a: $(CFG_docDraw) lib
	 cd docDraw && $(MAKE)

#### appFrame
CFG_appFrame=appFrame/appFrameConfig.h
DEP_appFrame= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(DEP_docRtf) $(DEP_ind) $(DEP_docEdit) $(DEP_guiBase) $(DEP_docLayout) $(DEP_docDraw) $(CFG_appFrame)

$(CFG_appFrame): appFrame/makefile.in $(DEP_appFrame) Makefile
	 cd appFrame && ./configure $(CONFIGURE_OPTIONS)

lib/appFrame.a: $(CFG_appFrame) lib
	 cd appFrame && $(MAKE)

#### docHtml
CFG_docHtml=docHtml/docHtmlConfig.h
DEP_docHtml= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(DEP_docRtf) $(DEP_ind) $(DEP_docEdit) $(DEP_guiBase) $(DEP_docLayout) $(DEP_docDraw) $(DEP_appFrame) $(CFG_docHtml)

$(CFG_docHtml): docHtml/makefile.in $(DEP_docHtml) Makefile
	 cd docHtml && ./configure $(CONFIGURE_OPTIONS)

lib/docHtml.a: $(CFG_docHtml) lib
	 cd docHtml && $(MAKE)

#### appTools
CFG_appTools=appTools/appToolsConfig.h
DEP_appTools= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(DEP_docRtf) $(DEP_ind) $(DEP_docEdit) $(DEP_guiBase) $(DEP_docLayout) $(DEP_docDraw) $(DEP_appFrame) $(DEP_docHtml) $(CFG_appTools)

$(CFG_appTools): appTools/makefile.in $(DEP_appTools) Makefile
	 cd appTools && ./configure $(CONFIGURE_OPTIONS)

lib/appTools.a: $(CFG_appTools) lib
	 cd appTools && $(MAKE)

#### tedTools
CFG_tedTools=tedTools/tedToolsConfig.h
DEP_tedTools= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(DEP_docRtf) $(DEP_ind) $(DEP_docEdit) $(DEP_guiBase) $(DEP_docLayout) $(DEP_docDraw) $(DEP_appFrame) $(DEP_docHtml) $(DEP_appTools) $(CFG_tedTools)

$(CFG_tedTools): tedTools/makefile.in $(DEP_tedTools) Makefile
	 cd tedTools && ./configure $(CONFIGURE_OPTIONS)

lib/tedTools.a: $(CFG_tedTools) lib
	 cd tedTools && $(MAKE)

#### tedResource
CFG_tedResource=tedResource/tedResourceConfig.h
DEP_tedResource= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(DEP_docRtf) $(DEP_ind) $(DEP_docEdit) $(DEP_guiBase) $(DEP_docLayout) $(DEP_docDraw) $(DEP_appFrame) $(DEP_docHtml) $(DEP_appTools) $(DEP_tedTools) $(CFG_tedResource)

$(CFG_tedResource): tedResource/makefile.in $(DEP_tedResource) Makefile
	 cd tedResource && ./configure $(CONFIGURE_OPTIONS)

lib/tedResource.a: $(CFG_tedResource) lib
	 cd tedResource && $(MAKE)

#### Ted
DEP_Ted= $(DEP_appUtil) $(DEP_utilGeo) $(DEP_textEncoding) $(DEP_utilPs) $(DEP_bitmap) $(DEP_docFont) $(DEP_drawMeta) $(DEP_docBase) $(DEP_docField) $(DEP_docBuf) $(DEP_docRtf) $(DEP_ind) $(DEP_docEdit) $(DEP_guiBase) $(DEP_docLayout) $(DEP_docDraw) $(DEP_appFrame) $(DEP_docHtml) $(DEP_appTools) $(DEP_tedTools) $(DEP_tedResource)
CFG_Ted=Ted/tedConfig.h

$(CFG_Ted): Ted/makefile.in Ted/tedConfig.h.in $(DEP_Ted) Makefile
	 cd Ted && ./configure $(CONFIGURE_OPTIONS)

Ted/Ted: tedlibs $(CFG_Ted)
	 cd Ted && $(MAKE)

Ted/Ted.static: tedlibs $(CFG_Ted) Ted/Ted
	 cd Ted && $(MAKE) Ted.static

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

