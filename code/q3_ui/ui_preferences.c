/*
===========================================================================
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of Spearmint Source Code.

Spearmint Source Code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

Spearmint Source Code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Spearmint Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, Spearmint Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following
the terms and conditions of the GNU General Public License.  If not, please
request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional
terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/
//
/*
=======================================================================

GAME OPTIONS MENU

=======================================================================
*/


#include "ui_local.h"


#define ART_FRAMEL				"menu/art/frame2_l"
#define ART_FRAMER				"menu/art/frame1_r"
#define ART_BACK0				"menu/art/back_0"
#define ART_BACK1				"menu/art/back_1"

#define PREFERENCES_X_POS		360

enum {
	ID_CROSSHAIR,
	ID_CROSSHAIRHEALTH,
	ID_VIEWBOB,
	ID_VIEWKICK,
	ID_SIMPLEITEMS,
	ID_HIGHQUALITYSKY,
	ID_EJECTINGBRASS,
	ID_WALLMARKS,
	ID_DYNAMICLIGHTS,
	ID_IDENTIFYTARGET,
	ID_SYNCEVERYFRAME,
	ID_FORCEMODEL,
	ID_DRAWTEAMOVERLAY,
	ID_ALLOWDOWNLOAD,
	ID_SPLITVERTICAL,
	ID_SPLITTEXTSIZE,
	ID_THIRDSIZE,

	ID_NUM_ITEMS,

	ID_BACK
};

#define	NUM_CROSSHAIRS			10


typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;

	menulist_s			crosshair;
	menuradiobutton_s	crosshairhealth;
	menuradiobutton_s	viewbob;
	menuradiobutton_s	viewkick;
	menuradiobutton_s	simpleitems;
	menuradiobutton_s	brass;
	menuradiobutton_s	wallmarks;
	menuradiobutton_s	dynamiclights;
	menuradiobutton_s	identifytarget;
	menuradiobutton_s	highqualitysky;
	menuradiobutton_s	synceveryframe;
	menuradiobutton_s	forcemodel;
	menulist_s			drawteamoverlay;
	menuradiobutton_s	allowdownload;
	menulist_s			splitvertical;
	menulist_s			splittextsize;
	menulist_s			thirdsize;
	menubitmap_s		back;

	qhandle_t			crosshairShader[NUM_CROSSHAIRS];
} preferences_t;

static preferences_t s_preferences;

static const char *teamoverlay_names[] =
{
	"off",
	"upper right",
	"lower right",
	"lower left",
	NULL
};

static const char *splitvertical_names[] =
{
	"horizontal",
	"vertical",
	NULL
};

static const char *splittextsize_names[] =
{
	"small",
	"medium",
	"large",
	NULL
};

static const char *thirdsize_names[] =
{
	"half of screen",
	"quarter of screen",
	NULL
};

static void Preferences_SetMenuItems( void ) {
	float textScale;

	s_preferences.crosshair.curvalue		= (int)trap_Cvar_VariableValue( "cg_drawCrosshair" ) % NUM_CROSSHAIRS;
	s_preferences.crosshairhealth.curvalue	= trap_Cvar_VariableValue( "cg_crosshairHealth" ) != 0;
	s_preferences.viewbob.curvalue			= trap_Cvar_VariableValue( "cg_viewbob" ) != 0;
	s_preferences.viewkick.curvalue			= trap_Cvar_VariableValue( "cg_viewkick" ) != 0;
	s_preferences.simpleitems.curvalue		= trap_Cvar_VariableValue( "cg_simpleItems" ) != 0;
	s_preferences.brass.curvalue			= trap_Cvar_VariableValue( "cg_brassTime" ) != 0;
	s_preferences.wallmarks.curvalue		= trap_Cvar_VariableValue( "cg_marks" ) != 0;
	s_preferences.identifytarget.curvalue	= trap_Cvar_VariableValue( "cg_drawCrosshairNames" ) != 0;
	s_preferences.dynamiclights.curvalue	= trap_Cvar_VariableValue( "r_dynamiclight" ) != 0;
	s_preferences.highqualitysky.curvalue	= trap_Cvar_VariableValue ( "r_fastsky" ) == 0;
	s_preferences.synceveryframe.curvalue	= trap_Cvar_VariableValue( "r_finish" ) != 0;
	s_preferences.forcemodel.curvalue		= trap_Cvar_VariableValue( "cg_forcemodel" ) != 0;
	s_preferences.drawteamoverlay.curvalue	= Com_Clamp( 0, 3, trap_Cvar_VariableValue( "cg_drawTeamOverlay" ) );
	s_preferences.allowdownload.curvalue	= trap_Cvar_VariableValue( "cl_allowDownload" ) != 0;
	s_preferences.splitvertical.curvalue	= trap_Cvar_VariableValue( "cg_splitviewVertical" ) != 0;

	textScale = trap_Cvar_VariableValue( "cg_splitviewTextScale" );
	if ( textScale <= 1.0f ) {
		s_preferences.splittextsize.curvalue	= 0;
	} else if ( textScale <= 1.5f ) {
		s_preferences.splittextsize.curvalue	= 1;
	} else {
		s_preferences.splittextsize.curvalue	= 2;
	}

	s_preferences.thirdsize.curvalue		= trap_Cvar_VariableValue( "cg_splitviewThirdEqual" ) != 0;
}


static void Preferences_Event( void* ptr, int notification ) {
	if( notification != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_CROSSHAIR:
		trap_Cvar_SetValue( "cg_drawCrosshair", s_preferences.crosshair.curvalue );
		break;

	case ID_CROSSHAIRHEALTH:
		trap_Cvar_SetValue( "cg_crosshairHealth", s_preferences.crosshairhealth.curvalue );
		break;

	case ID_VIEWBOB:
		trap_Cvar_SetValue( "cg_viewbob", s_preferences.viewbob.curvalue );
		break;

	case ID_VIEWKICK:
		trap_Cvar_SetValue( "cg_viewkick", s_preferences.viewkick.curvalue );
		break;

	case ID_SIMPLEITEMS:
		trap_Cvar_SetValue( "cg_simpleItems", s_preferences.simpleitems.curvalue );
		break;

	case ID_HIGHQUALITYSKY:
		trap_Cvar_SetValue( "r_fastsky", !s_preferences.highqualitysky.curvalue );
		break;

	case ID_EJECTINGBRASS:
		if ( s_preferences.brass.curvalue )
			trap_Cvar_Reset( "cg_brassTime" );
		else
			trap_Cvar_SetValue( "cg_brassTime", 0 );
		break;

	case ID_WALLMARKS:
		trap_Cvar_SetValue( "cg_marks", s_preferences.wallmarks.curvalue );
		break;

	case ID_DYNAMICLIGHTS:
		trap_Cvar_SetValue( "r_dynamiclight", s_preferences.dynamiclights.curvalue );
		break;		

	case ID_IDENTIFYTARGET:
		trap_Cvar_SetValue( "cg_drawCrosshairNames", s_preferences.identifytarget.curvalue );
		break;

	case ID_SYNCEVERYFRAME:
		trap_Cvar_SetValue( "r_finish", s_preferences.synceveryframe.curvalue );
		break;

	case ID_FORCEMODEL:
		trap_Cvar_SetValue( "cg_forcemodel", s_preferences.forcemodel.curvalue );
		break;

	case ID_DRAWTEAMOVERLAY:
		trap_Cvar_SetValue( "cg_drawTeamOverlay", s_preferences.drawteamoverlay.curvalue );
		break;

	case ID_ALLOWDOWNLOAD:
		trap_Cvar_SetValue( "cl_allowDownload", s_preferences.allowdownload.curvalue );
		trap_Cvar_SetValue( "sv_allowDownload", s_preferences.allowdownload.curvalue );
		break;

	case ID_SPLITVERTICAL:
		trap_Cvar_SetValue( "cg_splitviewVertical", s_preferences.splitvertical.curvalue );
		break;

	case ID_SPLITTEXTSIZE:
		trap_Cvar_SetValue( "cg_splitviewTextScale", 1.0f + (float)s_preferences.splittextsize.curvalue/2.0f );
		break;

	case ID_THIRDSIZE:
		trap_Cvar_SetValue( "cg_splitviewThirdEqual", s_preferences.thirdsize.curvalue );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}


/*
=================
Crosshair_Draw
=================
*/
static void Crosshair_Draw( void *self ) {
	menulist_s	*s;
	float		*color;
	int			x, y;
	int			style;
	qboolean	focus;
	vec4_t		crosshairColor;

	s = (menulist_s *)self;
	x = s->generic.x;
	y =	s->generic.y;

	style = UI_SMALLFONT;
	focus = (s->generic.parent->cursor == s->generic.menuPosition);

	if ( s->generic.flags & QMF_GRAYED )
		color = text_color_disabled;
	else if ( focus )
	{
		color = text_color_highlight;
		style |= UI_PULSE;
	}
	else if ( s->generic.flags & QMF_BLINK )
	{
		color = text_color_highlight;
		style |= UI_BLINK;
	}
	else
		color = text_color_normal;

	if ( focus )
	{
		// draw cursor
		CG_FillRect( s->generic.left, s->generic.top, s->generic.right-s->generic.left+1, s->generic.bottom-s->generic.top+1, listbar_color ); 
		UI_DrawChar( x, y, GLYPH_ARROW, UI_CENTER|UI_BLINK|UI_SMALLFONT, color);
	}

	UI_DrawString( x - SMALLCHAR_WIDTH, y, s->generic.name, style|UI_RIGHT, color );
	if( !s->curvalue ) {
		return;
	}

	// draw crosshair red if crosshair health is enabled and selected
	if ( s_preferences.crosshairhealth.curvalue && s->generic.parent->cursor == s_preferences.crosshairhealth.generic.menuPosition ) {
		VectorSet( crosshairColor, 1, 0, 0 );
	} else {
		VectorSet( crosshairColor, 1, 1, 1 );
	}
	crosshairColor[3] = 1;

	CG_DrawPicColor( x + SMALLCHAR_WIDTH, y - 4, 24, 24, s_preferences.crosshairShader[s->curvalue], crosshairColor );
}


static void Preferences_MenuInit( void ) {
	int				y;

	memset( &s_preferences, 0 ,sizeof(preferences_t) );

	Preferences_Cache();

	s_preferences.menu.wrapAround = qtrue;
	s_preferences.menu.fullscreen = qtrue;

	s_preferences.banner.generic.type  = MTYPE_BTEXT;
	s_preferences.banner.generic.x	   = 320;
	s_preferences.banner.generic.y	   = 16;
	s_preferences.banner.string		   = "GAME OPTIONS";
	s_preferences.banner.color         = text_banner_color;
	s_preferences.banner.style         = UI_CENTER;

	s_preferences.framel.generic.type  = MTYPE_BITMAP;
	s_preferences.framel.generic.name  = ART_FRAMEL;
	s_preferences.framel.generic.flags = QMF_INACTIVE;
	s_preferences.framel.generic.x	   = 0;
	s_preferences.framel.generic.y	   = 78;
	s_preferences.framel.width  	   = 256;
	s_preferences.framel.height  	   = 329;

	s_preferences.framer.generic.type  = MTYPE_BITMAP;
	s_preferences.framer.generic.name  = ART_FRAMER;
	s_preferences.framer.generic.flags = QMF_INACTIVE;
	s_preferences.framer.generic.x	   = 376;
	s_preferences.framer.generic.y	   = 76;
	s_preferences.framer.width  	   = 256;
	s_preferences.framer.height  	   = 334;

	y = ( SCREEN_HEIGHT - ID_NUM_ITEMS*(BIGCHAR_HEIGHT+2) - (2+4) ) / 2;
	s_preferences.crosshair.generic.type		= MTYPE_SPINCONTROL;
	s_preferences.crosshair.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT|QMF_NODEFAULTINIT|QMF_OWNERDRAW;
	s_preferences.crosshair.generic.x			= PREFERENCES_X_POS;
	s_preferences.crosshair.generic.y			= y;
	s_preferences.crosshair.generic.name		= "Crosshair:";
	s_preferences.crosshair.generic.callback	= Preferences_Event;
	s_preferences.crosshair.generic.ownerdraw	= Crosshair_Draw;
	s_preferences.crosshair.generic.id			= ID_CROSSHAIR;
	s_preferences.crosshair.generic.top			= y - 4;
	s_preferences.crosshair.generic.bottom		= y + 20;
	s_preferences.crosshair.generic.left		= PREFERENCES_X_POS - UI_DrawStrlen( s_preferences.crosshair.generic.name, UI_SMALLFONT ) - SMALLCHAR_WIDTH;
	s_preferences.crosshair.generic.right		= PREFERENCES_X_POS + 48;
	s_preferences.crosshair.numitems			= NUM_CROSSHAIRS;

	y += BIGCHAR_HEIGHT+2+4;
	s_preferences.crosshairhealth.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.crosshairhealth.generic.name	   = "Crosshair Health:";
	s_preferences.crosshairhealth.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.crosshairhealth.generic.callback = Preferences_Event;
	s_preferences.crosshairhealth.generic.id       = ID_CROSSHAIRHEALTH;
	s_preferences.crosshairhealth.generic.x	       = PREFERENCES_X_POS;
	s_preferences.crosshairhealth.generic.y	       = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.viewbob.generic.type            = MTYPE_RADIOBUTTON;
	s_preferences.viewbob.generic.name	          = "View Bobbing:";
	s_preferences.viewbob.generic.flags	          = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.viewbob.generic.callback        = Preferences_Event;
	s_preferences.viewbob.generic.id              = ID_VIEWBOB;
	s_preferences.viewbob.generic.x	              = PREFERENCES_X_POS;
	s_preferences.viewbob.generic.y	              = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.viewkick.generic.type            = MTYPE_RADIOBUTTON;
	s_preferences.viewkick.generic.name	          = "View Damage Kick:";
	s_preferences.viewkick.generic.flags	          = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.viewkick.generic.callback        = Preferences_Event;
	s_preferences.viewkick.generic.id              = ID_VIEWKICK;
	s_preferences.viewkick.generic.x	              = PREFERENCES_X_POS;
	s_preferences.viewkick.generic.y	              = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.simpleitems.generic.type        = MTYPE_RADIOBUTTON;
	s_preferences.simpleitems.generic.name	      = "Simple Items:";
	s_preferences.simpleitems.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.simpleitems.generic.callback    = Preferences_Event;
	s_preferences.simpleitems.generic.id          = ID_SIMPLEITEMS;
	s_preferences.simpleitems.generic.x	          = PREFERENCES_X_POS;
	s_preferences.simpleitems.generic.y	          = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.wallmarks.generic.type          = MTYPE_RADIOBUTTON;
	s_preferences.wallmarks.generic.name	      = "Marks on Walls:";
	s_preferences.wallmarks.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.wallmarks.generic.callback      = Preferences_Event;
	s_preferences.wallmarks.generic.id            = ID_WALLMARKS;
	s_preferences.wallmarks.generic.x	          = PREFERENCES_X_POS;
	s_preferences.wallmarks.generic.y	          = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.brass.generic.type              = MTYPE_RADIOBUTTON;
	s_preferences.brass.generic.name	          = "Ejecting Brass:";
	s_preferences.brass.generic.flags	          = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.brass.generic.callback          = Preferences_Event;
	s_preferences.brass.generic.id                = ID_EJECTINGBRASS;
	s_preferences.brass.generic.x	              = PREFERENCES_X_POS;
	s_preferences.brass.generic.y	              = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.dynamiclights.generic.type      = MTYPE_RADIOBUTTON;
	s_preferences.dynamiclights.generic.name	  = "Dynamic Lights:";
	s_preferences.dynamiclights.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.dynamiclights.generic.callback  = Preferences_Event;
	s_preferences.dynamiclights.generic.id        = ID_DYNAMICLIGHTS;
	s_preferences.dynamiclights.generic.x	      = PREFERENCES_X_POS;
	s_preferences.dynamiclights.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.identifytarget.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.identifytarget.generic.name	  = "Identify Target:";
	s_preferences.identifytarget.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.identifytarget.generic.callback = Preferences_Event;
	s_preferences.identifytarget.generic.id       = ID_IDENTIFYTARGET;
	s_preferences.identifytarget.generic.x	      = PREFERENCES_X_POS;
	s_preferences.identifytarget.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.highqualitysky.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.highqualitysky.generic.name	  = "High Quality Sky:";
	s_preferences.highqualitysky.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.highqualitysky.generic.callback = Preferences_Event;
	s_preferences.highqualitysky.generic.id       = ID_HIGHQUALITYSKY;
	s_preferences.highqualitysky.generic.x	      = PREFERENCES_X_POS;
	s_preferences.highqualitysky.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.synceveryframe.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.synceveryframe.generic.name	  = "Sync Every Frame:";
	s_preferences.synceveryframe.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.synceveryframe.generic.callback = Preferences_Event;
	s_preferences.synceveryframe.generic.id       = ID_SYNCEVERYFRAME;
	s_preferences.synceveryframe.generic.x	      = PREFERENCES_X_POS;
	s_preferences.synceveryframe.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.forcemodel.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.forcemodel.generic.name	  = "Force Player Models:";
	s_preferences.forcemodel.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.forcemodel.generic.callback = Preferences_Event;
	s_preferences.forcemodel.generic.id       = ID_FORCEMODEL;
	s_preferences.forcemodel.generic.x	      = PREFERENCES_X_POS;
	s_preferences.forcemodel.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.drawteamoverlay.generic.type     = MTYPE_SPINCONTROL;
	s_preferences.drawteamoverlay.generic.name	   = "Draw Team Overlay:";
	s_preferences.drawteamoverlay.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.drawteamoverlay.generic.callback = Preferences_Event;
	s_preferences.drawteamoverlay.generic.id       = ID_DRAWTEAMOVERLAY;
	s_preferences.drawteamoverlay.generic.x	       = PREFERENCES_X_POS;
	s_preferences.drawteamoverlay.generic.y	       = y;
	s_preferences.drawteamoverlay.itemnames			= teamoverlay_names;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.allowdownload.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.allowdownload.generic.name	   = "Automatic Downloading:";
	s_preferences.allowdownload.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.allowdownload.generic.callback = Preferences_Event;
	s_preferences.allowdownload.generic.id       = ID_ALLOWDOWNLOAD;
	s_preferences.allowdownload.generic.x	       = PREFERENCES_X_POS;
	s_preferences.allowdownload.generic.y	       = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.splitvertical.generic.type		= MTYPE_SPINCONTROL;
	s_preferences.splitvertical.generic.name		= "Splitscreen Mode:";
	s_preferences.splitvertical.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.splitvertical.generic.callback	= Preferences_Event;
	s_preferences.splitvertical.generic.id			= ID_SPLITVERTICAL;
	s_preferences.splitvertical.generic.x			= PREFERENCES_X_POS;
	s_preferences.splitvertical.generic.y			= y;
	s_preferences.splitvertical.itemnames			= splitvertical_names;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.splittextsize.generic.type		= MTYPE_SPINCONTROL;
	s_preferences.splittextsize.generic.name		= "Splitscreen Text:";
	s_preferences.splittextsize.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.splittextsize.generic.callback	= Preferences_Event;
	s_preferences.splittextsize.generic.id			= ID_SPLITTEXTSIZE;
	s_preferences.splittextsize.generic.x			= PREFERENCES_X_POS;
	s_preferences.splittextsize.generic.y			= y;
	s_preferences.splittextsize.itemnames			= splittextsize_names;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.thirdsize.generic.type			= MTYPE_SPINCONTROL;
	s_preferences.thirdsize.generic.name			= "Third Player View:";
	s_preferences.thirdsize.generic.flags			= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.thirdsize.generic.callback		= Preferences_Event;
	s_preferences.thirdsize.generic.id				= ID_THIRDSIZE;
	s_preferences.thirdsize.generic.x				= PREFERENCES_X_POS;
	s_preferences.thirdsize.generic.y				= y;
	s_preferences.thirdsize.itemnames				= thirdsize_names;

	s_preferences.back.generic.type	    = MTYPE_BITMAP;
	s_preferences.back.generic.name     = ART_BACK0;
	s_preferences.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_preferences.back.generic.callback = Preferences_Event;
	s_preferences.back.generic.id	    = ID_BACK;
	s_preferences.back.generic.x		= 0;
	s_preferences.back.generic.y		= 480-64;
	s_preferences.back.width  		    = 128;
	s_preferences.back.height  		    = 64;
	s_preferences.back.focuspic         = ART_BACK1;

	Menu_AddItem( &s_preferences.menu, &s_preferences.banner );
	Menu_AddItem( &s_preferences.menu, &s_preferences.framel );
	Menu_AddItem( &s_preferences.menu, &s_preferences.framer );

	Menu_AddItem( &s_preferences.menu, &s_preferences.crosshair );
	Menu_AddItem( &s_preferences.menu, &s_preferences.crosshairhealth );
	Menu_AddItem( &s_preferences.menu, &s_preferences.viewbob );
	Menu_AddItem( &s_preferences.menu, &s_preferences.viewkick );
	Menu_AddItem( &s_preferences.menu, &s_preferences.simpleitems );
	Menu_AddItem( &s_preferences.menu, &s_preferences.wallmarks );
	Menu_AddItem( &s_preferences.menu, &s_preferences.brass );
	Menu_AddItem( &s_preferences.menu, &s_preferences.dynamiclights );
	Menu_AddItem( &s_preferences.menu, &s_preferences.identifytarget );
	Menu_AddItem( &s_preferences.menu, &s_preferences.highqualitysky );
	Menu_AddItem( &s_preferences.menu, &s_preferences.synceveryframe );
	Menu_AddItem( &s_preferences.menu, &s_preferences.forcemodel );
	Menu_AddItem( &s_preferences.menu, &s_preferences.drawteamoverlay );
	Menu_AddItem( &s_preferences.menu, &s_preferences.allowdownload );
	Menu_AddItem( &s_preferences.menu, &s_preferences.splitvertical );
	Menu_AddItem( &s_preferences.menu, &s_preferences.splittextsize );
	Menu_AddItem( &s_preferences.menu, &s_preferences.thirdsize );

	Menu_AddItem( &s_preferences.menu, &s_preferences.back );

	Preferences_SetMenuItems();
}


/*
===============
Preferences_Cache
===============
*/
void Preferences_Cache( void ) {
	int		n;

	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
	for( n = 0; n < NUM_CROSSHAIRS; n++ ) {
		s_preferences.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%c", 'a' + n ) );
	}
}


/*
===============
UI_PreferencesMenu
===============
*/
void UI_PreferencesMenu( void ) {
	Preferences_MenuInit();
	UI_PushMenu( &s_preferences.menu );
}
