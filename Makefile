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
####	sysvpkg:	Build a Unix system V (Solaris) package. 
####	deb:		Build a Debian package. This is NOT the official 
####			Debian distribution but just a way to produce a 
####			Debian package. (Must be built as root)
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
####		--prefix		Use another place than /usr/local
####					for installation. E.G.
####					--prefix=/opt/Ted
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
####
####
####	P.S.	To port to Compaq OpenVMS, use the descrip.mms file
####		It contains an explanatory note as well.
####
####	P.S.	I admit that this makefile is more like a shell script.
####

CONFIGURE_OPTIONS=--with-GTK

compile:	tedlibs		\
		Ted/Ted		\
		Ted/Ted.static
	:
	: Static executable Ted/Ted.static ready
	: *   To make an installation package
	:     you can now run 'make package'. Depending on the platform, 
	:     it might be that this must be done AS ROOT.
	: *   To install Ted for yourself only
	:     you can now run 'make private'

compile.shared:	tedlibs		\
		Ted/Ted
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

tedlibs: 	lib			\
		lib/appUtil.a		\
		lib/textEncoding.a	\
		lib/utilPs.a		\
		lib/bitmap.a		\
		lib/docFont.a		\
		lib/docBase.a		\
		lib/docBuf.a		\
		lib/ind.a		\
		lib/drawMeta.a		\
		lib/docRtf.a		\
		lib/docEdit.a		\
		lib/docLayout.a		\
		lib/docHtml.a		\
		lib/appFrame.a

####
lib/bitmap.a: bitmap/makefile
	cd bitmap && $(MAKE)

bitmap/makefile: bitmap/makefile.in Makefile
	cd bitmap && ./configure $(CONFIGURE_OPTIONS)

####
lib/docBuf.a: docBuf/makefile
	cd docBuf && $(MAKE)

docBuf/makefile: docBuf/makefile.in Makefile
	cd docBuf && ./configure $(CONFIGURE_OPTIONS)

####
lib/docBase.a: docBase/makefile
	cd docBase && $(MAKE)

docBase/makefile: docBase/makefile.in Makefile
	cd docBase && ./configure $(CONFIGURE_OPTIONS)

####
lib/docHtml.a: docHtml/makefile
	cd docHtml && $(MAKE)

docHtml/makefile: docHtml/makefile.in Makefile
	cd docHtml && ./configure $(CONFIGURE_OPTIONS)

####
lib/docLayout.a: docLayout/makefile
	cd docLayout && $(MAKE)

docLayout/makefile: docLayout/makefile.in Makefile
	cd docLayout && ./configure $(CONFIGURE_OPTIONS)

####
lib/docRtf.a: docRtf/makefile
	cd docRtf && $(MAKE)

docRtf/makefile: docRtf/makefile.in Makefile
	cd docRtf && ./configure $(CONFIGURE_OPTIONS)

####
lib/docEdit.a: docEdit/makefile
	cd docEdit && $(MAKE)

docEdit/makefile: docEdit/makefile.in Makefile
	cd docEdit && ./configure $(CONFIGURE_OPTIONS)

####
lib/ind.a: ind/makefile
	cd ind && $(MAKE)

ind/makefile: ind/makefile.in Makefile
	cd ind && ./configure $(CONFIGURE_OPTIONS)

####
lib/appUtil.a: appUtil/makefile
	cd appUtil && $(MAKE)

appUtil/makefile: appUtil/makefile.in Makefile
	cd appUtil && ./configure $(CONFIGURE_OPTIONS)

####
lib/textEncoding.a: textEncoding/makefile
	cd textEncoding && $(MAKE)

textEncoding/makefile: textEncoding/makefile.in Makefile
	cd textEncoding && ./configure $(CONFIGURE_OPTIONS)

####
lib/utilPs.a: utilPs/makefile
	cd utilPs && $(MAKE)

utilPs/makefile: utilPs/makefile.in Makefile
	cd utilPs && ./configure $(CONFIGURE_OPTIONS)

####
lib/docFont.a: docFont/makefile
	cd docFont && $(MAKE)

docFont/makefile: docFont/makefile.in Makefile
	cd docFont && ./configure $(CONFIGURE_OPTIONS)

####
lib/appFrame.a: appFrame/makefile
	cd appFrame && $(MAKE)

appFrame/makefile: appFrame/makefile.in Makefile
	cd appFrame && ./configure $(CONFIGURE_OPTIONS)

####
lib/drawMeta.a: drawMeta/makefile
	cd drawMeta && $(MAKE)

drawMeta/makefile: drawMeta/makefile.in Makefile
	cd drawMeta && ./configure $(CONFIGURE_OPTIONS)

####
####	Compile and link Ted
####

Ted/Ted.static: tedlibs Ted/makefile
	cd Ted && $(MAKE) Ted.static

Ted/Ted: tedlibs Ted/makefile
	cd Ted && $(MAKE)

Ted/makefile: Ted/makefile.in Makefile
	cd Ted && ./configure $(CONFIGURE_OPTIONS)

####
####	Make a ready to install package
####	Must be run as root
####

package: tedPackage/makefile compile
	cd tedPackage && $(MAKE)
	:
	: Package ready.
	: To install Ted, you can now run 'make install' AS ROOT

package.shared: tedPackage/makefile compile.shared
	cd tedPackage && make package.shared
	:
	: Dynamically linked package ready.
	: To install Ted, you can now run 'make install' AS ROOT

tedPackage/makefile: tedPackage/makefile.in Makefile
	cd tedPackage && ./configure $(CONFIGURE_OPTIONS)

sysvpkg: tedPackage/makefile compile
	cd tedPackage && $(MAKE) sysvpkg
	:
	: UNIX system V Package ready.
	@echo : Run pkgadd -d tedPackage/*.pkg AS ROOT to install Ted

deb-dependencies:
	apt-get install `grep '^Build-Depends:' tedPackage/debian-control.in | sed -e 's/^Build-Depends://' -e 's/, */ /g'`

deb: deb-dependencies tedPackage/makefile compile
	cd tedPackage && $(MAKE) deb
	:
	: Debian Package ready.
	@echo : Use sudo dpkg -i tedPackage/*.deb to install Ted

####
####	Install Ted from the package just built
####

install: package
	sh ./tedPackage/installTed.sh

install.shared: package.shared
	sh ./tedPackage/installTed.sh

####
####	Install Ted for yourself
####

PRIVATE_FILES=\
	$(HOME)/Ted/afm/Helvetica.afm		\
	$(HOME)/Ted/afm/Courier.afm		\
	$(HOME)/Ted/afm/Symbol.afm		\
	$(HOME)/Ted/afm/Times-Roman.afm		\
	$(HOME)/Ted/TedDocument-en_US.rtf	\
	$(HOME)/Ted/config/Ted.ad.sample	\
	$(HOME)/Ted/dfa/en_US.dfa

$(HOME)/bin/Ted: Ted/Ted
	test -d $(HOME)/bin || mkdir $(HOME)/bin
	cp Ted/Ted.static $(HOME)/bin/Ted || cp Ted/Ted $(HOME)/bin/Ted

$(PRIVATE_FILES):
	h=`pwd` && cd && tar xvf $$h/tedPackage/TedBindist.tar

private: $(HOME)/bin/Ted $(PRIVATE_FILES)
	@echo ========= Updating $(HOME)/.Ted.properties:
	@echo Ted.documentFileName: $(HOME)/Ted/TedDocument-en_US.rtf | \
					    tee -a $(HOME)/.Ted.properties
	@echo Ted.afmDirectory: $(HOME)/Ted/afm | \
					    tee -a $(HOME)/.Ted.properties
	@echo Ted.spellToolSystemDicts: $(HOME)/Ted/dfa | \
					    tee -a $(HOME)/.Ted.properties

####
####	Cleanup
####

clean:
	rm -fr	lib
	rm -f	*/config.cache */config.log */config.status
	rm -f	*/*Config.h
	rm -f	*/makefile
	rm -f	*/*.o
	rm -f	Ted/Ted Ted/Ted.static
	rm -fr	tedPackage/scratch
	rm -f	tedPackage/*.gz
	rm -f	tedPackage/*.lsm
	rm -f	tedPackage/installTed.sh
	rm -f	tedPackage/README
	rm -f	tedPackage/package
	rm -f	tedPackage/package.shared

