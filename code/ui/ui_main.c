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

USER INTERFACE MAIN

=======================================================================
*/

// use this to get a demo build without an explicit demo build, i.e. to get the demo ui files to build
//#define PRE_RELEASE_TADEMO

#include "ui_local.h"

uiInfo_t uiInfo;

static const char *MonthAbbrev[] = {
	"Jan","Feb","Mar",
	"Apr","May","Jun",
	"Jul","Aug","Sep",
	"Oct","Nov","Dec"
};


static const char *skillLevels[] = {
  "I Can Win",
  "Bring It On",
  "Hurt Me Plenty",
  "Hardcore",
  "Nightmare"
};

static const int numSkillLevels = ARRAY_LEN( skillLevels );

#define COLUMN_HOST			0
#define COLUMN_MAP			1
#define COLUMN_CLIENTS		2
#define COLUMN_GAME			3
#define COLUMN_PING			4

#define UIAS_LOCAL				0
#define UIAS_GLOBAL0			1
#define UIAS_GLOBAL1			2
#define UIAS_GLOBAL2			3
#define UIAS_GLOBAL3			4
#define UIAS_GLOBAL4			5
#define UIAS_GLOBAL5			6
#define UIAS_FAVORITES			7

static const char *netSources[] = {
	"Local",
	"Internet",
	"Master1",
	"Master2",
	"Master3",
	"Master4",
	"Master5",
	"Favorites"
};
static const int numNetSources = ARRAY_LEN( netSources );


static char* netnames[] = {
	"???",
	"UDP",
	"UDP6"
};

static int gamecodetoui[NUM_COLOR_EFFECTS] = {8,4,6,0,10,2,12,1,3,5,7,9,11};
static int uitogamecode[NUM_COLOR_EFFECTS] = {4,8,6,9,2,10,3,11,1,12,5,13,7};


static void UI_StartServerRefresh(qboolean full, qboolean force);
static void UI_StopServerRefresh( void );
static void UI_DoServerRefresh( void );
static void UI_FeederSelection(float feederID, int index);
static void UI_BuildServerDisplayList(int force);
static void UI_BuildServerStatus(qboolean force);
static void UI_BuildFindPlayerList(qboolean force);
static int QDECL UI_ServersQsortCompare( const void *arg1, const void *arg2 );
static int UI_MapCountByGameType(qboolean singlePlayer);
static int UI_HeadCountByTeam( void );
static void UI_ParseGameInfo(const char *teamFile);
static void UI_ParseTeamInfo(const char *teamFile);
static const char *UI_SelectedMap(int index, int *actual);
static const char *UI_SelectedHead(int index, int *actual);
static int UI_GetIndexFromSelection(int actual);
static void UI_DrawCinematic(int handle, float x, float y, float w, float h);

int ProcessNewUI( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 );

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .qvm file
================
*/
vmCvar_t  ui_new;
vmCvar_t  ui_debug;
vmCvar_t  ui_initialized;
vmCvar_t  ui_teamArenaFirstRun;

qboolean UI_WantsBindKeys( void ) {
	return Display_WantsBindKeys();
}

void UI_WindowResized( void ) {

}

void AssetCache( void ) {
	int n;
	//if (Assets.textFont == NULL) {
	//}
	//Assets.background = trap_R_RegisterShaderNoMip( ASSET_BACKGROUND );
	//Com_Printf("Menu Size: %i bytes\n", sizeof(uiInfo.uiDC.Menus));
	uiInfo.uiDC.Assets.gradientBar = trap_R_RegisterShaderNoMip( ASSET_GRADIENTBAR );
	uiInfo.uiDC.Assets.fxBasePic = trap_R_RegisterShaderNoMip( ART_FX_BASE );
	uiInfo.uiDC.Assets.fxPic[0] = trap_R_RegisterShaderNoMip( ART_FX_RED );
	uiInfo.uiDC.Assets.fxPic[1] = trap_R_RegisterShaderNoMip( ART_FX_ORANGE );
	uiInfo.uiDC.Assets.fxPic[2] = trap_R_RegisterShaderNoMip( ART_FX_YELLOW );
	uiInfo.uiDC.Assets.fxPic[3] = trap_R_RegisterShaderNoMip( ART_FX_LIME );
	uiInfo.uiDC.Assets.fxPic[4] = trap_R_RegisterShaderNoMip( ART_FX_GREEN );
	uiInfo.uiDC.Assets.fxPic[5] = trap_R_RegisterShaderNoMip( ART_FX_VIVIDGREEN );
	uiInfo.uiDC.Assets.fxPic[6] = trap_R_RegisterShaderNoMip( ART_FX_TEAL );
	uiInfo.uiDC.Assets.fxPic[7] = trap_R_RegisterShaderNoMip( ART_FX_LIGHTBLUE );
	uiInfo.uiDC.Assets.fxPic[8] = trap_R_RegisterShaderNoMip( ART_FX_BLUE );
	uiInfo.uiDC.Assets.fxPic[9] = trap_R_RegisterShaderNoMip( ART_FX_PURPLE );
	uiInfo.uiDC.Assets.fxPic[10] = trap_R_RegisterShaderNoMip( ART_FX_CYAN );
	uiInfo.uiDC.Assets.fxPic[11] = trap_R_RegisterShaderNoMip( ART_FX_PINK );
	uiInfo.uiDC.Assets.fxPic[12] = trap_R_RegisterShaderNoMip( ART_FX_WHITE );
	uiInfo.uiDC.Assets.scrollBar = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR );
	uiInfo.uiDC.Assets.scrollBarArrowDown = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWDOWN );
	uiInfo.uiDC.Assets.scrollBarArrowUp = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWUP );
	uiInfo.uiDC.Assets.scrollBarArrowLeft = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWLEFT );
	uiInfo.uiDC.Assets.scrollBarArrowRight = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWRIGHT );
	uiInfo.uiDC.Assets.scrollBarThumb = trap_R_RegisterShaderNoMip( ASSET_SCROLL_THUMB );
	uiInfo.uiDC.Assets.sliderBar = trap_R_RegisterShaderNoMip( ASSET_SLIDER_BAR );
	uiInfo.uiDC.Assets.sliderThumb = trap_R_RegisterShaderNoMip( ASSET_SLIDER_THUMB );

	for( n = 0; n < NUM_CROSSHAIRS; n++ ) {
		uiInfo.uiDC.Assets.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%c", 'a' + n ) );
	}

	uiInfo.newHighScoreSound = trap_S_RegisterSound("sound/feedback/voc_newhighscore.wav", qfalse);
}

fontInfo_t *UI_FontForScale( float scale ) {
	fontInfo_t *font = &uiInfo.uiDC.Assets.textFont;
	if (scale <= ui_smallFont.value) {
		font = &uiInfo.uiDC.Assets.smallFont;
	} else if (scale >= ui_bigFont.value) {
		font = &uiInfo.uiDC.Assets.bigFont;
	}

	return font;
}

void UI_Text_PaintWithCursor(float x, float y, float scale, const vec4_t color, const char *text, int cursorPos, char cursor, int limit, int textStyle) {
	float shadowOffset;

	if ( textStyle == ITEM_TEXTSTYLE_SHADOWED ) {
		shadowOffset = 1;
	} else if ( textStyle == ITEM_TEXTSTYLE_SHADOWEDMORE ) {
		shadowOffset = 2;
	} else {
		shadowOffset = 0;
	}

	Text_PaintWithCursor(x, y, UI_FontForScale( scale ), scale, color, text, cursorPos, cursor, 0, limit, shadowOffset, 0, qfalse, qfalse);
}

int UI_Text_Width(const char *text, float scale, int limit) {
	return Text_Width( text, UI_FontForScale( scale ), scale, limit );
}

int UI_Text_Height(const char *text, float scale, int limit) {
	return Text_Height( text, UI_FontForScale( scale ), scale, limit );
}

void UI_Text_Paint(float x, float y, float scale, const vec4_t color, const char *text, float adjust, int limit, int textStyle) {
	float shadowOffset;

	if ( textStyle == ITEM_TEXTSTYLE_SHADOWED ) {
		shadowOffset = 1;
	} else if ( textStyle == ITEM_TEXTSTYLE_SHADOWEDMORE ) {
		shadowOffset = 2;
	} else {
		shadowOffset = 0;
	}

	Text_Paint( x, y, UI_FontForScale( scale ), scale, color, text, adjust, limit, shadowOffset, 0, qfalse, qfalse );
}

// this function does not align text position to pixel boundary so that text scrolls evenly
void UI_Text_PaintInMotion(float x, float y, float scale, const vec4_t color, const char *text, float adjust, int limit, int textStyle) {
	float shadowOffset;

	if ( textStyle == ITEM_TEXTSTYLE_SHADOWED ) {
		shadowOffset = 1;
	} else if ( textStyle == ITEM_TEXTSTYLE_SHADOWEDMORE ) {
		shadowOffset = 2;
	} else {
		shadowOffset = 0;
	}

	Text_Paint( x, y, UI_FontForScale( scale ), scale, color, text, adjust, limit, shadowOffset, 0, qfalse, qtrue );
}

void UI_Text_Paint_Limit(float *maxX, float x, float y, float scale, const vec4_t color, const char* text, float adjust, int limit) {
	Text_Paint_Limit( maxX, x, y, UI_FontForScale( scale ), scale, color, text, adjust, limit, qfalse );
}

// used by cgame/cg_info.c
void UI_DrawProportionalString( int x, int y, const char* str, int style, vec4_t color ) {
	int width;
	float scale;

	scale = ui_bigFont.value;

	switch( style & UI_FORMATMASK ) {
		case UI_CENTER:
			width = UI_Text_Width( str, scale, 0 );
			x -= width / 2;
			break;

		case UI_RIGHT:
			width = UI_Text_Width( str, scale, 0 );
			x -= width;
			break;

		case UI_LEFT:
		default:
			break;
	}

	UI_Text_Paint( x, y, scale, color, str, 0, 0, ( style & UI_DROPSHADOW ) ? ITEM_TEXTSTYLE_SHADOWED : 0 );
}

void UI_ShowPostGame(qboolean newHigh) {
	uiInfo.soundHighScore = newHigh;
	UI_SetActiveMenu(UIMENU_POSTGAME);
}
/*
=================
UI_Refresh
=================
*/

int frameCount = 0;
int startTime;

#define	UI_FPS_FRAMES	4
void UI_Refresh( int realtime )
{
	static int index;
	static int	previousTimes[UI_FPS_FRAMES];

	//if ( !( Key_GetCatcher() & KEYCATCH_UI ) ) {
	//	return;
	//}

	Init_Display(&uiInfo.uiDC);

	uiInfo.uiDC.frameTime = realtime - uiInfo.uiDC.realTime;
	uiInfo.uiDC.realTime = realtime;

	previousTimes[index % UI_FPS_FRAMES] = uiInfo.uiDC.frameTime;
	index++;
	if ( index > UI_FPS_FRAMES ) {
		int i, total;
		// average multiple frames together to smooth changes out a bit
		total = 0;
		for ( i = 0 ; i < UI_FPS_FRAMES ; i++ ) {
			total += previousTimes[i];
		}
		if ( !total ) {
			total = 1;
		}
		uiInfo.uiDC.FPS = 1000 * UI_FPS_FRAMES / (float)total;
	}



	UI_UpdateCvars();

	if ( UI_IsFullscreen() ) {
		CG_ClearViewport();
	}

	if (Menu_Count() > 0) {
		// paint all the menus
		Menu_PaintAll();
		// refresh server browser list
		UI_DoServerRefresh();
		// refresh server status
		UI_BuildServerStatus(qfalse);
		// refresh find player list
		UI_BuildFindPlayerList(qfalse);
	} 
	
	// draw cursor
	trap_R_SetColor( NULL );
	if (Menu_Count() > 0 && (Key_GetCatcher() & KEYCATCH_UI)) {
		CG_DrawPic( uiInfo.uiDC.cursorx-16, uiInfo.uiDC.cursory-16, 32, 32, uiInfo.uiDC.Assets.cursor);

		if (Display_DebugMode()) {
			// cursor coordinates
			CG_DrawString( 0, 0, va("(%d,%d)",uiInfo.uiDC.cursorx,uiInfo.uiDC.cursory), UI_LEFT|UI_SMALLFONT, colorRed );
		}
	}
}

/*
=================
UI_Shutdown
=================
*/
void UI_Shutdown( void ) {
	trap_LAN_SaveCachedServers();
}

char *defaultMenu = NULL;

char *GetMenuBuffer(const char *filename) {
	int	len;
	fileHandle_t	f;
	static char buf[MAX_MENUFILE];

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( S_COLOR_RED "menu file not found: %s, using default\n", filename ) );
		return defaultMenu;
	}
	if ( len >= MAX_MENUFILE ) {
		trap_Print( va( S_COLOR_RED "menu file too large: %s is %i, max allowed is %i\n", filename, len, MAX_MENUFILE ) );
		trap_FS_FCloseFile( f );
		return defaultMenu;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );
	//COM_Compress(buf);
  return buf;

}

qboolean Asset_Parse(int handle) {
	pc_token_t token;
	const char *tempStr;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (Q_stricmp(token.string, "{") != 0) {
		return qfalse;
	}
    
	while ( 1 ) {

		memset(&token, 0, sizeof(pc_token_t));

		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;

		if (Q_stricmp(token.string, "}") == 0) {
			return qtrue;
		}

		// font
		if (Q_stricmp(token.string, "font") == 0) {
			int pointSize;
			if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle,&pointSize)) {
				return qfalse;
			}
			if (ui_menuFont.string[0]) {
				if (CG_InitTrueTypeFont(ui_menuFont.string, pointSize, 0, &uiInfo.uiDC.Assets.textFont)) {
					Com_DPrintf("Overriding UI font '%s' with '%s'\n", tempStr, ui_menuFont.string);
					continue;
				}
			}
			if (!CG_InitTrueTypeFont(tempStr, pointSize, 0, &uiInfo.uiDC.Assets.textFont)) {
				CG_InitBitmapFont(&uiInfo.uiDC.Assets.textFont, pointSize, pointSize / 2);
			}
			uiInfo.uiDC.Assets.fontRegistered = qtrue;
			continue;
		}

		if (Q_stricmp(token.string, "smallFont") == 0) {
			int pointSize;
			if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle,&pointSize)) {
				return qfalse;
			}
			if (ui_menuFont.string[0]) {
				if (CG_InitTrueTypeFont(ui_menuFont.string, pointSize, 0, &uiInfo.uiDC.Assets.smallFont)) {
					Com_DPrintf("Overriding UI smallFont '%s' with '%s'\n", tempStr, ui_menuFont.string);
					continue;
				}
			}
			if (!CG_InitTrueTypeFont(tempStr, pointSize, 0, &uiInfo.uiDC.Assets.smallFont)) {
				CG_InitBitmapFont(&uiInfo.uiDC.Assets.smallFont, pointSize, pointSize / 2);
			}
			continue;
		}

		if (Q_stricmp(token.string, "bigFont") == 0) {
			int pointSize;
			if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle,&pointSize)) {
				return qfalse;
			}
			if (ui_menuFont.string[0]) {
				if (CG_InitTrueTypeFont(ui_menuFont.string, pointSize, 0, &uiInfo.uiDC.Assets.bigFont)) {
					Com_DPrintf("Overriding UI bigFont '%s' with '%s'\n", tempStr, ui_menuFont.string);
					continue;
				}
			}
			if (!CG_InitTrueTypeFont(tempStr, pointSize, 0, &uiInfo.uiDC.Assets.bigFont)) {
				CG_InitBitmapFont(&uiInfo.uiDC.Assets.bigFont, pointSize, pointSize / 2);
			}
			continue;
		}


		// gradientbar
		if (Q_stricmp(token.string, "gradientbar") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.gradientBar = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		// enterMenuSound
		if (Q_stricmp(token.string, "menuEnterSound") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.menuEnterSound = trap_S_RegisterSound( tempStr, qfalse );
			continue;
		}

		// exitMenuSound
		if (Q_stricmp(token.string, "menuExitSound") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.menuExitSound = trap_S_RegisterSound( tempStr, qfalse );
			continue;
		}

		// itemFocusSound
		if (Q_stricmp(token.string, "itemFocusSound") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.itemFocusSound = trap_S_RegisterSound( tempStr, qfalse );
			continue;
		}

		// menuBuzzSound
		if (Q_stricmp(token.string, "menuBuzzSound") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.menuBuzzSound = trap_S_RegisterSound( tempStr, qfalse );
			continue;
		}

		if (Q_stricmp(token.string, "cursor") == 0) {
			if (!PC_String_Parse(handle, &uiInfo.uiDC.Assets.cursorStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.cursor = trap_R_RegisterShaderNoMip( uiInfo.uiDC.Assets.cursorStr);
			continue;
		}

		if (Q_stricmp(token.string, "fadeClamp") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.fadeClamp)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "fadeCycle") == 0) {
			if (!PC_Int_Parse(handle, &uiInfo.uiDC.Assets.fadeCycle)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "fadeAmount") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.fadeAmount)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowX") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.shadowX)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowY") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.shadowY)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowColor") == 0) {
			if (!PC_Color_Parse(handle, &uiInfo.uiDC.Assets.shadowColor)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.shadowFadeClamp = uiInfo.uiDC.Assets.shadowColor[3];
			continue;
		}

	}
	return qfalse;
}

void Font_Report( void ) {
  int i;
  Com_Printf("Font Info\n");
  Com_Printf("=========\n");
  for ( i = 32; i < 96; i++) {
    Com_Printf("Glyph handle %i: %i\n", i, uiInfo.uiDC.Assets.textFont.glyphs[i].glyph);
  }
}

void UI_Report( void ) {
  String_Report();
  //Font_Report();

}

void UI_ParseMenu(const char *menuFile) {
	int handle;
	pc_token_t token;

	Com_DPrintf("Parsing menu file: %s\n", menuFile);

	handle = trap_PC_LoadSource(menuFile, NULL);
	if (!handle) {
		return;
	}

	while ( 1 ) {
		memset(&token, 0, sizeof(pc_token_t));
		if (!trap_PC_ReadToken( handle, &token )) {
			break;
		}

		//if ( Q_stricmp( token, "{" ) ) {
		//	Com_Printf( "Missing { in menu file\n" );
		//	break;
		//}

		//if ( uiInfo.uiDC.menuCount == MAX_MENUS ) {
		//	Com_Printf( "Too many menus!\n" );
		//	break;
		//}

		if ( token.string[0] == '}' ) {
			break;
		}

		if (Q_stricmp(token.string, "assetGlobalDef") == 0) {
			if (Asset_Parse(handle)) {
				continue;
			} else {
				break;
			}
		}

		if (Q_stricmp(token.string, "menudef") == 0) {
			// start a new menu
			Menu_New(handle);
		}
	}
	trap_PC_FreeSource(handle);
}

qboolean Load_Menu(int handle) {
	pc_token_t token;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (token.string[0] != '{') {
		return qfalse;
	}

	while ( 1 ) {

		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;
    
		if ( token.string[0] == 0 ) {
			return qfalse;
		}

		if ( token.string[0] == '}' ) {
			return qtrue;
		}

		UI_ParseMenu(token.string); 
	}
	return qfalse;
}

void UI_LoadMenus(const char *menuFile, qboolean reset) {
	pc_token_t token;
	int handle;
	int start;

	start = trap_Milliseconds();

	Init_Display(&uiInfo.uiDC);

	handle = trap_PC_LoadSource( menuFile, NULL );
	if (!handle) {
		Com_Printf( S_COLOR_YELLOW "menu file not found: %s, using default\n", menuFile );
		handle = trap_PC_LoadSource( "ui/menus.txt", NULL );
		if (!handle) {
			trap_Error( S_COLOR_RED "default menu file not found: ui/menus.txt, unable to continue!" );
		}
	}

	ui_new.integer = 1;

	if (reset) {
		Menu_Reset();
	}

	while ( 1 ) {
		if (!trap_PC_ReadToken(handle, &token))
			break;
		if( token.string[0] == 0 || token.string[0] == '}') {
			break;
		}

		if ( token.string[0] == '}' ) {
			break;
		}

		if (Q_stricmp(token.string, "loadmenu") == 0) {
			if (Load_Menu(handle)) {
				continue;
			} else {
				break;
			}
		}
	}

	Com_DPrintf("UI menu load time = %d milli seconds\n", trap_Milliseconds() - start);

	trap_PC_FreeSource( handle );
}

void UI_Load(void) {
	char lastName[1024];
	menuDef_t *menu;
	char *menuSet;

	Init_Display(&uiInfo.uiDC);

	menu = Menu_GetFocused();
	menuSet = CG_Cvar_VariableString("ui_menuFiles");

	if (menu && menu->window.name) {
		Q_strncpyz(lastName, menu->window.name, sizeof(lastName));
	} else {
		lastName[0] = '\0';
	}
	if (menuSet == NULL || menuSet[0] == '\0') {
		menuSet = "ui/menus.txt";
	}

	String_Init();

#ifdef PRE_RELEASE_TADEMO
	UI_ParseGameInfo("demogameinfo.txt");
#else
	UI_ParseGameInfo("gameinfo.txt");
	UI_LoadArenas();
#endif

	UI_LoadMenus(menuSet, qtrue);
	UI_LoadMenus("ui/ingame.txt", qfalse);
	Menus_CloseAll();
	if ( lastName[0] ) {
		Menus_ActivateByName(lastName);
	}

}

// Convert ui's net source to AS_* used by trap calls.
int UI_SourceForLAN(void) {
	switch (ui_netSource.integer) {
		default:
		case UIAS_LOCAL:
			return AS_LOCAL;
		case UIAS_GLOBAL0:
		case UIAS_GLOBAL1:
		case UIAS_GLOBAL2:
		case UIAS_GLOBAL3:
		case UIAS_GLOBAL4:
		case UIAS_GLOBAL5:
			return AS_GLOBAL;
		case UIAS_FAVORITES:
			return AS_FAVORITES;
	}
}

const char *UI_FilterDescription( int value ) {
	if ( value <= 0 || value > uiInfo.modCount ) {
		return "All";
	}

	return uiInfo.modList[value - 1].modDescr;
}

const char *UI_FilterDir( int value ) {
	if ( value <= 0 || value > uiInfo.modCount ) {
		return "";
	}

	return uiInfo.modList[value - 1].modName;
}


static const char *handicapValues[] = {"None","95","90","85","80","75","70","65","60","55","50","45","40","35","30","25","20","15","10","5",NULL};

static void UI_DrawHandicap(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
  int i, h;

  h = Com_Clamp( 5, 100, trap_Cvar_VariableValue("handicap") );
  i = 20 - h / 5;

  UI_Text_Paint(rect->x, rect->y, scale, color, handicapValues[i], 0, 0, textStyle);
}

static void UI_DrawClanName(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
  UI_Text_Paint(rect->x, rect->y, scale, color, CG_Cvar_VariableString("ui_teamName"), 0, 0, textStyle);
}


static void UI_SetCapFragLimits(qboolean uiVars) {
	int cap = 5;
	int frag = 10;
	if (uiInfo.gameTypes[ui_gameType.integer].gtEnum == GT_OBELISK) {
		cap = 4;
	} else if (uiInfo.gameTypes[ui_gameType.integer].gtEnum == GT_HARVESTER) {
		cap = 15;
	}
	if (uiVars) {
		trap_Cvar_SetValue("ui_captureLimit", cap);
		trap_Cvar_SetValue("ui_fragLimit", frag);
	} else {
		trap_Cvar_SetValue("capturelimit", cap);
		trap_Cvar_SetValue("fraglimit", frag);
	}
}
// ui_gameType assumes gametype 0 is -1 ALL and will not show
static void UI_DrawGameType(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
  UI_Text_Paint(rect->x, rect->y, scale, color, uiInfo.gameTypes[ui_gameType.integer].gameType, 0, 0, textStyle);
}

static void UI_DrawNetGameType(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
  UI_Text_Paint(rect->x, rect->y, scale, color, uiInfo.gameTypes[ui_netGameType.integer].gameType , 0, 0, textStyle);
}

static void UI_DrawJoinGameType(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
  UI_Text_Paint(rect->x, rect->y, scale, color, uiInfo.joinGameTypes[ui_joinGameType.integer].gameType , 0, 0, textStyle);
}



static int UI_TeamIndexFromName(const char *name) {
  int i;

  if (name && *name) {
    for (i = 0; i < uiInfo.teamCount; i++) {
      if (Q_stricmp(name, uiInfo.teamList[i].teamName) == 0) {
        return i;
      }
    }
  } 

  return 0;

}

static void UI_DrawClanLogo(rectDef_t *rect, float scale, vec4_t color) {
  int i;
  i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));
  if (i >= 0 && i < uiInfo.teamCount) {
  	trap_R_SetColor( color );

		if (uiInfo.teamList[i].teamIcon == -1) {
      uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
      uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
      uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
		}

  	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon);
    trap_R_SetColor(NULL);
  }
}

static void UI_DrawClanCinematic(rectDef_t *rect, float scale, vec4_t color) {
  int i;
  i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));
  if (i >= 0 && i < uiInfo.teamCount) {

		if (uiInfo.teamList[i].cinematic >= -2) {
			if (uiInfo.teamList[i].cinematic == -1) {
				uiInfo.teamList[i].cinematic = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.teamList[i].imageName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
			}
			if (uiInfo.teamList[i].cinematic >= 0) {
			  trap_CIN_RunCinematic(uiInfo.teamList[i].cinematic);
			  UI_DrawCinematic(uiInfo.teamList[i].cinematic, rect->x, rect->y, rect->w, rect->h);
			} else {
			  	trap_R_SetColor( color );
				CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon_Metal);
				trap_R_SetColor(NULL);
				uiInfo.teamList[i].cinematic = -2;
			}
		} else {
	  	trap_R_SetColor( color );
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon);
			trap_R_SetColor(NULL);
		}
	}

}

static void UI_DrawPreviewCinematic(rectDef_t *rect, float scale, vec4_t color) {
	if (uiInfo.previewMovie > -2) {
		uiInfo.previewMovie = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.movieList[uiInfo.movieIndex]), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
		if (uiInfo.previewMovie >= 0) {
		  trap_CIN_RunCinematic(uiInfo.previewMovie);
		  UI_DrawCinematic(uiInfo.previewMovie, rect->x, rect->y, rect->w, rect->h);
		} else {
			uiInfo.previewMovie = -2;
		}
	} 

}



static void UI_DrawSkill(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
  int i;
	i = trap_Cvar_VariableValue( "g_spSkill" );
  if (i < 1 || i > numSkillLevels) {
    i = 1;
  }
  UI_Text_Paint(rect->x, rect->y, scale, color, skillLevels[i-1],0, 0, textStyle);
}


static void UI_DrawTeamName(rectDef_t *rect, float scale, vec4_t color, qboolean blue, int textStyle) {
  int i;
  i = UI_TeamIndexFromName(CG_Cvar_VariableString((blue) ? "ui_blueTeam" : "ui_redTeam"));
  if (i >= 0 && i < uiInfo.teamCount) {
    UI_Text_Paint(rect->x, rect->y, scale, color, va("%s: %s", (blue) ? "Blue" : "Red", uiInfo.teamList[i].teamName),0, 0, textStyle);
  }
}

static void UI_DrawTeamMember(rectDef_t *rect, float scale, vec4_t color, qboolean blue, int num, int textStyle) {
	// 0 - None
	// 1 - Human
	// 2 - Random Bot
	// 3..NumCharacters - Bot
	int value = trap_Cvar_VariableValue(va(blue ? "ui_blueteam%i" : "ui_redteam%i", num));
	const char *text;
	if (value <= 0) {
		text = "Closed";
	} else if (value == 1) {
		text = "Human";
	} else if (value == 2) {
		text = "Random Bot";
	} else {
		value -= 3;

		if (ui_actualNetGameType.integer >= GT_TEAM) {
			if (value >= uiInfo.characterCount) {
				value = 0;
			}
			text = uiInfo.characterList[value].name;
		} else {
			if (value >= UI_GetNumBots()) {
				value = 0;
			}
			text = UI_GetBotNameByNumber(value);
		}
	}
  UI_Text_Paint(rect->x, rect->y, scale, color, text, 0, 0, textStyle);
}

static void UI_DrawEffects(rectDef_t *rect, float scale, vec4_t color) {
	float xOffset = 128.0f / (NUM_COLOR_EFFECTS + 1);
	qhandle_t colorShader;

	CG_DrawPic( rect->x, rect->y - 14, 128, 8, uiInfo.uiDC.Assets.fxBasePic );

	colorShader = uiInfo.uiDC.Assets.fxPic[uiInfo.effectsColor];
	if ( !colorShader ) {
		vec4_t picColor;

		colorShader = uiInfo.uiDC.Assets.fxPic[NUM_COLOR_EFFECTS-1]; // white
		if ( !colorShader )
			colorShader = uiInfo.uiDC.whiteShader;

		CG_PlayerColorFromIndex( uitogamecode[uiInfo.effectsColor], picColor );
		picColor[3] = 1;
		trap_R_SetColor( picColor );
	}

	CG_DrawPic( rect->x + uiInfo.effectsColor * xOffset + xOffset * 0.5f, rect->y - 16, 16, 12, colorShader );
	trap_R_SetColor( NULL );
}

static void UI_DrawMapPreview(rectDef_t *rect, float scale, vec4_t color, qboolean net) {
	int map = (net) ? ui_currentNetMap.integer : ui_currentMap.integer;

	if (uiInfo.mapList[map].levelShot == -1) {
		uiInfo.mapList[map].levelShot = trap_R_RegisterShaderNoMip(uiInfo.mapList[map].imageName);
	}

	if (uiInfo.mapList[map].levelShot > 0) {
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.mapList[map].levelShot);
	} else {
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, trap_R_RegisterShaderNoMip("menu/art/unknownmap"));
	}
}						 


static void UI_DrawMapTimeToBeat(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
	int minutes, seconds, time;

	time = uiInfo.mapList[ui_currentMap.integer].timeToBeat[uiInfo.gameTypes[ui_gameType.integer].gtEnum];

	minutes = time / 60;
	seconds = time % 60;

  UI_Text_Paint(rect->x, rect->y, scale, color, va("%02i:%02i", minutes, seconds), 0, 0, textStyle);
}



static void UI_DrawMapCinematic(rectDef_t *rect, float scale, vec4_t color, qboolean net) {

	int map = (net) ? ui_currentNetMap.integer : ui_currentMap.integer; 

	if (uiInfo.mapList[map].cinematic >= -1) {
		if (uiInfo.mapList[map].cinematic == -1) {
			uiInfo.mapList[map].cinematic = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.mapList[map].mapLoadName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
		}
		if (uiInfo.mapList[map].cinematic >= 0) {
		  trap_CIN_RunCinematic(uiInfo.mapList[map].cinematic);
		  UI_DrawCinematic(uiInfo.mapList[map].cinematic, rect->x, rect->y, rect->w, rect->h);
		} else {
			uiInfo.mapList[map].cinematic = -2;
		}
	} else {
		UI_DrawMapPreview(rect, scale, color, net);
	}
}



static qboolean updateModel = qtrue;
static qboolean q3Model = qfalse;
static qboolean updateModelColor = qtrue;

static void UI_DrawPlayerModel(rectDef_t *rect) {
  static uiPlayerInfo_t info;
  char model[MAX_QPATH];
  char team[256];
	char head[256];
	vec3_t	viewangles;
	vec3_t	moveangles;

	  if (trap_Cvar_VariableValue("ui_Q3Model")) {
		Q_strncpyz(model, CG_Cvar_VariableString("model"), sizeof(model));
		Q_strncpyz(head, CG_Cvar_VariableString("headmodel"), sizeof(head));
		if (!q3Model) {
			q3Model = qtrue;
			updateModel = qtrue;
		}
		team[0] = '\0';
	} else {

		Q_strncpyz(team, CG_Cvar_VariableString("ui_teamName"), sizeof(team));
		Q_strncpyz(model, CG_Cvar_VariableString("team_model"), sizeof(model));
		Q_strncpyz(head, CG_Cvar_VariableString("team_headmodel"), sizeof(head));
		if (q3Model) {
			q3Model = qfalse;
			updateModel = qtrue;
		}
	}
  if (updateModel) {
  	memset( &info, 0, sizeof(uiPlayerInfo_t) );
  	viewangles[YAW]   = 180 - 10;
  	viewangles[PITCH] = 0;
  	viewangles[ROLL]  = 0;
  	VectorClear( moveangles );
    UI_PlayerInfo_SetModel( &info, model, head, team);
    UI_PlayerInfo_SetInfo( &info, 0, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
//		UI_RegisterPlayerModelname( &info, model, head, team);
    updateModel = qfalse;
    updateModelColor = qfalse; // playerinfo setinfo calls updatecolor
  }
  if (updateModelColor) {
    UI_PlayerInfo_UpdateColor( &info );
    updateModelColor = qfalse;
  }

  UI_DrawPlayer( rect->x, rect->y, rect->w, rect->h, &info, uiInfo.uiDC.realTime / 2);

}

static void UI_DrawNetSource(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
	if (ui_netSource.integer < 0 || ui_netSource.integer >= numNetSources) {
		ui_netSource.integer = 0;
	}
  UI_Text_Paint(rect->x, rect->y, scale, color, va("Source: %s", netSources[ui_netSource.integer]), 0, 0, textStyle);
}

static void UI_DrawNetMapPreview(rectDef_t *rect, float scale, vec4_t color) {

	if (uiInfo.serverStatus.currentServerPreview > 0) {
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.serverStatus.currentServerPreview);
	} else {
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, trap_R_RegisterShaderNoMip("menu/art/unknownmap"));
	}
}

static void UI_DrawNetMapCinematic(rectDef_t *rect, float scale, vec4_t color) {
	if (uiInfo.serverStatus.currentServerCinematic >= 0) {
	  trap_CIN_RunCinematic(uiInfo.serverStatus.currentServerCinematic);
	  UI_DrawCinematic(uiInfo.serverStatus.currentServerCinematic, rect->x, rect->y, rect->w, rect->h);
	} else {
		UI_DrawNetMapPreview(rect, scale, color);
	}
}



static void UI_DrawNetFilter(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
	UI_Text_Paint(rect->x, rect->y, scale, color, va("Filter: %s", UI_FilterDescription( ui_serverFilterType.integer ) ), 0, 0, textStyle);
}


static void UI_DrawTier(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
  int i;
	i = trap_Cvar_VariableValue( "ui_currentTier" );
  if (i < 0 || i >= uiInfo.tierCount) {
    i = 0;
  }
  UI_Text_Paint(rect->x, rect->y, scale, color, va("Tier: %s", uiInfo.tierList[i].tierName),0, 0, textStyle);
}

static void UI_DrawTierMap(rectDef_t *rect, int index) {
  int i;
	i = trap_Cvar_VariableValue( "ui_currentTier" );
  if (i < 0 || i >= uiInfo.tierCount) {
    i = 0;
  }

	if (uiInfo.tierList[i].mapHandles[index] == -1) {
		uiInfo.tierList[i].mapHandles[index] = trap_R_RegisterShaderNoMip(va("levelshots/%s", uiInfo.tierList[i].maps[index]));
	}
												 
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.tierList[i].mapHandles[index]);
}

static const char *UI_EnglishMapName(const char *map) {
	int i;
	for (i = 0; i < uiInfo.mapCount; i++) {
		if (Q_stricmp(map, uiInfo.mapList[i].mapLoadName) == 0) {
			return uiInfo.mapList[i].mapName;
		}
	}
	return "";
}

static void UI_DrawTierMapName(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
  int i, j;
	i = trap_Cvar_VariableValue( "ui_currentTier" );
  if (i < 0 || i >= uiInfo.tierCount) {
    i = 0;
  }
	j = trap_Cvar_VariableValue("ui_currentMap");
	if (j < 0 || j >= MAPS_PER_TIER) {
		j = 0;
	}

  UI_Text_Paint(rect->x, rect->y, scale, color, UI_EnglishMapName(uiInfo.tierList[i].maps[j]), 0, 0, textStyle);
}

static void UI_DrawTierGameType(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
  int i, j;
	i = trap_Cvar_VariableValue( "ui_currentTier" );
  if (i < 0 || i >= uiInfo.tierCount) {
    i = 0;
  }
	j = trap_Cvar_VariableValue("ui_currentMap");
	if (j < 0 || j >= MAPS_PER_TIER) {
		j = 0;
	}

  UI_Text_Paint(rect->x, rect->y, scale, color, uiInfo.gameTypes[uiInfo.tierList[i].gameTypes[j]].gameType , 0, 0, textStyle);
}


static const char *UI_AIFromName(const char *name) {
	int j;
	for (j = 0; j < uiInfo.aliasCount; j++) {
		if (Q_stricmp(uiInfo.aliasList[j].name, name) == 0) {
			return uiInfo.aliasList[j].ai;
		}
	}
	for (j = 0; j < uiInfo.characterCount; j++) {
		if (Q_stricmp(uiInfo.characterList[j].name, name) == 0) {
			return uiInfo.characterList[j].name;
		}
	}
	// Name is listed in team list but not in alias or characters list.
	Com_Printf( S_COLOR_YELLOW "WARNING: Unknown team character '%s'.\n", name );
	return name;
}


static qboolean updateOpponentModel = qtrue;
static void UI_DrawOpponent(rectDef_t *rect) {
  static uiPlayerInfo_t info2;
  char model[MAX_QPATH];
  char headmodel[MAX_QPATH];
  char team[256];
	vec3_t	viewangles;
	vec3_t	moveangles;
  
	if (updateOpponentModel) {
		
		Q_strncpyz(model, CG_Cvar_VariableString("ui_opponentModel"), sizeof(model));
		Q_strncpyz(headmodel, CG_Cvar_VariableString("ui_opponentModel"), sizeof(headmodel));
		team[0] = '\0';

  	memset( &info2, 0, sizeof(uiPlayerInfo_t) );
  	viewangles[YAW]   = 180 - 10;
  	viewangles[PITCH] = 0;
  	viewangles[ROLL]  = 0;
  	VectorClear( moveangles );
    UI_PlayerInfo_SetModel( &info2, model, headmodel, "");
    UI_PlayerInfo_SetInfo( &info2, 0, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
		UI_RegisterPlayerModelname( &info2, model, headmodel, team);
    updateOpponentModel = qfalse;
  }

  UI_DrawPlayer( rect->x, rect->y, rect->w, rect->h, &info2, uiInfo.uiDC.realTime / 2);

}

static void UI_NextOpponent( void ) {
  int i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_opponentName"));
  int j = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));
	i++;
	if (i >= uiInfo.teamCount) {
		i = 0;
	}
	if (i == j) {
		i++;
		if ( i >= uiInfo.teamCount) {
			i = 0;
		}
	}
 	trap_Cvar_Set( "ui_opponentName", uiInfo.teamList[i].teamName );
}

static void UI_PriorOpponent( void ) {
  int i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_opponentName"));
  int j = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));
	i--;
	if (i < 0) {
		i = uiInfo.teamCount - 1;
	}
	if (i == j) {
		i--;
		if ( i < 0) {
			i = uiInfo.teamCount - 1;
		}
	}
 	trap_Cvar_Set( "ui_opponentName", uiInfo.teamList[i].teamName );
}

static void	UI_DrawPlayerLogo(rectDef_t *rect, vec3_t color) {
  int i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));

	if (uiInfo.teamList[i].teamIcon == -1) {
    uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
    uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
    uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

 	trap_R_SetColor( color );
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon );
 	trap_R_SetColor( NULL );
}

static void	UI_DrawPlayerLogoMetal(rectDef_t *rect, vec3_t color) {
  int i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));
	if (uiInfo.teamList[i].teamIcon == -1) {
    uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
    uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
    uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

 	trap_R_SetColor( color );
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon_Metal );
 	trap_R_SetColor( NULL );
}

static void	UI_DrawPlayerLogoName(rectDef_t *rect, vec3_t color) {
  int i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));
	if (uiInfo.teamList[i].teamIcon == -1) {
    uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
    uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
    uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

 	trap_R_SetColor( color );
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon_Name );
 	trap_R_SetColor( NULL );
}

static void	UI_DrawOpponentLogo(rectDef_t *rect, vec3_t color) {
  int i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_opponentName"));
	if (uiInfo.teamList[i].teamIcon == -1) {
    uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
    uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
    uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

 	trap_R_SetColor( color );
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon );
 	trap_R_SetColor( NULL );
}

static void	UI_DrawOpponentLogoMetal(rectDef_t *rect, vec3_t color) {
  int i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_opponentName"));
	if (uiInfo.teamList[i].teamIcon == -1) {
    uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
    uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
    uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

 	trap_R_SetColor( color );
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon_Metal );
 	trap_R_SetColor( NULL );
}

static void	UI_DrawOpponentLogoName(rectDef_t *rect, vec3_t color) {
  int i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_opponentName"));
	if (uiInfo.teamList[i].teamIcon == -1) {
    uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
    uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
    uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

 	trap_R_SetColor( color );
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon_Name );
 	trap_R_SetColor( NULL );
}

static void UI_DrawAllMapsSelection(rectDef_t *rect, float scale, vec4_t color, int textStyle, qboolean net) {
	int map = (net) ? ui_currentNetMap.integer : ui_currentMap.integer;
	if (map >= 0 && map < uiInfo.mapCount) {
	  UI_Text_Paint(rect->x, rect->y, scale, color, uiInfo.mapList[map].mapName, 0, 0, textStyle);
	}
}

static void UI_DrawOpponentName(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
  UI_Text_Paint(rect->x, rect->y, scale, color, CG_Cvar_VariableString("ui_opponentName"), 0, 0, textStyle);
}


static int UI_OwnerDrawWidth(int ownerDraw, float scale) {
	int i, h, value;
	const char *text;
	const char *s = NULL;

  switch (ownerDraw) {
    case UI_HANDICAP:
			  h = Com_Clamp( 5, 100, trap_Cvar_VariableValue("handicap") );
				i = 20 - h / 5;
				s = handicapValues[i];
      break;
    case UI_CLANNAME:
				s = CG_Cvar_VariableString("ui_teamName");
      break;
    case UI_GAMETYPE:
				s = uiInfo.gameTypes[ui_gameType.integer].gameType;
      break;
    case UI_SKILL:
				i = trap_Cvar_VariableValue( "g_spSkill" );
				if (i < 1 || i > numSkillLevels) {
					i = 1;
				}
			  s = skillLevels[i-1];
      break;
    case UI_BLUETEAMNAME:
			  i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_blueTeam"));
			  if (i >= 0 && i < uiInfo.teamCount) {
			    s = va("%s: %s", "Blue", uiInfo.teamList[i].teamName);
			  }
      break;
    case UI_REDTEAMNAME:
			  i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_redTeam"));
			  if (i >= 0 && i < uiInfo.teamCount) {
			    s = va("%s: %s", "Red", uiInfo.teamList[i].teamName);
			  }
      break;
    case UI_BLUETEAM1:
		case UI_BLUETEAM2:
		case UI_BLUETEAM3:
		case UI_BLUETEAM4:
		case UI_BLUETEAM5:
			value = trap_Cvar_VariableValue(va("ui_blueteam%i", ownerDraw-UI_BLUETEAM1 + 1));
			if (value <= 0) {
				text = "Closed";
			} else if (value == 1) {
				text = "Human";
			} else {
				value -= 2;
				if (value >= uiInfo.aliasCount) {
					value = 0;
				}
				text = uiInfo.aliasList[value].name;
			}
			s = va("%i. %s", ownerDraw-UI_BLUETEAM1 + 1, text);
      break;
    case UI_REDTEAM1:
		case UI_REDTEAM2:
		case UI_REDTEAM3:
		case UI_REDTEAM4:
		case UI_REDTEAM5:
			value = trap_Cvar_VariableValue(va("ui_redteam%i", ownerDraw-UI_REDTEAM1 + 1));
			if (value <= 0) {
				text = "Closed";
			} else if (value == 1) {
				text = "Human";
			} else {
				value -= 2;
				if (value >= uiInfo.aliasCount) {
					value = 0;
				}
				text = uiInfo.aliasList[value].name;
			}
			s = va("%i. %s", ownerDraw-UI_REDTEAM1 + 1, text);
      break;
		case UI_NETSOURCE:
			if (ui_netSource.integer < 0 || ui_netSource.integer >= numNetSources) {
				ui_netSource.integer = 0;
			}
			s = va("Source: %s", netSources[ui_netSource.integer]);
			break;
		case UI_NETFILTER:
			s = va("Filter: %s", UI_FilterDescription( ui_serverFilterType.integer ) );
			break;
		case UI_TIER:
			break;
		case UI_TIER_MAPNAME:
			break;
		case UI_TIER_GAMETYPE:
			break;
		case UI_ALLMAPS_SELECTION:
			break;
		case UI_OPPONENT_NAME:
			break;
		case UI_KEYBINDSTATUS:
			if (Display_KeyBindPending()) {
				s = "Waiting for new key... Press ESCAPE to cancel";
			} else {
				s = "Press ENTER or CLICK to change, Press BACKSPACE to clear";
			}
			break;
		case UI_SERVERREFRESHDATE:
			s = CG_Cvar_VariableString(va("ui_lastServerRefresh_%i", ui_netSource.integer));
			break;
    default:
      break;
  }

	if (s) {
		return UI_Text_Width(s, scale, 0);
	}
	return 0;
}

static void UI_DrawBotName(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
	// 0 - Random Bot
	// 1..NumCharacters - Bot
	int value = uiInfo.botIndex;
	int game = trap_Cvar_VariableValue("g_gametype");
	const char *text = "";
	if (value == 0) {
		text = "Random Bot";
	} else if (game >= GT_TEAM) {
		text = uiInfo.characterList[value-1].name;
	} else {
		text = UI_GetBotNameByNumber(value-1);
	}
  UI_Text_Paint(rect->x, rect->y, scale, color, text, 0, 0, textStyle);
}

static void UI_DrawBotSkill(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
	if (uiInfo.skillIndex >= 0 && uiInfo.skillIndex < numSkillLevels) {
	  UI_Text_Paint(rect->x, rect->y, scale, color, skillLevels[uiInfo.skillIndex], 0, 0, textStyle);
	}
}

static void UI_DrawRedBlue(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
  UI_Text_Paint(rect->x, rect->y, scale, color, (uiInfo.redBlue == 0) ? "Red" : "Blue", 0, 0, textStyle);
}

static void UI_DrawCrosshair(rectDef_t *rect, float scale, vec4_t color) {
	if (!uiInfo.currentCrosshair) {
		return;
	}
	trap_R_SetColor( color );
	CG_DrawPic( rect->x, rect->y - rect->h, rect->w, rect->h, uiInfo.uiDC.Assets.crosshairShader[uiInfo.currentCrosshair]);
 	trap_R_SetColor( NULL );
}

/*
===============
UI_BuildPlayerList
===============
*/
static void UI_BuildPlayerList( void ) {
	int		n, count, team, team2, playerTeamNumber;
	char	info[MAX_INFO_STRING];

	uiInfo.teamLeader = qfalse;
	uiInfo.playerCount = 0;
	uiInfo.myTeamCount = 0;

	if ( !cg.snap || cg.localPlayers[0].playerNum == -1 ) {
		return;
	}

	uiInfo.playerNumber = cg.snap->pss[0].playerNum;

	trap_GetConfigString( CS_PLAYERS + uiInfo.playerNumber, info, MAX_INFO_STRING );
	uiInfo.teamLeader = atoi(Info_ValueForKey(info, "tl"));
	team = atoi(Info_ValueForKey(info, "t"));
	trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) );
	count = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
	playerTeamNumber = 0;
	for( n = 0; n < count; n++ ) {
		trap_GetConfigString( CS_PLAYERS + n, info, MAX_INFO_STRING );

		if (info[0]) {
			Q_strncpyz( uiInfo.playerNames[uiInfo.playerCount], Info_ValueForKey( info, "n" ), MAX_NAME_LENGTH );
			Q_CleanStr( uiInfo.playerNames[uiInfo.playerCount] );
			uiInfo.playerNums[uiInfo.playerCount] = n;
			uiInfo.playerCount++;
			team2 = atoi(Info_ValueForKey(info, "t"));
			if (team2 == team) {
				Q_strncpyz( uiInfo.teamPlayerNames[uiInfo.myTeamCount], Info_ValueForKey( info, "n" ), MAX_NAME_LENGTH );
				Q_CleanStr( uiInfo.teamPlayerNames[uiInfo.myTeamCount] );
				uiInfo.teamPlayerNums[uiInfo.myTeamCount] = n;
				if (uiInfo.playerNumber == n) {
					playerTeamNumber = uiInfo.myTeamCount;
				}
				uiInfo.myTeamCount++;
			}
		}
	}

	if (!uiInfo.teamLeader) {
		trap_Cvar_SetValue("cg_selectedPlayer", playerTeamNumber);
	}

	n = trap_Cvar_VariableValue("cg_selectedPlayer");
	if (n < 0 || n > uiInfo.myTeamCount) {
		n = 0;
	}
	if (n < uiInfo.myTeamCount) {
		trap_Cvar_Set("cg_selectedPlayerName", uiInfo.teamPlayerNames[n]);
	}
}


static void UI_DrawSelectedPlayer(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
	if (uiInfo.uiDC.realTime > uiInfo.playerRefresh) {
		uiInfo.playerRefresh = uiInfo.uiDC.realTime + 3000;
		UI_BuildPlayerList();
	}
  UI_Text_Paint(rect->x, rect->y, scale, color, (uiInfo.teamLeader) ? CG_Cvar_VariableString("cg_selectedPlayerName") : CG_Cvar_VariableString("name") , 0, 0, textStyle);
}

static void UI_DrawServerRefreshDate(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
	if (uiInfo.serverStatus.refreshActive) {
		vec4_t lowLight, newColor;
		lowLight[0] = 0.8 * color[0]; 
		lowLight[1] = 0.8 * color[1]; 
		lowLight[2] = 0.8 * color[2]; 
		lowLight[3] = 0.8 * color[3]; 
		LerpColor(color,lowLight,newColor,0.5+0.5*sin(uiInfo.uiDC.realTime / PULSE_DIVISOR));
	  UI_Text_Paint(rect->x, rect->y, scale, newColor, va("Getting info for %d servers (ESC to cancel)", trap_LAN_GetServerCount(UI_SourceForLAN())), 0, 0, textStyle);
	} else {
		char buff[64];
		Q_strncpyz(buff, CG_Cvar_VariableString(va("ui_lastServerRefresh_%i", ui_netSource.integer)), 64);
	  UI_Text_Paint(rect->x, rect->y, scale, color, va("Refresh Time: %s", buff), 0, 0, textStyle);
	}
}

#define MOTD_PIXELS_PER_SECOND 30.0f

static void UI_DrawServerMOTD(rectDef_t *rect, float scale, vec4_t color) {
	char  *text = uiInfo.serverStatus.motd;
	float textWidth = MAX(rect->w, UI_Text_Width( text, scale, 0 ));
	int now = uiInfo.uiDC.realTime;
	int delta = now - uiInfo.serverStatus.motdTime;

	CG_SetClipRegion( rect->x, rect->y, rect->w, rect->h );

	UI_Text_PaintInMotion( rect->x - uiInfo.serverStatus.motdOffset, rect->y + rect->h - 3, scale, color, text, 0, 0, 0 );
	UI_Text_PaintInMotion( rect->x + textWidth - uiInfo.serverStatus.motdOffset, rect->y + rect->h - 3, scale, color, text, 0, 0, 0 );

	CG_ClearClipRegion( );

	uiInfo.serverStatus.motdOffset += ( delta / 1000.0f ) * MOTD_PIXELS_PER_SECOND;

	while( uiInfo.serverStatus.motdOffset > textWidth )
		uiInfo.serverStatus.motdOffset -= textWidth;

	uiInfo.serverStatus.motdTime = now;
}

static void UI_DrawKeyBindStatus(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
//	int ofs = 0; TTimo: unused
	if (Display_KeyBindPending()) {
		UI_Text_Paint(rect->x, rect->y, scale, color, "Waiting for new key... Press ESCAPE to cancel", 0, 0, textStyle);
	} else {
		UI_Text_Paint(rect->x, rect->y, scale, color, "Press ENTER or CLICK to change, Press BACKSPACE to clear", 0, 0, textStyle);
	}
}

static void UI_DrawGLInfo(rectDef_t *rect, float scale, vec4_t color, int textStyle) {
	char * eptr;
	char buff[1024];
	const char *lines[64];
	int y, numLines, i;

	UI_Text_Paint(rect->x + 2, rect->y, scale, color, va("VENDOR: %s", cgs.glconfig.vendor_string), 0, 30, textStyle);
	UI_Text_Paint(rect->x + 2, rect->y + 15, scale, color, va("VERSION: %s: %s", cgs.glconfig.version_string,cgs.glconfig.renderer_string), 0, 30, textStyle);
	UI_Text_Paint(rect->x + 2, rect->y + 30, scale, color, va ("PIXELFORMAT: color(%d-bits) Z(%d-bits) stencil(%d-bits)", cgs.glconfig.colorBits, cgs.glconfig.depthBits, cgs.glconfig.stencilBits), 0, 30, textStyle);

	// build null terminated extension strings
  // TTimo: https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=399
  // in Team Arena this was not directly crashing, but displaying a nasty broken shader right in the middle
  // brought down the string size to 1024, there's not much that can be shown on the screen anyway
	Q_strncpyz(buff, cgs.glconfig.extensions_string, 1024);
	eptr = buff;
	y = rect->y + 45;
	numLines = 0;
	while ( y < rect->y + rect->h && *eptr )
	{
		while ( *eptr && *eptr == ' ' )
			*eptr++ = '\0';

		// track start of valid string
		if (*eptr && *eptr != ' ') {
			lines[numLines++] = eptr;
		}

		while ( *eptr && *eptr != ' ' )
			eptr++;
	}

	i = 0;
	while (i < numLines) {
		UI_Text_Paint(rect->x + 2, y, scale, color, lines[i++], 0, 20, textStyle);
		if (i < numLines) {
			UI_Text_Paint(rect->x + rect->w / 2, y, scale, color, lines[i++], 0, 20, textStyle);
		}
		y += 10;
		if (y > rect->y + rect->h - 11) {
			break;
		}
	}


}

// FIXME: table drive
//
static void UI_OwnerDraw(float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, float scale, vec4_t color, qhandle_t shader, int textStyle) {
	rectDef_t rect;

  rect.x = x + text_x;
  rect.y = y + text_y;
  rect.w = w;
  rect.h = h;

  switch (ownerDraw) {
    case UI_HANDICAP:
      UI_DrawHandicap(&rect, scale, color, textStyle);
      break;
    case UI_EFFECTS:
      UI_DrawEffects(&rect, scale, color);
      break;
    case UI_PLAYERMODEL:
      UI_DrawPlayerModel(&rect);
      break;
    case UI_CLANNAME:
      UI_DrawClanName(&rect, scale, color, textStyle);
      break;
    case UI_CLANLOGO:
      UI_DrawClanLogo(&rect, scale, color);
      break;
    case UI_CLANCINEMATIC:
      UI_DrawClanCinematic(&rect, scale, color);
      break;
    case UI_PREVIEWCINEMATIC:
      UI_DrawPreviewCinematic(&rect, scale, color);
      break;
    case UI_GAMETYPE:
      UI_DrawGameType(&rect, scale, color, textStyle);
      break;
    case UI_NETGAMETYPE:
      UI_DrawNetGameType(&rect, scale, color, textStyle);
      break;
    case UI_JOINGAMETYPE:
	  UI_DrawJoinGameType(&rect, scale, color, textStyle);
	  break;
    case UI_MAPPREVIEW:
      UI_DrawMapPreview(&rect, scale, color, qtrue);
      break;
    case UI_MAP_TIMETOBEAT:
      UI_DrawMapTimeToBeat(&rect, scale, color, textStyle);
      break;
    case UI_MAPCINEMATIC:
      UI_DrawMapCinematic(&rect, scale, color, qfalse);
      break;
    case UI_STARTMAPCINEMATIC:
      UI_DrawMapCinematic(&rect, scale, color, qtrue);
      break;
    case UI_SKILL:
      UI_DrawSkill(&rect, scale, color, textStyle);
      break;
    case UI_BLUETEAMNAME:
      UI_DrawTeamName(&rect, scale, color, qtrue, textStyle);
      break;
    case UI_REDTEAMNAME:
      UI_DrawTeamName(&rect, scale, color, qfalse, textStyle);
      break;
    case UI_BLUETEAM1:
		case UI_BLUETEAM2:
		case UI_BLUETEAM3:
		case UI_BLUETEAM4:
		case UI_BLUETEAM5:
      UI_DrawTeamMember(&rect, scale, color, qtrue, ownerDraw - UI_BLUETEAM1 + 1, textStyle);
      break;
    case UI_REDTEAM1:
		case UI_REDTEAM2:
		case UI_REDTEAM3:
		case UI_REDTEAM4:
		case UI_REDTEAM5:
      UI_DrawTeamMember(&rect, scale, color, qfalse, ownerDraw - UI_REDTEAM1 + 1, textStyle);
      break;
		case UI_NETSOURCE:
      UI_DrawNetSource(&rect, scale, color, textStyle);
			break;
    case UI_NETMAPPREVIEW:
      UI_DrawNetMapPreview(&rect, scale, color);
      break;
    case UI_NETMAPCINEMATIC:
      UI_DrawNetMapCinematic(&rect, scale, color);
      break;
		case UI_NETFILTER:
      UI_DrawNetFilter(&rect, scale, color, textStyle);
			break;
		case UI_TIER:
			UI_DrawTier(&rect, scale, color, textStyle);
			break;
		case UI_OPPONENTMODEL:
			UI_DrawOpponent(&rect);
			break;
		case UI_TIERMAP1:
			UI_DrawTierMap(&rect, 0);
			break;
		case UI_TIERMAP2:
			UI_DrawTierMap(&rect, 1);
			break;
		case UI_TIERMAP3:
			UI_DrawTierMap(&rect, 2);
			break;
		case UI_PLAYERLOGO:
			UI_DrawPlayerLogo(&rect, color);
			break;
		case UI_PLAYERLOGO_METAL:
			UI_DrawPlayerLogoMetal(&rect, color);
			break;
		case UI_PLAYERLOGO_NAME:
			UI_DrawPlayerLogoName(&rect, color);
			break;
		case UI_OPPONENTLOGO:
			UI_DrawOpponentLogo(&rect, color);
			break;
		case UI_OPPONENTLOGO_METAL:
			UI_DrawOpponentLogoMetal(&rect, color);
			break;
		case UI_OPPONENTLOGO_NAME:
			UI_DrawOpponentLogoName(&rect, color);
			break;
		case UI_TIER_MAPNAME:
			UI_DrawTierMapName(&rect, scale, color, textStyle);
			break;
		case UI_TIER_GAMETYPE:
			UI_DrawTierGameType(&rect, scale, color, textStyle);
			break;
		case UI_ALLMAPS_SELECTION:
			UI_DrawAllMapsSelection(&rect, scale, color, textStyle, qtrue);
			break;
		case UI_MAPS_SELECTION:
			UI_DrawAllMapsSelection(&rect, scale, color, textStyle, qfalse);
			break;
		case UI_OPPONENT_NAME:
			UI_DrawOpponentName(&rect, scale, color, textStyle);
			break;
		case UI_BOTNAME:
			UI_DrawBotName(&rect, scale, color, textStyle);
			break;
		case UI_BOTSKILL:
			UI_DrawBotSkill(&rect, scale, color, textStyle);
			break;
		case UI_REDBLUE:
			UI_DrawRedBlue(&rect, scale, color, textStyle);
			break;
		case UI_CROSSHAIR:
			UI_DrawCrosshair(&rect, scale, color);
			break;
		case UI_SELECTEDPLAYER:
			UI_DrawSelectedPlayer(&rect, scale, color, textStyle);
			break;
		case UI_SERVERREFRESHDATE:
			UI_DrawServerRefreshDate(&rect, scale, color, textStyle);
			break;
		case UI_SERVERMOTD:
			UI_DrawServerMOTD(&rect, scale, color);
			break;
		case UI_GLINFO:
			UI_DrawGLInfo(&rect,scale, color, textStyle);
			break;
		case UI_KEYBINDSTATUS:
			UI_DrawKeyBindStatus(&rect,scale, color, textStyle);
			break;
    default:
      break;
  }

}

static qboolean UI_OwnerDrawVisible(int flags) {
	qboolean vis = qtrue;

	while (flags) {

		if (flags & UI_SHOW_FFA) {
			if (trap_Cvar_VariableValue("g_gametype") != GT_FFA) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_FFA;
		}

		if (flags & UI_SHOW_NOTFFA) {
			if (trap_Cvar_VariableValue("g_gametype") == GT_FFA) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NOTFFA;
		}

		if (flags & UI_SHOW_LEADER) {
			// these need to show when this player can give orders to a player or a group
			if (!uiInfo.teamLeader) {
				vis = qfalse;
			} else {
				// if showing yourself
				if (ui_selectedPlayer.integer < uiInfo.myTeamCount && uiInfo.teamPlayerNums[ui_selectedPlayer.integer] == uiInfo.playerNumber) { 
					vis = qfalse;
				}
			}
			flags &= ~UI_SHOW_LEADER;
		} 
		if (flags & UI_SHOW_NOTLEADER) {
			// these need to show when this player is assigning their own status or they are NOT the leader
			if (uiInfo.teamLeader) {
				// if not showing yourself
				if (!(ui_selectedPlayer.integer < uiInfo.myTeamCount && uiInfo.teamPlayerNums[ui_selectedPlayer.integer] == uiInfo.playerNumber)) { 
					vis = qfalse;
				}
				// these need to show when this player can give orders to a player or a group
			}
			flags &= ~UI_SHOW_NOTLEADER;
		} 
		if (flags & UI_SHOW_FAVORITESERVERS) {
			// this assumes you only put this type of display flag on something showing in the proper context
			if (ui_netSource.integer != UIAS_FAVORITES) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_FAVORITESERVERS;
		} 
		if (flags & UI_SHOW_NOTFAVORITESERVERS) {
			// this assumes you only put this type of display flag on something showing in the proper context
			if (ui_netSource.integer == UIAS_FAVORITES) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NOTFAVORITESERVERS;
		} 
		if (flags & UI_SHOW_ANYTEAMGAME) {
			if (uiInfo.gameTypes[ui_gameType.integer].gtEnum <= GT_TEAM ) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYTEAMGAME;
		} 
		if (flags & UI_SHOW_ANYNONTEAMGAME) {
			if (uiInfo.gameTypes[ui_gameType.integer].gtEnum > GT_TEAM ) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYNONTEAMGAME;
		} 
		if (flags & UI_SHOW_NETANYTEAMGAME) {
			if (uiInfo.gameTypes[ui_netGameType.integer].gtEnum <= GT_TEAM ) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYTEAMGAME;
		} 
		if (flags & UI_SHOW_NETANYNONTEAMGAME) {
			if (uiInfo.gameTypes[ui_netGameType.integer].gtEnum > GT_TEAM ) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYNONTEAMGAME;
		} 
		if (flags & UI_SHOW_NEWHIGHSCORE) {
			if (uiInfo.newHighScoreTime < uiInfo.uiDC.realTime) {
				vis = qfalse;
			} else {
				if (uiInfo.soundHighScore) {
					trap_S_StartLocalSound(uiInfo.newHighScoreSound, CHAN_ANNOUNCER);
					uiInfo.soundHighScore = qfalse;
				}
			}
			flags &= ~UI_SHOW_NEWHIGHSCORE;
		} 
		if (flags & UI_SHOW_NEWBESTTIME) {
			if (uiInfo.newBestTime < uiInfo.uiDC.realTime) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NEWBESTTIME;
		} 
		if (flags & UI_SHOW_DEMOAVAILABLE) {
			if (!uiInfo.demoAvailable) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_DEMOAVAILABLE;
		} else {
			flags = 0;
		}
	}
  return vis;
}

static qboolean UI_Handicap_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		int h;

		h = Com_Clamp( 5, 100, trap_Cvar_VariableValue("handicap") );
		h += 5 * select;

		if (h > 100) {
			h = 5;
		} else if (h < 5) {
			h = 100;
		}

		trap_Cvar_SetValue( "handicap", h );
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_Effects_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		uiInfo.effectsColor += select;

		if( uiInfo.effectsColor > NUM_COLOR_EFFECTS-1 ) {
			uiInfo.effectsColor = 0;
		} else if (uiInfo.effectsColor < 0) {
			uiInfo.effectsColor = NUM_COLOR_EFFECTS-1;
		}

		trap_Cvar_SetValue( "color1", uitogamecode[uiInfo.effectsColor] );
		updateModelColor = qtrue;
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_ClanName_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		int i;

		i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));

		if (uiInfo.teamList[i].cinematic >= 0) {
			trap_CIN_StopCinematic(uiInfo.teamList[i].cinematic);
			uiInfo.teamList[i].cinematic = -1;
		}

		i += select;

		if (i >= uiInfo.teamCount) {
			i = 0;
		} else if (i < 0) {
			i = uiInfo.teamCount - 1;
		}

		trap_Cvar_Set( "ui_teamName", uiInfo.teamList[i].teamName);
		UI_HeadCountByTeam();
		UI_FeederSelection(FEEDER_HEADS, 0);
		updateModel = qtrue;
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_GameType_HandleKey(int flags, float *special, int key, qboolean resetMap) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		int oldCount = UI_MapCountByGameType(qtrue);

		// hard coded mess here
		if (select < 0) {
			ui_gameType.integer--;
			if (ui_gameType.integer == 2) {
				ui_gameType.integer = 1;
			} else if (ui_gameType.integer < 2) {
				ui_gameType.integer = uiInfo.numGameTypes - 1;
			}
		} else {
			ui_gameType.integer++;
			if (ui_gameType.integer >= uiInfo.numGameTypes) {
				ui_gameType.integer = 1;
			} else if (ui_gameType.integer == 2) {
				ui_gameType.integer = 3;
			}
		}
    
		if (uiInfo.gameTypes[ui_gameType.integer].gtEnum < GT_TEAM) {
			trap_Cvar_SetValue( "ui_Q3Model", 1 );
		} else {
			trap_Cvar_SetValue( "ui_Q3Model", 0 );
		}

		trap_Cvar_SetValue("ui_gameType", ui_gameType.integer);
		UI_SetCapFragLimits(qtrue);
		UI_LoadBestScores(uiInfo.mapList[ui_currentMap.integer].mapLoadName, uiInfo.gameTypes[ui_gameType.integer].gtEnum);
		if (resetMap && oldCount != UI_MapCountByGameType(qtrue)) {
			trap_Cvar_SetValue( "ui_currentMap", 0);
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, 0, NULL);
		}
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_NetGameType_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		ui_netGameType.integer += select;

		if (ui_netGameType.integer < 0) {
			ui_netGameType.integer = uiInfo.numGameTypes - 1;
		} else if (ui_netGameType.integer >= uiInfo.numGameTypes) {
			ui_netGameType.integer = 0;
		}

		trap_Cvar_SetValue( "ui_netGameType", ui_netGameType.integer);
		trap_Cvar_SetValue( "ui_actualnetGameType", uiInfo.gameTypes[ui_netGameType.integer].gtEnum);
		trap_Cvar_SetValue( "ui_currentNetMap", 0);
		UI_MapCountByGameType(qfalse);
		Menu_SetFeederSelection(NULL, FEEDER_ALLMAPS, 0, NULL);
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_JoinGameType_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		ui_joinGameType.integer += select;

		if (ui_joinGameType.integer < 0) {
			ui_joinGameType.integer = uiInfo.numJoinGameTypes - 1;
		} else if (ui_joinGameType.integer >= uiInfo.numJoinGameTypes) {
			ui_joinGameType.integer = 0;
		}

		trap_Cvar_SetValue( "ui_joinGameType", ui_joinGameType.integer);
		UI_BuildServerDisplayList(qtrue);
		return qtrue;
	}
	return qfalse;
}



static qboolean UI_Skill_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		int i = trap_Cvar_VariableValue( "g_spSkill" );

		i += select;

		if (i < 1) {
			i = numSkillLevels;
		} else if (i > numSkillLevels) {
			i = 1;
		}

		trap_Cvar_SetValue("g_spSkill", i);
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_TeamName_HandleKey(int flags, float *special, int key, qboolean blue) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		int i;

		i = UI_TeamIndexFromName(CG_Cvar_VariableString((blue) ? "ui_blueTeam" : "ui_redTeam"));
		i += select;

		if (i >= uiInfo.teamCount) {
			i = 0;
		} else if (i < 0) {
			i = uiInfo.teamCount - 1;
		}

		trap_Cvar_Set( (blue) ? "ui_blueTeam" : "ui_redTeam", uiInfo.teamList[i].teamName);
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_TeamMember_HandleKey(int flags, float *special, int key, qboolean blue, int num) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		// 0 - None
		// 1 - Human
		// 2 - Random Bot
		// 3..NumCharacters - Bot
		char *cvar = va(blue ? "ui_blueteam%i" : "ui_redteam%i", num);
		int value = trap_Cvar_VariableValue(cvar);

		value += select;

		if (ui_actualNetGameType.integer >= GT_TEAM) {
			if (value >= uiInfo.characterCount + 3) {
				value = 0;
			} else if (value < 0) {
				value = uiInfo.characterCount + 3 - 1;
			}
		} else {
			if (value >= UI_GetNumBots() + 3) {
				value = 0;
			} else if (value < 0) {
				value = UI_GetNumBots() + 3 - 1;
			}
		}

		trap_Cvar_SetValue(cvar, value);
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_NetSource_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		ui_netSource.integer += select;

		if(ui_netSource.integer >= UIAS_GLOBAL1 && ui_netSource.integer <= UIAS_GLOBAL5)
		{
			char masterstr[2], cvarname[sizeof("sv_master1")];
		
			while(ui_netSource.integer >= UIAS_GLOBAL1 && ui_netSource.integer <= UIAS_GLOBAL5)
			{
				if (ui_browserSeparateMasters.integer) {
					Com_sprintf(cvarname, sizeof(cvarname), "sv_master%d", ui_netSource.integer - UIAS_GLOBAL0);
					trap_Cvar_VariableStringBuffer(cvarname, masterstr, sizeof(masterstr));
					if(*masterstr)
						break;
				}

				ui_netSource.integer += select;
			}
		}

		if (ui_netSource.integer >= numNetSources) {
			ui_netSource.integer = 0;
		} else if (ui_netSource.integer < 0) {
			ui_netSource.integer = numNetSources - 1;
		}

		UI_BuildServerDisplayList(qtrue);
		UI_StartServerRefresh(qtrue, qfalse);
		trap_Cvar_SetValue( "ui_netSource", ui_netSource.integer);
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_NetFilter_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		ui_serverFilterType.integer += select;

		if (ui_serverFilterType.integer > uiInfo.modCount) {
			ui_serverFilterType.integer = 0;
		} else if (ui_serverFilterType.integer < 0) {
			ui_serverFilterType.integer = uiInfo.modCount;
		}
		UI_BuildServerDisplayList(qtrue);
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_OpponentName_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		if (select < 0) {
			UI_PriorOpponent();
		} else {
			UI_NextOpponent();
		}
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_BotName_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		// 0 - Random Bot
		// 1..NumCharacters - Bot
		int game = trap_Cvar_VariableValue("g_gametype");
		int value = uiInfo.botIndex;

		value += select;

		if (game >= GT_TEAM) {
			if (value >= uiInfo.characterCount + 1) {
				value = 0;
			} else if (value < 0) {
				value = uiInfo.characterCount + 1 - 1;
			}
		} else {
			if (value >= UI_GetNumBots() + 1) {
				value = 0;
			} else if (value < 0) {
				value = UI_GetNumBots() + 1 - 1;
			}
		}
		uiInfo.botIndex = value;
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_BotSkill_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		uiInfo.skillIndex += select;

		if (uiInfo.skillIndex >= numSkillLevels) {
			uiInfo.skillIndex = 0;
		} else if (uiInfo.skillIndex < 0) {
			uiInfo.skillIndex = numSkillLevels-1;
		}
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_RedBlue_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		uiInfo.redBlue ^= 1;
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_Crosshair_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		uiInfo.currentCrosshair += select;

		if (uiInfo.currentCrosshair >= NUM_CROSSHAIRS) {
			uiInfo.currentCrosshair = 0;
		} else if (uiInfo.currentCrosshair < 0) {
			uiInfo.currentCrosshair = NUM_CROSSHAIRS - 1;
		}
		trap_Cvar_SetValue("cg_drawCrosshair", uiInfo.currentCrosshair);
		return qtrue;
	}
	return qfalse;
}



static qboolean UI_SelectedPlayer_HandleKey(int flags, float *special, int key) {
	int select = UI_SelectForKey(key);
	if (select != 0) {
		int selected;

		UI_BuildPlayerList();
		if (!uiInfo.teamLeader) {
			return qfalse;
		}
		selected = trap_Cvar_VariableValue("cg_selectedPlayer");

		selected += select;

		if (selected > uiInfo.myTeamCount) {
			selected = 0;
		} else if (selected < 0) {
			selected = uiInfo.myTeamCount;
		}

		if (selected == uiInfo.myTeamCount) {
			trap_Cvar_Set( "cg_selectedPlayerName", "Everyone");
		} else {
			trap_Cvar_Set( "cg_selectedPlayerName", uiInfo.teamPlayerNames[selected]);
		}
		trap_Cvar_SetValue( "cg_selectedPlayer", selected);
	}
	return qfalse;
}


static qboolean UI_OwnerDrawHandleKey(int ownerDraw, int flags, float *special, int key) {
  switch (ownerDraw) {
    case UI_HANDICAP:
      return UI_Handicap_HandleKey(flags, special, key);
      break;
    case UI_EFFECTS:
      return UI_Effects_HandleKey(flags, special, key);
      break;
    case UI_CLANNAME:
      return UI_ClanName_HandleKey(flags, special, key);
      break;
    case UI_GAMETYPE:
      return UI_GameType_HandleKey(flags, special, key, qtrue);
      break;
    case UI_NETGAMETYPE:
      return UI_NetGameType_HandleKey(flags, special, key);
      break;
    case UI_JOINGAMETYPE:
      return UI_JoinGameType_HandleKey(flags, special, key);
      break;
    case UI_SKILL:
      return UI_Skill_HandleKey(flags, special, key);
      break;
    case UI_BLUETEAMNAME:
      return UI_TeamName_HandleKey(flags, special, key, qtrue);
      break;
    case UI_REDTEAMNAME:
      return UI_TeamName_HandleKey(flags, special, key, qfalse);
      break;
    case UI_BLUETEAM1:
		case UI_BLUETEAM2:
		case UI_BLUETEAM3:
		case UI_BLUETEAM4:
		case UI_BLUETEAM5:
      UI_TeamMember_HandleKey(flags, special, key, qtrue, ownerDraw - UI_BLUETEAM1 + 1);
      break;
    case UI_REDTEAM1:
		case UI_REDTEAM2:
		case UI_REDTEAM3:
		case UI_REDTEAM4:
		case UI_REDTEAM5:
      UI_TeamMember_HandleKey(flags, special, key, qfalse, ownerDraw - UI_REDTEAM1 + 1);
      break;
		case UI_NETSOURCE:
      UI_NetSource_HandleKey(flags, special, key);
			break;
		case UI_NETFILTER:
      UI_NetFilter_HandleKey(flags, special, key);
			break;
		case UI_OPPONENT_NAME:
			UI_OpponentName_HandleKey(flags, special, key);
			break;
		case UI_BOTNAME:
			return UI_BotName_HandleKey(flags, special, key);
			break;
		case UI_BOTSKILL:
			return UI_BotSkill_HandleKey(flags, special, key);
			break;
		case UI_REDBLUE:
			UI_RedBlue_HandleKey(flags, special, key);
			break;
		case UI_CROSSHAIR:
			UI_Crosshair_HandleKey(flags, special, key);
			break;
		case UI_SELECTEDPLAYER:
			UI_SelectedPlayer_HandleKey(flags, special, key);
			break;
    default:
      break;
  }

  return qfalse;
}


static float UI_GetValue(int ownerDraw) {
  return 0;
}

/*
=================
UI_ServersQsortCompare
=================
*/
static int QDECL UI_ServersQsortCompare( const void *arg1, const void *arg2 ) {
	return trap_LAN_CompareServers( UI_SourceForLAN(), uiInfo.serverStatus.sortKey, uiInfo.serverStatus.sortDir, *(int*)arg1, *(int*)arg2);
}


/*
=================
UI_ServersSort
=================
*/
void UI_ServersSort(int column, qboolean force) {
	int sortKey;

	switch ( column ) {
		case COLUMN_HOST:
			sortKey = SORT_HOST;
			break;
		case COLUMN_MAP:
			sortKey = SORT_MAP;
			break;
		case COLUMN_CLIENTS:
			if ( ui_browserShowBots.integer == 0 ) {
				sortKey = SORT_HUMANS;
			} else {
				sortKey = SORT_CLIENTS;
			}
			break;
		case COLUMN_GAME:
			sortKey = SORT_GAMETYPE;
			break;
		case COLUMN_PING:
			sortKey = SORT_PING;
			break;
		default:
			return;
	}

	if ( !force ) {
		if ( uiInfo.serverStatus.sortKey == sortKey ) {
			return;
		}
	}

	uiInfo.serverStatus.sortKey = sortKey;
	qsort( &uiInfo.serverStatus.displayServers[0], uiInfo.serverStatus.numDisplayServers, sizeof(int), UI_ServersQsortCompare);

	// update displayed levelshot
	UI_FeederSelection( FEEDER_SERVERS, uiInfo.serverStatus.currentServer );
}

/*
static void UI_StartSinglePlayer(void) {
	int i,j, k, skill;
	char buff[1024];
	i = trap_Cvar_VariableValue( "ui_currentTier" );
  if (i < 0 || i >= tierCount) {
    i = 0;
  }
	j = trap_Cvar_VariableValue("ui_currentMap");
	if (j < 0 || j >= MAPS_PER_TIER) {
		j = 0;
	}

 	trap_Cvar_SetValue( "singleplayer", 1 );
 	trap_Cvar_SetValue( "g_gametype", Com_Clamp( 0, GT_MAX_GAME_TYPE-1, tierList[i].gameTypes[j] ) );
	trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait ; wait ; map %s\n", tierList[i].maps[j] ) );
	skill = trap_Cvar_VariableValue( "g_spSkill" );

	if (j == MAPS_PER_TIER-1) {
		k = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_opponentName"));
		Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %i %s 250 %s\n", UI_AIFromName(teamList[k].teamMembers[0]), skill, "", teamList[k].teamMembers[0]);
	} else {
		k = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_opponentName"));
		for (i = 0; i < PLAYERS_PER_TEAM; i++) {
			Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %i %s 250 %s\n", UI_AIFromName(teamList[k].teamMembers[i]), skill, "Blue", teamList[k].teamMembers[i]);
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
		}

		k = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));
		for (i = 1; i < PLAYERS_PER_TEAM; i++) {
			Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %i %s 250 %s\n", UI_AIFromName(teamList[k].teamMembers[i]), skill, "Red", teamList[k].teamMembers[i]);
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
		}
		trap_Cmd_ExecuteText( EXEC_APPEND, "wait 5; team Red\n" );
	}
	

}
*/

/*
===============
UI_LoadMods
===============
*/
static void UI_LoadMods( void ) {
	int		numdirs;
	char	dirlist[2048];
	char	*dirptr;
  char  *descptr;
	int		i;
	int		dirlen;

	uiInfo.modCount = 0;
	numdirs = trap_FS_GetFileList( "$modlist", "", dirlist, sizeof(dirlist) );
	dirptr  = dirlist;
	for( i = 0; i < numdirs; i++ ) {
		dirlen = strlen( dirptr ) + 1;
    descptr = dirptr + dirlen;
		uiInfo.modList[uiInfo.modCount].modName = String_Alloc(dirptr);
		uiInfo.modList[uiInfo.modCount].modDescr = String_Alloc(descptr);
    dirptr += dirlen + strlen(descptr) + 1;
		uiInfo.modCount++;
		if (uiInfo.modCount >= MAX_MODS) {
			break;
		}
	}

}


/*
===============
UI_LoadTeams
===============
*/
static void UI_LoadTeams( void ) {
	char	teamList[4096];
	char	*teamName;
	int		i, len, count;

	count = trap_FS_GetFileList( "", "team", teamList, 4096 );

	if (count) {
		teamName = teamList;
		for ( i = 0; i < count; i++ ) {
			len = strlen( teamName );
			UI_ParseTeamInfo(teamName);
			teamName += len + 1;
		}
	}

}


/*
===============
UI_LoadMovies
===============
*/
static void UI_LoadMovies( void ) {
	char	movielist[4096];
	char	*moviename;
	int		i, len;

	uiInfo.movieCount = trap_FS_GetFileList( "video", "$videos", movielist, 4096 );

	if (uiInfo.movieCount) {
		if (uiInfo.movieCount > MAX_MOVIES) {
			uiInfo.movieCount = MAX_MOVIES;
		}
		moviename = movielist;
		for ( i = 0; i < uiInfo.movieCount; i++ ) {
			len = strlen( moviename );
			COM_StripExtension( moviename, moviename, len );
			uiInfo.movieList[i] = String_Alloc(moviename);
			moviename += len + 1;
		}
	}

}

#define NAMEBUFSIZE (MAX_DEMOS * 32)

/*
===============
UI_LoadDemos
===============
*/
static void UI_LoadDemos( void ) {
	char	demolist[NAMEBUFSIZE];
	char	*demoname;
	int	i, len;
	int		protocol, length, runTime;
	char	startTime[20];
	char	endTime[20];
	qboolean	valid;

	uiInfo.demoCount = trap_FS_GetFileList("demos", "$demos", demolist, ARRAY_LEN(demolist));
	
	demoname = demolist;

	if(uiInfo.demoCount > MAX_DEMOS)
		uiInfo.demoCount = MAX_DEMOS;

	for(i = 0; i < uiInfo.demoCount; i++)
	{
		// information to build into new UI...
		valid = trap_GetDemoFileInfo( demoname, &protocol, &length, startTime, endTime, &runTime );
		Com_Printf("Demo: %s%s, protocol %d, %dKB, %d seconds, %s -> %s\n", demoname, valid ? "" : " (unsupported)", protocol, length/1024, runTime/1000, startTime, endTime );

		len = strlen(demoname);
		uiInfo.demoList[i] = String_Alloc(demoname);
		demoname += len + 1;
	}
}


static qboolean UI_SetNextMap(int actual, int index) {
	int i;
	for (i = actual + 1; i < uiInfo.mapCount; i++) {
		if (uiInfo.mapList[i].active) {
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, index + 1, "skirmish");
			return qtrue;
		}
	}
	return qfalse;
}


static void UI_StartSkirmish(qboolean next) {
	int i, k, g, delay, temp;
	float skill;
	char buff[MAX_STRING_CHARS];

	if (next) {
		int actual;
		int index = trap_Cvar_VariableValue("ui_mapIndex");
	 	UI_MapCountByGameType(qtrue);
		UI_SelectedMap(index, &actual);
		if (UI_SetNextMap(actual, index)) {
		} else {
			UI_GameType_HandleKey(0, NULL, K_MOUSE1, qfalse);
			UI_MapCountByGameType(qtrue);
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, 0, "skirmish");
		}
	}

	g = uiInfo.gameTypes[ui_gameType.integer].gtEnum;
	trap_Cvar_SetValue( "g_gametype", g );
	trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait ; wait ; map %s\n", uiInfo.mapList[ui_currentMap.integer].mapLoadName) );
	skill = trap_Cvar_VariableValue( "g_spSkill" );
	trap_Cvar_Set("ui_scoreMap", uiInfo.mapList[ui_currentMap.integer].mapName);

	k = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_opponentName"));

	trap_Cvar_SetValue( "ui_singlePlayerActive", 1 );

	// set up sp overrides, will be replaced on postgame
	temp = trap_Cvar_VariableValue( "capturelimit" );
	trap_Cvar_SetValue("ui_saveCaptureLimit", temp);
	temp = trap_Cvar_VariableValue( "fraglimit" );
	trap_Cvar_SetValue("ui_saveFragLimit", temp);

	UI_SetCapFragLimits(qfalse);

	temp = trap_Cvar_VariableValue( "cg_drawTimer" );
	trap_Cvar_SetValue("ui_drawTimer", temp);
	temp = trap_Cvar_VariableValue( "g_doWarmup" );
	trap_Cvar_SetValue("ui_doWarmup", temp);
	temp = trap_Cvar_VariableValue( "g_friendlyFire" );
	trap_Cvar_SetValue("ui_friendlyFire", temp);
	temp = trap_Cvar_VariableValue( "sv_maxClients" );
	trap_Cvar_SetValue("ui_maxClients", temp);
	temp = trap_Cvar_VariableValue( "g_warmup" );
	trap_Cvar_SetValue("ui_Warmup", temp);
	temp = trap_Cvar_VariableValue( "sv_pure" );
	trap_Cvar_SetValue("ui_pure", temp);

	trap_Cvar_SetValue("cg_drawTimer", 1);
	trap_Cvar_SetValue("g_doWarmup", 1);
	trap_Cvar_SetValue("g_warmup", 15);
	trap_Cvar_SetValue("sv_pure", 0);
	trap_Cvar_SetValue("g_friendlyFire", 0);
	trap_Cvar_Set("g_redTeam", CG_Cvar_VariableString("ui_teamName"));
	trap_Cvar_Set("g_blueTeam", CG_Cvar_VariableString("ui_opponentName"));

	if (trap_Cvar_VariableValue("ui_recordSPDemo")) {
		Com_sprintf(buff, MAX_STRING_CHARS, "%s_%i", uiInfo.mapList[ui_currentMap.integer].mapLoadName, g);
		trap_Cvar_Set("ui_recordSPDemoName", buff);
	}

	delay = 500;

	if (g == GT_TOURNAMENT) {
		trap_Cvar_SetValue("sv_maxClients", 2);
		Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %f "", %i \n", uiInfo.mapList[ui_currentMap.integer].opponentName, skill, delay);
		trap_Cmd_ExecuteText( EXEC_APPEND, buff );
	} else {
		temp = uiInfo.mapList[ui_currentMap.integer].teamMembers * 2;
		trap_Cvar_SetValue("sv_maxClients", temp);
		for (i =0; i < uiInfo.mapList[ui_currentMap.integer].teamMembers; i++) {
			Com_sprintf( buff, sizeof(buff), "addbot %s %f %s %i %s\n", UI_AIFromName(uiInfo.teamList[k].teamMembers[i]), skill, (g == GT_FFA) ? "" : "Blue", delay, uiInfo.teamList[k].teamMembers[i]);
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
			delay += 500;
		}
		k = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));
		for (i =0; i < uiInfo.mapList[ui_currentMap.integer].teamMembers-1; i++) {
			Com_sprintf( buff, sizeof(buff), "addbot %s %f %s %i %s\n", UI_AIFromName(uiInfo.teamList[k].teamMembers[i]), skill, (g == GT_FFA) ? "" : "Red", delay, uiInfo.teamList[k].teamMembers[i]);
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
			delay += 500;
		}
	}
	if (g >= GT_TEAM ) {
		trap_Cvar_Set( "teampref", "red" );
	}
}

static void UI_Update(const char *name) {
	int	val = trap_Cvar_VariableValue(name);

 	if (Q_stricmp(name, "ui_SetName") == 0) {
		trap_Cvar_Set( "name", CG_Cvar_VariableString("ui_Name"));
 	} else if (Q_stricmp(name, "ui_setRate") == 0) {
		float rate = trap_Cvar_VariableValue("rate");
		if (rate >= 5000) {
			trap_Cvar_SetValue( "cl_maxpackets", 30 );
			trap_Cvar_SetValue( "cl_packetdup", 1 );
		} else if (rate >= 4000) {
			trap_Cvar_SetValue( "cl_maxpackets", 15 );
			trap_Cvar_SetValue( "cl_packetdup", 2 );		// favor less prediction errors when there's packet loss
		} else {
			trap_Cvar_SetValue( "cl_maxpackets", 15 );
			trap_Cvar_SetValue( "cl_packetdup", 1 );		// favor lower bandwidth
		}
 	} else if (Q_stricmp(name, "ui_GetName") == 0) {
		trap_Cvar_Set( "ui_Name", CG_Cvar_VariableString("name"));
 	} else if (Q_stricmp(name, "r_colorbits") == 0) {
		switch (val) {
			case 0:
				trap_Cvar_SetValue( "r_depthbits", 0 );
				trap_Cvar_Reset( "r_stencilbits" );
			break;
			case 16:
				trap_Cvar_SetValue( "r_depthbits", 16 );
				trap_Cvar_SetValue( "r_stencilbits", 0 );
			break;
			case 32:
				trap_Cvar_SetValue( "r_depthbits", 24 );
				trap_Cvar_SetValue( "r_stencilbits", 8 );
			break;
		}
	} else if (Q_stricmp(name, "r_lodbias") == 0) {
		switch (val) {
			case 0:
				trap_Cvar_SetValue( "r_subdivisions", 4 );
			break;
			case 1:
				trap_Cvar_SetValue( "r_subdivisions", 12 );
			break;
			case 2:
				trap_Cvar_SetValue( "r_subdivisions", 20 );
			break;
		}
	} else if (Q_stricmp(name, "ui_glCustom") == 0) {
		switch (val) {
			case 0:	// high quality
				trap_Cvar_SetValue( "r_fullScreen", 1 );
				trap_Cvar_SetValue( "r_subdivisions", 4 );
				trap_Cvar_SetValue( "r_vertexlight", 0 );
				trap_Cvar_SetValue( "r_lodbias", 0 );
				trap_Cvar_SetValue( "r_colorbits", 32 );
				trap_Cvar_SetValue( "r_depthbits", 24 );
				trap_Cvar_SetValue( "r_stencilbits", 8 );
				trap_Cvar_SetValue( "r_picmip", 0 );
				trap_Cvar_SetValue( "r_mode", 4 );
				trap_Cvar_SetValue( "r_texturebits", 32 );
				trap_Cvar_SetValue( "r_fastSky", 0 );
				trap_Cvar_SetValue( "r_inGameVideo", 1 );
				trap_Cvar_SetValue( "cg_shadows", 1 );
				trap_Cvar_SetValue( "cg_brassTime", 2500 );
				trap_Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_LINEAR" );
			break;
			case 1: // normal 
				trap_Cvar_SetValue( "r_fullScreen", 1 );
				trap_Cvar_SetValue( "r_subdivisions", 12 );
				trap_Cvar_SetValue( "r_vertexlight", 0 );
				trap_Cvar_SetValue( "r_lodbias", 0 );
				trap_Cvar_SetValue( "r_colorbits", 0 );
				trap_Cvar_SetValue( "r_depthbits", 0 );
				trap_Cvar_Reset( "r_stencilbits" );
				trap_Cvar_SetValue( "r_picmip", 1 );
				trap_Cvar_SetValue( "r_mode", 3 );
				trap_Cvar_SetValue( "r_texturebits", 0 );
				trap_Cvar_SetValue( "r_fastSky", 0 );
				trap_Cvar_SetValue( "r_inGameVideo", 1 );
				trap_Cvar_SetValue( "cg_brassTime", 2500 );
				trap_Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_LINEAR" );
				trap_Cvar_SetValue( "cg_shadows", 0 );
			break;
			case 2: // fast
				trap_Cvar_SetValue( "r_fullScreen", 1 );
				trap_Cvar_SetValue( "r_subdivisions", 8 );
				trap_Cvar_SetValue( "r_vertexlight", 0 );
				trap_Cvar_SetValue( "r_lodbias", 1 );
				trap_Cvar_SetValue( "r_colorbits", 0 );
				trap_Cvar_SetValue( "r_depthbits", 0 );
				trap_Cvar_Reset( "r_stencilbits" );
				trap_Cvar_SetValue( "r_picmip", 1 );
				trap_Cvar_SetValue( "r_mode", 3 );
				trap_Cvar_SetValue( "r_texturebits", 0 );
				trap_Cvar_SetValue( "cg_shadows", 0 );
				trap_Cvar_SetValue( "r_fastSky", 1 );
				trap_Cvar_SetValue( "r_inGameVideo", 0 );
				trap_Cvar_SetValue( "cg_brassTime", 0 );
				trap_Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_NEAREST" );
			break;
			case 3: // fastest
				trap_Cvar_SetValue( "r_fullScreen", 1 );
				trap_Cvar_SetValue( "r_subdivisions", 20 );
				trap_Cvar_SetValue( "r_vertexlight", 1 );
				trap_Cvar_SetValue( "r_lodbias", 2 );
				trap_Cvar_SetValue( "r_colorbits", 16 );
				trap_Cvar_SetValue( "r_depthbits", 16 );
				trap_Cvar_SetValue( "r_stencilbits", 0 );
				trap_Cvar_SetValue( "r_mode", 3 );
				trap_Cvar_SetValue( "r_picmip", 2 );
				trap_Cvar_SetValue( "r_texturebits", 16 );
				trap_Cvar_SetValue( "cg_shadows", 0 );
				trap_Cvar_SetValue( "cg_brassTime", 0 );
				trap_Cvar_SetValue( "r_fastSky", 1 );
				trap_Cvar_SetValue( "r_inGameVideo", 0 );
				trap_Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_NEAREST" );
			break;
		}
	} else if (Q_stricmp(name, "ui_mousePitch") == 0) {
		if (val == 0) {
			trap_Cvar_SetValue( "m_pitch", 0.022f );
		} else {
			trap_Cvar_SetValue( "m_pitch", -0.022f );
		}
	}
}

void UI_ForceMenuOff (void) {
	Key_SetCatcher( Key_GetCatcher() & ~KEYCATCH_UI );
	trap_Cvar_SetValue( "cl_paused", 0 );
	Menus_CloseAll();
}

void UI_EnterMenu (void) {
	Key_SetCatcher( KEYCATCH_UI );
	trap_Cvar_SetValue( "cl_paused", 1 );
}

static void UI_RunMenuScript(char **args) {
	const char *name, *name2;
	char buff[1024];

	if (String_Parse(args, &name)) {
		if (Q_stricmp(name, "StartServer") == 0) {
			int i, clients, oldclients;
			float skill;
			trap_Cvar_SetValue( "ui_singlePlayerActive", 0 );
			trap_Cvar_SetValue( "dedicated", Com_Clamp( 0, 2, ui_dedicated.integer ) );
			trap_Cvar_SetValue( "sv_public", (ui_dedicated.integer == 2) );
			trap_Cvar_SetValue( "g_gametype", uiInfo.gameTypes[ui_netGameType.integer].gtEnum );
			trap_Cvar_Set( "g_redTeam", CG_Cvar_VariableString("ui_teamName") );
			trap_Cvar_Set( "g_blueTeam", CG_Cvar_VariableString("ui_opponentName") );
			trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait ; wait ; map %s\n", uiInfo.mapList[ui_currentNetMap.integer].mapLoadName ) );
			skill = trap_Cvar_VariableValue( "g_spSkill" );
			// set max clients based on spots
			oldclients = trap_Cvar_VariableValue( "sv_maxClients" );
			clients = 0;
			for (i = 0; i < PLAYERS_PER_TEAM; i++) {
				int bot = trap_Cvar_VariableValue( va("ui_blueteam%i", i+1));
				if (bot >= 0) {
					clients++;
				}
				bot = trap_Cvar_VariableValue( va("ui_redteam%i", i+1));
				if (bot >= 0) {
					clients++;
				}
			}
			if (clients == 0) {
				clients = 8;
			}
			
			if (oldclients > clients) {
				clients = oldclients;
			}

			trap_Cvar_SetValue("sv_maxClients", clients);

			for (i = 0; i < PLAYERS_PER_TEAM; i++) {
				// 0 - None
				// 1 - Human
				// 2 - Random Bot
				// 3..NumCharacters - Bot
				int bot = trap_Cvar_VariableValue( va("ui_blueteam%i", i+1));
				if (bot > 1) {
					if (bot == 2) {
						name = "random";
					} else if (ui_actualNetGameType.integer >= GT_TEAM) {
						name = uiInfo.characterList[bot-3].name;
					} else {
						name = UI_GetBotNameByNumber(bot-3);
					}

					if (ui_actualNetGameType.integer >= GT_TEAM) {
						Com_sprintf( buff, sizeof(buff), "addbot %s %f %s\n", name, skill, "Blue");
					} else {
						Com_sprintf( buff, sizeof(buff), "addbot %s %f\n", name, skill);
					}
					trap_Cmd_ExecuteText( EXEC_APPEND, buff );
				}
				bot = trap_Cvar_VariableValue( va("ui_redteam%i", i+1));
				if (bot > 1) {
					if (bot == 2) {
						name = "random";
					} else if (ui_actualNetGameType.integer >= GT_TEAM) {
						name = uiInfo.characterList[bot-3].name;
					} else {
						name = UI_GetBotNameByNumber(bot-3);
					}

					if (ui_actualNetGameType.integer >= GT_TEAM) {
						Com_sprintf( buff, sizeof(buff), "addbot %s %f %s\n", name, skill, "Red");
					} else {
						Com_sprintf( buff, sizeof(buff), "addbot %s %f\n", name, skill);
					}
					trap_Cmd_ExecuteText( EXEC_APPEND, buff );
				}
			}
		} else if (Q_stricmp(name, "updateSPMenu") == 0) {
			UI_SetCapFragLimits(qtrue);
			UI_MapCountByGameType(qtrue);
			ui_mapIndex.integer = UI_GetIndexFromSelection(ui_currentMap.integer);
			trap_Cvar_SetValue("ui_mapIndex", ui_mapIndex.integer);
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, ui_mapIndex.integer, "skirmish");
			UI_GameType_HandleKey(0, NULL, K_MOUSE1, qfalse);
			UI_GameType_HandleKey(0, NULL, K_MOUSE2, qfalse);
		} else if (Q_stricmp(name, "resetDefaults") == 0) {
			trap_Cmd_ExecuteText( EXEC_APPEND, "exec default.cfg\n");
			trap_Cmd_ExecuteText( EXEC_APPEND, "cvar_restart\n");
			Controls_SetDefaults();
			trap_Cvar_SetValue( "com_introPlayed", 1 );
			trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart\n" );
		} else if (Q_stricmp(name, "loadArenas") == 0) {
			UI_LoadArenasIntoMapList();
			UI_MapCountByGameType(qfalse);
			Menu_SetFeederSelection(NULL, FEEDER_ALLMAPS, 0, "createserver");
		} else if (Q_stricmp(name, "saveControls") == 0) {
			Controls_SetConfig(qtrue);
		} else if (Q_stricmp(name, "loadControls") == 0) {
			Controls_GetConfig();
		} else if (Q_stricmp(name, "clearError") == 0) {
			trap_Cvar_Set("com_errorMessage", "");
		} else if (Q_stricmp(name, "loadGameInfo") == 0) {
#ifdef PRE_RELEASE_TADEMO
			UI_ParseGameInfo("demogameinfo.txt");
#else
			UI_ParseGameInfo("gameinfo.txt");
#endif
			UI_LoadBestScores(uiInfo.mapList[ui_currentMap.integer].mapLoadName, uiInfo.gameTypes[ui_gameType.integer].gtEnum);
		} else if (Q_stricmp(name, "resetScores") == 0) {
			UI_ClearScores();
		} else if (Q_stricmp(name, "RefreshServers") == 0) {
			UI_StartServerRefresh(qtrue, qtrue);
			UI_BuildServerDisplayList(qtrue);
		} else if (Q_stricmp(name, "RefreshFilter") == 0) {
			UI_StartServerRefresh(qfalse, qtrue);
			UI_BuildServerDisplayList(qtrue);
		} else if (Q_stricmp(name, "RunSPDemo") == 0) {
			if (uiInfo.demoAvailable) {
			  trap_Cmd_ExecuteText( EXEC_APPEND, va("demo %s_%i\n", uiInfo.mapList[ui_currentMap.integer].mapLoadName, uiInfo.gameTypes[ui_gameType.integer].gtEnum));
			}
		} else if (Q_stricmp(name, "LoadDemos") == 0) {
			UI_LoadDemos();
		} else if (Q_stricmp(name, "LoadMovies") == 0) {
			UI_LoadMovies();
		} else if (Q_stricmp(name, "LoadMods") == 0) {
			UI_LoadMods();
		} else if (Q_stricmp(name, "playMovie") == 0) {
			if (uiInfo.previewMovie >= 0) {
			  trap_CIN_StopCinematic(uiInfo.previewMovie);
			}
			trap_Cmd_ExecuteText( EXEC_APPEND, va("cinematic %s.roq 2\n", uiInfo.movieList[uiInfo.movieIndex]));
		} else if (Q_stricmp(name, "RunMod") == 0) {
			trap_Cvar_Set( "fs_game", uiInfo.modList[uiInfo.modIndex].modName);
			trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
		} else if (Q_stricmp(name, "RunDemo") == 0) {
			trap_Cmd_ExecuteText( EXEC_APPEND, va("demo %s\n", uiInfo.demoList[uiInfo.demoIndex]));
		} else if (Q_stricmp(name, "Quake3") == 0) {
			trap_Cvar_Set( "fs_game", BASEQ3);
			trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
		} else if (Q_stricmp(name, "closeJoin") == 0) {
			if (uiInfo.serverStatus.refreshActive) {
				UI_StopServerRefresh();
				uiInfo.serverStatus.nextDisplayRefresh = 0;
				uiInfo.nextServerStatusRefresh = 0;
				uiInfo.nextFindPlayerRefresh = 0;
				UI_BuildServerDisplayList(qtrue);
			} else {
				Menus_CloseByName("joinserver");
				Menus_OpenByName("main");
			}
		} else if (Q_stricmp(name, "StopRefresh") == 0) {
			UI_StopServerRefresh();
			uiInfo.serverStatus.nextDisplayRefresh = 0;
			uiInfo.nextServerStatusRefresh = 0;
			uiInfo.nextFindPlayerRefresh = 0;
		} else if (Q_stricmp(name, "UpdateFilter") == 0) {
			// UpdateFilter is called when server broser menu is opened and when a favorite server is deleted.
			UI_StartServerRefresh(qtrue, qfalse);
			UI_BuildServerDisplayList(qtrue);
			UI_FeederSelection(FEEDER_SERVERS, 0);

			UI_LoadMods();
		} else if (Q_stricmp(name, "ServerStatus") == 0) {
			trap_LAN_GetServerAddressString(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], uiInfo.serverStatusAddress, sizeof(uiInfo.serverStatusAddress));
			UI_BuildServerStatus(qtrue);
		} else if (Q_stricmp(name, "FoundPlayerServerStatus") == 0) {
			Q_strncpyz(uiInfo.serverStatusAddress, uiInfo.foundPlayerServerAddresses[uiInfo.currentFoundPlayerServer], sizeof(uiInfo.serverStatusAddress));
			UI_BuildServerStatus(qtrue);
			Menu_SetFeederSelection(NULL, FEEDER_FINDPLAYER, 0, NULL);
		} else if (Q_stricmp(name, "FindPlayer") == 0) {
			UI_BuildFindPlayerList(qtrue);
			// clear the displayed server status info
			uiInfo.serverStatusInfo.numLines = 0;
			Menu_SetFeederSelection(NULL, FEEDER_FINDPLAYER, 0, NULL);
		} else if (Q_stricmp(name, "JoinServer") == 0) {
			trap_Cvar_SetValue( "ui_singlePlayerActive", 0 );
			if (uiInfo.serverStatus.currentServer >= 0 && uiInfo.serverStatus.currentServer < uiInfo.serverStatus.numDisplayServers) {
				trap_LAN_GetServerAddressString(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, 1024);
				trap_Cmd_ExecuteText( EXEC_APPEND, va( "connect %s\n", buff ) );
			}
		} else if (Q_stricmp(name, "FoundPlayerJoinServer") == 0) {
			trap_Cvar_SetValue( "ui_singlePlayerActive", 0 );
			if (uiInfo.currentFoundPlayerServer >= 0 && uiInfo.currentFoundPlayerServer < uiInfo.numFoundPlayerServers) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va( "connect %s\n", uiInfo.foundPlayerServerAddresses[uiInfo.currentFoundPlayerServer] ) );
			}
		} else if (Q_stricmp(name, "Quit") == 0) {
			trap_Cvar_SetValue( "ui_singlePlayerActive", 0 );
			trap_Cmd_ExecuteText( EXEC_NOW, "quit");
		} else if (Q_stricmp(name, "Controls") == 0) {
			UI_EnterMenu();
			Menus_CloseAll();
			Menus_ActivateByName("setup_menu2");
		} else if (Q_stricmp(name, "Leave") == 0) {
			trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect\n" );
			UI_EnterMenu();
			Menus_CloseAll();
			Menus_ActivateByName("main");
		} else if (Q_stricmp(name, "ServerSort") == 0) {
			int sortColumn;
			if (Int_Parse(args, &sortColumn)) {
				// if same column we're already sorting on then flip the direction
				if (sortColumn == uiInfo.serverStatus.sortKey) {
					uiInfo.serverStatus.sortDir = !uiInfo.serverStatus.sortDir;
				}
				// make sure we sort again
				UI_ServersSort(sortColumn, qtrue);
			}
		} else if (Q_stricmp(name, "nextSkirmish") == 0) {
			UI_StartSkirmish(qtrue);
		} else if (Q_stricmp(name, "SkirmishStart") == 0) {
			UI_StartSkirmish(qfalse);
		} else if (Q_stricmp(name, "closeingame") == 0) {
			UI_ForceMenuOff();
		} else if (Q_stricmp(name, "voteMap") == 0) {
			if (ui_currentNetMap.integer >=0 && ui_currentNetMap.integer < uiInfo.mapCount) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote map %s\n",uiInfo.mapList[ui_currentNetMap.integer].mapLoadName) );
			}
		} else if (Q_stricmp(name, "voteKick") == 0) {
			if (uiInfo.playerIndex >= 0 && uiInfo.playerIndex < uiInfo.playerCount) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote kicknum %i\n",uiInfo.playerNums[uiInfo.playerIndex]) );
			}
		} else if (Q_stricmp(name, "voteGame") == 0) {
			if (ui_netGameType.integer >= 0 && ui_netGameType.integer < uiInfo.numGameTypes) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote g_gametype %i\n",uiInfo.gameTypes[ui_netGameType.integer].gtEnum) );
			}
		} else if (Q_stricmp(name, "voteLeader") == 0) {
			if (uiInfo.teamIndex >= 0 && uiInfo.teamIndex < uiInfo.myTeamCount) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callteamvote leader %s\n",uiInfo.teamPlayerNames[uiInfo.teamIndex]) );
			}
		} else if (Q_stricmp(name, "addBot") == 0) {
			// 0 - Random Bot
			// 1..NumCharacters - Bot
			if (uiInfo.botIndex == 0) {
				name = "random";
			} else if (trap_Cvar_VariableValue("g_gametype") >= GT_TEAM) {
				name = uiInfo.characterList[uiInfo.botIndex-1].name;
			} else {
				name = UI_GetBotNameByNumber(uiInfo.botIndex-1);
			}

			trap_Cmd_ExecuteText( EXEC_APPEND, va("addbot %s %i %s\n", name, uiInfo.skillIndex+1, (uiInfo.redBlue == 0) ? "red" : "blue") );
		} else if (Q_stricmp(name, "addFavorite") == 0) {
			if (ui_netSource.integer != UIAS_FAVORITES) {
				char name[MAX_NAME_LENGTH];
				char addr[MAX_ADDRESSLENGTH];
				int res;

				trap_LAN_GetServerInfo(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, MAX_STRING_CHARS);
				name[0] = addr[0] = '\0';
				Q_strncpyz(name, 	Info_ValueForKey(buff, "hostname"), sizeof ( name ) );
				Q_strncpyz(addr, 	Info_ValueForKey(buff, "addr"), sizeof ( addr ) );
				if (strlen(name) > 0 && strlen(addr) > 0) {
					res = trap_LAN_AddServer(AS_FAVORITES, name, addr);
					if (res == 0) {
						// server already in the list
						Com_Printf("Favorite already in list\n");
					}
					else if (res == -1) {
						// list full
						Com_Printf("Favorite list full\n");
					}
					else {
						// successfully added
						Com_Printf("Added favorite server %s\n", addr);
					}
				}
			}
		} else if (Q_stricmp(name, "deleteFavorite") == 0) {
			if (ui_netSource.integer == UIAS_FAVORITES) {
				char addr[MAX_ADDRESSLENGTH];
				trap_LAN_GetServerInfo(AS_FAVORITES, uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, MAX_STRING_CHARS);
				addr[0] = '\0';
				Q_strncpyz(addr, 	Info_ValueForKey(buff, "addr"), sizeof ( addr ) );
				if (strlen(addr) > 0) {
					trap_LAN_RemoveServer(AS_FAVORITES, addr);
				}
			}
		} else if (Q_stricmp(name, "createFavorite") == 0) {
			char name[MAX_NAME_LENGTH];
			char addr[MAX_ADDRESSLENGTH];
			int res;

			name[0] = addr[0] = '\0';
			Q_strncpyz(name, 	CG_Cvar_VariableString("ui_favoriteName"), sizeof ( name ) );
			Q_strncpyz(addr, 	CG_Cvar_VariableString("ui_favoriteAddress"), sizeof ( addr ) );
			if (strlen(name) > 0 && strlen(addr) > 0) {
				res = trap_LAN_AddServer(AS_FAVORITES, name, addr);
				if (res == 0) {
					// server already in the list
					Com_Printf("Favorite already in list\n");
				}
				else if (res == -1) {
					// list full
					Com_Printf("Favorite list full\n");
				}
				else {
					// successfully added
					Com_Printf("Added favorite server %s\n", addr);
				}
			}
		} else if (Q_stricmp(name, "orders") == 0) {
			const char *orders;
			if (String_Parse(args, &orders)) {
				int selectedPlayer = trap_Cvar_VariableValue("cg_selectedPlayer");
				if (selectedPlayer < uiInfo.myTeamCount) {
					Com_sprintf( buff, sizeof( buff ), orders, uiInfo.teamPlayerNums[selectedPlayer] );
					trap_Cmd_ExecuteText( EXEC_APPEND, buff );
					trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );
				} else {
					int i;
					for (i = 0; i < uiInfo.myTeamCount; i++) {
						if (uiInfo.playerNumber == uiInfo.teamPlayerNums[i]) {
							continue;
						}
						Com_sprintf( buff, sizeof( buff ), orders, uiInfo.teamPlayerNums[i] );
						trap_Cmd_ExecuteText( EXEC_APPEND, buff );
						trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );
					}
				}
				UI_ForceMenuOff();
			}
		} else if (Q_stricmp(name, "voiceOrdersTeam") == 0) {
			const char *orders;
			if (String_Parse(args, &orders)) {
				int selectedPlayer = trap_Cvar_VariableValue("cg_selectedPlayer");
				if (selectedPlayer == uiInfo.myTeamCount) {
					trap_Cmd_ExecuteText( EXEC_APPEND, orders );
					trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );
				}
				UI_ForceMenuOff();
			}
		} else if (Q_stricmp(name, "voiceOrders") == 0) {
			const char *orders;
			if (String_Parse(args, &orders)) {
				int selectedPlayer = trap_Cvar_VariableValue("cg_selectedPlayer");
				if (selectedPlayer < uiInfo.myTeamCount) {
					Com_sprintf( buff, sizeof( buff ), orders, uiInfo.teamPlayerNums[selectedPlayer] );
					trap_Cmd_ExecuteText( EXEC_APPEND, buff );
					trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );
				}
				UI_ForceMenuOff();
			}
		} else if (Q_stricmp(name, "glCustom") == 0) {
			trap_Cvar_SetValue( "ui_glCustom", 4 );
		} else if (Q_stricmp(name, "update") == 0) {
			if (String_Parse(args, &name2)) {
				UI_Update(name2);
			}
		} else {
			Com_Printf("unknown UI script %s\n", name);
		}
	}
}

static void UI_GetTeamColor(vec4_t *color) {
}

/*
==================
UI_MapCountByGameType
==================
*/
static int UI_MapCountByGameType(qboolean singlePlayer) {
	int i, c, game;
	c = 0;
	game = singlePlayer ? uiInfo.gameTypes[ui_gameType.integer].gtEnum : uiInfo.gameTypes[ui_netGameType.integer].gtEnum;
	if (game == GT_SINGLE_PLAYER) {
		game++;
	} 
	if (game == GT_TEAM) {
		game = GT_FFA;
	}

	for (i = 0; i < uiInfo.mapCount; i++) {
		uiInfo.mapList[i].active = qfalse;
		if ( uiInfo.mapList[i].typeBits & (1 << game)) {
			if (singlePlayer) {
				if (!(uiInfo.mapList[i].typeBits & (1 << GT_SINGLE_PLAYER))) {
					continue;
				}
			}
			c++;
			uiInfo.mapList[i].active = qtrue;
		}
	}
	return c;
}

qboolean UI_hasSkinForBase(const char *base, const char *team) {
	char	test[MAX_QPATH];
	
	Com_sprintf( test, sizeof( test ), "models/players/%s/%s/lower_default.skin", base, team );

	if (trap_FS_FOpenFile(test, NULL, FS_READ)) {
		return qtrue;
	}
	return qfalse;
}

/*
==================
UI_MapCountByTeam
==================
*/
static int UI_HeadCountByTeam(void) {
	static int init = 0;
	int i, j, k, c, tIndex;
	
	c = 0;
	if (!init) {
		for (i = 0; i < uiInfo.characterCount; i++) {
			uiInfo.characterList[i].reference = 0;
			for (j = 0; j < uiInfo.teamCount; j++) {
			  if (UI_hasSkinForBase(uiInfo.characterList[i].base, uiInfo.teamList[j].teamName)) {
					uiInfo.characterList[i].reference |= (1<<j);
			  }
			}
		}
		init = 1;
	}

	tIndex = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));

	// do names
	for (i = 0; i < uiInfo.characterCount; i++) {
		uiInfo.characterList[i].active = qfalse;
		for(j = 0; j < TEAM_MEMBERS; j++) {
			if (uiInfo.teamList[tIndex].teamMembers[j] != NULL) {
				if (uiInfo.characterList[i].reference&(1<<tIndex)) {// && Q_stricmp(uiInfo.teamList[tIndex].teamMembers[j], uiInfo.characterList[i].name)==0) {
					uiInfo.characterList[i].active = qtrue;
					c++;
					break;
				}
			}
		}
	}

	// and then aliases
	for(j = 0; j < TEAM_MEMBERS; j++) {
		for(k = 0; k < uiInfo.aliasCount; k++) {
			if (uiInfo.aliasList[k].name != NULL) {
				if (Q_stricmp(uiInfo.teamList[tIndex].teamMembers[j], uiInfo.aliasList[k].name)==0) {
					for (i = 0; i < uiInfo.characterCount; i++) {
						if (uiInfo.characterList[i].headImage != -1 && uiInfo.characterList[i].reference&(1<<tIndex) && Q_stricmp(uiInfo.aliasList[k].ai, uiInfo.characterList[i].name)==0) {
							if (uiInfo.characterList[i].active == qfalse) {
								uiInfo.characterList[i].active = qtrue;
								c++;
							}
							break;
						}
					}
				}
			}
		}
	}
	return c;
}

/*
==================
UI_InsertServerIntoDisplayList
==================
*/
static void UI_InsertServerIntoDisplayList(int num, int position) {
	int i;

	if (position < 0 || position > uiInfo.serverStatus.numDisplayServers ) {
		return;
	}
	//
	uiInfo.serverStatus.numDisplayServers++;
	for (i = uiInfo.serverStatus.numDisplayServers; i > position; i--) {
		uiInfo.serverStatus.displayServers[i] = uiInfo.serverStatus.displayServers[i-1];
	}
	uiInfo.serverStatus.displayServers[position] = num;

	// update displayed levelshot
	if ( position == uiInfo.serverStatus.currentServer ) {
		UI_FeederSelection( FEEDER_SERVERS, uiInfo.serverStatus.currentServer );
	}
}

/*
==================
UI_RemoveServerFromDisplayList
==================
*/
static void UI_RemoveServerFromDisplayList(int num) {
	int i, j;

	for (i = 0; i < uiInfo.serverStatus.numDisplayServers; i++) {
		if (uiInfo.serverStatus.displayServers[i] == num) {
			uiInfo.serverStatus.numDisplayServers--;
			for (j = i; j < uiInfo.serverStatus.numDisplayServers; j++) {
				uiInfo.serverStatus.displayServers[j] = uiInfo.serverStatus.displayServers[j+1];
			}
			return;
		}
	}
}

/*
==================
UI_BinaryServerInsertion
==================
*/
static void UI_BinaryServerInsertion(int num) {
	int mid, offset, res, len;

	// use binary search to insert server
	len = uiInfo.serverStatus.numDisplayServers;
	mid = len;
	offset = 0;
	res = 0;
	while(mid > 0) {
		mid = len >> 1;
		//
		res = trap_LAN_CompareServers( UI_SourceForLAN(), uiInfo.serverStatus.sortKey,
					uiInfo.serverStatus.sortDir, num, uiInfo.serverStatus.displayServers[offset+mid]);
		// if equal
		if (res == 0) {
			UI_InsertServerIntoDisplayList(num, offset+mid);
			return;
		}
		// if larger
		else if (res == 1) {
			offset += mid;
			len -= mid;
		}
		// if smaller
		else {
			len -= mid;
		}
	}
	if (res == 1) {
		offset++;
	}
	UI_InsertServerIntoDisplayList(num, offset);
}

/*
==================
UI_BuildServerDisplayList
==================
*/
static void UI_BuildServerDisplayList(int force) {
	int i, count, clients, maxClients, ping, len, visible;
	char info[MAX_STRING_CHARS];
//	qboolean startRefresh = qtrue; TTimo: unused
	static int numinvisible;
	int	lanSource;

	if (!(force || uiInfo.uiDC.realTime > uiInfo.serverStatus.nextDisplayRefresh)) {
		return;
	}
	// if we shouldn't reset
	if ( force == 2 ) {
		force = 0;
	}

	// do motd updates here too
	trap_Cvar_VariableStringBuffer( "cl_motdString", uiInfo.serverStatus.motd, sizeof(uiInfo.serverStatus.motd) );
	len = strlen(uiInfo.serverStatus.motd);
	if (len == 0) {
		strcpy(uiInfo.serverStatus.motd, "Welcome to Team Arena!");
	} 

	lanSource = UI_SourceForLAN();

	if (force) {
		numinvisible = 0;
		// clear number of displayed servers
		uiInfo.serverStatus.numDisplayServers = 0;
		uiInfo.serverStatus.numPlayersOnServers = 0;
		// set list box index to zero
		Menu_SetFeederSelection(NULL, FEEDER_SERVERS, 0, NULL);
		// mark all servers as visible so we store ping updates for them
		trap_LAN_MarkServerVisible(lanSource, -1, qtrue);
	}

	// get the server count (comes from the master)
	count = trap_LAN_GetServerCount(lanSource);
	if (count == -1 || (ui_netSource.integer == UIAS_LOCAL && count == 0) ) {
		// still waiting on a response from the master
		uiInfo.serverStatus.numDisplayServers = 0;
		uiInfo.serverStatus.numPlayersOnServers = 0;
		uiInfo.serverStatus.nextDisplayRefresh = uiInfo.uiDC.realTime + 500;
		return;
	}

	visible = qfalse;
	for (i = 0; i < count; i++) {
		// if we already got info for this server
		if (!trap_LAN_ServerIsVisible(lanSource, i)) {
			continue;
		}
		visible = qtrue;
		// get the ping for this server
		ping = trap_LAN_GetServerPing(lanSource, i);
		if (ping > 0 || ui_netSource.integer == UIAS_FAVORITES) {
			// Remove favorite servers so they do not appear multiple times
			// or appear when the cached server info was not filtered out
			// but the new server info is filtered out.
			if (ui_netSource.integer == UIAS_FAVORITES) {
				UI_RemoveServerFromDisplayList(i);
			}

			trap_LAN_GetServerInfo(lanSource, i, info, MAX_STRING_CHARS);

			if (ui_browserShowBots.integer == 0) {
				clients = atoi(Info_ValueForKey(info, "g_humanplayers"));
			} else {
				clients = atoi(Info_ValueForKey(info, "clients"));
			}

			if (ui_browserShowEmpty.integer == 0) {
				if (clients == 0) {
					if (ping > 0) {
						trap_LAN_MarkServerVisible(lanSource, i, qfalse);
					}
					continue;
				}
			}

			if (ui_browserShowFull.integer == 0) {
				maxClients = atoi(Info_ValueForKey(info, "sv_maxclients"));
				if (clients == maxClients) {
					if (ping > 0) {
						trap_LAN_MarkServerVisible(lanSource, i, qfalse);
					}
					continue;
				}
			}

			if (uiInfo.joinGameTypes[ui_joinGameType.integer].gtEnum >= 0
				&& uiInfo.joinGameTypes[ui_joinGameType.integer].gtEnum < GT_MAX_GAME_TYPE ) {
				if (Q_stricmp(Info_ValueForKey(info, "gametype"), bg_netGametypeNames[uiInfo.joinGameTypes[ui_joinGameType.integer].gtEnum]) != 0) {
					if (ping > 0) {
						trap_LAN_MarkServerVisible(lanSource, i, qfalse);
					}
					continue;
				}
			}
				
			if (ui_serverFilterType.integer > 0) {
				if (Q_stricmp(Info_ValueForKey(info, "game"), UI_FilterDir( ui_serverFilterType.integer ) ) != 0) {
					if (ping > 0) {
						trap_LAN_MarkServerVisible(lanSource, i, qfalse);
					}
					continue;
				}
			}
			// insert the server into the list
			UI_BinaryServerInsertion(i);
			// done with this server
			if (ping > 0) {
				trap_LAN_MarkServerVisible(lanSource, i, qfalse);
				uiInfo.serverStatus.numPlayersOnServers += clients;
				numinvisible++;
			}
		}
	}

	uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime;

	// if there were no servers visible for ping updates
	if (!visible) {
//		UI_StopServerRefresh();
//		uiInfo.serverStatus.nextDisplayRefresh = 0;
	}
}

typedef struct
{
	char *name, *altName;
} serverStatusCvar_t;

serverStatusCvar_t serverStatusCvars[] = {
	{"sv_hostname", "Name"},
	{"Address", ""},
	{"fs_game", "Game"},
	{"sv_gametypeName", "Game type"},
	{"sv_gametypeNetName", "Game type abbr"},
	{"g_gametype", "Game type number"},
	{"mapname", "Map"},
	{"version", ""},
	{"protocol", ""},
	{"gameversion", ""},
	{"gameprotocol", ""},
	{"timelimit", ""},
	{"fraglimit", ""},
	{"capturelimit", ""},
	{NULL, NULL}
};

/*
==================
UI_SortServerStatusInfo
==================
*/
static void UI_SortServerStatusInfo( serverStatusInfo_t *info ) {
	int i, j, index;
	char *tmp1, *tmp2;

	index = 0;
	for (i = 0; serverStatusCvars[i].name; i++) {
		for (j = 0; j < info->numLines; j++) {
			if ( !info->lines[j][1] || info->lines[j][1][0] ) {
				continue;
			}
			if ( !Q_stricmp(serverStatusCvars[i].name, info->lines[j][0]) ) {
				// swap lines
				tmp1 = info->lines[index][0];
				tmp2 = info->lines[index][3];
				info->lines[index][0] = info->lines[j][0];
				info->lines[index][3] = info->lines[j][3];
				info->lines[j][0] = tmp1;
				info->lines[j][3] = tmp2;
				//
				if ( strlen(serverStatusCvars[i].altName) ) {
					info->lines[index][0] = serverStatusCvars[i].altName;
				}
				index++;
			}
		}
	}
}

/*
==================
UI_GetServerStatusInfo
==================
*/
static int UI_GetServerStatusInfo( const char *serverAddress, serverStatusInfo_t *info ) {
	char *p, *score, *ping, *name;
	int i, len;

	if (info) {
		memset(info, 0, sizeof(*info));
	}

	// ignore initial unset addresses
	if (serverAddress && *serverAddress == '\0') {
		return qfalse;
	}

	// reset server status request for this address
	if (!info) {
		trap_LAN_ServerStatus( serverAddress, NULL, 0);
		return qfalse;
	}

	if ( trap_LAN_ServerStatus( serverAddress, info->text, sizeof(info->text)) ) {
		Q_strncpyz(info->address, serverAddress, sizeof(info->address));
		p = info->text;
		info->numLines = 0;
		info->lines[info->numLines][0] = "Address";
		info->lines[info->numLines][1] = "";
		info->lines[info->numLines][2] = "";
		info->lines[info->numLines][3] = info->address;
		info->numLines++;
		// get the cvars
		while (p && *p) {
			p = strchr(p, '\\');
			if (!p) break;
			*p++ = '\0';
			if (*p == '\\')
				break;
			info->lines[info->numLines][0] = p;
			info->lines[info->numLines][1] = "";
			info->lines[info->numLines][2] = "";
			p = strchr(p, '\\');
			if (!p) break;
			*p++ = '\0';
			info->lines[info->numLines][3] = p;

			info->numLines++;
			if (info->numLines >= MAX_SERVERSTATUS_LINES)
				break;
		}
		// get the player list
		if (info->numLines < MAX_SERVERSTATUS_LINES-3) {
			// empty line
			info->lines[info->numLines][0] = "";
			info->lines[info->numLines][1] = "";
			info->lines[info->numLines][2] = "";
			info->lines[info->numLines][3] = "";
			info->numLines++;
			// header
			info->lines[info->numLines][0] = "num";
			info->lines[info->numLines][1] = "score";
			info->lines[info->numLines][2] = "ping";
			info->lines[info->numLines][3] = "name";
			info->numLines++;
			// parse players
			i = 0;
			len = 0;
			while (p && *p) {
				if (*p == '\\')
					*p++ = '\0';
				score = p;
				p = strchr(p, ' ');
				if (!p)
					break;
				*p++ = '\0';
				ping = p;
				p = strchr(p, ' ');
				if (!p)
					break;
				*p++ = '\0';
				name = p;
				Com_sprintf(&info->pings[len], sizeof(info->pings)-len, "%d", i);
				info->lines[info->numLines][0] = &info->pings[len];
				len += strlen(&info->pings[len]) + 1;
				info->lines[info->numLines][1] = score;
				info->lines[info->numLines][2] = ping;
				info->lines[info->numLines][3] = name;
				info->numLines++;
				if (info->numLines >= MAX_SERVERSTATUS_LINES)
					break;
				p = strchr(p, '\\');
				if (!p)
					break;
				*p++ = '\0';
				//
				i++;
			}
		}
		UI_SortServerStatusInfo( info );
		return qtrue;
	}
	return qfalse;
}

/*
==================
stristr
==================
*/
static char *stristr(char *str, char *charset) {
	int i;

	while(*str) {
		for (i = 0; charset[i] && str[i]; i++) {
			if (toupper(charset[i]) != toupper(str[i])) break;
		}
		if (!charset[i]) return str;
		str++;
	}
	return NULL;
}

/*
==================
UI_BuildFindPlayerList
==================
*/
static void UI_BuildFindPlayerList(qboolean force) {
	static int numFound, numTimeOuts;
	int i, j, resend;
	serverStatusInfo_t info;
	char name[MAX_NAME_LENGTH+2];
	char infoString[MAX_STRING_CHARS];
	int  lanSource;

	if (!force) {
		if (!uiInfo.nextFindPlayerRefresh || uiInfo.nextFindPlayerRefresh > uiInfo.uiDC.realTime) {
			return;
		}
	}
	else {
		memset(&uiInfo.pendingServerStatus, 0, sizeof(uiInfo.pendingServerStatus));
		uiInfo.numFoundPlayerServers = 0;
		uiInfo.currentFoundPlayerServer = 0;
		trap_Cvar_VariableStringBuffer( "ui_findPlayer", uiInfo.findPlayerName, sizeof(uiInfo.findPlayerName));
		Q_CleanStr(uiInfo.findPlayerName);
		// should have a string of some length
		if (!strlen(uiInfo.findPlayerName)) {
			uiInfo.nextFindPlayerRefresh = 0;
			return;
		}
		// set resend time
		resend = ui_serverStatusTimeOut.integer / 2 - 10;
		if (resend < 50) {
			resend = 50;
		}
		trap_Cvar_SetValue("cl_serverStatusResendTime", resend);
		// reset all server status requests
		trap_LAN_ServerStatus( NULL, NULL, 0);
		//
		uiInfo.numFoundPlayerServers = 1;
		Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
						sizeof(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1]),
							"searching %d...", uiInfo.pendingServerStatus.num);
		numFound = 0;
		numTimeOuts++;
	}
	for (i = 0; i < MAX_SERVERSTATUSREQUESTS; i++) {
		// if this pending server is valid
		if (uiInfo.pendingServerStatus.server[i].valid) {
			// try to get the server status for this server
			if (UI_GetServerStatusInfo( uiInfo.pendingServerStatus.server[i].adrstr, &info ) ) {
				//
				numFound++;
				// parse through the server status lines
				for (j = 0; j < info.numLines; j++) {
					// should have ping info
					if ( !info.lines[j][2] || !info.lines[j][2][0] ) {
						continue;
					}
					// clean string first
					Q_strncpyz(name, info.lines[j][3], sizeof(name));
					Q_CleanStr(name);
					// if the player name is a substring
					if (stristr(name, uiInfo.findPlayerName)) {
						// add to found server list if we have space (always leave space for a line with the number found)
						if (uiInfo.numFoundPlayerServers < MAX_FOUNDPLAYER_SERVERS-1) {
							//
							Q_strncpyz(uiInfo.foundPlayerServerAddresses[uiInfo.numFoundPlayerServers-1],
										uiInfo.pendingServerStatus.server[i].adrstr,
											sizeof(uiInfo.foundPlayerServerAddresses[0]));
							Q_strncpyz(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
										uiInfo.pendingServerStatus.server[i].name,
											sizeof(uiInfo.foundPlayerServerNames[0]));
							uiInfo.numFoundPlayerServers++;
						}
						else {
							// can't add any more so we're done
							uiInfo.pendingServerStatus.num = uiInfo.serverStatus.numDisplayServers;
						}
					}
				}
				Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
								sizeof(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1]),
									"searching %d/%d...", uiInfo.pendingServerStatus.num, numFound);
				// retrieved the server status so reuse this spot
				uiInfo.pendingServerStatus.server[i].valid = qfalse;
			}
		}
		// if empty pending slot or timed out
		if (!uiInfo.pendingServerStatus.server[i].valid ||
			uiInfo.pendingServerStatus.server[i].startTime < uiInfo.uiDC.realTime - ui_serverStatusTimeOut.integer) {
			if (uiInfo.pendingServerStatus.server[i].valid) {
				numTimeOuts++;
			}
			// reset server status request for this address
			UI_GetServerStatusInfo( uiInfo.pendingServerStatus.server[i].adrstr, NULL );
			// reuse pending slot
			uiInfo.pendingServerStatus.server[i].valid = qfalse;
			// if we didn't try to get the status of all servers in the main browser yet
			if (uiInfo.pendingServerStatus.num < uiInfo.serverStatus.numDisplayServers) {
				uiInfo.pendingServerStatus.server[i].startTime = uiInfo.uiDC.realTime;
				lanSource = UI_SourceForLAN();
				trap_LAN_GetServerAddressString(lanSource, uiInfo.serverStatus.displayServers[uiInfo.pendingServerStatus.num],
							uiInfo.pendingServerStatus.server[i].adrstr, sizeof(uiInfo.pendingServerStatus.server[i].adrstr));
				trap_LAN_GetServerInfo(lanSource, uiInfo.serverStatus.displayServers[uiInfo.pendingServerStatus.num], infoString, sizeof(infoString));
				Q_strncpyz(uiInfo.pendingServerStatus.server[i].name, Info_ValueForKey(infoString, "hostname"), sizeof(uiInfo.pendingServerStatus.server[0].name));
				uiInfo.pendingServerStatus.server[i].valid = qtrue;
				uiInfo.pendingServerStatus.num++;
				Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
								sizeof(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1]),
									"searching %d/%d...", uiInfo.pendingServerStatus.num, numFound);
			}
		}
	}
	for (i = 0; i < MAX_SERVERSTATUSREQUESTS; i++) {
		if (uiInfo.pendingServerStatus.server[i].valid) {
			break;
		}
	}
	// if still trying to retrieve server status info
	if (i < MAX_SERVERSTATUSREQUESTS) {
		uiInfo.nextFindPlayerRefresh = uiInfo.uiDC.realTime + 25;
	}
	else {
		// add a line that shows the number of servers found
		if (!uiInfo.numFoundPlayerServers) {
			Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1], sizeof(uiInfo.foundPlayerServerAddresses[0]), "no servers found");
		}
		else {
			Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1], sizeof(uiInfo.foundPlayerServerAddresses[0]),
						"%d server%s found with player %s", uiInfo.numFoundPlayerServers-1,
						uiInfo.numFoundPlayerServers == 2 ? "":"s", uiInfo.findPlayerName);
		}
		uiInfo.nextFindPlayerRefresh = 0;
		// show the server status info for the selected server
		UI_FeederSelection(FEEDER_FINDPLAYER, uiInfo.currentFoundPlayerServer);
	}
}

/*
==================
UI_BuildServerStatus
==================
*/
static void UI_BuildServerStatus(qboolean force) {

	if (uiInfo.nextFindPlayerRefresh) {
		return;
	}
	if (!force) {
		if (!uiInfo.nextServerStatusRefresh || uiInfo.nextServerStatusRefresh > uiInfo.uiDC.realTime) {
			return;
		}
	}
	else {
		Menu_SetFeederSelection(NULL, FEEDER_SERVERSTATUS, 0, NULL);
		uiInfo.serverStatusInfo.numLines = 0;
		// reset all server status requests
		trap_LAN_ServerStatus( NULL, NULL, 0);
	}
	if (uiInfo.serverStatus.currentServer < 0 || uiInfo.serverStatus.currentServer > uiInfo.serverStatus.numDisplayServers || uiInfo.serverStatus.numDisplayServers == 0) {
		return;
	}
	if (UI_GetServerStatusInfo( uiInfo.serverStatusAddress, &uiInfo.serverStatusInfo ) ) {
		uiInfo.nextServerStatusRefresh = 0;
		UI_GetServerStatusInfo( uiInfo.serverStatusAddress, NULL );
	}
	else {
		uiInfo.nextServerStatusRefresh = uiInfo.uiDC.realTime + 500;
	}
}

/*
==================
UI_FeederCount
==================
*/
static int UI_FeederCount(float feederID) {
	if (feederID == FEEDER_HEADS) {
		return UI_HeadCountByTeam();
	} else if (feederID == FEEDER_Q3HEADS) {
		return uiInfo.q3HeadCount;
	} else if (feederID == FEEDER_CINEMATICS) {
		return uiInfo.movieCount;
	} else if (feederID == FEEDER_MAPS || feederID == FEEDER_ALLMAPS) {
		return UI_MapCountByGameType(feederID == FEEDER_MAPS ? qtrue : qfalse);
	} else if (feederID == FEEDER_SERVERS) {
		return uiInfo.serverStatus.numDisplayServers;
	} else if (feederID == FEEDER_SERVERSTATUS) {
		return uiInfo.serverStatusInfo.numLines;
	} else if (feederID == FEEDER_FINDPLAYER) {
		return uiInfo.numFoundPlayerServers;
	} else if (feederID == FEEDER_PLAYER_LIST) {
		if (uiInfo.uiDC.realTime > uiInfo.playerRefresh) {
			uiInfo.playerRefresh = uiInfo.uiDC.realTime + 3000;
			UI_BuildPlayerList();
		}
		return uiInfo.playerCount;
	} else if (feederID == FEEDER_TEAM_LIST) {
		if (uiInfo.uiDC.realTime > uiInfo.playerRefresh) {
			uiInfo.playerRefresh = uiInfo.uiDC.realTime + 3000;
			UI_BuildPlayerList();
		}
		return uiInfo.myTeamCount;
	} else if (feederID == FEEDER_MODS) {
		return uiInfo.modCount;
	} else if (feederID == FEEDER_DEMOS) {
		return uiInfo.demoCount;
	}
	return 0;
}

static const char *UI_SelectedMap(int index, int *actual) {
	int i, c;
	c = 0;
	*actual = 0;
	for (i = 0; i < uiInfo.mapCount; i++) {
		if (uiInfo.mapList[i].active) {
			if (c == index) {
				*actual = i;
				return uiInfo.mapList[i].mapName;
			} else {
				c++;
			}
		}
	}
	return "";
}

static const char *UI_SelectedHead(int index, int *actual) {
	int i, c;
	c = 0;
	*actual = 0;
	for (i = 0; i < uiInfo.characterCount; i++) {
		if (uiInfo.characterList[i].active) {
			if (c == index) {
				*actual = i;
				return uiInfo.characterList[i].name;
			} else {
				c++;
			}
		}
	}
	return "";
}

static int UI_GetIndexFromSelection(int actual) {
	int i, c;
	c = 0;
	for (i = 0; i < uiInfo.mapCount; i++) {
		if (uiInfo.mapList[i].active) {
			if (i == actual) {
				return c;
			}
				c++;
		}
	}
  return 0;
}

static void UI_UpdatePendingPings( void ) { 
	trap_LAN_ResetPings(UI_SourceForLAN());
	uiInfo.serverStatus.refreshActive = qtrue;
	uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 1000;

}

static const char *UI_FeederItemText(float feederID, int index, int column, qhandle_t *handle) {
	static char info[MAX_STRING_CHARS];
	static char hostname[1024];
	static char clientBuff[32];
	static int lastColumn = -1;
	static int lastTime = 0;
	*handle = -1;
	if (feederID == FEEDER_HEADS) {
		int actual;
		return UI_SelectedHead(index, &actual);
	} else if (feederID == FEEDER_Q3HEADS) {
		if (index >= 0 && index < uiInfo.q3HeadCount) {
			return uiInfo.q3HeadNames[index];
		}
	} else if (feederID == FEEDER_MAPS || feederID == FEEDER_ALLMAPS) {
		int actual;
		return UI_SelectedMap(index, &actual);
	} else if (feederID == FEEDER_SERVERS) {
		if (index >= 0 && index < uiInfo.serverStatus.numDisplayServers) {
			int ping;
			if (lastColumn != column || lastTime > uiInfo.uiDC.realTime + 5000) {
				trap_LAN_GetServerInfo(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[index], info, MAX_STRING_CHARS);
				lastColumn = column;
				lastTime = uiInfo.uiDC.realTime;
			}
			ping = atoi(Info_ValueForKey(info, "ping"));
			if (ping == -1) {
				// if we ever see a ping that is out of date, do a server refresh
				// UI_UpdatePendingPings();
			}
			switch (column) {
				case COLUMN_HOST : 
					if (ping <= 0) {
						return Info_ValueForKey(info, "addr");
					} else {
						if ( ui_netSource.integer == UIAS_LOCAL ) {
							int nettype = atoi(Info_ValueForKey(info, "nettype"));

							if (nettype < 0 || nettype >= ARRAY_LEN(netnames)) {
								nettype = 0;
							}

							Com_sprintf( hostname, sizeof(hostname), "%s [%s]",
											Info_ValueForKey(info, "hostname"),
											netnames[nettype] );
							return hostname;
						}
						else {
							Com_sprintf( hostname, sizeof(hostname), "%s", Info_ValueForKey(info, "hostname"));
							return hostname;
						}
					}
				case COLUMN_MAP : return Info_ValueForKey(info, "mapname");
				case COLUMN_CLIENTS :
					{
						char *clients;

						if (ui_browserShowBots.integer == 0) {
							clients = Info_ValueForKey(info, "g_humanplayers");
						} else {
							clients = Info_ValueForKey(info, "clients");
						}

						Com_sprintf( clientBuff, sizeof(clientBuff), "%s (%s)", clients, Info_ValueForKey(info, "sv_maxclients"));
						return clientBuff;
					}
				case COLUMN_GAME :
					return Info_ValueForKey(info, "gametype");
				case COLUMN_PING : 
					if (ping <= 0) {
						return "...";
					} else {
						return Info_ValueForKey(info, "ping");
					}
			}
		}
	} else if (feederID == FEEDER_SERVERSTATUS) {
		if ( index >= 0 && index < uiInfo.serverStatusInfo.numLines ) {
			if ( column >= 0 && column < 4 ) {
				return uiInfo.serverStatusInfo.lines[index][column];
			}
		}
	} else if (feederID == FEEDER_FINDPLAYER) {
		if ( index >= 0 && index < uiInfo.numFoundPlayerServers ) {
			//return uiInfo.foundPlayerServerAddresses[index];
			return uiInfo.foundPlayerServerNames[index];
		}
	} else if (feederID == FEEDER_PLAYER_LIST) {
		if (index >= 0 && index < uiInfo.playerCount) {
			return uiInfo.playerNames[index];
		}
	} else if (feederID == FEEDER_TEAM_LIST) {
		if (index >= 0 && index < uiInfo.myTeamCount) {
			return uiInfo.teamPlayerNames[index];
		}
	} else if (feederID == FEEDER_MODS) {
		if (index >= 0 && index < uiInfo.modCount) {
			if (uiInfo.modList[index].modDescr && *uiInfo.modList[index].modDescr) {
				return uiInfo.modList[index].modDescr;
			} else {
				return uiInfo.modList[index].modName;
			}
		}
	} else if (feederID == FEEDER_CINEMATICS) {
		if (index >= 0 && index < uiInfo.movieCount) {
			return uiInfo.movieList[index];
		}
	} else if (feederID == FEEDER_DEMOS) {
		if (index >= 0 && index < uiInfo.demoCount) {
			return uiInfo.demoList[index];
		}
	}
	return "";
}


static qhandle_t UI_FeederItemImage(float feederID, int index) {
  if (feederID == FEEDER_HEADS) {
	int actual;
	UI_SelectedHead(index, &actual);
	index = actual;
	if (index >= 0 && index < uiInfo.characterCount) {
		if (uiInfo.characterList[index].headImage == -1) {
			uiInfo.characterList[index].headImage = trap_R_RegisterShaderNoMip(uiInfo.characterList[index].imageName);
		}
		return uiInfo.characterList[index].headImage;
	}
  } else if (feederID == FEEDER_Q3HEADS) {
    if (index >= 0 && index < uiInfo.q3HeadCount) {
      return uiInfo.q3HeadIcons[index];
    }
	} else if (feederID == FEEDER_ALLMAPS || feederID == FEEDER_MAPS) {
		int actual;
		UI_SelectedMap(index, &actual);
		index = actual;
		if (index >= 0 && index < uiInfo.mapCount) {
			if (uiInfo.mapList[index].levelShot == -1) {
				uiInfo.mapList[index].levelShot = trap_R_RegisterShaderNoMip(uiInfo.mapList[index].imageName);
			}
			return uiInfo.mapList[index].levelShot;
		}
	}
  return 0;
}

static void UI_FeederSelection(float feederID, int index) {
	static char info[MAX_STRING_CHARS];
  if (feederID == FEEDER_HEADS) {
	int actual;
	UI_SelectedHead(index, &actual);
	index = actual;
    if (index >= 0 && index < uiInfo.characterCount) {
		trap_Cvar_Set( "team_model", uiInfo.characterList[index].base);
		trap_Cvar_Set( "team_headmodel", va("*%s", uiInfo.characterList[index].name)); 
		updateModel = qtrue;
    }
  } else if (feederID == FEEDER_Q3HEADS) {
    if (index >= 0 && index < uiInfo.q3HeadCount) {
      trap_Cvar_Set( "model", uiInfo.q3HeadNames[index]);
      trap_Cvar_Set( "headmodel", uiInfo.q3HeadNames[index]);
			updateModel = qtrue;
		}
  } else if (feederID == FEEDER_MAPS || feederID == FEEDER_ALLMAPS) {
		int actual, map;
		map = (feederID == FEEDER_ALLMAPS) ? ui_currentNetMap.integer : ui_currentMap.integer;
		if (uiInfo.mapList[map].cinematic >= 0) {
		  trap_CIN_StopCinematic(uiInfo.mapList[map].cinematic);
		  uiInfo.mapList[map].cinematic = -1;
		}
		UI_SelectedMap(index, &actual);
		trap_Cvar_SetValue("ui_mapIndex", index);
		ui_mapIndex.integer = index;

		if (feederID == FEEDER_MAPS) {
			ui_currentMap.integer = actual;
			trap_Cvar_SetValue("ui_currentMap", actual);
	  	uiInfo.mapList[ui_currentMap.integer].cinematic = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.mapList[ui_currentMap.integer].mapLoadName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
			UI_LoadBestScores(uiInfo.mapList[ui_currentMap.integer].mapLoadName, uiInfo.gameTypes[ui_gameType.integer].gtEnum);
			trap_Cvar_Set("ui_opponentModel", uiInfo.mapList[ui_currentMap.integer].opponentName);
			updateOpponentModel = qtrue;
		} else {
			ui_currentNetMap.integer = actual;
			trap_Cvar_SetValue("ui_currentNetMap", actual);
	  	uiInfo.mapList[ui_currentNetMap.integer].cinematic = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.mapList[ui_currentNetMap.integer].mapLoadName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
		}

  } else if (feederID == FEEDER_SERVERS) {
		const char *mapName = NULL;
		uiInfo.serverStatus.currentServer = index;
		trap_LAN_GetServerInfo(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[index], info, MAX_STRING_CHARS);
		uiInfo.serverStatus.currentServerPreview = trap_R_RegisterShaderNoMip(va("levelshots/%s", Info_ValueForKey(info, "mapname")));
		if (uiInfo.serverStatus.currentServerCinematic >= 0) {
		  trap_CIN_StopCinematic(uiInfo.serverStatus.currentServerCinematic);
			uiInfo.serverStatus.currentServerCinematic = -1;
		}
		mapName = Info_ValueForKey(info, "mapname");
		if (mapName && *mapName) {
			uiInfo.serverStatus.currentServerCinematic = trap_CIN_PlayCinematic(va("%s.roq", mapName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
		}
  } else if (feederID == FEEDER_SERVERSTATUS) {
		//
  } else if (feederID == FEEDER_FINDPLAYER) {
	  uiInfo.currentFoundPlayerServer = index;
	  //
	  if ( index < uiInfo.numFoundPlayerServers-1) {
			// build a new server status for this server
			Q_strncpyz(uiInfo.serverStatusAddress, uiInfo.foundPlayerServerAddresses[uiInfo.currentFoundPlayerServer], sizeof(uiInfo.serverStatusAddress));
			Menu_SetFeederSelection(NULL, FEEDER_SERVERSTATUS, 0, NULL);
			UI_BuildServerStatus(qtrue);
	  }
  } else if (feederID == FEEDER_PLAYER_LIST) {
		uiInfo.playerIndex = index;
  } else if (feederID == FEEDER_TEAM_LIST) {
		uiInfo.teamIndex = index;
  } else if (feederID == FEEDER_MODS) {
		uiInfo.modIndex = index;
  } else if (feederID == FEEDER_CINEMATICS) {
		uiInfo.movieIndex = index;
		if (uiInfo.previewMovie >= 0) {
		  trap_CIN_StopCinematic(uiInfo.previewMovie);
		}
		uiInfo.previewMovie = -1;
  } else if (feederID == FEEDER_DEMOS) {
		uiInfo.demoIndex = index;
	}
}

static qboolean Team_Parse(char **p) {
  char *token;
  const char *tempStr;
	int i;

  token = COM_ParseExt(p, qtrue);

  if (token[0] != '{') {
    return qfalse;
  }

  while ( 1 ) {

    token = COM_ParseExt(p, qtrue);
    
    if (Q_stricmp(token, "}") == 0) {
      return qtrue;
    }

    if (!token[0]) {
      return qfalse;
    }

    if (token[0] == '{') {
      if (uiInfo.teamCount == MAX_TEAMS) {
        uiInfo.teamCount--;
        Com_Printf("Too many teams, last team replaced!\n");
      }

      // seven tokens per line, team name and icon, and 5 team member names
      if (!String_Parse(p, &uiInfo.teamList[uiInfo.teamCount].teamName) || !String_Parse(p, &tempStr)) {
        return qfalse;
      }
    

			uiInfo.teamList[uiInfo.teamCount].imageName = tempStr;
	    uiInfo.teamList[uiInfo.teamCount].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[uiInfo.teamCount].imageName);
		  uiInfo.teamList[uiInfo.teamCount].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[uiInfo.teamCount].imageName));
			uiInfo.teamList[uiInfo.teamCount].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[uiInfo.teamCount].imageName));

			uiInfo.teamList[uiInfo.teamCount].cinematic = -1;

			for (i = 0; i < TEAM_MEMBERS; i++) {
				uiInfo.teamList[uiInfo.teamCount].teamMembers[i] = NULL;
				if (!String_Parse(p, &uiInfo.teamList[uiInfo.teamCount].teamMembers[i])) {
					return qfalse;
				}
			}

      Com_DPrintf("Loaded team %s with team icon %s.\n", uiInfo.teamList[uiInfo.teamCount].teamName, tempStr);
      uiInfo.teamCount++;

      token = COM_ParseExt(p, qtrue);
      if (token[0] != '}') {
        return qfalse;
      }
    }
  }

  return qfalse;
}

static qboolean Character_Parse(char **p) {
  char *token;
  const char *tempStr;

  token = COM_ParseExt(p, qtrue);

  if (token[0] != '{') {
    return qfalse;
  }


  while ( 1 ) {
    token = COM_ParseExt(p, qtrue);

    if (Q_stricmp(token, "}") == 0) {
      return qtrue;
    }

    if (!token[0]) {
      return qfalse;
    }

    if (token[0] == '{') {
      if (uiInfo.characterCount == MAX_HEADS) {
        uiInfo.characterCount--;
        Com_Printf("Too many characters, last character replaced!\n");
      }

      // two tokens per line, character name and sex
      if (!String_Parse(p, &uiInfo.characterList[uiInfo.characterCount].name) || !String_Parse(p, &tempStr)) {
        return qfalse;
      }
    
      uiInfo.characterList[uiInfo.characterCount].headImage = -1;
			uiInfo.characterList[uiInfo.characterCount].imageName = String_Alloc(va("models/players/heads/%s/icon_default.tga", uiInfo.characterList[uiInfo.characterCount].name));

	  if (tempStr && (!Q_stricmp(tempStr, "female"))) {
        uiInfo.characterList[uiInfo.characterCount].base = String_Alloc(ui_defaultFemaleTeamModel.string);
      } else if (tempStr && (!Q_stricmp(tempStr, "male"))) {
        uiInfo.characterList[uiInfo.characterCount].base = String_Alloc(ui_defaultMaleTeamModel.string);
	  } else {
        uiInfo.characterList[uiInfo.characterCount].base = String_Alloc(tempStr);
	  }

      Com_DPrintf("Loaded %s character %s.\n", uiInfo.characterList[uiInfo.characterCount].base, uiInfo.characterList[uiInfo.characterCount].name);
      uiInfo.characterCount++;
     
      token = COM_ParseExt(p, qtrue);
      if (token[0] != '}') {
        return qfalse;
      }
    }
  }

  return qfalse;
}


static qboolean Alias_Parse(char **p) {
  char *token;

  token = COM_ParseExt(p, qtrue);

  if (token[0] != '{') {
    return qfalse;
  }

  while ( 1 ) {
    token = COM_ParseExt(p, qtrue);

    if (Q_stricmp(token, "}") == 0) {
      return qtrue;
    }

    if (!token[0]) {
      return qfalse;
    }

    if (token[0] == '{') {
      if (uiInfo.aliasCount == MAX_ALIASES) {
        uiInfo.aliasCount--;
        Com_Printf("Too many aliases, last alias replaced!\n");
      }

      // three tokens per line, character name, bot alias, and preferred action a - all purpose, d - defense, o - offense
      if (!String_Parse(p, &uiInfo.aliasList[uiInfo.aliasCount].name) || !String_Parse(p, &uiInfo.aliasList[uiInfo.aliasCount].ai) || !String_Parse(p, &uiInfo.aliasList[uiInfo.aliasCount].action)) {
        return qfalse;
      }
    
      Com_DPrintf("Loaded character alias %s using character ai %s.\n", uiInfo.aliasList[uiInfo.aliasCount].name, uiInfo.aliasList[uiInfo.aliasCount].ai);
      uiInfo.aliasCount++;
     
      token = COM_ParseExt(p, qtrue);
      if (token[0] != '}') {
        return qfalse;
      }
    }
  }

  return qfalse;
}



// mode 
// 0 - high level parsing
// 1 - team parsing
// 2 - character parsing
static void UI_ParseTeamInfo(const char *teamFile) {
	char	*token;
  char *p;
  char *buff = NULL;
  //static int mode = 0; TTimo: unused

  buff = GetMenuBuffer(teamFile);
  if (!buff) {
    return;
  }

  p = buff;

	while ( 1 ) {
		token = COM_ParseExt( &p, qtrue );
		if(!token[0] || token[0] == '}') {
			break;
		}

		if ( Q_stricmp( token, "}" ) == 0 ) {
      break;
    }

    if (Q_stricmp(token, "teams") == 0) {

      if (Team_Parse(&p)) {
        continue;
      } else {
        break;
      }
    }

    if (Q_stricmp(token, "characters") == 0) {
      Character_Parse(&p);
    }

    if (Q_stricmp(token, "aliases") == 0) {
      Alias_Parse(&p);
    }

  }
}

void UI_ValidateTeams( void ) {
	int i, j;

	// Check for unknown team characters
	for (i = 0; i < uiInfo.teamCount; i++) {
		for (j = 0; j < TEAM_MEMBERS; j++) {
			UI_AIFromName(uiInfo.teamList[i].teamMembers[j]);
		}
	}
}


static qboolean GameType_Parse(char **p, qboolean join) {
	char *token;

	token = COM_ParseExt(p, qtrue);

	if (token[0] != '{') {
		return qfalse;
	}

	if (join) {
		uiInfo.numJoinGameTypes = 0;
	} else {
		uiInfo.numGameTypes = 0;
	}

	while ( 1 ) {
		token = COM_ParseExt(p, qtrue);

		if (Q_stricmp(token, "}") == 0) {
			return qtrue;
		}

		if (!token[0]) {
			return qfalse;
		}

		if (token[0] == '{') {
			// two tokens per line, gametype name and number
			if (join) {
				if (!String_Parse(p, &uiInfo.joinGameTypes[uiInfo.numJoinGameTypes].gameType) || !Int_Parse(p, &uiInfo.joinGameTypes[uiInfo.numJoinGameTypes].gtEnum)) {
					return qfalse;
				}
			} else {
				if (!String_Parse(p, &uiInfo.gameTypes[uiInfo.numGameTypes].gameType) || !Int_Parse(p, &uiInfo.gameTypes[uiInfo.numGameTypes].gtEnum)) {
					return qfalse;
				}
			}
    
			if (join) {
				if (uiInfo.numJoinGameTypes < MAX_GAMETYPES) {
					uiInfo.numJoinGameTypes++;
				} else {
					Com_Printf("Too many net game types, last one replace!\n");
				}		
			} else {
				if (uiInfo.numGameTypes < MAX_GAMETYPES) {
					uiInfo.numGameTypes++;
				} else {
					Com_Printf("Too many game types, last one replace!\n");
				}		
			}
     
			token = COM_ParseExt(p, qtrue);
			if (token[0] != '}') {
				return qfalse;
			}
		}
	}
	return qfalse;
}

static qboolean MapList_Parse(char **p) {
	char *token;

	token = COM_ParseExt(p, qtrue);

	if (token[0] != '{') {
		return qfalse;
	}

	uiInfo.mapCount = 0;

	while ( 1 ) {
		token = COM_ParseExt(p, qtrue);

		if (Q_stricmp(token, "}") == 0) {
			return qtrue;
		}

		if (!token[0]) {
			return qfalse;
		}

		if (token[0] == '{') {
			if (!String_Parse(p, &uiInfo.mapList[uiInfo.mapCount].mapName) || !String_Parse(p, &uiInfo.mapList[uiInfo.mapCount].mapLoadName) 
				||!Int_Parse(p, &uiInfo.mapList[uiInfo.mapCount].teamMembers) ) {
				return qfalse;
			}

			if (!String_Parse(p, &uiInfo.mapList[uiInfo.mapCount].opponentName)) {
				return qfalse;
			}

			uiInfo.mapList[uiInfo.mapCount].typeBits = 0;

			while (1) {
				token = COM_ParseExt(p, qtrue);
				if (token[0] >= '0' && token[0] <= '9') {
					uiInfo.mapList[uiInfo.mapCount].typeBits |= (1 << (token[0] - 0x030));
					if (!Int_Parse(p, &uiInfo.mapList[uiInfo.mapCount].timeToBeat[token[0] - 0x30])) {
						return qfalse;
					}
				} else {
					break;
				} 
			}

			//mapList[mapCount].imageName = String_Alloc(va("levelshots/%s", mapList[mapCount].mapLoadName));
			//if (uiInfo.mapCount == 0) {
			  // only load the first cinematic, selection loads the others
  			//  uiInfo.mapList[uiInfo.mapCount].cinematic = trap_CIN_PlayCinematic(va("%s.roq",uiInfo.mapList[uiInfo.mapCount].mapLoadName), qfalse, qfalse, qtrue, 0, 0, 0, 0);
			//}
  		uiInfo.mapList[uiInfo.mapCount].cinematic = -1;
			uiInfo.mapList[uiInfo.mapCount].levelShot = trap_R_RegisterShaderNoMip(va("levelshots/%s_small", uiInfo.mapList[uiInfo.mapCount].mapLoadName));

			if (uiInfo.mapCount < MAX_MAPS) {
				uiInfo.mapCount++;
			} else {
				Com_Printf("Too many maps, last one replaced!\n");
			}
		}
	}
	return qfalse;
}

static void UI_ParseGameInfo(const char *teamFile) {
	char	*token;
	char *p;
	char *buff = NULL;
	//int mode = 0; TTimo: unused

	buff = GetMenuBuffer(teamFile);
	if (!buff) {
		return;
	}

	p = buff;

	while ( 1 ) {
		token = COM_ParseExt( &p, qtrue );
		if(!token[0] || token[0] == '}') {
			break;
		}

		if ( Q_stricmp( token, "}" ) == 0 ) {
			break;
		}

		if (Q_stricmp(token, "gametypes") == 0) {

			if (GameType_Parse(&p, qfalse)) {
				continue;
			} else {
				break;
			}
		}

		if (Q_stricmp(token, "joingametypes") == 0) {

			if (GameType_Parse(&p, qtrue)) {
				continue;
			} else {
				break;
			}
		}

		if (Q_stricmp(token, "maps") == 0) {
			// start a new menu
			MapList_Parse(&p);
		}

	}

	trap_Cvar_CheckRange( "ui_netGameType", 0, uiInfo.numGameTypes-1, qtrue );
	trap_Cvar_CheckRange( "ui_joinGameType", 0, uiInfo.numJoinGameTypes-1, qtrue );
}

static void UI_Pause(qboolean b) {
	if (b) {
		// pause the game and set the ui keycatcher
		UI_EnterMenu();
	} else {
		// unpause the game and clear the ui keycatcher
		UI_ForceMenuOff();
	}
}

static int UI_PlayCinematic(const char *name, float x, float y, float w, float h) {
	CG_AdjustFrom640( &x, &y, &w, &h );
  return trap_CIN_PlayCinematic(name, x, y, w, h, (CIN_loop | CIN_silent));
}

static void UI_StopCinematic(int handle) {
	if (handle >= 0) {
	  trap_CIN_StopCinematic(handle);
	} else {
		handle = abs(handle);
		if (handle == UI_MAPCINEMATIC) {
			if (uiInfo.mapList[ui_currentMap.integer].cinematic >= 0) {
			  trap_CIN_StopCinematic(uiInfo.mapList[ui_currentMap.integer].cinematic);
			  uiInfo.mapList[ui_currentMap.integer].cinematic = -1;
			}
		} else if (handle == UI_NETMAPCINEMATIC) {
			if (uiInfo.serverStatus.currentServerCinematic >= 0) {
			  trap_CIN_StopCinematic(uiInfo.serverStatus.currentServerCinematic);
				uiInfo.serverStatus.currentServerCinematic = -1;
			}
		} else if (handle == UI_CLANCINEMATIC) {
		  int i = UI_TeamIndexFromName(CG_Cvar_VariableString("ui_teamName"));
		  if (i >= 0 && i < uiInfo.teamCount) {
				if (uiInfo.teamList[i].cinematic >= 0) {
				  trap_CIN_StopCinematic(uiInfo.teamList[i].cinematic);
					uiInfo.teamList[i].cinematic = -1;
				}
			}
		}
	}
}

static void UI_DrawCinematic(int handle, float x, float y, float w, float h) {
	CG_AdjustFrom640( &x, &y, &w, &h );
	trap_CIN_SetExtents(handle, x, y, w, h);
  trap_CIN_DrawCinematic(handle);
}

static void UI_RunCinematicFrame(int handle) {
  trap_CIN_RunCinematic(handle);
}



/*
=================
PlayerModel_BuildList
=================
*/
static void UI_BuildQ3Model_List( void )
{
	int		numdirs;
	int		numfiles;
	char	dirlist[2048];
	char	filelist[2048];
	char	skinname[MAX_QPATH];
	char	scratch[256];
	char*	dirptr;
	char*	fileptr;
	int		i;
	int		j, k, dirty;
	int		dirlen;
	int		filelen;

	uiInfo.q3HeadCount = 0;

	// iterate directory of all player models
	numdirs = trap_FS_GetFileList("models/players", "/", dirlist, 2048 );
	dirptr  = dirlist;
	for (i=0; i<numdirs && uiInfo.q3HeadCount < MAX_PLAYERMODELS; i++,dirptr+=dirlen+1)
	{
		dirlen = strlen(dirptr);
		
		if (dirlen && dirptr[dirlen-1]=='/') dirptr[dirlen-1]='\0';

		if (!strcmp(dirptr,".") || !strcmp(dirptr,".."))
			continue;
			
		// iterate all skin files in directory
		numfiles = trap_FS_GetFileList( va("models/players/%s",dirptr), "$images", filelist, 2048 );
		fileptr  = filelist;
		for (j=0; j<numfiles && uiInfo.q3HeadCount < MAX_PLAYERMODELS;j++,fileptr+=filelen+1)
		{
			filelen = strlen(fileptr);

			COM_StripExtension(fileptr, skinname, sizeof(skinname));

			// look for icon_????
			if (Q_stricmpn(skinname, "icon_", 5) == 0 && !(Q_stricmp(skinname,"icon_blue") == 0 || Q_stricmp(skinname,"icon_red") == 0))
			{
				if (Q_stricmp(skinname, "icon_default") == 0) {
					Com_sprintf( scratch, sizeof(scratch), "%s", dirptr);
				} else {
					Com_sprintf( scratch, sizeof(scratch), "%s/%s",dirptr, skinname + 5);
				}
				dirty = 0;
				for(k=0;k<uiInfo.q3HeadCount;k++) {
					if (!Q_stricmp(scratch, uiInfo.q3HeadNames[uiInfo.q3HeadCount])) {
						dirty = 1;
						break;
					}
				}
				if (!dirty) {
					Com_sprintf( uiInfo.q3HeadNames[uiInfo.q3HeadCount], sizeof(uiInfo.q3HeadNames[uiInfo.q3HeadCount]), "%s", scratch);
					uiInfo.q3HeadIcons[uiInfo.q3HeadCount++] = trap_R_RegisterShaderNoMip(va("models/players/%s/%s",dirptr,skinname));
				}
			}

		}
	}	

}


static float UI_Cvar_Get(const char *cvar) {
	char buff[128];
	memset(buff, 0, sizeof(buff));
	trap_Cvar_LatchedVariableStringBuffer(cvar, buff, sizeof(buff));
	return atof(buff);
}


/*
=================
UI_Init
=================
*/
void UI_Init( qboolean inGameLoad, int maxSplitView ) {
	const char *menuSet;

	uiInfo.maxSplitView = Com_Clamp(1, MAX_SPLITVIEW, maxSplitView);
	//uiInfo.inGameLoad = inGameLoad;

	UI_RegisterCvars();
	UI_InitMemory();

  //UI_Load();
	uiInfo.uiDC.registerShaderNoMip = &trap_R_RegisterShaderNoMip;
	uiInfo.uiDC.setColor = &trap_R_SetColor;
	uiInfo.uiDC.drawHandlePic = &CG_DrawPic;
	uiInfo.uiDC.drawStretchPic = &trap_R_DrawStretchPic;
	uiInfo.uiDC.drawText = &UI_Text_Paint;
	uiInfo.uiDC.textWidth = &UI_Text_Width;
	uiInfo.uiDC.textHeight = &UI_Text_Height;
	uiInfo.uiDC.registerModel = &trap_R_RegisterModel;
	uiInfo.uiDC.modelBounds = &trap_R_ModelBounds;
	uiInfo.uiDC.fillRect = &CG_FillRect;
	uiInfo.uiDC.drawRect = &CG_DrawRect;
	uiInfo.uiDC.drawSides = &CG_DrawSides;
	uiInfo.uiDC.drawTopBottom = &CG_DrawTopBottom;
	uiInfo.uiDC.clearScene = &trap_R_ClearScene;
	uiInfo.uiDC.addRefEntityToScene = &CG_AddRefEntityWithMinLight;
	uiInfo.uiDC.renderScene = &trap_R_RenderScene;
	uiInfo.uiDC.registerFont = &trap_R_RegisterFont;
	uiInfo.uiDC.ownerDrawItem = &UI_OwnerDraw;
	uiInfo.uiDC.getValue = &UI_GetValue;
	uiInfo.uiDC.ownerDrawVisible = &UI_OwnerDrawVisible;
	uiInfo.uiDC.runScript = &UI_RunMenuScript;
	uiInfo.uiDC.getTeamColor = &UI_GetTeamColor;
	uiInfo.uiDC.setCVar = trap_Cvar_Set;
	uiInfo.uiDC.getCVarString = trap_Cvar_LatchedVariableStringBuffer;
	uiInfo.uiDC.getCVarValue = UI_Cvar_Get;
	uiInfo.uiDC.drawTextWithCursor = &UI_Text_PaintWithCursor;
	uiInfo.uiDC.setOverstrikeMode = &trap_Key_SetOverstrikeMode;
	uiInfo.uiDC.getOverstrikeMode = &trap_Key_GetOverstrikeMode;
	uiInfo.uiDC.startLocalSound = &trap_S_StartLocalSound;
	uiInfo.uiDC.ownerDrawHandleKey = &UI_OwnerDrawHandleKey;
	uiInfo.uiDC.feederCount = &UI_FeederCount;
	uiInfo.uiDC.feederItemImage = &UI_FeederItemImage;
	uiInfo.uiDC.feederItemText = &UI_FeederItemText;
	uiInfo.uiDC.feederSelection = &UI_FeederSelection;
	uiInfo.uiDC.setBinding = &trap_Key_SetBinding;
	uiInfo.uiDC.getBindingBuf = &trap_Key_GetBindingBuf;
	uiInfo.uiDC.keynumToStringBuf = &trap_Key_KeynumToStringBuf;
	uiInfo.uiDC.getKey = &trap_Key_GetKey;
	uiInfo.uiDC.executeText = &trap_Cmd_ExecuteText;
	uiInfo.uiDC.Error = &Com_Error; 
	uiInfo.uiDC.Print = &Com_Printf; 
	uiInfo.uiDC.Pause = &UI_Pause;
	uiInfo.uiDC.ownerDrawWidth = &UI_OwnerDrawWidth;
	uiInfo.uiDC.registerSound = &trap_S_RegisterSound;
	uiInfo.uiDC.startBackgroundTrack = &trap_S_StartBackgroundTrack;
	uiInfo.uiDC.stopBackgroundTrack = &trap_S_StopBackgroundTrack;
	uiInfo.uiDC.playCinematic = &UI_PlayCinematic;
	uiInfo.uiDC.stopCinematic = &UI_StopCinematic;
	uiInfo.uiDC.drawCinematic = &UI_DrawCinematic;
	uiInfo.uiDC.runCinematicFrame = &UI_RunCinematicFrame;
	uiInfo.uiDC.adjustFrom640 = &CG_AdjustFrom640;
	uiInfo.uiDC.setScreenPlacement = &CG_SetScreenPlacement;
	uiInfo.uiDC.popScreenPlacement = &CG_PopScreenPlacement;

	Init_Display(&uiInfo.uiDC);

	String_Init();
  
	uiInfo.uiDC.cursor	= trap_R_RegisterShaderNoMip( "menu/art/3_cursor2" );
	uiInfo.uiDC.whiteShader = trap_R_RegisterShaderNoMip( "white" );

	AssetCache();

  uiInfo.teamCount = 0;
  uiInfo.characterCount = 0;
  uiInfo.aliasCount = 0;

#ifdef PRE_RELEASE_TADEMO
	UI_ParseTeamInfo("demoteaminfo.txt");
	UI_ParseGameInfo("demogameinfo.txt");
#else
	UI_ParseTeamInfo("teaminfo.txt");
	UI_LoadTeams();
	UI_ParseGameInfo("gameinfo.txt");
	UI_LoadArenas();
#endif
	UI_ValidateTeams();

	menuSet = CG_Cvar_VariableString("ui_menuFiles");
	if (menuSet == NULL || menuSet[0] == '\0') {
		menuSet = "ui/menus.txt";
	}

#if 0
	if (uiInfo.inGameLoad) {
		UI_LoadMenus("ui/ingame.txt", qtrue);
	} else {
	}
#else 
	UI_LoadMenus(menuSet, qtrue);
	UI_LoadMenus("ui/ingame.txt", qfalse);
#endif
	
	Menus_CloseAll();

	trap_LAN_LoadCachedServers();
	UI_LoadBestScores(uiInfo.mapList[ui_currentMap.integer].mapLoadName, uiInfo.gameTypes[ui_gameType.integer].gtEnum);

	UI_BuildQ3Model_List();
	UI_LoadBots();

	// sets defaults for ui temp cvars
	uiInfo.effectsColor = gamecodetoui[(int)trap_Cvar_VariableValue("color1")-1];
	uiInfo.currentCrosshair = (int)trap_Cvar_VariableValue("cg_drawCrosshair") % NUM_CROSSHAIRS;
	if (uiInfo.currentCrosshair < 0) {
		uiInfo.currentCrosshair = 0;
	}
	trap_Cvar_SetValue( "ui_mousePitch", (trap_Cvar_VariableValue("m_pitch") >= 0) ? 0 : 1 );

	uiInfo.serverStatus.currentServerCinematic = -1;
	uiInfo.previewMovie = -1;

	if ( trap_Cvar_VariableValue("ui_TeamArenaFirstRun") == 0 ) {
		trap_Cvar_SetValue( "s_volume", 0.8 );
		trap_Cvar_SetValue( "s_musicvolume", 0.5 );
		trap_Cvar_SetValue( "ui_TeamArenaFirstRun", 1 );
	}

	trap_Cvar_Register(NULL, "debug_protocol", "", 0 );

	trap_Cvar_SetValue("ui_actualNetGameType", ui_netGameType.integer);
}


/*
=================
UI_KeyEvent
=================
*/
void UI_KeyEvent( int key, qboolean down ) {

	Init_Display(&uiInfo.uiDC);

  if (Menu_Count() > 0) {
    menuDef_t *menu = Menu_GetFocused();
		if (menu) {
			if (key == K_ESCAPE && down && !Menus_AnyFullScreenVisible()) {
				Menus_CloseAll();
			} else {
				Menu_HandleKey(menu, key, down );
			}
		} else {
			UI_ForceMenuOff();
		}
  }

  //if ((s > 0) && (s != menu_null_sound)) {
	//  trap_S_StartLocalSound( s, CHAN_LOCAL_SOUND );
  //}
}

/*
=================
UI_MouseEvent
=================
*/
void UI_MouseEvent( int localPlayerNum, int dx, int dy )
{
	float ax, ay, aw, ah;
	int xbias, ybias;

	if ( localPlayerNum != 0 ) {
		// ui currently only supports one cursor
		return;
	}

	ax = 0;
	ay = 0;
	aw = 1;
	ah = 1;
	CG_AdjustFrom640( &ax, &ay, &aw, &ah );

	xbias = ax/aw;
	ybias = ay/ah;

	// update mouse screen position
	uiInfo.uiDC.cursorx = Com_Clamp( -xbias, SCREEN_WIDTH+xbias, uiInfo.uiDC.cursorx + dx );
	uiInfo.uiDC.cursory = Com_Clamp( -ybias, SCREEN_HEIGHT+ybias, uiInfo.uiDC.cursory + dy );

	Init_Display(&uiInfo.uiDC);

  if (Menu_Count() > 0) {
    //menuDef_t *menu = Menu_GetFocused();
    //Menu_HandleMouseMove(menu, uiInfo.uiDC.cursorx, uiInfo.uiDC.cursory);
		Display_MouseMove(NULL, uiInfo.uiDC.cursorx, uiInfo.uiDC.cursory);
  }

}

/*
=================
UI_GetCursorPos
=================
*/
void UI_GetCursorPos( int localPlayerNum, int *x, int *y )
{
	if (localPlayerNum != 0) {
		// ui currently only supports one cursor
		if ( x ) *x = 0;
		if ( y ) *y = 0;
	}

	if ( x ) *x = uiInfo.uiDC.cursorx;
	if ( y ) *y = uiInfo.uiDC.cursory;
}

/*
=================
UI_SetCursorPos
=================
*/
void UI_SetCursorPos( int localPlayerNum, int x, int y )
{
	if (localPlayerNum != 0) {
		// ui currently only supports one cursor
		return;
	}

	if ( uiInfo.uiDC.cursorx == x && uiInfo.uiDC.cursory == y ) {
		// ignore duplicate events
		return;
	}

	uiInfo.uiDC.cursorx = x;
	uiInfo.uiDC.cursory = y;

	Init_Display(&uiInfo.uiDC);

	if( Menu_Count( ) > 0 ) {
		Display_MouseMove( NULL, uiInfo.uiDC.cursorx, uiInfo.uiDC.cursory );
	}
}

void UI_LoadNonIngame( void ) {
	const char *menuSet = CG_Cvar_VariableString("ui_menuFiles");
	if (menuSet == NULL || menuSet[0] == '\0') {
		menuSet = "ui/menus.txt";
	}
	UI_LoadMenus(menuSet, qfalse);
	uiInfo.inGameLoad = qfalse;
}

void UI_SetActiveMenu( uiMenuCommand_t menu ) {
	char buf[256];

	Init_Display(&uiInfo.uiDC);

	// this should be the ONLY way the menu system is brought up
	// enusure minimum menu data is cached
	if (Menu_Count() <= 0) {
		return;
	}

	switch ( menu ) {
		case UIMENU_NONE:
			UI_ForceMenuOff();
			return;
		case UIMENU_MAIN:
			CG_KillServer();
			UI_EnterMenu();
			//trap_S_StartLocalSound( trap_S_RegisterSound("sound/misc/menu_background.wav", qfalse) , CHAN_LOCAL_SOUND );
			//trap_S_StartBackgroundTrack("sound/misc/menu_background.wav", NULL, 1.0f, 1.0f);
			if (uiInfo.inGameLoad) {
				UI_LoadNonIngame();
			}
			Menus_CloseAll();
			Menus_ActivateByName("main");
			trap_Cvar_VariableStringBuffer("com_errorMessage", buf, sizeof(buf));
			if (strlen(buf)) {
				if (!ui_singlePlayerActive.integer) {
					Menus_ActivateByName("error_popmenu");
				} else {
					trap_Cvar_Set("com_errorMessage", "");
				}
			}
			return;
		case UIMENU_TEAM:
			UI_EnterMenu();
			Menus_ActivateByName("team");
			return;
		case UIMENU_POSTGAME:
			CG_KillServer();
			UI_EnterMenu();
			if (uiInfo.inGameLoad) {
				UI_LoadNonIngame();
			}
			Menus_CloseAll();
			Menus_ActivateByName("endofgame");
			return;
		case UIMENU_INGAME:
			UI_EnterMenu();
			UI_BuildPlayerList();
			Menus_CloseAll();
			Menus_ActivateByName("ingame");
			return;
		default:
			Com_Printf( "UI_SetActiveMenu: unknown enum %d\n", menu );
	}
}

qboolean UI_IsFullscreen( void ) {
	Init_Display(&uiInfo.uiDC);

	return Menus_AnyFullScreenVisible();
}



static connstate_t	lastConnState;
static char			lastLoadingText[MAX_INFO_VALUE];

static void UI_ReadableSize ( char *buf, int bufsize, int value )
{
	if (value > 1024*1024*1024 ) { // gigs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d GB", 
			(value % (1024*1024*1024))*100 / (1024*1024*1024) );
	} else if (value > 1024*1024 ) { // megs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d MB", 
			(value % (1024*1024))*100 / (1024*1024) );
	} else if (value > 1024 ) { // kilos
		Com_sprintf( buf, bufsize, "%d KB", value / 1024 );
	} else { // bytes
		Com_sprintf( buf, bufsize, "%d bytes", value );
	}
}

// Assumes time is in msec
static void UI_PrintTime ( char *buf, int bufsize, int time ) {
	time /= 1000;  // change to seconds

	if (time > 3600) { // in the hours range
		Com_sprintf( buf, bufsize, "%d hr %d min", time / 3600, (time % 3600) / 60 );
	} else if (time > 60) { // mins
		Com_sprintf( buf, bufsize, "%d min %d sec", time / 60, time % 60 );
	} else  { // secs
		Com_sprintf( buf, bufsize, "%d sec", time );
	}
}

void UI_Text_PaintCenter(float x, float y, float scale, vec4_t color, const char *text, float adjust) {
	int len = UI_Text_Width(text, scale, 0);
	UI_Text_Paint(x - len / 2, y, scale, color, text, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
}

void UI_Text_PaintCenter_AutoWrapped(float x, float y, float xmax, float ystep, float scale, vec4_t color, const char *str, float adjust) {
	int width;
	char *s1,*s2,*s3;
	char c_bcp;
	char buf[1024];

	if (!str || str[0]=='\0')
		return;

	Q_strncpyz(buf, str, sizeof(buf));
	s1 = s2 = s3 = buf;

	while (1) {
		do {
			s3++;
		} while (*s3!=' ' && *s3!='\0');
		c_bcp = *s3;
		*s3 = '\0';
		width = UI_Text_Width(s1, scale, 0);
		*s3 = c_bcp;
		if (width > xmax) {
			if (s1==s2)
			{
				// fuck, don't have a clean cut, we'll overflow
				s2 = s3;
			}
			*s2 = '\0';
			UI_Text_PaintCenter(x, y, scale, color, s1, adjust);
			y += ystep;
			if (c_bcp == '\0')
      {
				// that was the last word
        // we could start a new loop, but that wouldn't be much use
        // even if the word is too long, we would overflow it (see above)
        // so just print it now if needed
        s2++;
        if (*s2 != '\0') // if we are printing an overflowing line we have s2 == s3
          UI_Text_PaintCenter(x, y, scale, color, s2, adjust);
        break;
      }
			s2++;
			s1 = s2;
			s3 = s2;
		}
		else
		{
			s2 = s3;
			if (c_bcp == '\0') // we reached the end
			{
				UI_Text_PaintCenter(x, y, scale, color, s1, adjust);
				break;
			}
		}
	}
}

static void UI_DisplayDownloadInfo( const char *downloadName, float centerPoint, float yStart, float scale ) {
	static char dlText[]	= "Downloading:";
	static char etaText[]	= "Estimated time left:";
	static char xferText[]	= "Transfer rate:";

	int downloadSize, downloadCount, downloadTime;
	char dlSizeBuf[64], totalSizeBuf[64], xferRateBuf[64], dlTimeBuf[64];
	int xferRate;
	int leftWidth;
	const char *s;

	downloadSize = trap_Cvar_VariableValue( "cl_downloadSize" );
	downloadCount = trap_Cvar_VariableValue( "cl_downloadCount" );
	downloadTime = trap_Cvar_VariableValue( "cl_downloadTime" );

	leftWidth = 320;

	trap_R_SetColor(colorWhite);
	UI_Text_PaintCenter(centerPoint, yStart + 112, scale, colorWhite, dlText, 0);
	UI_Text_PaintCenter(centerPoint, yStart + 192, scale, colorWhite, etaText, 0);
	UI_Text_PaintCenter(centerPoint, yStart + 248, scale, colorWhite, xferText, 0);

	if (downloadSize > 0) {
		s = va( "%s (%d%%)", downloadName,
			(int)( (float)downloadCount * 100.0f / downloadSize ) );
	} else {
		s = downloadName;
	}

	UI_Text_PaintCenter(centerPoint, yStart+136, scale, colorWhite, s, 0);

	UI_ReadableSize( dlSizeBuf,		sizeof dlSizeBuf,		downloadCount );
	UI_ReadableSize( totalSizeBuf,	sizeof totalSizeBuf,	downloadSize );

	if (downloadCount < 4096 || !downloadTime) {
		UI_Text_PaintCenter(leftWidth, yStart+216, scale, colorWhite, "estimating", 0);
		UI_Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s of %s copied)", dlSizeBuf, totalSizeBuf), 0);
	} else {
		if ((uiInfo.uiDC.realTime - downloadTime) / 1000) {
			xferRate = downloadCount / ((uiInfo.uiDC.realTime - downloadTime) / 1000);
		} else {
			xferRate = 0;
		}
		UI_ReadableSize( xferRateBuf, sizeof xferRateBuf, xferRate );

		// Extrapolate estimated completion time
		if (downloadSize && xferRate) {
			int n = downloadSize / xferRate; // estimated time for entire d/l in secs

			// We do it in K (/1024) because we'd overflow around 4MB
			UI_PrintTime ( dlTimeBuf, sizeof dlTimeBuf, 
				(n - (((downloadCount/1024) * n) / (downloadSize/1024))) * 1000);

			UI_Text_PaintCenter(leftWidth, yStart+216, scale, colorWhite, dlTimeBuf, 0);
			UI_Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s of %s copied)", dlSizeBuf, totalSizeBuf), 0);
		} else {
			UI_Text_PaintCenter(leftWidth, yStart+216, scale, colorWhite, "estimating", 0);
			if (downloadSize) {
				UI_Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s of %s copied)", dlSizeBuf, totalSizeBuf), 0);
			} else {
				UI_Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s copied)", dlSizeBuf), 0);
			}
		}

		if (xferRate) {
			UI_Text_PaintCenter(leftWidth, yStart+272, scale, colorWhite, va("%s/Sec", xferRateBuf), 0);
		}
	}
}

/*
========================
UI_DrawConnectScreen

This will also be overlaid on the cgame info screen during loading
to prevent it from blinking away too rapidly on local or lan games.
========================
*/
void UI_DrawConnectScreen( qboolean overlay ) {
	char			*s;
	uiClientState_t	cstate;
	char			info[MAX_INFO_VALUE];
	char text[256];
	float centerPoint, yStart, scale;
	
	menuDef_t *menu = Menus_FindByName("Connect");


	if ( !overlay && menu ) {
		CG_ClearViewport();
		Menu_Paint(menu, qtrue);
	}

	if (!overlay) {
		centerPoint = 320;
		yStart = 130;
		scale = 0.5f;
	} else {
		return;
	}

	// see what information we should display
	trap_GetClientState( &cstate );

	info[0] = '\0';
	if( trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) ) ) {
		UI_Text_PaintCenter(centerPoint, yStart, scale, colorWhite, va( "Loading %s", Info_ValueForKey( info, "mapname" )), 0);
	}

	if (!Q_stricmp(cstate.servername,"localhost")) {
		UI_Text_PaintCenter(centerPoint, yStart + 48, scale, colorWhite, "Starting up...", ITEM_TEXTSTYLE_SHADOWEDMORE);
	} else {
		Com_sprintf(text, sizeof(text), "Connecting to %s", cstate.servername);
		UI_Text_PaintCenter(centerPoint, yStart + 48, scale, colorWhite,text , ITEM_TEXTSTYLE_SHADOWEDMORE);
	}

	// display global MOTD at bottom
	UI_Text_PaintCenter(centerPoint, 600, scale, colorWhite, Info_ValueForKey( cstate.updateInfoString, "motd" ), 0);
	// print any server info (server full, bad version, etc)
	if ( cstate.connState < CA_CONNECTED ) {
		UI_Text_PaintCenter_AutoWrapped(centerPoint, yStart + 176, 630, 20, scale, colorWhite, cstate.messageString, 0);
	}

	if ( lastConnState > cstate.connState ) {
		lastLoadingText[0] = '\0';
	}
	lastConnState = cstate.connState;

	switch ( cstate.connState ) {
	case CA_CONNECTING:
		s = va("Awaiting connection...%i", cstate.connectPacketCount);
		break;
	case CA_CHALLENGING:
		s = va("Awaiting challenge...%i", cstate.connectPacketCount);
		break;
	case CA_CONNECTED: {
		char downloadName[MAX_INFO_VALUE];

			trap_Cvar_VariableStringBuffer( "cl_downloadName", downloadName, sizeof(downloadName) );
			if (*downloadName) {
				UI_DisplayDownloadInfo( downloadName, centerPoint, yStart, scale );
				return;
			}
		}
		s = "Awaiting gamestate...";
		break;
	case CA_LOADING:
		return;
	case CA_PRIMED:
		return;
	default:
		return;
	}


	if (Q_stricmp(cstate.servername,"localhost")) {
		UI_Text_PaintCenter(centerPoint, yStart + 80, scale, colorWhite, s, 0);
	}

	// password required / connection rejected information goes here
}


/*
================
cvars
================
*/

typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
} cvarTable_t;

vmCvar_t	ui_arenasFile;
vmCvar_t	ui_botsFile;
vmCvar_t	ui_spSkill;

vmCvar_t	ui_browserShowFull;
vmCvar_t	ui_browserShowEmpty;
vmCvar_t	ui_browserShowBots;
vmCvar_t	ui_browserSeparateMasters;

vmCvar_t	ui_brassTime;
vmCvar_t	ui_drawCrosshair;
vmCvar_t	ui_drawCrosshairNames;
vmCvar_t	ui_marks;

vmCvar_t	ui_redteam;
vmCvar_t	ui_redteam1;
vmCvar_t	ui_redteam2;
vmCvar_t	ui_redteam3;
vmCvar_t	ui_redteam4;
vmCvar_t	ui_redteam5;
vmCvar_t	ui_blueteam;
vmCvar_t	ui_blueteam1;
vmCvar_t	ui_blueteam2;
vmCvar_t	ui_blueteam3;
vmCvar_t	ui_blueteam4;
vmCvar_t	ui_blueteam5;
vmCvar_t	ui_teamName;
vmCvar_t	ui_dedicated;
vmCvar_t	ui_gameType;
vmCvar_t	ui_netGameType;
vmCvar_t	ui_actualNetGameType;
vmCvar_t	ui_joinGameType;
vmCvar_t	ui_netSource;
vmCvar_t	ui_serverFilterType;
vmCvar_t	ui_opponentName;
vmCvar_t	ui_menuFiles;
vmCvar_t	ui_currentTier;
vmCvar_t	ui_currentMap;
vmCvar_t	ui_currentNetMap;
vmCvar_t	ui_mapIndex;
vmCvar_t	ui_currentOpponent;
vmCvar_t	ui_selectedPlayer;
vmCvar_t	ui_selectedPlayerName;
vmCvar_t	ui_lastServerRefresh_0;
vmCvar_t	ui_lastServerRefresh_1;
vmCvar_t	ui_lastServerRefresh_2;
vmCvar_t	ui_lastServerRefresh_3;
vmCvar_t	ui_lastServerRefresh_4;
vmCvar_t	ui_lastServerRefresh_5;
vmCvar_t	ui_lastServerRefresh_6;
vmCvar_t	ui_singlePlayerActive;
vmCvar_t	ui_scoreAccuracy;
vmCvar_t	ui_scoreImpressives;
vmCvar_t	ui_scoreExcellents;
vmCvar_t	ui_scoreCaptures;
vmCvar_t	ui_scoreDefends;
vmCvar_t	ui_scoreAssists;
vmCvar_t	ui_scoreGauntlets;
vmCvar_t	ui_scoreScore;
vmCvar_t	ui_scorePerfect;
vmCvar_t	ui_scoreTeam;
vmCvar_t	ui_scoreBase;
vmCvar_t	ui_scoreTimeBonus;
vmCvar_t	ui_scoreSkillBonus;
vmCvar_t	ui_scoreShutoutBonus;
vmCvar_t	ui_scoreTime;
vmCvar_t	ui_captureLimit;
vmCvar_t	ui_fragLimit;
vmCvar_t	ui_smallFont;
vmCvar_t	ui_bigFont;
vmCvar_t	ui_findPlayer;
vmCvar_t	ui_Q3Model;
vmCvar_t	ui_hudFiles;
vmCvar_t	ui_recordSPDemo;
vmCvar_t	ui_serverStatusTimeOut;
vmCvar_t	ui_defaultMaleTeamModel;
vmCvar_t	ui_defaultFemaleTeamModel;
vmCvar_t	ui_menuFont;

static cvarTable_t		cvarTable[] = {
	{ &ui_arenasFile, "g_arenasFile", "", CVAR_INIT|CVAR_ROM },
	{ &ui_botsFile, "g_botsFile", "", CVAR_INIT|CVAR_ROM },
	{ &ui_spSkill, "g_spSkill", "2", CVAR_ARCHIVE },

	{ &ui_browserShowFull, "ui_browserShowFull", "1", CVAR_ARCHIVE },
	{ &ui_browserShowEmpty, "ui_browserShowEmpty", "1", CVAR_ARCHIVE },
	{ &ui_browserShowBots, "ui_browserShowBots", "1", CVAR_ARCHIVE },
	{ &ui_browserSeparateMasters, "ui_browserSeparateMasters", "0", CVAR_ARCHIVE },

	{ &ui_brassTime, "cg_brassTime", "2500", CVAR_ARCHIVE },
	{ &ui_drawCrosshair, "cg_drawCrosshair", "4", CVAR_ARCHIVE },
	{ &ui_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE },
	{ &ui_marks, "cg_marks", "1", CVAR_ARCHIVE },

	{ &ui_new, "ui_new", "0", CVAR_TEMP },
	{ &ui_debug, "ui_debug", "0", CVAR_TEMP },
	{ &ui_initialized, "ui_initialized", "0", CVAR_TEMP },
	{ &ui_teamName, "ui_teamName", DEFAULT_REDTEAM_NAME, CVAR_ARCHIVE },
	{ &ui_opponentName, "ui_opponentName", DEFAULT_BLUETEAM_NAME, CVAR_ARCHIVE },
	{ &ui_redteam, "ui_redteam", DEFAULT_REDTEAM_NAME, CVAR_ARCHIVE },
	{ &ui_blueteam, "ui_blueteam", DEFAULT_BLUETEAM_NAME, CVAR_ARCHIVE },
	{ &ui_dedicated, "ui_dedicated", "0", CVAR_ARCHIVE },
	{ &ui_gameType, "ui_gametype", "3", CVAR_ARCHIVE },
	{ &ui_joinGameType, "ui_joinGametype", "0", CVAR_ARCHIVE },
	{ &ui_netGameType, "ui_netGametype", "3", CVAR_ARCHIVE },
	{ &ui_actualNetGameType, "ui_actualNetGametype", "3", CVAR_ARCHIVE },
	{ &ui_redteam1, "ui_redteam1", "0", CVAR_ARCHIVE },
	{ &ui_redteam2, "ui_redteam2", "0", CVAR_ARCHIVE },
	{ &ui_redteam3, "ui_redteam3", "0", CVAR_ARCHIVE },
	{ &ui_redteam4, "ui_redteam4", "0", CVAR_ARCHIVE },
	{ &ui_redteam5, "ui_redteam5", "0", CVAR_ARCHIVE },
	{ &ui_blueteam1, "ui_blueteam1", "0", CVAR_ARCHIVE },
	{ &ui_blueteam2, "ui_blueteam2", "0", CVAR_ARCHIVE },
	{ &ui_blueteam3, "ui_blueteam3", "0", CVAR_ARCHIVE },
	{ &ui_blueteam4, "ui_blueteam4", "0", CVAR_ARCHIVE },
	{ &ui_blueteam5, "ui_blueteam5", "0", CVAR_ARCHIVE },
	{ &ui_netSource, "ui_netSource", "1", CVAR_ARCHIVE },
	{ &ui_menuFiles, "ui_menuFiles", "ui/menus.txt", CVAR_ARCHIVE },
	{ &ui_currentTier, "ui_currentTier", "0", CVAR_ARCHIVE },
	{ &ui_currentMap, "ui_currentMap", "0", CVAR_ARCHIVE },
	{ &ui_currentNetMap, "ui_currentNetMap", "0", CVAR_ARCHIVE },
	{ &ui_mapIndex, "ui_mapIndex", "0", CVAR_ARCHIVE },
	{ &ui_currentOpponent, "ui_currentOpponent", "0", CVAR_ARCHIVE },
	{ &ui_selectedPlayer, "cg_selectedPlayer", "0", CVAR_ARCHIVE},
	{ &ui_selectedPlayerName, "cg_selectedPlayerName", "", CVAR_ARCHIVE},
	{ &ui_lastServerRefresh_0, "ui_lastServerRefresh_0", "", CVAR_ARCHIVE},
	{ &ui_lastServerRefresh_1, "ui_lastServerRefresh_1", "", CVAR_ARCHIVE},
	{ &ui_lastServerRefresh_2, "ui_lastServerRefresh_2", "", CVAR_ARCHIVE},
	{ &ui_lastServerRefresh_3, "ui_lastServerRefresh_3", "", CVAR_ARCHIVE},
	{ &ui_lastServerRefresh_4, "ui_lastServerRefresh_4", "", CVAR_ARCHIVE},
	{ &ui_lastServerRefresh_5, "ui_lastServerRefresh_5", "", CVAR_ARCHIVE},
	{ &ui_lastServerRefresh_6, "ui_lastServerRefresh_6", "", CVAR_ARCHIVE},
	{ &ui_singlePlayerActive, "ui_singlePlayerActive", "0", CVAR_SYSTEMINFO | CVAR_ROM},
	{ &ui_scoreAccuracy, "ui_scoreAccuracy", "0", CVAR_ARCHIVE},
	{ &ui_scoreImpressives, "ui_scoreImpressives", "0", CVAR_ARCHIVE},
	{ &ui_scoreExcellents, "ui_scoreExcellents", "0", CVAR_ARCHIVE},
	{ &ui_scoreCaptures, "ui_scoreCaptures", "0", CVAR_ARCHIVE},
	{ &ui_scoreDefends, "ui_scoreDefends", "0", CVAR_ARCHIVE},
	{ &ui_scoreAssists, "ui_scoreAssists", "0", CVAR_ARCHIVE},
	{ &ui_scoreGauntlets, "ui_scoreGauntlets", "0",CVAR_ARCHIVE},
	{ &ui_scoreScore, "ui_scoreScore", "0", CVAR_ARCHIVE},
	{ &ui_scorePerfect, "ui_scorePerfect", "0", CVAR_ARCHIVE},
	{ &ui_scoreTeam, "ui_scoreTeam", "0 to 0", CVAR_ARCHIVE},
	{ &ui_scoreBase, "ui_scoreBase", "0", CVAR_ARCHIVE},
	{ &ui_scoreTime, "ui_scoreTime", "00:00", CVAR_ARCHIVE},
	{ &ui_scoreTimeBonus, "ui_scoreTimeBonus", "0", CVAR_ARCHIVE},
	{ &ui_scoreSkillBonus, "ui_scoreSkillBonus", "0", CVAR_ARCHIVE},
	{ &ui_scoreShutoutBonus, "ui_scoreShutoutBonus", "0", CVAR_ARCHIVE},
	{ &ui_fragLimit, "ui_fragLimit", "10", 0},
	{ &ui_captureLimit, "ui_captureLimit", "5", 0},
	{ &ui_smallFont, "ui_smallFont", "0.25", CVAR_ARCHIVE},
	{ &ui_bigFont, "ui_bigFont", "0.4", CVAR_ARCHIVE},
	{ &ui_findPlayer, "ui_findPlayer", DEFAULT_MODEL, CVAR_ARCHIVE},
	{ &ui_Q3Model, "ui_q3model", "0", CVAR_ARCHIVE},
	{ &ui_hudFiles, "cg_hudFiles", "ui/hud.txt", CVAR_ARCHIVE},
	{ &ui_recordSPDemo, "ui_recordSPDemo", "0", CVAR_ARCHIVE},
	{ &ui_teamArenaFirstRun, "ui_teamArenaFirstRun", "0", CVAR_ARCHIVE},
	{ NULL, "g_warmup", "20", CVAR_ARCHIVE},
	{ NULL, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART},
	{ NULL, "fraglimit", "20", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART},
	{ NULL, "capturelimit", "8", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART},
	{ &ui_serverStatusTimeOut, "ui_serverStatusTimeOut", "7000", CVAR_ARCHIVE},
	{ &ui_defaultMaleTeamModel, "default_male_team_model", DEFAULT_TEAM_MODEL_MALE, CVAR_ARCHIVE},
	{ &ui_defaultFemaleTeamModel, "default_female_team_model", DEFAULT_TEAM_MODEL_FEMALE, CVAR_ARCHIVE},
	{ &ui_menuFont, "ui_menuFont", "fonts/LiberationSans-Bold.ttf", CVAR_ARCHIVE | CVAR_LATCH},
};

static int		cvarTableSize = ARRAY_LEN( cvarTable );


/*
=================
UI_RegisterCvars
=================
*/
void UI_RegisterCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags );
	}

	trap_Cvar_CheckRange( "ui_actualNetGameType", 0, GT_MAX_GAME_TYPE-1, qtrue );
}

/*
=================
UI_UpdateCvars
=================
*/
void UI_UpdateCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		if ( !cv->vmCvar ) {
			continue;
		}
		trap_Cvar_Update( cv->vmCvar );
	}
}


/*
=================
ArenaServers_StopRefresh
=================
*/
static void UI_StopServerRefresh( void )
{
	int count;

	if (!uiInfo.serverStatus.refreshActive) {
		// not currently refreshing
		return;
	}
	uiInfo.serverStatus.refreshActive = qfalse;
	Com_Printf("%d servers listed in browser with %d players.\n",
					uiInfo.serverStatus.numDisplayServers,
					uiInfo.serverStatus.numPlayersOnServers);
	count = trap_LAN_GetServerCount(UI_SourceForLAN());
	if (count - uiInfo.serverStatus.numDisplayServers > 0) {
		Com_Printf("%d servers not listed due to packet loss or pings higher than %d\n",
						count - uiInfo.serverStatus.numDisplayServers,
						(int) trap_Cvar_VariableValue("cl_maxPing"));
	}

}

/*
=================
UI_DoServerRefresh
=================
*/
static void UI_DoServerRefresh( void )
{
	qboolean wait = qfalse;

	if (!uiInfo.serverStatus.refreshActive) {
		return;
	}
	if (ui_netSource.integer != UIAS_FAVORITES) {
		if (ui_netSource.integer == UIAS_LOCAL) {
			if (!trap_LAN_GetServerCount(AS_LOCAL)) {
				wait = qtrue;
			}
		} else {
			if (trap_LAN_GetServerCount(AS_GLOBAL) < 0) {
				wait = qtrue;
			}
		}
	}

	if (uiInfo.uiDC.realTime < uiInfo.serverStatus.refreshtime) {
		if (wait) {
			return;
		}
	}

	// if still trying to retrieve pings
	if (trap_LAN_UpdateVisiblePings(UI_SourceForLAN())) {
		uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 1000;
	} else if (!wait) {
		// get the last servers in the list
		UI_BuildServerDisplayList(2);
		// stop the refresh
		UI_StopServerRefresh();
	} else if ( ui_netSource.integer == UIAS_LOCAL ) {
		// no local servers found, check again
		trap_Cmd_ExecuteText( EXEC_NOW, "localservers\n" );
		uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 5000;
	}
	//
	UI_BuildServerDisplayList(qfalse);
}

/*
=================
UI_StartServerRefresh
=================
*/
static void UI_StartServerRefresh(qboolean full, qboolean force)
{
	char	*ptr;
	int		lanSource;
	qtime_t q;

	// This function is called with force=qfalse when server browser menu opens or net source changes.
	// Automatically update local and favorite servers.
	// Only auto update master server list if there is no server info cache.
	if ( !force && ( ui_netSource.integer >= UIAS_GLOBAL0 && ui_netSource.integer <= UIAS_GLOBAL5 ) ) {
		if ( trap_LAN_GetServerCount( UI_SourceForLAN() ) > 0 ) {
			return; // have cached list
		}
	}

	trap_RealTime(&q);
	trap_Cvar_Set( va("ui_lastServerRefresh_%i", ui_netSource.integer), va("%s-%i, %i at %i:%02i", MonthAbbrev[q.tm_mon],q.tm_mday, 1900+q.tm_year,q.tm_hour,q.tm_min));

	if (!full) {
		UI_UpdatePendingPings();
		return;
	}

	uiInfo.serverStatus.refreshActive = qtrue;
	uiInfo.serverStatus.nextDisplayRefresh = uiInfo.uiDC.realTime + 1000;
	// clear number of displayed servers
	uiInfo.serverStatus.numDisplayServers = 0;
	uiInfo.serverStatus.numPlayersOnServers = 0;

	lanSource = UI_SourceForLAN();
	// mark all servers as visible so we store ping updates for them
	trap_LAN_MarkServerVisible(lanSource, -1, qtrue);
	// reset all the pings
	trap_LAN_ResetPings(lanSource);
	//
	if( ui_netSource.integer == UIAS_LOCAL ) {
		trap_Cmd_ExecuteText( EXEC_NOW, "localservers\n" );
		uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 5000;
		return;
	}

	uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 5000;
	if( ui_netSource.integer >= UIAS_GLOBAL0 && ui_netSource.integer <= UIAS_GLOBAL5 ) {

		ptr = CG_Cvar_VariableString("debug_protocol");
		if (strlen(ptr)) {
			trap_Cmd_ExecuteText( EXEC_NOW, va( "globalservers %d %s full empty\n", ui_netSource.integer - UIAS_GLOBAL0, ptr ) );
		}
		else {
			trap_Cmd_ExecuteText( EXEC_NOW, va( "globalservers %d %d full empty\n", ui_netSource.integer - UIAS_GLOBAL0, (int)trap_Cvar_VariableValue( "protocol" ) ) );
		}
	}
}

