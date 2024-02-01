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

#include "cg_local.h"
#include "../ui/ui_shared.h"

#if defined MISSIONPACK || defined MISSIONPACK_HUD

#ifdef MISSIONPACK_HUD
extern displayContextDef_t cgDC;

fontInfo_t *CG_FontForScale( float scale ) {
	fontInfo_t *font = &cgDC.Assets.textFont;
	if (scale <= cg_smallFont.value) {
		font = &cgDC.Assets.smallFont;
	} else if (scale >= cg_bigFont.value) {
		font = &cgDC.Assets.bigFont;
	}

	return font;
}

// CG_Text_* are for Team Arena HUD code compatiblity
void CG_Text_PaintWithCursor(float x, float y, float scale, const vec4_t color, const char *text, int cursorPos, char cursor, int limit, int textStyle) {
	float shadowOffset;

	if ( textStyle == ITEM_TEXTSTYLE_SHADOWED ) {
		shadowOffset = 1;
	} else if ( textStyle == ITEM_TEXTSTYLE_SHADOWEDMORE ) {
		shadowOffset = 2;
	} else {
		shadowOffset = 0;
	}

	Text_PaintWithCursor( x, y, CG_FontForScale( scale ), scale, color, text, cursorPos, cursor, 0, limit, shadowOffset, 0, qfalse, qfalse );
}

int CG_Text_Width(const char *text, float scale, int limit) {
	return Text_Width( text, CG_FontForScale( scale ), scale, limit );
}

int CG_Text_Height(const char *text, float scale, int limit) {
	return Text_Height( text, CG_FontForScale( scale ), scale, limit );
}

void CG_Text_Paint(float x, float y, float scale, const vec4_t color, const char *text, float adjust, int limit, int textStyle) {
	float shadowOffset;

	if ( textStyle == ITEM_TEXTSTYLE_SHADOWED ) {
		shadowOffset = 1;
	} else if ( textStyle == ITEM_TEXTSTYLE_SHADOWEDMORE ) {
		shadowOffset = 2;
	} else {
		shadowOffset = 0;
	}

	Text_Paint( x, y, CG_FontForScale( scale ), scale, color, text, adjust, limit, shadowOffset, 0, qfalse, qfalse );
}

void CG_Text_PaintGradient(float x, float y, float scale, const vec4_t color, const char *text, float adjust, int limit, int textStyle) {
	float shadowOffset;

	if ( textStyle == ITEM_TEXTSTYLE_SHADOWED ) {
		shadowOffset = 1;
	} else if ( textStyle == ITEM_TEXTSTYLE_SHADOWEDMORE ) {
		shadowOffset = 2;
	} else {
		shadowOffset = 0;
	}

	Text_Paint( x, y, CG_FontForScale( scale ), scale, color, text, adjust, limit, shadowOffset, 0.4f, qfalse, qfalse );
}

// this function does not align text position to pixel boundary so that text scrolls evenly
void CG_Text_PaintInMotion(float x, float y, float scale, const vec4_t color, const char *text, float adjust, int limit, int textStyle) {
	float shadowOffset;

	if ( textStyle == ITEM_TEXTSTYLE_SHADOWED ) {
		shadowOffset = 1;
	} else if ( textStyle == ITEM_TEXTSTYLE_SHADOWEDMORE ) {
		shadowOffset = 2;
	} else {
		shadowOffset = 0;
	}

	Text_Paint( x, y, CG_FontForScale( scale ), scale, color, text, adjust, limit, shadowOffset, 0, qfalse, qtrue );
}

void CG_Text_Paint_Limit(float *maxX, float x, float y, float scale, const vec4_t color, const char* text, float adjust, int limit) {
	Text_Paint_Limit( maxX, x, y, CG_FontForScale( scale ), scale, color, text, adjust, limit, qfalse );
}
#endif


// set in CG_ParseTeamInfo

//static int sortedTeamPlayers[TEAM_MAXOVERLAY];
//static int numSortedTeamPlayers;
#ifdef MISSIONPACK_HUD
int drawTeamOverlayModificationCount = -1;
#endif

//static char systemChat[256];
//static char teamChat1[256];
//static char teamChat2[256];

void CG_InitTeamChat(void) {
  memset(teamChat1, 0, sizeof(teamChat1));
  memset(teamChat2, 0, sizeof(teamChat2));
  memset(systemChat, 0, sizeof(systemChat));
}

void CG_SetPrintString(q3print_t type, const char *p) {
  if (type == SYSTEM_PRINT) {
    strcpy(systemChat, p);
  } else {
    strcpy(teamChat2, teamChat1);
    strcpy(teamChat1, p);
  }
}

#ifdef MISSIONPACK
void CG_CheckOrderPending( int localPlayerNum ) {
	localPlayer_t *localPlayer;

	if (cgs.gametype < GT_CTF) {
		return;
	}

	localPlayer = &cg.localPlayers[localPlayerNum];

	if ( localPlayer->playerNum == -1 ) {
		return;
	}

	if (localPlayer->orderPending) {
		//playerInfo_t *pi;
		const char *p1, *p2;
		char	b[128];
		int		playerNum;
		int		team;
		int		selectedPlayer;

		p1 = p2 = NULL;
		b[0] = '\0';

		playerNum = cg.snap->pss[localPlayerNum].playerNum;
		team = cg.snap->pss[localPlayerNum].persistant[PERS_TEAM];
#ifdef MISSIONPACK_HUD
		selectedPlayer = cg_currentSelectedPlayer[localPlayerNum].integer;
#else
		selectedPlayer = playerNum;
#endif

		//pi = cgs.playerinfo + sortedTeamPlayers[team][selectedPlayer];

		switch (localPlayer->currentOrder) {
			case TEAMTASK_OFFENSE:
				p1 = VOICECHAT_ONOFFENSE;
				p2 = VOICECHAT_OFFENSE;
				Com_sprintf( b, sizeof ( b ), "%s; wait; %s", Com_LocalPlayerCvarName( localPlayerNum, "+button7" ), Com_LocalPlayerCvarName( localPlayerNum, "-button7" ) );
			break;
			case TEAMTASK_DEFENSE:
				p1 = VOICECHAT_ONDEFENSE;
				p2 = VOICECHAT_DEFEND;
				Com_sprintf( b, sizeof ( b ), "%s; wait; %s", Com_LocalPlayerCvarName( localPlayerNum, "+button8" ), Com_LocalPlayerCvarName( localPlayerNum, "-button8" ) );
			break;					
			case TEAMTASK_PATROL:
				p1 = VOICECHAT_ONPATROL;
				p2 = VOICECHAT_PATROL;
				Com_sprintf( b, sizeof ( b ), "%s; wait; %s", Com_LocalPlayerCvarName( localPlayerNum, "+button9" ), Com_LocalPlayerCvarName( localPlayerNum, "-button9" ) );
			break;
			case TEAMTASK_FOLLOW: 
				p1 = VOICECHAT_ONFOLLOW;
				p2 = VOICECHAT_FOLLOWME;
				Com_sprintf( b, sizeof ( b ), "%s; wait; %s", Com_LocalPlayerCvarName( localPlayerNum, "+button10" ), Com_LocalPlayerCvarName( localPlayerNum, "-button10" ) );
			break;
			case TEAMTASK_CAMP:
				p1 = VOICECHAT_ONCAMPING;
				p2 = VOICECHAT_CAMP;
			break;
			case TEAMTASK_RETRIEVE:
				p1 = VOICECHAT_ONGETFLAG;
				p2 = VOICECHAT_RETURNFLAG;
			break;
			case TEAMTASK_ESCORT:
				p1 = VOICECHAT_ONFOLLOWCARRIER;
				p2 = VOICECHAT_FOLLOWFLAGCARRIER;
			break;
		}

		if (selectedPlayer == numSortedTeamPlayers[team]) {
			// to everyone
			trap_Cmd_ExecuteText(EXEC_APPEND, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay_team"), p2));
		} else {
			// for the player self
			if (sortedTeamPlayers[team][selectedPlayer] == playerNum && p1) {
				trap_Cmd_ExecuteText(EXEC_APPEND, va("%s %i\n", Com_LocalPlayerCvarName(localPlayerNum, "teamtask"), localPlayer->currentOrder));
				//trap_Cmd_ExecuteText(EXEC_APPEND, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "say_team"), p2));
				trap_Cmd_ExecuteText(EXEC_APPEND, va("cmd %s %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vsay_team"), p1));
			} else if (p2) {
				//trap_Cmd_ExecuteText(EXEC_APPEND, va("cmd %s %s, %s\n", Com_LocalPlayerCvarName(localPlayerNum, "say_team"), pi->name,p));
				trap_Cmd_ExecuteText(EXEC_APPEND, va("cmd %s %d %s\n", Com_LocalPlayerCvarName(localPlayerNum, "vtell"), sortedTeamPlayers[team][selectedPlayer], p2));
			}
		}
		if (b[0]) {
			trap_Cmd_ExecuteText(EXEC_APPEND, b);
		}
		localPlayer->orderPending = qfalse;
	}
}
#endif

#ifdef MISSIONPACK_HUD
static void CG_SetSelectedPlayerName( int localPlayerNum ) {
	int team;

	team = cg.snap->pss[ localPlayerNum ].persistant[PERS_TEAM];

	if (cg_currentSelectedPlayer[ localPlayerNum ].integer >= 0 && cg_currentSelectedPlayer[ localPlayerNum ].integer < numSortedTeamPlayers[team]) {
		playerInfo_t *pi = cgs.playerinfo + sortedTeamPlayers[team][cg_currentSelectedPlayer[ localPlayerNum ].integer];
		if (pi) {
			trap_Cvar_Set(Com_LocalPlayerCvarName(localPlayerNum, "cg_selectedPlayerName"), pi->name);
			trap_Cvar_SetValue(Com_LocalPlayerCvarName(localPlayerNum, "cg_selectedPlayer"), sortedTeamPlayers[team][cg_currentSelectedPlayer[ localPlayerNum ].integer]);
			cg.localPlayers[ localPlayerNum ].currentOrder = pi->teamTask;
		}
	} else {
		trap_Cvar_Set(Com_LocalPlayerCvarName(localPlayerNum, "cg_selectedPlayerName"), "Everyone");
	}
}
int CG_GetSelectedPlayer( int localPlayerNum ) {
	int team;

	team = cg.snap->pss[ localPlayerNum ].persistant[PERS_TEAM];

	if (cg_currentSelectedPlayer[ localPlayerNum ].integer < 0 || cg_currentSelectedPlayer[ localPlayerNum ].integer >= numSortedTeamPlayers[team]) {
		cg_currentSelectedPlayer[ localPlayerNum ].integer = 0;
	}
	return cg_currentSelectedPlayer[ localPlayerNum ].integer;
}

void CG_SelectNextPlayer( int localPlayerNum ) {
	int team;

	team = cg.snap->pss[ localPlayerNum ].persistant[PERS_TEAM];

#ifdef MISSIONPACK
	CG_CheckOrderPending( localPlayerNum );
#endif
	if (cg_currentSelectedPlayer[ localPlayerNum ].integer >= 0 && cg_currentSelectedPlayer[ localPlayerNum ].integer < numSortedTeamPlayers[team]) {
		cg_currentSelectedPlayer[ localPlayerNum ].integer++;
	} else {
		cg_currentSelectedPlayer[ localPlayerNum ].integer = 0;
	}
	CG_SetSelectedPlayerName( localPlayerNum );
}

void CG_SelectPrevPlayer( int localPlayerNum ) {
	int team;

	team = cg.snap->pss[ localPlayerNum ].persistant[PERS_TEAM];

#ifdef MISSIONPACK
	CG_CheckOrderPending( localPlayerNum );
#endif
	if (cg_currentSelectedPlayer[ localPlayerNum ].integer > 0 && cg_currentSelectedPlayer[ localPlayerNum ].integer <= numSortedTeamPlayers[team]) {
		cg_currentSelectedPlayer[ localPlayerNum ].integer--;
	} else {
		cg_currentSelectedPlayer[ localPlayerNum ].integer = numSortedTeamPlayers[team];
	}
	CG_SetSelectedPlayerName( localPlayerNum );
}


static void CG_DrawPlayerArmorIcon( rectDef_t *rect, qboolean draw2D ) {
	vec3_t		angles;
	vec3_t		origin;

	if ( cg_drawStatus.integer == 0 ) {
		return;
	}

	if ( draw2D || ( !cg_draw3dIcons.integer && cg_drawIcons.integer) ) {
		CG_DrawPic( rect->x, rect->y + rect->h/2 + 1, rect->w, rect->h, cgs.media.armorIcon );
	} else if (cg_draw3dIcons.integer) {
		VectorClear( angles );
		origin[0] = 90;
		origin[1] = 0;
		origin[2] = -10;
		angles[YAW] = ( cg.time & 2047 ) * 360 / 2048.0f;
		CG_Draw3DModel( rect->x, rect->y, rect->w, rect->h, cgs.media.armorModel, NULL, origin, angles );
	}
}

static void CG_DrawPlayerArmorValue(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle) {
	char	num[16];
	int		value;
	playerState_t	*ps;

	ps = cg.cur_ps;

	value = ps->stats[STAT_ARMOR];

	if (shader) {
		trap_R_SetColor( color );
		CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
		trap_R_SetColor( NULL );
	} else {
		Com_sprintf (num, sizeof(num), "%i", value);
		value = CG_Text_Width(num, scale, 0);
		CG_Text_PaintGradient(rect->x + (rect->w - value) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
	}
}

static void CG_DrawPlayerAmmoIcon( rectDef_t *rect, qboolean draw2D ) {
	centity_t	*cent;
	vec3_t		angles;
	vec3_t		origin;

	cent = &cg_entities[cg.cur_ps->playerNum];

	if ( draw2D || (!cg_draw3dIcons.integer && cg_drawIcons.integer) ) {
		qhandle_t	icon;
		icon = cg_weapons[ cg.cur_lc->predictedPlayerState.weapon ].ammoIcon;
		if ( icon ) {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, icon );
		}
	} else if (cg_draw3dIcons.integer) {
		if ( cent->currentState.weapon && cg_weapons[ cent->currentState.weapon ].ammoModel ) {
			VectorClear( angles );
			origin[0] = 70;
			origin[1] = 0;
			origin[2] = 0;
			angles[YAW] = 90 + 20 * sin( cg.time / 1000.0 );
			CG_Draw3DModel( rect->x, rect->y, rect->w, rect->h, cg_weapons[ cent->currentState.weapon ].ammoModel, NULL, origin, angles );
		}
	}
}

static void CG_DrawPlayerAmmoValue(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle) {
	char	num[16];
	int value;
	centity_t	*cent;
	playerState_t	*ps;

	cent = &cg_entities[cg.cur_ps->playerNum];
	ps = cg.cur_ps;

	if ( cent->currentState.weapon ) {
		value = ps->ammo[cent->currentState.weapon];
		if ( value > -1 ) {
			if (shader) {
		    trap_R_SetColor( color );
				CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
			  trap_R_SetColor( NULL );
			} else {
				Com_sprintf (num, sizeof(num), "%i", value);
				value = CG_Text_Width(num, scale, 0);
				CG_Text_PaintGradient(rect->x + (rect->w - value) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
			}
		}
	}

}



static void CG_DrawPlayerHead(rectDef_t *rect, qboolean draw2D) {
	vec3_t		angles;
	float		size, stretch;
	float		frac;
	float		x = rect->x;

	VectorClear( angles );

	if ( cg.cur_lc->damageTime && cg.time - cg.cur_lc->damageTime < DAMAGE_TIME ) {
		frac = (float)(cg.time - cg.cur_lc->damageTime ) / DAMAGE_TIME;
		size = rect->w * 1.25 * ( 1.5 - frac * 0.5 );

		stretch = size - rect->w * 1.25;
		// kick in the direction of damage
		x -= stretch * 0.5 + cg.cur_lc->damageX * stretch * 0.5;

		cg.cur_lc->headStartYaw = 180 + cg.cur_lc->damageX * 45;

		cg.cur_lc->headEndYaw = 180 + 20 * cos( crandom()*M_PI );
		cg.cur_lc->headEndPitch = 5 * cos( crandom()*M_PI );

		cg.cur_lc->headStartTime = cg.time;
		cg.cur_lc->headEndTime = cg.time + 100 + random() * 2000;
	} else {
		if ( cg.time >= cg.cur_lc->headEndTime ) {
			// select a new head angle
			cg.cur_lc->headStartYaw = cg.cur_lc->headEndYaw;
			cg.cur_lc->headStartPitch = cg.cur_lc->headEndPitch;
			cg.cur_lc->headStartTime = cg.cur_lc->headEndTime;
			cg.cur_lc->headEndTime = cg.time + 100 + random() * 2000;

			cg.cur_lc->headEndYaw = 180 + 20 * cos( crandom()*M_PI );
			cg.cur_lc->headEndPitch = 5 * cos( crandom()*M_PI );
		}
	}

	// if the server was frozen for a while we may have a bad head start time
	if ( cg.cur_lc->headStartTime > cg.time ) {
		cg.cur_lc->headStartTime = cg.time;
	}

	frac = ( cg.time - cg.cur_lc->headStartTime ) / (float)( cg.cur_lc->headEndTime - cg.cur_lc->headStartTime );
	frac = frac * frac * ( 3 - 2 * frac );
	angles[YAW] = cg.cur_lc->headStartYaw + ( cg.cur_lc->headEndYaw - cg.cur_lc->headStartYaw ) * frac;
	angles[PITCH] = cg.cur_lc->headStartPitch + ( cg.cur_lc->headEndPitch - cg.cur_lc->headStartPitch ) * frac;

	CG_DrawHead( x, rect->y, rect->w, rect->h, cg.cur_ps->playerNum, angles );
}

static void CG_DrawSelectedPlayerHealth( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	playerInfo_t *pi;
	int value;
	char num[16];
	int team;

	team = cg.cur_ps->persistant[PERS_TEAM];

	pi = cgs.playerinfo + sortedTeamPlayers[team][CG_GetSelectedPlayer( cg.cur_localPlayerNum )];
	if (pi) {
		if (shader) {
			trap_R_SetColor( color );
			CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
			trap_R_SetColor( NULL );
		} else {
			Com_sprintf (num, sizeof(num), "%i", pi->health);
			value = CG_Text_Width(num, scale, 0);
			CG_Text_Paint(rect->x + (rect->w - value) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
		}
	}
}

static void CG_DrawSelectedPlayerArmor( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	playerInfo_t *pi;
	int value;
	char num[16];
	int team;

	team = cg.cur_ps->persistant[PERS_TEAM];
	pi = cgs.playerinfo + sortedTeamPlayers[team][CG_GetSelectedPlayer( cg.cur_localPlayerNum )];

	if (pi) {
		if (pi->armor > 0) {
			if (shader) {
				trap_R_SetColor( color );
				CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
				trap_R_SetColor( NULL );
			} else {
				Com_sprintf (num, sizeof(num), "%i", pi->armor);
				value = CG_Text_Width(num, scale, 0);
				CG_Text_Paint(rect->x + (rect->w - value) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
			}
		}
 	}
}

#ifdef MISSIONPACK
qhandle_t CG_StatusHandle(int task) {
	qhandle_t h;
	switch (task) {
		case TEAMTASK_OFFENSE :
			h = cgs.media.assaultShader;
			break;
		case TEAMTASK_DEFENSE :
			h = cgs.media.defendShader;
			break;
		case TEAMTASK_PATROL :
			h = cgs.media.patrolShader;
			break;
		case TEAMTASK_FOLLOW :
			h = cgs.media.followShader;
			break;
		case TEAMTASK_CAMP :
			h = cgs.media.campShader;
			break;
		case TEAMTASK_RETRIEVE :
			h = cgs.media.retrieveShader; 
			break;
		case TEAMTASK_ESCORT :
			h = cgs.media.escortShader; 
			break;
		default : 
			h = cgs.media.assaultShader;
			break;
	}
	return h;
}

static void CG_DrawSelectedPlayerStatus( rectDef_t *rect ) {
	playerInfo_t *pi;
	int team;

	team = cg.cur_ps->persistant[PERS_TEAM];
	pi = cgs.playerinfo + sortedTeamPlayers[team][CG_GetSelectedPlayer( cg.cur_localPlayerNum )];

	if (pi) {
		qhandle_t h;
		if (cg.cur_lc->orderPending) {
			// blink the icon
			if ( cg.time > cg.cur_lc->orderTime - 2500 && (cg.time >> 9 ) & 1 ) {
				return;
			}
			h = CG_StatusHandle(cg.cur_lc->currentOrder);
		}	else {
			h = CG_StatusHandle(pi->teamTask);
		}
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, h );
	}
}


static void CG_DrawPlayerStatus( rectDef_t *rect ) {
	playerInfo_t *pi = &cgs.playerinfo[cg.cur_ps->playerNum];
	if (pi) {
		qhandle_t h = CG_StatusHandle(pi->teamTask);
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, h);
	}
}
#endif


static void CG_DrawSelectedPlayerName( rectDef_t *rect, float scale, vec4_t color, qboolean voice, int textStyle) {
	playerInfo_t *pi;
	int team;

	team = cg.cur_ps->persistant[PERS_TEAM];

#ifdef MISSIONPACK
	if ( voice ) {
		pi = cgs.playerinfo + cg.cur_lc->currentVoicePlayerNum;
	} else
#endif
	{
		pi = cgs.playerinfo + sortedTeamPlayers[team][CG_GetSelectedPlayer( cg.cur_localPlayerNum )];
	}

	if (pi) {
		CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, pi->name, 0, 0, textStyle);
	}
}

static void CG_DrawSelectedPlayerLocation( rectDef_t *rect, float scale, vec4_t color, int textStyle ) {
	playerInfo_t	*pi;
	int				team;

	team = cg.cur_ps->persistant[PERS_TEAM];
	pi = cgs.playerinfo + sortedTeamPlayers[team][CG_GetSelectedPlayer( cg.cur_localPlayerNum )];
	if (pi) {
		const char *p = CG_ConfigString(CS_LOCATIONS + pi->location);
		if (!p || !*p) {
			p = "unknown";
		}
		CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, p, 0, 0, textStyle);
	}
}

static void CG_DrawPlayerLocation( rectDef_t *rect, float scale, vec4_t color, int textStyle  ) {
	playerInfo_t *pi = &cgs.playerinfo[cg.cur_ps->playerNum];
	if (pi) {
		const char *p = CG_ConfigString(CS_LOCATIONS + pi->location);
		if (!p || !*p) {
			p = "unknown";
		}
		CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, p, 0, 0, textStyle);
	}
}



static void CG_DrawSelectedPlayerWeapon( rectDef_t *rect ) {
	playerInfo_t *pi;
	int				team;

	team = cg.cur_ps->persistant[PERS_TEAM];
	pi = cgs.playerinfo + sortedTeamPlayers[team][CG_GetSelectedPlayer( cg.cur_localPlayerNum )];
	if (pi) {
		if ( cg_weapons[pi->curWeapon].weaponIcon ) {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cg_weapons[pi->curWeapon].weaponIcon );
		} else {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.deferShader);
		}
	}
}

static void CG_DrawPlayerScore( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	char num[16];
	int value;

	if (!cg.cur_ps) {
		return;
	}

	value = cg.cur_ps->persistant[PERS_SCORE];

	if (shader) {
		trap_R_SetColor( color );
		CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
		trap_R_SetColor( NULL );
	} else {
		Com_sprintf (num, sizeof(num), "%i", value);
		value = CG_Text_Width(num, scale, 0);
	  CG_Text_Paint(rect->x + (rect->w - value) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
	}
}

static void CG_DrawPlayerItem( rectDef_t *rect, float scale, qboolean draw2D) {
	int		value;
  vec3_t origin, angles;

	value = cg.cur_ps->stats[STAT_HOLDABLE_ITEM];
	if ( value ) {
		CG_RegisterItemVisuals( value );

		if (qtrue) {
		  CG_RegisterItemVisuals( value );
		  CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cg_items[ value ].icon );
		} else {
 			VectorClear( angles );
			origin[0] = 90;
  		origin[1] = 0;
   		origin[2] = -10;
  		angles[YAW] = ( cg.time & 2047 ) * 360 / 2048.0;
			CG_Draw3DModel(rect->x, rect->y, rect->w, rect->h, cg_items[ value ].models[0], NULL, origin, angles );
		}
	}

}


static void CG_DrawSelectedPlayerPowerup( rectDef_t *rect, qboolean draw2D ) {
	playerInfo_t *pi;
	int				team;
	int j;
	float x, y;

	team = cg.cur_ps->persistant[PERS_TEAM];
	pi = cgs.playerinfo + sortedTeamPlayers[team][CG_GetSelectedPlayer( cg.cur_localPlayerNum )];
	if (pi) {
		x = rect->x;
		y = rect->y;

		for (j = 0; j < PW_NUM_POWERUPS; j++) {
			if (pi->powerups & (1 << j)) {
				gitem_t	*item;
				item = BG_FindItemForPowerup( j );
				if (item) {
					CG_DrawPic( x, y, rect->w, rect->h, trap_R_RegisterShader( item->icon ) );
					return;
				}
			}
		}
	}
}


static void CG_DrawSelectedPlayerHead( rectDef_t *rect, qboolean draw2D, qboolean voice ) {
	clipHandle_t	cm;
	playerInfo_t	*pi;
	int				team;
	float			len;
	vec3_t			origin;
	vec3_t			mins, maxs, angles;

	team = cg.cur_ps->persistant[PERS_TEAM];
#ifdef MISSIONPACK
	if ( voice ) {
		pi = cgs.playerinfo + cg.cur_lc->currentVoicePlayerNum;
	} else
#endif
	{
		pi = cgs.playerinfo + sortedTeamPlayers[team][CG_GetSelectedPlayer( cg.cur_localPlayerNum )];
	}

	if (pi) {
		if ( cg_draw3dIcons.integer ) {
			cm = pi->headModel;
			if ( !cm ) {
				return;
			}

			// offset the origin y and z to center the head
			trap_R_ModelBounds( cm, mins, maxs, 0, 0, 0 );

			origin[2] = -0.5 * ( mins[2] + maxs[2] );
			origin[1] = 0.5 * ( mins[1] + maxs[1] );

			// calculate distance so the head nearly fills the box
			// assume heads are taller than wide
			len = 0.7 * ( maxs[2] - mins[2] );		
			origin[0] = len / 0.268;	// len / tan( fov/2 )

			// allow per-model tweaking
			VectorAdd( origin, pi->headOffset, origin );

			angles[PITCH] = 0;
			angles[YAW] = 180;
			angles[ROLL] = 0;

			CG_Draw3DModel( rect->x, rect->y, rect->w, rect->h, pi->headModel, &pi->modelSkin, origin, angles );
		} else if ( cg_drawIcons.integer ) {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, pi->modelIcon );
		}

		// if they are deferred, draw a cross out
		if ( pi->deferred ) {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.deferShader );
		}
	}
}


static void CG_DrawPlayerHealth(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	playerState_t	*ps;
	int value;
	char	num[16];

	ps = cg.cur_ps;

	value = ps->stats[STAT_HEALTH];

	if (shader) {
		trap_R_SetColor( color );
		CG_DrawPic(rect->x, rect->y, rect->w, rect->h, shader);
		trap_R_SetColor( NULL );
	} else {
		Com_sprintf (num, sizeof(num), "%i", value);
		value = CG_Text_Width(num, scale, 0);
		CG_Text_PaintGradient(rect->x + (rect->w - value) / 2, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
	}
}


static void CG_DrawRedScore(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	int value;
	char num[16];
	if ( cgs.scores1 == SCORE_NOT_PRESENT ) {
		Com_sprintf (num, sizeof(num), "-");
	}
	else {
		Com_sprintf (num, sizeof(num), "%i", cgs.scores1);
	}
	value = CG_Text_Width(num, scale, 0);
	CG_Text_Paint(rect->x + rect->w - value, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
}

static void CG_DrawBlueScore(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	int value;
	char num[16];

	if ( cgs.scores2 == SCORE_NOT_PRESENT ) {
		Com_sprintf (num, sizeof(num), "-");
	}
	else {
		Com_sprintf (num, sizeof(num), "%i", cgs.scores2);
	}
	value = CG_Text_Width(num, scale, 0);
	CG_Text_Paint(rect->x + rect->w - value, rect->y + rect->h, scale, color, num, 0, 0, textStyle);
}

// FIXME: team name support
static void CG_DrawRedName(rectDef_t *rect, float scale, vec4_t color, int textStyle ) {
#ifdef MISSIONPACK
  CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, cg_redTeamName.string , 0, 0, textStyle);
#else
  CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, "Red Team", 0, 0, textStyle);
#endif
}

static void CG_DrawBlueName(rectDef_t *rect, float scale, vec4_t color, int textStyle ) {
#ifdef MISSIONPACK
  CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, cg_blueTeamName.string, 0, 0, textStyle);
#else
  CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, "Blue Team", 0, 0, textStyle);
#endif
}

static void CG_DrawBlueFlagName(rectDef_t *rect, float scale, vec4_t color, int textStyle ) {
  int i;
  for ( i = 0 ; i < cgs.maxplayers ; i++ ) {
	  if ( cgs.playerinfo[i].infoValid && cgs.playerinfo[i].team == TEAM_RED  && cgs.playerinfo[i].powerups & ( 1<< PW_BLUEFLAG )) {
      CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, cgs.playerinfo[i].name, 0, 0, textStyle);
      return;
    }
  }
}

static void CG_DrawBlueFlagStatus(rectDef_t *rect, qhandle_t shader) {
#ifdef MISSIONPACK
	if (cgs.gametype != GT_CTF && cgs.gametype != GT_1FCTF) {
		if (cgs.gametype == GT_HARVESTER) {
		  vec4_t color = {0, 0, 1, 1};
		  trap_R_SetColor(color);
	    CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.blueCubeIcon );
		  trap_R_SetColor(NULL);
		}
		return;
	}
#else
	if (cgs.gametype != GT_CTF) {
		return;
	}
#endif
  if (shader) {
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
  } else {
	  gitem_t *item = BG_FindItemForPowerup( PW_BLUEFLAG );
    if (item) {
		  vec4_t color = {0, 0, 1, 1};
		  trap_R_SetColor(color);
	    if( cgs.blueflag >= 0 && cgs.blueflag <= 2 ) {
		    CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.flagShaders[cgs.blueflag] );
			} else {
		    CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.flagShaders[0] );
			}
		  trap_R_SetColor(NULL);
	  }
  }
}

static void CG_DrawBlueFlagHead(rectDef_t *rect) {
  int i;
  for ( i = 0 ; i < cgs.maxplayers ; i++ ) {
	  if ( cgs.playerinfo[i].infoValid && cgs.playerinfo[i].team == TEAM_RED  && cgs.playerinfo[i].powerups & ( 1<< PW_BLUEFLAG )) {
      vec3_t angles;
      VectorClear( angles );
 		  angles[YAW] = 180 + 20 * sin( cg.time / 650.0 );;
      CG_DrawHead( rect->x, rect->y, rect->w, rect->h, 0,angles );
      return;
    }
  }
}

static void CG_DrawRedFlagName(rectDef_t *rect, float scale, vec4_t color, int textStyle ) {
  int i;
  for ( i = 0 ; i < cgs.maxplayers ; i++ ) {
	  if ( cgs.playerinfo[i].infoValid && cgs.playerinfo[i].team == TEAM_BLUE  && cgs.playerinfo[i].powerups & ( 1<< PW_REDFLAG )) {
      CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, cgs.playerinfo[i].name, 0, 0, textStyle);
      return;
    }
  }
}

static void CG_DrawRedFlagStatus(rectDef_t *rect, qhandle_t shader) {
#ifdef MISSIONPACK
	if (cgs.gametype != GT_CTF && cgs.gametype != GT_1FCTF) {
		if (cgs.gametype == GT_HARVESTER) {
		  vec4_t color = {1, 0, 0, 1};
		  trap_R_SetColor(color);
	    CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.redCubeIcon );
		  trap_R_SetColor(NULL);
		}
		return;
	}
#else
	if (cgs.gametype != GT_CTF) {
		return;
	}
#endif
  if (shader) {
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
  } else {
	  gitem_t *item = BG_FindItemForPowerup( PW_REDFLAG );
    if (item) {
		  vec4_t color = {1, 0, 0, 1};
		  trap_R_SetColor(color);
	    if( cgs.redflag >= 0 && cgs.redflag <= 2) {
		    CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.flagShaders[cgs.redflag] );
			} else {
		    CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.flagShaders[0] );
			}
		  trap_R_SetColor(NULL);
	  }
  }
}

static void CG_DrawRedFlagHead(rectDef_t *rect) {
  int i;
  for ( i = 0 ; i < cgs.maxplayers ; i++ ) {
	  if ( cgs.playerinfo[i].infoValid && cgs.playerinfo[i].team == TEAM_BLUE  && cgs.playerinfo[i].powerups & ( 1<< PW_REDFLAG )) {
      vec3_t angles;
      VectorClear( angles );
 		  angles[YAW] = 180 + 20 * sin( cg.time / 650.0 );;
      CG_DrawHead( rect->x, rect->y, rect->w, rect->h, 0,angles );
      return;
    }
  }
}

#ifdef MISSIONPACK
static void CG_HarvesterSkulls(rectDef_t *rect, float scale, vec4_t color, qboolean force2D, int textStyle ) {
	char num[16];
	vec3_t origin, angles;
	qhandle_t handle;
	int value = cg.cur_ps->tokens;

	if (cgs.gametype != GT_HARVESTER) {
		return;
	}

	if( value > 99 ) {
		value = 99;
	}

	Com_sprintf (num, sizeof(num), "%i", value);
	value = CG_Text_Width(num, scale, 0);
	CG_Text_Paint(rect->x + (rect->w - value), rect->y + rect->h, scale, color, num, 0, 0, textStyle);

	if (cg_drawIcons.integer) {
		if (!force2D && cg_draw3dIcons.integer) {
			VectorClear(angles);
			origin[0] = 90;
			origin[1] = 0;
			origin[2] = -10;
			angles[YAW] = ( cg.time & 2047 ) * 360 / 2048.0;
			if( cg.cur_ps->persistant[PERS_TEAM] == TEAM_BLUE ) {
				handle = cgs.media.redCubeModel;
			} else {
				handle = cgs.media.blueCubeModel;
			}
			CG_Draw3DModel( rect->x, rect->y, 35, 35, handle, NULL, origin, angles );
		} else {
			if( cg.cur_ps->persistant[PERS_TEAM] == TEAM_BLUE ) {
				handle = cgs.media.redCubeIcon;
			} else {
				handle = cgs.media.blueCubeIcon;
			}
			CG_DrawPic( rect->x + 3, rect->y + 16, 20, 20, handle );
		}
	}
}

static void CG_OneFlagStatus(rectDef_t *rect) {
	if (cgs.gametype != GT_1FCTF) {
		return;
	} else {
		gitem_t *item = BG_FindItemForPowerup( PW_NEUTRALFLAG );
		if (item) {
			if( cgs.flagStatus >= 0 && cgs.flagStatus <= 4 ) {
				vec4_t color = {1, 1, 1, 1};
				int index = 0;
				if (cgs.flagStatus == FLAG_TAKEN_RED) {
					color[1] = color[2] = 0;
					index = 1;
				} else if (cgs.flagStatus == FLAG_TAKEN_BLUE) {
					color[0] = color[1] = 0;
					index = 1;
				} else if (cgs.flagStatus == FLAG_DROPPED) {
					index = 2;
				}
			  trap_R_SetColor(color);
				CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.flagShaders[index] );
			}
		}
	}
}


static void CG_DrawCTFPowerUp(rectDef_t *rect) {
	int		value;

	if (cgs.gametype < GT_CTF) {
		return;
	}
	value = cg.cur_ps->stats[STAT_PERSISTANT_POWERUP];
	if ( value ) {
		CG_RegisterItemVisuals( value );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cg_items[ value ].icon );
	}
}
#endif



static void CG_DrawTeamColor(rectDef_t *rect, vec4_t color) {
	CG_DrawTeamBackground(rect->x, rect->y, rect->w, rect->h, color[3], cg.cur_ps->persistant[PERS_TEAM]);
}

static void CG_DrawAreaPowerUp(rectDef_t *rect, int align, float special, float scale, vec4_t color) {
	char num[16];
	int		sorted[MAX_POWERUPS];
	int		sortedTime[MAX_POWERUPS];
	int		i, j, k;
	int		active;
	playerState_t	*ps;
	int		t;
	gitem_t	*item;
	float	f;
	rectDef_t r2;
	float *inc;
	r2.x = rect->x;
	r2.y = rect->y;
	r2.w = rect->w;
	r2.h = rect->h;

	inc = (align == HUD_VERTICAL) ? &r2.y : &r2.x;

	ps = cg.cur_ps;

	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	// sort the list by time remaining
	active = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( !ps->powerups[ i ] ) {
			continue;
		}

		// ZOID--don't draw if the power up has unlimited time
		// This is true of the CTF flags
		if ( ps->powerups[ i ] == INT_MAX ) {
			continue;
		}

		t = ps->powerups[ i ] - cg.time;
		if ( t <= 0 ) {
			continue;
		}

		// insert into the list
		for ( j = 0 ; j < active ; j++ ) {
			if ( sortedTime[j] >= t ) {
				for ( k = active - 1 ; k >= j ; k-- ) {
					sorted[k+1] = sorted[k];
					sortedTime[k+1] = sortedTime[k];
				}
				break;
			}
		}
		sorted[j] = i;
		sortedTime[j] = t;
		active++;
	}

	// draw the icons and timers
	for ( i = 0 ; i < active ; i++ ) {
		item = BG_FindItemForPowerup( sorted[i] );

		if (item) {
			t = ps->powerups[ sorted[i] ];
			if ( t - cg.time >= POWERUP_BLINKS * POWERUP_BLINK_TIME ) {
				trap_R_SetColor( NULL );
			} else {
				vec4_t	modulate;

				f = (float)( t - cg.time ) / POWERUP_BLINK_TIME;
				f -= (int)f;
				modulate[0] = modulate[1] = modulate[2] = modulate[3] = f;
				trap_R_SetColor( modulate );
			}

			CG_DrawPic( r2.x, r2.y, r2.w * .75, r2.h, trap_R_RegisterShader( item->icon ) );

			Com_sprintf (num, sizeof(num), "%i", sortedTime[i] / 1000);
			CG_Text_Paint(r2.x + (r2.w * .75) + 3 , r2.y + r2.h, scale, color, num, 0, 0, 0);
			*inc += r2.w + special;
		}

	}
	trap_R_SetColor( NULL );

}

float CG_GetValue(int ownerDraw) {
	centity_t	*cent;
 	playerInfo_t *pi;
	playerState_t	*ps;

	ps = cg.cur_ps;
	cent = &cg_entities[ps->playerNum];

  switch (ownerDraw) {
  case CG_SELECTEDPLAYER_ARMOR:
    pi = cgs.playerinfo + sortedTeamPlayers[ps->persistant[PERS_TEAM]][CG_GetSelectedPlayer( cg.cur_localPlayerNum )];
    return pi->armor;
    break;
  case CG_SELECTEDPLAYER_HEALTH:
    pi = cgs.playerinfo + sortedTeamPlayers[ps->persistant[PERS_TEAM]][CG_GetSelectedPlayer( cg.cur_localPlayerNum )];
    return pi->health;
    break;
  case CG_PLAYER_ARMOR_VALUE:
		return ps->stats[STAT_ARMOR];
    break;
  case CG_PLAYER_AMMO_VALUE:
		if ( cent->currentState.weapon ) {
		  return ps->ammo[cent->currentState.weapon];
		}
    break;
  case CG_PLAYER_SCORE:
	  return ps->persistant[PERS_SCORE];
    break;
  case CG_PLAYER_HEALTH:
		return ps->stats[STAT_HEALTH];
    break;
  case CG_RED_SCORE:
		return cgs.scores1;
    break;
  case CG_BLUE_SCORE:
		return cgs.scores2;
    break;
  default:
    break;
  }
	return -1;
}
#endif // MISSIONPACK_HUD

qboolean CG_OtherTeamHasFlag(void) {
	if (cgs.gametype == GT_CTF) {
		int team = cg.cur_ps->persistant[PERS_TEAM];
		if (team == TEAM_RED && cgs.redflag == FLAG_TAKEN) {
			return qtrue;
		} else if (team == TEAM_BLUE && cgs.blueflag == FLAG_TAKEN) {
			return qtrue;
		} else {
			return qfalse;
		}
	}
#ifdef MISSIONPACK
	if (cgs.gametype == GT_1FCTF) {
		int team = cg.cur_ps->persistant[PERS_TEAM];
		if (team == TEAM_RED && cgs.flagStatus == FLAG_TAKEN_BLUE) {
			return qtrue;
		} else if (team == TEAM_BLUE && cgs.flagStatus == FLAG_TAKEN_RED) {
			return qtrue;
		} else {
			return qfalse;
		}
	}
#endif
	return qfalse;
}

qboolean CG_YourTeamHasFlag(void) {
	if (cgs.gametype == GT_CTF) {
		int team = cg.cur_ps->persistant[PERS_TEAM];
		if (team == TEAM_RED && cgs.blueflag == FLAG_TAKEN) {
			return qtrue;
		} else if (team == TEAM_BLUE && cgs.redflag == FLAG_TAKEN) {
			return qtrue;
		} else {
			return qfalse;
		}
	}
#ifdef MISSIONPACK
	if (cgs.gametype == GT_1FCTF) {
		int team = cg.cur_ps->persistant[PERS_TEAM];
		if (team == TEAM_RED && cgs.flagStatus == FLAG_TAKEN_RED) {
			return qtrue;
		} else if (team == TEAM_BLUE && cgs.flagStatus == FLAG_TAKEN_BLUE) {
			return qtrue;
		} else {
			return qfalse;
		}
	}
#endif
	return qfalse;
}

#ifdef MISSIONPACK_HUD
// THINKABOUTME: should these be exclusive or inclusive.. 
// 
qboolean CG_OwnerDrawVisible(int flags) {

	if (flags & CG_SHOW_TEAMINFO) {
		return (cg_currentSelectedPlayer[cg.cur_localPlayerNum].integer == numSortedTeamPlayers[cg.cur_ps->persistant[PERS_TEAM]]);
	}

	if (flags & CG_SHOW_NOTEAMINFO) {
		return !(cg_currentSelectedPlayer[cg.cur_localPlayerNum].integer == numSortedTeamPlayers[cg.cur_ps->persistant[PERS_TEAM]]);
	}

	if (flags & CG_SHOW_OTHERTEAMHASFLAG) {
		return CG_OtherTeamHasFlag();
	}

	if (flags & CG_SHOW_YOURTEAMHASENEMYFLAG) {
		return CG_YourTeamHasFlag();
	}

	if (flags & (CG_SHOW_BLUE_TEAM_HAS_REDFLAG | CG_SHOW_RED_TEAM_HAS_BLUEFLAG)) {
		if (flags & CG_SHOW_BLUE_TEAM_HAS_REDFLAG && (cgs.redflag == FLAG_TAKEN || cgs.flagStatus == FLAG_TAKEN_RED)) {
			return qtrue;
		} else if (flags & CG_SHOW_RED_TEAM_HAS_BLUEFLAG && (cgs.blueflag == FLAG_TAKEN || cgs.flagStatus == FLAG_TAKEN_BLUE)) {
			return qtrue;
		}
		return qfalse;
	}

	if (flags & CG_SHOW_ANYTEAMGAME) {
		if( cgs.gametype >= GT_TEAM) {
			return qtrue;
		}
	}

	if (flags & CG_SHOW_ANYNONTEAMGAME) {
		if( cgs.gametype < GT_TEAM) {
			return qtrue;
		}
	}

	if (flags & CG_SHOW_HARVESTER) {
#ifdef MISSIONPACK
		if( cgs.gametype == GT_HARVESTER ) {
			return qtrue;
		} else
#endif
		{
			return qfalse;
		}
	}

	if (flags & CG_SHOW_ONEFLAG) {
#ifdef MISSIONPACK
		if( cgs.gametype == GT_1FCTF ) {
			return qtrue;
		} else
#endif
		{
			return qfalse;
		}
	}

	if (flags & CG_SHOW_CTF) {
		if( cgs.gametype == GT_CTF ) {
			return qtrue;
		}
	}

	if (flags & CG_SHOW_OBELISK) {
#ifdef MISSIONPACK
		if( cgs.gametype == GT_OBELISK ) {
			return qtrue;
		} else
#endif
		{
			return qfalse;
		}
	}

	if (flags & CG_SHOW_HEALTHCRITICAL) {
		if (cg.cur_ps->stats[STAT_HEALTH] < 25) {
			return qtrue;
		}
	}

	if (flags & CG_SHOW_HEALTHOK) {
		if (cg.cur_ps->stats[STAT_HEALTH] >= 25) {
			return qtrue;
		}
	}

	if (flags & CG_SHOW_SINGLEPLAYER) {
		if( cgs.gametype == GT_SINGLE_PLAYER ) {
			return qtrue;
		}
	}

	if (flags & CG_SHOW_TOURNAMENT) {
		if( cgs.gametype == GT_TOURNAMENT ) {
			return qtrue;
		}
	}

	if (flags & CG_SHOW_DURINGINCOMINGVOICE) {
	}

	if (flags & CG_SHOW_IF_PLAYER_HAS_FLAG) {
		if (cg.cur_ps->powerups[PW_REDFLAG] || cg.cur_ps->powerups[PW_BLUEFLAG]) {
			return qtrue;
		}
#ifdef MISSIONPACK
		if (cg.cur_ps->powerups[PW_NEUTRALFLAG]) {
			return qtrue;
		}
#endif
	}
	return qfalse;
}



static void CG_DrawPlayerHasFlag(rectDef_t *rect, qboolean force2D) {
	int adj = (force2D) ? 0 : 2;
	if( cg.cur_lc->predictedPlayerState.powerups[PW_REDFLAG] ) {
  	CG_DrawFlagModel( rect->x + adj, rect->y + adj, rect->w - adj, rect->h - adj, TEAM_RED, force2D);
	} else if( cg.cur_lc->predictedPlayerState.powerups[PW_BLUEFLAG] ) {
  	CG_DrawFlagModel( rect->x + adj, rect->y + adj, rect->w - adj, rect->h - adj, TEAM_BLUE, force2D);
	} else if( cg.cur_lc->predictedPlayerState.powerups[PW_NEUTRALFLAG] ) {
  	CG_DrawFlagModel( rect->x + adj, rect->y + adj, rect->w - adj, rect->h - adj, TEAM_FREE, force2D);
	}
}

static void CG_DrawAreaSystemChat(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader) {
  CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, systemChat, 0, 0, 0);
}

static void CG_DrawAreaTeamChat(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader) {
  CG_Text_Paint(rect->x, rect->y + rect->h, scale, color,teamChat1, 0, 0, 0);
}

static void CG_DrawAreaChat(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader) {
  CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, teamChat2, 0, 0, 0);
}

const char *CG_GetKillerText(void) {
	const char *s = "";
	if ( cg.cur_lc && cg.cur_lc->killerName[0] ) {
		s = va("Fragged by %s", cg.cur_lc->killerName );
	}
	return s;
}


static void CG_DrawKiller(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	// fragged by ... line
	if ( cg.cur_lc && cg.cur_lc->killerName[0] ) {
		int x = rect->x + rect->w / 2;
	  CG_Text_Paint(x - CG_Text_Width(CG_GetKillerText(), scale, 0) / 2, rect->y + rect->h, scale, color, CG_GetKillerText(), 0, 0, textStyle);
	}
	
}


static void CG_DrawCapFragLimit(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle) {
	int limit = (cgs.gametype >= GT_CTF) ? cgs.capturelimit : cgs.fraglimit;
	CG_Text_Paint(rect->x, rect->y, scale, color, va("%2i", limit),0, 0, textStyle); 
}

static void CG_Draw1stPlace(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle) {
	if (cgs.scores1 != SCORE_NOT_PRESENT) {
		CG_Text_Paint(rect->x, rect->y, scale, color, va("%2i", cgs.scores1),0, 0, textStyle); 
	}
}

static void CG_Draw2ndPlace(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle) {
	if (cgs.scores2 != SCORE_NOT_PRESENT) {
		CG_Text_Paint(rect->x, rect->y, scale, color, va("%2i", cgs.scores2),0, 0, textStyle); 
	}
}

const char *CG_GetGameStatusText(void) {
	const char *s = "";
	if ( cgs.gametype < GT_TEAM) {
		if (cg.cur_ps && cg.cur_ps->persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
			s = va("%s place with %i",CG_PlaceString( cg.cur_ps->persistant[PERS_RANK] + 1 ),cg.cur_ps->persistant[PERS_SCORE] );
		}
	} else {
		if ( cg.teamScores[0] == cg.teamScores[1] ) {
			s = va("Teams are tied at %i", cg.teamScores[0] );
		} else if ( cg.teamScores[0] >= cg.teamScores[1] ) {
			s = va("Red leads Blue, %i to %i", cg.teamScores[0], cg.teamScores[1] );
		} else {
			s = va("Blue leads Red, %i to %i", cg.teamScores[1], cg.teamScores[0] );
		}
	}
	return s;
}
	
static void CG_DrawGameStatus(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, CG_GetGameStatusText(), 0, 0, textStyle);
}

const char *CG_GameTypeString(void) {
	return cgs.gametypeName;
}
static void CG_DrawGameType(rectDef_t *rect, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	CG_Text_Paint(rect->x, rect->y + rect->h, scale, color, CG_GameTypeString(), 0, 0, textStyle);
}



#define PIC_WIDTH 12

void CG_DrawNewTeamInfo(rectDef_t *rect, float text_x, float text_y, float scale, vec4_t color, qhandle_t shader) {
	int xx;
	float y;
	int i, j, len, count;
	const char *p;
	vec4_t		hcolor;
	float pwidth, lwidth, maxx, leftOver;
	playerInfo_t *pi;
	gitem_t	*item;
#ifdef MISSIONPACK
	qhandle_t h;
#endif
	int team;

	team = cg.cur_ps->persistant[PERS_TEAM];

	if (cg.time - sortedTeamPlayersTime[team] > 5000) {
		// Info is too out of date.
		return;
	}

	// max player name width
	pwidth = 0;
	count = (numSortedTeamPlayers[team] > 8) ? 8 : numSortedTeamPlayers[team];
	for (i = 0; i < count; i++) {
		pi = cgs.playerinfo + sortedTeamPlayers[team][i];
		if ( pi->infoValid && pi->team == team) {
			len = CG_Text_Width( pi->name, scale, 0);
			if (len > pwidth)
				pwidth = len;
		}
	}

	// max location name width
	lwidth = 0;
	for (i = 1; i < MAX_LOCATIONS; i++) {
		p = CG_ConfigString(CS_LOCATIONS + i);
		if (p && *p) {
			len = CG_Text_Width(p, scale, 0);
			if (len > lwidth)
				lwidth = len;
		}
	}

	y = rect->y;

	for (i = 0; i < count; i++) {
		pi = cgs.playerinfo + sortedTeamPlayers[team][i];
		if ( pi->infoValid && pi->team == team) {

			xx = rect->x + 1;
			for (j = 0; j <= PW_NUM_POWERUPS; j++) {
				if (pi->powerups & (1 << j)) {

					item = BG_FindItemForPowerup( j );

					if (item) {
						CG_DrawPic( xx, y, PIC_WIDTH, PIC_WIDTH, trap_R_RegisterShader( item->icon ) );
						xx += PIC_WIDTH;
					}
				}
			}

			// FIXME: max of 3 powerups shown properly
			xx = rect->x + (PIC_WIDTH * 3) + 2;

			CG_GetColorForHealth( pi->health, pi->armor, hcolor );
			trap_R_SetColor(hcolor);
			CG_DrawPic( xx, y + 1, PIC_WIDTH - 2, PIC_WIDTH - 2, cgs.media.heartShader );

			//Com_sprintf (st, sizeof(st), "%3i %3i", pi->health,	pi->armor);
			//CG_Text_Paint(xx, y + text_y, scale, hcolor, st, 0, 0); 

			// draw weapon icon
			xx += PIC_WIDTH + 1;

// weapon used is not that useful, use the space for task
#if 0
			if ( cg_weapons[pi->curWeapon].weaponIcon ) {
				CG_DrawPic( xx, y, PIC_WIDTH, PIC_WIDTH, cg_weapons[pi->curWeapon].weaponIcon );
			} else {
				CG_DrawPic( xx, y, PIC_WIDTH, PIC_WIDTH, cgs.media.deferShader );
			}
#endif

			trap_R_SetColor(NULL);
#ifdef MISSIONPACK
			if (cg.cur_lc->orderPending) {
				// blink the icon
				if ( cg.time > cg.cur_lc->orderTime - 2500 && (cg.time >> 9 ) & 1 ) {
					h = 0;
				} else {
					h = CG_StatusHandle(cg.cur_lc->currentOrder);
				}
			}	else {
				h = CG_StatusHandle(pi->teamTask);
			}

			if (h) {
				CG_DrawPic( xx, y, PIC_WIDTH, PIC_WIDTH, h);
			}
#endif

			xx += PIC_WIDTH + 1;

			leftOver = rect->w - xx;
			maxx = xx + leftOver / 3;



			CG_Text_Paint_Limit(&maxx, xx, y + text_y, scale, color, pi->name, 0, 0); 

			p = CG_ConfigString(CS_LOCATIONS + pi->location);
			if (!p || !*p) {
				p = "unknown";
			}

			xx += leftOver / 3 + 2;
			maxx = rect->w - 4;

			CG_Text_Paint_Limit(&maxx, xx, y + text_y, scale, color, p, 0, 0); 
			y += text_y + 2;
			if ( y + text_y + 2 > rect->y + rect->h ) {
				break;
			}

		}
	}
}


#define SPECTATORS_PIXELS_PER_SECOND 30.0f

static void CG_DrawTeamSpectators( rectDef_t *rect, float scale, vec4_t color, qhandle_t shader ) {
	char  *text = cg.spectatorList;
	float textWidth = MAX(rect->w, CG_Text_Width( text, scale, 0 ));
	int now = trap_Milliseconds();
	int delta = now - cg.spectatorTime;

	CG_SetClipRegion( rect->x, rect->y, rect->w, rect->h );

	CG_Text_PaintInMotion( rect->x - cg.spectatorOffset, rect->y + rect->h - 3, scale, color, text, 0, 0, 0 );
	CG_Text_PaintInMotion( rect->x + textWidth - cg.spectatorOffset, rect->y + rect->h - 3, scale, color, text, 0, 0, 0 );

	CG_ClearClipRegion( );

	cg.spectatorOffset += ( delta / 1000.0f ) * SPECTATORS_PIXELS_PER_SECOND;

	while( cg.spectatorOffset > textWidth )
		cg.spectatorOffset -= textWidth;

	cg.spectatorTime = now;
}


void CG_DrawMedal(int ownerDraw, rectDef_t *rect, float scale, vec4_t color, qhandle_t shader) {
	score_t *score;
	float value = 0;
	char *text = NULL;
	color[3] = 0.25;

	if ( !cg.cur_lc ) {
		return;
	}

	score = &cg.scores[cg.cur_lc->selectedScore];

	switch (ownerDraw) {
		case CG_ACCURACY:
			value = score->accuracy;
			break;
		case CG_ASSISTS:
			value = score->assistCount;
			break;
		case CG_DEFEND:
			value = score->defendCount;
			break;
		case CG_EXCELLENT:
			value = score->excellentCount;
			break;
		case CG_IMPRESSIVE:
			value = score->impressiveCount;
			break;
		case CG_PERFECT:
			value = score->perfect;
			break;
		case CG_GAUNTLET:
			value = score->guantletCount;
			break;
		case CG_CAPTURES:
			value = score->captures;
			break;
	}

	if (value > 0) {
		if (ownerDraw != CG_PERFECT) {
			if (ownerDraw == CG_ACCURACY) {
				text = va("%i%%", (int)value);
				if (value > 50) {
					color[3] = 1.0;
				}
			} else {
				text = va("%i", (int)value);
				color[3] = 1.0;
			}
		} else {
			if (value) {
				color[3] = 1.0;
			}
			text = "Wow";
		}
	}

	trap_R_SetColor(color);
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );

	if (text) {
		color[3] = 1.0;
		value = CG_Text_Width(text, scale, 0);
		CG_Text_Paint(rect->x + (rect->w - value) / 2, rect->y + rect->h + 10 , scale, color, text, 0, 0, 0);
	}
	trap_R_SetColor(NULL);

}

	
//
void CG_OwnerDraw(float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, float scale, vec4_t color, qhandle_t shader, int textStyle) {
	rectDef_t rect;

  if ( cg_drawStatus.integer == 0 ) {
		return;
	}

	//if (ownerDrawFlags != 0 && !CG_OwnerDrawVisible(ownerDrawFlags)) {
	//	return;
	//}

  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  switch (ownerDraw) {
  case CG_PLAYER_ARMOR_ICON:
    CG_DrawPlayerArmorIcon(&rect, ownerDrawFlags & CG_SHOW_2DONLY);
    break;
  case CG_PLAYER_ARMOR_ICON2D:
    CG_DrawPlayerArmorIcon(&rect, qtrue);
    break;
  case CG_PLAYER_ARMOR_VALUE:
    CG_DrawPlayerArmorValue(&rect, scale, color, shader, textStyle);
    break;
  case CG_PLAYER_AMMO_ICON:
    CG_DrawPlayerAmmoIcon(&rect, ownerDrawFlags & CG_SHOW_2DONLY);
    break;
  case CG_PLAYER_AMMO_ICON2D:
    CG_DrawPlayerAmmoIcon(&rect, qtrue);
    break;
  case CG_PLAYER_AMMO_VALUE:
    CG_DrawPlayerAmmoValue(&rect, scale, color, shader, textStyle);
    break;
  case CG_SELECTEDPLAYER_HEAD:
    CG_DrawSelectedPlayerHead(&rect, ownerDrawFlags & CG_SHOW_2DONLY, qfalse);
    break;
#ifdef MISSIONPACK
  case CG_VOICE_HEAD:
    CG_DrawSelectedPlayerHead(&rect, ownerDrawFlags & CG_SHOW_2DONLY, qtrue);
    break;
  case CG_VOICE_NAME:
    CG_DrawSelectedPlayerName(&rect, scale, color, qtrue, textStyle);
    break;
  case CG_SELECTEDPLAYER_STATUS:
    CG_DrawSelectedPlayerStatus(&rect);
    break;
#endif
  case CG_SELECTEDPLAYER_ARMOR:
    CG_DrawSelectedPlayerArmor(&rect, scale, color, shader, textStyle);
    break;
  case CG_SELECTEDPLAYER_HEALTH:
    CG_DrawSelectedPlayerHealth(&rect, scale, color, shader, textStyle);
    break;
  case CG_SELECTEDPLAYER_NAME:
    CG_DrawSelectedPlayerName(&rect, scale, color, qfalse, textStyle);
    break;
  case CG_SELECTEDPLAYER_LOCATION:
    CG_DrawSelectedPlayerLocation(&rect, scale, color, textStyle);
    break;
  case CG_SELECTEDPLAYER_WEAPON:
    CG_DrawSelectedPlayerWeapon(&rect);
    break;
  case CG_SELECTEDPLAYER_POWERUP:
    CG_DrawSelectedPlayerPowerup(&rect, ownerDrawFlags & CG_SHOW_2DONLY);
    break;
  case CG_PLAYER_HEAD:
    CG_DrawPlayerHead(&rect, ownerDrawFlags & CG_SHOW_2DONLY);
    break;
  case CG_PLAYER_ITEM:
    CG_DrawPlayerItem(&rect, scale, ownerDrawFlags & CG_SHOW_2DONLY);
    break;
  case CG_PLAYER_SCORE:
    CG_DrawPlayerScore(&rect, scale, color, shader, textStyle);
    break;
  case CG_PLAYER_HEALTH:
    CG_DrawPlayerHealth(&rect, scale, color, shader, textStyle);
    break;
  case CG_RED_SCORE:
    CG_DrawRedScore(&rect, scale, color, shader, textStyle);
    break;
  case CG_BLUE_SCORE:
    CG_DrawBlueScore(&rect, scale, color, shader, textStyle);
    break;
  case CG_RED_NAME:
    CG_DrawRedName(&rect, scale, color, textStyle);
    break;
  case CG_BLUE_NAME:
    CG_DrawBlueName(&rect, scale, color, textStyle);
    break;
  case CG_BLUE_FLAGHEAD:
    CG_DrawBlueFlagHead(&rect);
    break;
  case CG_BLUE_FLAGSTATUS:
    CG_DrawBlueFlagStatus(&rect, shader);
    break;
  case CG_BLUE_FLAGNAME:
    CG_DrawBlueFlagName(&rect, scale, color, textStyle);
    break;
  case CG_RED_FLAGHEAD:
    CG_DrawRedFlagHead(&rect);
    break;
  case CG_RED_FLAGSTATUS:
    CG_DrawRedFlagStatus(&rect, shader);
    break;
  case CG_RED_FLAGNAME:
    CG_DrawRedFlagName(&rect, scale, color, textStyle);
    break;
#ifdef MISSIONPACK
  case CG_HARVESTER_SKULLS:
    CG_HarvesterSkulls(&rect, scale, color, qfalse, textStyle);
    break;
  case CG_HARVESTER_SKULLS2D:
    CG_HarvesterSkulls(&rect, scale, color, qtrue, textStyle);
    break;
  case CG_ONEFLAG_STATUS:
    CG_OneFlagStatus(&rect);
    break;
#endif
  case CG_PLAYER_LOCATION:
    CG_DrawPlayerLocation(&rect, scale, color, textStyle);
    break;
  case CG_TEAM_COLOR:
    CG_DrawTeamColor(&rect, color);
    break;
#ifdef MISSIONPACK
  case CG_CTF_POWERUP:
    CG_DrawCTFPowerUp(&rect);
    break;
#endif
  case CG_AREA_POWERUP:
		CG_DrawAreaPowerUp(&rect, align, special, scale, color);
    break;
#ifdef MISSIONPACK
  case CG_PLAYER_STATUS:
    CG_DrawPlayerStatus(&rect);
    break;
#endif
  case CG_PLAYER_HASFLAG:
    CG_DrawPlayerHasFlag(&rect, qfalse);
    break;
  case CG_PLAYER_HASFLAG2D:
    CG_DrawPlayerHasFlag(&rect, qtrue);
    break;
  case CG_AREA_SYSTEMCHAT:
    CG_DrawAreaSystemChat(&rect, scale, color, shader);
    break;
  case CG_AREA_TEAMCHAT:
    CG_DrawAreaTeamChat(&rect, scale, color, shader);
    break;
  case CG_AREA_CHAT:
    CG_DrawAreaChat(&rect, scale, color, shader);
    break;
  case CG_GAME_TYPE:
    CG_DrawGameType(&rect, scale, color, shader, textStyle);
    break;
  case CG_GAME_STATUS:
    CG_DrawGameStatus(&rect, scale, color, shader, textStyle);
		break;
  case CG_KILLER:
    CG_DrawKiller(&rect, scale, color, shader, textStyle);
		break;
	case CG_ACCURACY:
	case CG_ASSISTS:
	case CG_DEFEND:
	case CG_EXCELLENT:
	case CG_IMPRESSIVE:
	case CG_PERFECT:
	case CG_GAUNTLET:
	case CG_CAPTURES:
		CG_DrawMedal(ownerDraw, &rect, scale, color, shader);
		break;
  case CG_SPECTATORS:
		CG_DrawTeamSpectators(&rect, scale, color, shader);
		break;
  case CG_TEAMINFO:
		if (cg_currentSelectedPlayer[cg.cur_localPlayerNum].integer == numSortedTeamPlayers[cg.cur_ps->persistant[PERS_TEAM]]) {
			CG_DrawNewTeamInfo(&rect, text_x, text_y, scale, color, shader);
		}
		break;
  case CG_CAPFRAGLIMIT:
    CG_DrawCapFragLimit(&rect, scale, color, shader, textStyle);
		break;
  case CG_1STPLACE:
    CG_Draw1stPlace(&rect, scale, color, shader, textStyle);
		break;
  case CG_2NDPLACE:
    CG_Draw2ndPlace(&rect, scale, color, shader, textStyle);
		break;
  default:
    break;
  }
}

void CG_MouseEvent(int localPlayerNum, int x, int y) {
	int n;
	localPlayer_t *player;

	if (localPlayerNum != 0) {
		// Missionpack HUD currently only supports one cursor
		return;
	}

	Init_Display(&cgDC);

	cgDC.cursorx = cgs.cursorX;
	cgDC.cursory = cgs.cursorY;

	player = &cg.localPlayers[0];

	if ( (player->predictedPlayerState.pm_type == PM_NORMAL || player->predictedPlayerState.pm_type == PM_SPECTATOR) && player->showScores == qfalse) {
		Key_SetCatcher(0);
		return;
	}

	cgs.cursorX+= x;
	if (cgs.cursorX < 0)
		cgs.cursorX = 0;
	else if (cgs.cursorX > 640)
		cgs.cursorX = 640;

	cgs.cursorY += y;
	if (cgs.cursorY < 0)
		cgs.cursorY = 0;
	else if (cgs.cursorY > 480)
		cgs.cursorY = 480;

	n = Display_CursorType(cgs.cursorX, cgs.cursorY);
	cgs.activeCursor = 0;
	if (n == CURSOR_ARROW) {
		cgs.activeCursor = cgs.media.selectCursor;
	} else if (n == CURSOR_SIZER) {
		cgs.activeCursor = cgs.media.sizeCursor;
	}

  if (cgs.capturedItem) {
	  Display_MouseMove(cgs.capturedItem, x, y);
  } else {
	  Display_MouseMove(NULL, cgs.cursorX, cgs.cursorY);
  }

}

/*
==================
CG_HideTeamMenus
==================

*/
void CG_HideTeamMenu( void ) {
  Init_Display(&cgDC);
  Menus_CloseByName("teamMenu");
  Menus_CloseByName("getMenu");
}

/*
==================
CG_ShowTeamMenus
==================

*/
void CG_ShowTeamMenu( void ) {
  Init_Display(&cgDC);
  Menus_OpenByName("teamMenu");
}




/*
==================
CG_EventHandling
==================
 type 0 - no event handling
      1 - team menu
      2 - hud editor

*/
void CG_EventHandling(int type) {
	cgs.eventHandling = type;
  if (type == CGAME_EVENT_NONE) {
    CG_HideTeamMenu();
  } else if (type == CGAME_EVENT_TEAMMENU) {
    //CG_ShowTeamMenu();
  } else if (type == CGAME_EVENT_SCOREBOARD) {
  }

}



void CG_KeyEvent(int key, qboolean down) {
	localPlayer_t *player;

	if (!down) {
		return;
	}

	Init_Display(&cgDC);

	// escape always gets out of CGAME stuff
	if (key == K_ESCAPE) {
		Key_SetCatcher( Key_GetCatcher( ) & ~KEYCATCH_CGAME );
		CG_EventHandling(CGAME_EVENT_NONE);
		return;
	}

	player = &cg.localPlayers[0];

	if ( player->predictedPlayerState.pm_type == PM_NORMAL || (player->predictedPlayerState.pm_type == PM_SPECTATOR && player->showScores == qfalse)) {
		CG_EventHandling(CGAME_EVENT_NONE);
		Key_SetCatcher(0);
		return;
	}

  //if (key == trap_Key_GetKey("teamMenu", 0) || !Display_CaptureItem(cgs.cursorX, cgs.cursorY)) {
    // if we see this then we should always be visible
  //  CG_EventHandling(CGAME_EVENT_NONE);
  //  Key_SetCatcher(0);
  //}



  Display_HandleKey(key, down, cgs.cursorX, cgs.cursorY);

	if (cgs.capturedItem) {
		cgs.capturedItem = NULL;
	}	else {
		if (key == K_MOUSE2 && down) {
			cgs.capturedItem = Display_CaptureItem(cgs.cursorX, cgs.cursorY);
		}
	}
}
#endif // MISSIONPACK_HUD

void CG_RunMenuScript(char **args) {
}


void CG_GetTeamColor(vec4_t *color) {
  if (cg.cur_ps && cg.cur_ps->persistant[PERS_TEAM] == TEAM_RED) {
    (*color)[0] = 1.0f;
    (*color)[3] = 0.25f;
    (*color)[1] = (*color)[2] = 0.0f;
  } else if (cg.cur_ps && cg.cur_ps->persistant[PERS_TEAM] == TEAM_BLUE) {
    (*color)[0] = (*color)[1] = 0.0f;
    (*color)[2] = 1.0f;
    (*color)[3] = 0.25f;
  } else {
    (*color)[0] = (*color)[2] = 0.0f;
    (*color)[1] = 0.17f;
    (*color)[3] = 0.25f;
	}
}

#endif

