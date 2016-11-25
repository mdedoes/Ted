##
##  Preamble:
##
Summary:        Ted, an easy Rich Text Processor
Name:           ted
Version:        2.22
Release:        1
#Copyright:     GPL
License:        GPL
Group:          Applications/Editors
Source:         ftp://ftp.nluug.nl/pub/editors/ted/ted-2.22.src.tar.gz
URL:            http://www.nllgg.nl/Ted/index.html
Packager:	Mark de Does
#BuildRoot:	@RPM_BUILD_ROOT@
#BuildRoot:	%{_tmppath}/%{name}-%{version}-root
BuildRequires:	gcc, zlib-devel, pcre-devel, libX11-devel, libXpm-devel, libjpeg-devel, libtiff-devel, libpng-devel, gtk2-devel, libpaper-devel
requires:	zlib, pcre, libX11, libXpm, libjpeg, libtiff, libpng, gtk2, libpaper

Summary(cs_CZ):	Ted, editor pro snadné formátovaní textu.
Summary(nl_NL):	Ted, een makkelijke 'Rich Text' verwerker

##
##  Description
##
%description
Ted is an easy rich text processor. It can edit RTF files
in a wysiwyg manner. It supports multiple fonts and can
print to PostScript printers.  Additionally Ted is an rtf2ps,
rtf2pdf, rtf2html and an rtf2epub converter.

Ted consists of a general part: The program, something.afm files
and an American spelling checker. Additional packages with
spell checking dictionaries for different languages exist.

This package is the general part.

%description -l cs_CZ
Ted je jednoduchý textový procesor.
Umo¾òuje editovat RTF soubory WYSIWYG zpùsobem. Podporuje pou¾ití
více fontù a tisk na PostScriptové tiskárny.

Ted obsahuje v základní èásti: program, pár .afm souborù a americký
korektor pøeklepù. Existují i dal¹í balíèky obsahující slovníky
pro korekturu pøeklepù jiných jazykù.

Toto je základní èást.

%description -l nl_NL
Ted is een makkelijke 'Rich Text' verwerker. Je kan er RTF bestanden
op een visuele manier mee redigeren. Ted werkt met meerdere lettertypes
en kan op PostScript printers afdrukken. Daarbij is Ted een rtf2ps en 
een rtf2pdf conversieprogramma.

Ted bestaat uit een algemeen gedeelte: Het programma, allerlei.afm
bestanden en een Amerikaanse spellingwoordenlijst. Extra pakketten
met spellingwoordenlijsten voor andere talen zijn beschikbaar.

Dit pakket is het algemene gedeelte.

##
##  Prep section:
##
%prep
rm -rf Ted-2.22
zcat ${RPM_SOURCE_DIR}/ted-2.22.src.tar.gz | tar xvf -
##
##  Build section:
##
%build
( cd Ted-2.22 && make CONFIGURE_OPTIONS=--with-GTK )
( cd Ted-2.22 && make package CONFIGURE_OPTIONS=--with-GTK )
##
##  Install section:
##
%install
( cd Ted-2.22/tedPackage && ./installTed.sh COMMON )
##
##  Files section:
##
%files
%dir %attr(755,root,root) /usr/local/Ted
%dir %attr(755,root,root) /usr/local/Ted/afm
%dir %attr(755,root,root) /usr/local/Ted/dfa
%dir %attr(755,root,root) /usr/local/Ted/varia
%dir %attr(755,root,root) /usr/local/Ted/config
%attr(755,root,root) /usr/local/bin/Ted
%attr(644,root,root) /usr/local/Ted/afm/AvantGarde-Book.afm
%attr(644,root,root) /usr/local/Ted/afm/AvantGarde-BookOblique.afm
%attr(644,root,root) /usr/local/Ted/afm/AvantGarde-Demi.afm
%attr(644,root,root) /usr/local/Ted/afm/AvantGarde-DemiOblique.afm
%attr(644,root,root) /usr/local/Ted/afm/Bookman-Demi.afm
%attr(644,root,root) /usr/local/Ted/afm/Bookman-DemiItalic.afm
%attr(644,root,root) /usr/local/Ted/afm/Bookman-Light.afm
%attr(644,root,root) /usr/local/Ted/afm/Bookman-LightItalic.afm
%attr(644,root,root) /usr/local/Ted/afm/Courier.afm
%attr(644,root,root) /usr/local/Ted/afm/Courier-Bold.afm
%attr(644,root,root) /usr/local/Ted/afm/Courier-BoldOblique.afm
%attr(644,root,root) /usr/local/Ted/afm/Courier-Oblique.afm
%attr(644,root,root) /usr/local/Ted/afm/Helvetica.afm
%attr(644,root,root) /usr/local/Ted/afm/Helvetica-Bold.afm
%attr(644,root,root) /usr/local/Ted/afm/Helvetica-BoldOblique.afm
%attr(644,root,root) /usr/local/Ted/afm/Helvetica-Narrow.afm
%attr(644,root,root) /usr/local/Ted/afm/Helvetica-Narrow-Bold.afm
%attr(644,root,root) /usr/local/Ted/afm/Helvetica-Narrow-BoldOblique.afm
%attr(644,root,root) /usr/local/Ted/afm/Helvetica-Narrow-Oblique.afm
%attr(644,root,root) /usr/local/Ted/afm/Helvetica-Oblique.afm
%attr(644,root,root) /usr/local/Ted/afm/NewCenturySchlbk-Bold.afm
%attr(644,root,root) /usr/local/Ted/afm/NewCenturySchlbk-BoldItalic.afm
%attr(644,root,root) /usr/local/Ted/afm/NewCenturySchlbk-Italic.afm
%attr(644,root,root) /usr/local/Ted/afm/NewCenturySchlbk-Roman.afm
%attr(644,root,root) /usr/local/Ted/afm/Palatino-Bold.afm
%attr(644,root,root) /usr/local/Ted/afm/Palatino-BoldItalic.afm
%attr(644,root,root) /usr/local/Ted/afm/Palatino-Italic.afm
%attr(644,root,root) /usr/local/Ted/afm/Palatino-Roman.afm
%attr(644,root,root) /usr/local/Ted/afm/Symbol.afm
%attr(644,root,root) /usr/local/Ted/afm/Times-Bold.afm
%attr(644,root,root) /usr/local/Ted/afm/Times-BoldItalic.afm
%attr(644,root,root) /usr/local/Ted/afm/Times-Italic.afm
%attr(644,root,root) /usr/local/Ted/afm/Times-Roman.afm
%attr(644,root,root) /usr/local/Ted/afm/ZapfChancery-MediumItalic.afm
%attr(644,root,root) /usr/local/Ted/afm/ZapfDingbats.afm
%attr(644,root,root) /usr/local/Ted/dfa/en_US.dfa
%attr(644,root,root) /usr/local/Ted/TedDocument-en_US.rtf
%attr(755,root,root) /usr/local/Ted/rtf2ps.sh
%attr(755,root,root) /usr/local/Ted/rtf2pdf.sh
%attr(755,root,root) /usr/local/Ted/rtf2txt.sh
%attr(755,root,root) /usr/local/Ted/rtf2html.sh
%attr(755,root,root) /usr/local/Ted/rtf2epub.sh
%attr(755,root,root) /usr/local/Ted/gsafm.sh
%attr(755,root,root) /usr/local/Ted/fixOpenOfficeRtf.sh
%attr(644,root,root) /usr/local/Ted/Ted.1
%attr(644,root,root) /usr/local/Ted/rtf2ps.1
%attr(644,root,root) /usr/local/Ted/rtf2pdf.1
%attr(644,root,root) /usr/local/Ted/gsfontmetrics.ps
%attr(644,root,root) /usr/local/Ted/gslibpath.ps
%attr(644,root,root) /usr/local/Ted/gslsfonts.ps
%attr(644,root,root) /usr/local/Ted/font.rtf
%attr(644,root,root) /usr/local/Ted/font-i.rtf
%attr(644,root,root) /usr/local/Ted/font-b.rtf
%attr(644,root,root) /usr/local/Ted/font-bi.rtf
%attr(644,root,root) /usr/local/Ted/Ted.png
%attr(644,root,root) /usr/local/Ted/varia/Ted.mailcap
%attr(644,root,root) /usr/local/Ted/varia/Ted.desktop
%attr(644,root,root) /usr/local/Ted/varia/base35.fonts.conf
%attr(644,root,root) /usr/local/Ted/varia/base35.rtf
%attr(644,root,root) /usr/local/Ted/varia/checkfontsdocs.sh
%attr(644,root,root) /usr/local/Ted/varia/Ted-2.22.lsm
%attr(644,root,root) /usr/local/Ted/config/ted.gtkrc-2.0
%attr(644,root,root) /usr/local/Ted/config/Ted.ad.sample
%attr(644,root,root) /usr/local/share/applications/Ted.desktop
##
##  Cleanup section:
##
%clean
( cd Ted-2.22 && make clean )
