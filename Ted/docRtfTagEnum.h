/************************************************************************/
/*									*/
/*  Identifies RTF control words					*/
/*									*/
/************************************************************************/

typedef enum RtfTag
    {
    RTFidRTF,
    RTFidRULER,

    RTFidUPR,
				/****************************************/
				/*  Document group.			*/
				/****************************************/
    RTFidFONTTBL,
    RTFidCOLORTBL,
    RTFidPNTEXT,
    RTFidPN,
    RTFidSTYLESHEET,
    RTFidINFO,
				/****************************************/
				/*  Groups inside the document.		*/
				/****************************************/
    RTFidF,
    RTFidREVTBL,
				/****************************************/
				/*  Font number.			*/
				/****************************************/
    RTFidUC,
				/****************************************/
				/*  Character sets.			*/
				/****************************************/
    RTFidRED,
    RTFidGREEN,
    RTFidBLUE,
				/****************************************/
				/*  Color references.			*/
				/****************************************/
    RTFidOGUTTER,

    RTFidDEFLANGFE,
    RTFidVIEWKIND,
    RTFidVIEWSCALE,
				/****************************************/
				/*  Document properties.		*/
				/****************************************/
    RTFidSECTDEFAULTCL,
    RTFidLYTPRTMET,
				/****************************************/
				/*  Document Formatting properties.	*/
				/****************************************/
    RTFidALLPROT,
    RTFidANNOTPROT,
    RTFidBRKFRM,
    RTFidCVMME,
    RTFidDEFFORMAT,
    RTFidDEFLANG,
    RTFidDOCTEMP,

    RTFidFLDALT,
    RTFidFORMDISP,
    RTFidFORMPROT,
    RTFidFORMSHADE,
    RTFidFRACWIDTH,
    RTFidFTNALT,

    RTFidHYPHAUTO,
    RTFidHYPHCAPS,
    RTFidHYPHCONSEC,
    RTFidHYPHHOTZ,
    RTFidLANDSCAPE,
    RTFidLINESTART,
    RTFidLINKSTYLES,
    RTFidLTRDOC,
    RTFidMAKEBACKUP,

    RTFidMARGMIRROR,
    RTFidNOCOLBAL,
    RTFidNOEXTRASPRL,
    RTFidNOTABIND,
    RTFidOTBLRUL,
    RTFidPRCOLBL,
    RTFidPRINTDATA,
    RTFidPSOVER,
    RTFidPSZ,
    RTFidREVBAR,
    RTFidREVISIONS,
    RTFidREVPROP,
    RTFidREVPROT,
    RTFidRSID,
    RTFidRTLDOC,
    RTFidSPRSLNSP,
    RTFidSPRSSPBF,
    RTFidSPRSTSP,
    RTFidSUBFONTBYSIZE,
    RTFidSWPBDR,
    RTFidTRANSMF,
    RTFidIGHT,
    RTFidWRAPTRSP,
    RTFidVIEWZK,

    RTFidPGBRDRHEAD,
    RTFidPGBRDRFOOT,
    RTFidPGBRDRT,
    RTFidPGBRDRB,
    RTFidPGBRDRL,
    RTFidPGBRDRR,
    RTFidBRDRART,
    RTFidPGBRDROPT,
    RTFidPGBRDRSNAP,

    RTFidNOXLATTOYEN,
    RTFidEXPSHRTN,
    RTFidNOULTRLSPC,
    RTFidDNTBLNSBDB,
    RTFidNOSPACEFORUL,

    RTFidJCOMPRESS,
    RTFidJEXPAND,

    RTFidHORZDOC,
    RTFidDGHSPACE,
    RTFidDGVSPACE,
    RTFidDGHORIGIN,
    RTFidDGVORIGIN,
    RTFidDGHSHOW,
    RTFidDGVSHOW,
    RTFidDGMARGIN,

    RTFidNOLEAD,
    RTFidSPRSTSM,
    RTFidTRUNCEX,
    RTFidMSMCAP,
    RTFidSPLYTWNINE,
    RTFidFTNLYTWNINE,
    RTFidHTMAUTSP,
    RTFidUSELTBALN,
    RTFidALNTBLIND,
    RTFidLYTCALCTBLWD,
    RTFidLYTTBLRTGR,
    RTFidLNBRKRULE,

    RTFidBDRRLSWSIX,
    RTFidOLDAS,
				/****************************************/
				/*  Section Formatting Properties	*/
				/****************************************/
    RTFidBINFSXN,
    RTFidBINSXN,
    RTFidCOLNO,
    RTFidCOLSR,
    RTFidDS,
    RTFidENDNHERE,
    RTFidLINEBETCOL,
    RTFidLINECONT,
    RTFidLINEMOD,
    RTFidLINEPPAGE,
    RTFidLINERESTART,
    RTFidLINESTARTS,
    RTFidLINEX,
    RTFidLNDSCPSXN,
    RTFidLTRSECT,
    RTFidPGNHN,
    RTFidPGNHNSC,
    RTFidPGNHNSH,
    RTFidPGNHNSM,
    RTFidPGNHNSN,
    RTFidPGNHNSP,
    RTFidPGNX,
    RTFidPGNY,
    RTFidRTLSECT,
    RTFidSECTUNLOCKED,
    RTFidVERTALB,
    RTFidVERTALC,
    RTFidVERTALJ,
    RTFidVERTALT,

    RTFidSECTLINEGRID,
				/****************************************/
				/*  Paragraph Formatting Properties	*/
				/****************************************/
    RTFidINTBL,
    RTFidLEVEL,
    RTFidLTRPAR,
    RTFidNOLINE,
    RTFidRTLPAR,
    RTFidS,
    RTFidSBYS,
    RTFidSUBDOCUMENT,

    RTFidASPALPHA,
    RTFidASPNUM,

    RTFidSBAUTO,
    RTFidSAAUTO,

    RTFidNOOVERFLOW,

				/****************************************/
				/*  Revision marks for paragraph	*/
				/*  numbers.				*/
				/****************************************/
    RTFidPNRAUTH,
    RTFidPNR_REVPROP,
    RTFidPNRNFC,
    RTFidPNRPNBR,
    RTFidPNRRGB,
    RTFidPNRSTART,
    RTFidPNRSTOP,
    RTFidPNRXST,
				/****************************************/
				/*  Multilevel Lists.			*/
				/****************************************/
    RTFidPNSECLVL,

				/****************************************/
				/*  Tabs.				*/
				/****************************************/
    RTFidTB,
    RTFidTX,

    RTFidADJUSTRIGHT, RTFidADJUSTLEFT,

    RTFidFC,
				/****************************************/
				/*  Character Formatting Properties	*/
				/****************************************/
    RTFidCB,
    RTFidCCHS,
    RTFidCS,
    RTFidDELETED,
    RTFidDN,
    RTFidEXPND,
    RTFidEXPNDTW,
    /* RTFidF,	*/
    RTFidKERNING,
    RTFidLANG,
    RTFidNOPROOF,
    RTFidLTRCH,
    RTFidOUTL,
    RTFidREVAUTH,
    RTFidREVAUTHDEL,
    RTFidREVDTTM,
    RTFidREVDTTMDEL,
    RTFidREVISED,
    RTFidRTLCH,
    RTFidSHAD,
    RTFidULD,
    RTFidULDB,
    RTFidULNONE,
    RTFidULW,
    RTFidUP,
    RTFidV,
    RTFidWEBHIDDEN,
    RTFidHIGHLIGHT,

    RTFidCGRID,
    RTFidCHARSCALEX,

    RTFidCHBGHORIZ,
    RTFidCHBGVERT,
    RTFidCHBGFDIAG,
    RTFidCHBGBDIAG,
    RTFidCHBGCROSS,
    RTFidCHBGDCROSS,
    RTFidCHBGDKHORIZ,
    RTFidCHBGDKVERT,
    RTFidCHBGDKFDIAG,
    RTFidCHBGDKBDIAG,
    RTFidCHBGDKCROSS,
    RTFidCHBGDKDCROSS,

    RTFidCHBRDR,

    RTFidCHCBPAT,
    RTFidCHCFPAT,
    RTFidCHSHDNG,

    RTFid_FONTALIGN,
				/****************************************/
				/*  Pictures				*/
				/****************************************/
    RTFidPICT,
    RTFidNONSHPPICT,
    RTFidSHPPICT,

    RTFidBIN,

				/****************************************/
				/*  Pictures, objects			*/
				/****************************************/
    RTFidOBJECT,
    RTFidOBJCLASS,
    RTFidOBJNAME,
    RTFidOBJDATA,

    RTFidOBJSETSIZE,
    RTFidRSLTMERGE,
				/****************************************/
				/*  Special characters			*/
				/****************************************/
    RTFidBULLET,
    RTFidCHATN,
    RTFidCHDATE,
    RTFidCHDPA,
    RTFidCHDPL,
    RTFidCHFTN,
    RTFidCHFTNSEP,
    RTFidCHFTNSEPC,
    RTFidCHPGN,
    RTFidCHTIME,
    RTFidCOLUMN,
    RTFidEMDASH,
    RTFidEMSPACE,
    RTFidENDASH,
    RTFidENSPACE,
    RTFidLDBLQUOTE,
    RTFidLINE,
    RTFidLQUOTE,
    RTFidLTRMARK,
    RTFidPAGE,
    RTFidRDBLQUOTE,
    RTFidRQUOTE,
    RTFidRTLMARK,
    RTFidSECTNUM,
    RTFidSOFTCOL,
    RTFidSOFTLHEIGHT,
    RTFidSOFTLINE,
    RTFidSOFTPAGE,
    RTFidTAB,
    RTFidZWJ,
    RTFidZWNJ,

    RTFidU,
				/****************************************/
				/*  Hierarchy.				*/
				/****************************************/
    RTFidPAR,
    RTFidCELL,
    RTFidROW,
    RTFidSECT,
    RTFidNESTCELL,

    RTFidNONESTTABLES,
				/****************************************/
				/*  Table definitions			*/
				/****************************************/
    RTFidCELLX,

    RTFidLTRROW,
    RTFidRTLROW,

				/****************************************/
				/*  Positioned objects and frames	*/
				/****************************************/
    RTFidDROPCAPLI,
    RTFidDROPCAPT,
				/****************************************/
				/*  Bullets and Numbering.		*/
				/*  (No destinations)			*/
				/****************************************/
    RTFidPNACROSS,
    RTFidPNCARD,
    RTFidPNDEC,
    RTFidPNHANG,
    RTFidPNINDENT,
    RTFidPNLCLTR,
    RTFidPNLCRM,
    RTFidPNLVL,
    RTFidPNLVLBLT,
    RTFidPNLVLBODY,
    RTFidPNLVLCONT,
    RTFidPNNUMONCE,
    RTFidPNORD,
    RTFidPNORDT,
    RTFidPNPREV,
    RTFidPNQC,
    RTFidPNQL,
    RTFidPNQR,
    RTFidPNRESTART,
    RTFidPNSP,
    RTFidPNSTART,
    RTFidPNUCLTR,
    RTFidPNUCRM,
    RTFidPNULD,
    RTFidPNULDB,
    RTFidPNULNONE,
    RTFidPNULW,

    RTFidILVL,
    RTFidPNRNOT,
				/****************************************/
				/*  Fields, Bookmarks.			*/
				/****************************************/
    RTFidDATAFIELD,
    RTFidFIELD,
    RTFidFLDDIRTY,
    RTFidFLDEDIT,
    RTFidFLDINST,
    RTFidFLDLOCK,
    RTFidFLDPRIV,
    RTFidFLDRSLT,

    RTFidBKMKSTART,
    RTFidBKMKEND,
    RTFidBKMKCOLF,
    RTFidBKMKCOLL,
				/****************************************/
				/*  Index entries.			*/
				/*  Table of contents entries.		*/
				/****************************************/
    RTFidXE,
    RTFidTXE,
    RTFidRXE,
    RTFidIXE,
    RTFidBXE,
    RTFidXEF,

    RTFidTC,
    RTFidTCN,
    RTFidTCF,
    RTFidTCL,
				/****************************************/
				/*  Associated Character Properties	*/
				/****************************************/
    RTFidAB,
    RTFidACAPS,
    RTFidACF,
    RTFidADN,
    RTFidAEXPND,
    RTFidAF,
    RTFidAFS,
    RTFidAI,
    RTFidALANG,
    RTFidAOUTL,
    RTFidASCAPS,
    RTFidASHAD,
    RTFidASTRIKE,
    RTFidAUL,
    RTFidAULD,
    RTFidAULDB,
    RTFidAULNONE,
    RTFidAULW,
    RTFidAUP,

    RTFidLOCH,
    RTFidHICH,
    RTFidDBCH,
				/****************************************/
				/*  About bullet numbering.		*/
				/****************************************/
    RTFidPNTXTA,
    RTFidPNTXTB,
    RTFidLISTTEXT,
				/****************************************/
				/*  Groups inside the 'info' group.	*/
				/****************************************/
    RTFidOPERATOR,
    RTFidVERSION,
    RTFidEDMINS,
    RTFidNOFPAGES,
    RTFidNOFWORDS,
    RTFidNOFCHARS,
    RTFidNOFCHARSWS,
    RTFidVERN,
				/****************************************/
				/*  Shapes.				*/
				/****************************************/
    RTFidSHP,

    RTFidSHPLEFT,
    RTFidSHPTOP,
    RTFidSHPBOTTOM,
    RTFidSHPRIGHT,
    RTFidSHPLID,
    RTFidSHPZ,
    RTFidSHPFHDR,
    RTFidSHPBXPAGE,
    RTFidSHPBXMARGIN,
    RTFidSHPBXCOLUMN,
    RTFidSHPBXIGNORE,
    RTFidSHPBYPAGE,
    RTFidSHPBYMARGIN,
    RTFidSHPBYCOLUMN,
    RTFidSHPBYIGNORE,
    RTFidSHPBYPARA,
    RTFidSHPWR,
    RTFidSHPWRK,
    RTFidSHPFBLWTXT,
    RTFidSHPLOCKANCHOR,

    RTFidSHPTXT,
    RTFidSHPRSLT,
    RTFidSHPINST,
    RTFidSHPGRP,

    RTFidSP,
    RTFidSV,
				/****************************************/
				/*  Fields in dates and times.		*/
				/****************************************/
    RTFidHR,
    RTFidMIN,
    RTFidSEC,
    RTFidDY,
    RTFidMO,
    RTFidYR,
				/****************************************/
				/*  Unknown				*/
				/****************************************/
    RTFidITAP,
    RTFidLIN,
    RTFidRIN,
    RTFidLANGNP,
    RTFidLANGFE,
    RTFidLANGFENP,
    RTFidNOLNHTADJTBL,

    RTFid__COUNT

    } RtfTag;

