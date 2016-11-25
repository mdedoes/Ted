/************************************************************************/
/*									*/
/*  Icons and pictures for 'Ted'.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<appIcons.h>
#   include	"tedApp.h"

#   include	"teddoc.xpm"
#   include	"tedprint.xpm"
#   include	"tedmain.xpm"
#   include	"tedabout.xpm"
#   include	"tedtable.xpm"
#   include	"tedprops.xpm"

static NamedPicture	TEDPictures[]=
    {
	{ "teddoc",	teddoc_xpm },
	{ "tedmain",	tedmain_xpm },
	{ "print",	tedprint_xpm },
	{ "tedabout",	tedabout_xpm },
	{ "tedtable",	tedtable_xpm },
	{ "props",	tedprops_xpm },
    };

void tedGetNamedPictures(	EditApplication *	ea )
    {
    ea->eaNamedPictures= TEDPictures;
    ea->eaNamedPictureCount= sizeof(TEDPictures)/sizeof(NamedPicture);
    }
