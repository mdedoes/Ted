# include "psPrint.h"

/**
 * The XMP metadata that tells that a document is a PDF-UA document.
 * The GhostScript Ext_Metadata pdfmark takes an rdf:Description and assumes
 * that the RDF namespace shorthand is rdf.
 * From https://taggedpdf.com/xmp/pdfUA-ID.xmp
 * See https://taggedpdf.com/508-pdf-help-center/pdfua-identifier-missing/
 */
const char PS_PDF_UA_XMP[]=

/*"<?xpacket begin=\"\" id=\"W5M0MpCehiHzreSzNTczkc9d\"?>"*/
/*"<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP Core 5.2-c001 63.139439, 2010/09/27-13:37:26\">"*/
/*"<rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">"*/
" <rdf:Description rdf:about=\"\""
"  xmlns:pdfaExtension=\"http://www.aiim.org/pdfa/ns/extension/\""
"  xmlns:pdfaSchema=\"http://www.aiim.org/pdfa/ns/schema#\""
"  xmlns:pdfaProperty=\"http://www.aiim.org/pdfa/ns/property#\""
"  xmlns:pdfuaid=\"http://www.aiim.org/pdfua/ns/id/\">"
"   <pdfaExtension:schemas>"
"    <rdf:Bag>"
"     <rdf:li rdf:parseType=\"Resource\">"
"      <pdfaSchema:schema>PDF/UA Universal Accessibility Schema</pdfaSchema:schema>"
"      <pdfaSchema:namespaceURI>http://www.aiim.org/pdfua/ns/id/</pdfaSchema:namespaceURI>"
"      <pdfaSchema:prefix>pdfuaid</pdfaSchema:prefix>"
"       <pdfaSchema:property>"
"        <rdf:Seq>"
"         <rdf:li rdf:parseType=\"Resource\">"
"          <pdfaProperty:name>part</pdfaProperty:name>"
"          <pdfaProperty:valueType>Integer</pdfaProperty:valueType>"
"          <pdfaProperty:category>internal</pdfaProperty:category>"
"          <pdfaProperty:description>Indicates, which part of ISO 14289 standard is followed</pdfaProperty:description>"
"         </rdf:li>"
"        </rdf:Seq>"
"       </pdfaSchema:property>"
"      </rdf:li>"
"     </rdf:Bag>"
"    </pdfaExtension:schemas>"
"   <pdfuaid:part>1</pdfuaid:part>"
" </rdf:Description>"
/*"</rdf:RDF>"*/
/*"</x:xmpmeta>"*/
/*"<?xpacket end=\"w\"?>"*/
;
