set -u -x

BASE=simple
BASE=link
BASE=link2
BASE=link3
BASE=link4
BASE=embedded-image
BASE=symbol_ko_shppict
BASE=separate-links
BASE=split-link
BASE=cvexperience
BASE=symbol
BASE=white-text
BASE=TEST_CIR2_KO
BASE=TEST_CIR2_XX
BASE=TEST_CIR4
BASE=rt
BASE=rounded-table
BASE=justified
BASE=backgrounds
BASE=colspan
BASE=pp
BASE=justified
BASE=giovanni-table
BASE=symbol_ko_includepicture
BASE=table-header-continued-row
BASE=list
BASE=split-list-item
BASE=split-list
BASE=nested-list
BASE=flatten-table
BASE=ordinals
BASE=table-header
BASE=giovanni-lists
BASE=two-level-list
BASE=two-level-list-sects-pgnrestart-2
BASE=two-level-list-sects-pgnrestart
BASE=languages
BASE=readme
BASE=embedded-plain-table
BASE=hdft
BASE=link
BASE=table-end-at-end-of-page
BASE=keepn_table_row
BASE=TOC
BASE=toc-a8
BASE=TOC-pages

SOURCE=/home/tmp/${BASE}.rtf

cp ${SOURCE} /tmp/${BASE}.rtf
./Ted --pdfMarkContent=1 --pdfMarkInvisibleAsArtifact=1 --pdfDeclareUACompliant=1 --pdfFlattenPlainTables=1 --saveTo ${SOURCE} /tmp/${BASE}.ps
#./Ted --pdfMarkContent=1 --drawParticulesSeparately=1 --pdfMarkInvisibleAsArtifact=1 --pdfFlattenPlainTables=1 --saveTo ${SOURCE} /tmp/${BASE}.pdf

/opt/gs10/bin/gs10 -q	-dNOPAUSE				\
			-sDEVICE=pdfwrite			\
			-sOutputFile="/tmp/${BASE}.pdf"		\
			/tmp/${BASE}.ps				\
			-c quit

pdftk /tmp/${BASE}.pdf output /tmp/${BASE}-uncompressed.pdf uncompress

cp /home/tmp/${BASE}.rtf /mnt/transip-stack/downloads/${BASE}.rtf
cp /tmp/${BASE}.ps /mnt/transip-stack/downloads/${BASE}.ps
cp /tmp/${BASE}.pdf /mnt/transip-stack/downloads/${BASE}.pdf

# --omitHeadersOnEmptyPages=1
