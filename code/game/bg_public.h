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
// bg_public.h -- definitions shared by both the server game and client game modules

#include "../qcommon/surfaceflags.h"

#ifndef MODDIR
  #ifdef MISSIONPACK
    #define MODDIR "missionpack"
  #else
    #define MODDIR "baseq3"
  #endif
#endif

#define PRODUCT_NAME				MODDIR

// Keep this in-sync with VERSION in Makefile.
#ifndef PRODUCT_VERSION
	#define PRODUCT_VERSION			"1.0.3"
#endif

// because games can change separately from the main system protocol, we need a
// second protocol that must match between game and cgame

#define	GAME_PROTOCOL		MODDIR "-4"

// used for switching fs_game
#ifndef BASEQ3
	#define BASEQ3			"baseq3"
#endif
#ifndef BASETA
	#define BASETA			"missionpack"
#endif

#define	DEFAULT_GRAVITY		800
#define	GIB_HEALTH			-40
#define	ARMOR_PROTECTION	0.66

#define	MAX_LOCATIONS		64

#define	MAX_SAY_TEXT	150

#define	SAY_ALL			0
#define	SAY_TEAM		1
#define	SAY_TELL		2

#define CHATPLAYER_SERVER	-1
#define CHATPLAYER_UNKNOWN	-2

#define	MODELINDEX_BITS		10

// these are networked using the modelindex and/or modelindex2 field, must fit in MODELINDEX_BITS
#define	MAX_SUBMODELS		1024	// max bsp models, q3map2 limits to 1024 via MAX_MAP_MODELS
#define	MAX_ITEMS			256		// max item types
#define	MAX_MODELS			256		// max model filenames set by game VM

#define	MAX_SOUNDS			256		// this is sent over the net as 8 bits (in eventParm)
									// so they cannot be blindly increased

#define	RANK_TIED_FLAG		0x4000

#define DEFAULT_SHOTGUN_SPREAD	700
#define DEFAULT_SHOTGUN_COUNT	11

#define	ITEM_RADIUS			15		// item sizes are needed for client side pickup detection

#define	LIGHTNING_RANGE		768

#define	SCORE_NOT_PRESENT	-9999	// for the CS_SCORES[12] when only one player is present

#define	VOTE_TIME			30000	// 30 seconds before vote times out

#define	MINS_Z				-24
#define	DEFAULT_VIEWHEIGHT	26
#define CROUCH_VIEWHEIGHT	12
#define	DEAD_VIEWHEIGHT		-16

#define STEPSIZE			18

#define	BODY_SINK_DELAY		5000
#define	BODY_SINK_TIME		1500

#ifdef MISSIONPACK
#define OBELISK_TARGET_HEIGHT	56
#endif


#define MAX_DLIGHT_CONFIGSTRINGS 128

//
// config strings are a general means of communicating variable length strings
// from the server to all connected clients.
//

// CS_SERVERINFO and CS_SYSTEMINFO are defined in q_shared.h
#define	CS_MUSIC				2
#define	CS_MESSAGE				3		// from the map worldspawn's message field
#define	CS_MOTD					4		// g_motd string for server message of the day
#define	CS_WARMUP				5		// server time when the match will be restarted
#define	CS_SCORES1				6
#define	CS_SCORES2				7
#define CS_VOTE_TIME			8
#define CS_VOTE_STRING			9
#define	CS_VOTE_YES				10
#define	CS_VOTE_NO				11

#define CS_TEAMVOTE_TIME		12
#define CS_TEAMVOTE_STRING		14
#define	CS_TEAMVOTE_YES			16
#define	CS_TEAMVOTE_NO			18

#define	CS_GAME_PROTOCOL		20
#define	CS_LEVEL_START_TIME		21		// so the timer only shows the current level
#define	CS_INTERMISSION			22		// when 1, fraglimit/timelimit has been hit and intermission will start in a second or two
#define CS_FLAGSTATUS			23		// string indicating flag status in CTF
#define CS_SHADERSTATE			24
#define	CS_PLAYERS_READY		25		// players wishing to exit the intermission

#define	CS_ITEMS				27		// string of 0's and 1's that tell which items are present

#define	CS_MODELS				32
#define	CS_SOUNDS				(CS_MODELS+MAX_MODELS)
#define	CS_PLAYERS				(CS_SOUNDS+MAX_SOUNDS)
#define CS_LOCATIONS			(CS_PLAYERS+MAX_CLIENTS)
#define CS_PARTICLES			(CS_LOCATIONS+MAX_LOCATIONS)
#define CS_DLIGHTS              (CS_PARTICLES+MAX_LOCATIONS)
#define CS_BOTINFO				(CS_DLIGHTS+MAX_DLIGHT_CONFIGSTRINGS)

#define CS_MAX					(CS_BOTINFO+MAX_CLIENTS)

#if (CS_MAX) > MAX_CONFIGSTRINGS
#error overflow: (CS_MAX) > MAX_CONFIGSTRINGS
#endif

typedef enum {
	GT_FFA,				// free for all
	GT_TOURNAMENT,		// one on one tournament
	GT_SINGLE_PLAYER,	// single player ffa

	//-- team games go after this --

	GT_TEAM,			// team deathmatch
	GT_CTF,				// capture the flag
#ifdef MISSIONPACK
	GT_1FCTF,
	GT_OBELISK,
	GT_HARVESTER,
#endif
	GT_MAX_GAME_TYPE
} gametype_t;

extern const char *bg_netGametypeNames[GT_MAX_GAME_TYPE];
extern const char *bg_displayGametypeNames[GT_MAX_GAME_TYPE];

typedef enum { GENDER_MALE, GENDER_FEMALE, GENDER_NEUTER } gender_t;

typedef enum {
	TR_STATIONARY,
	TR_INTERPOLATE,				// non-parametric, but interpolate between snapshots
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_SINE,					// value = base + sin( time / duration ) * delta
	TR_GRAVITY
} trType_t;

typedef struct {
	trType_t	trType;
	int		trTime;
	int		trDuration;			// if non 0, trTime + trDuration = stop time
	vec3_t	trBase;
	vec3_t	trDelta;			// velocity, etc
} trajectory_t;

/*
===================================================================================

ENTITY STATE / PLAYER STATE

server game / client game definitions of entity state and player state.

Add new fields to bg_entityStateFields / bg_playerStateFields in bg_misc.c
===================================================================================
*/

// entityState_t is the information conveyed from the server
// in an update message about entities that the client will
// need to render in some way
// Different eTypes may use the information in different ways
// The messages are delta compressed, so it doesn't really matter if
// the structure size is fairly large

typedef struct entityState_s {
	int		number;			// entity index

	int		contents;		// CONTENTS_TRIGGER, CONTENTS_SOLID, CONTENTS_BODY, etc
							// a non-solid entity should set to 0

	collisionType_t	collisionType;	// if CT_SUBMODEL, modelindex is an inline model number. only set by trap_SetBrushModel
									// if CT_CAPSULE, use capsule instead of bbox for clipping against this ent
									// else (CT_AABB), assume an explicit mins / maxs bounding box

	int		modelindex;

	vec3_t		mins, maxs;	// bounding box size

	vec3_t	origin;
	vec3_t	origin2;

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
	//================================

	int		eType;			// entityType_t
	int		eFlags;

	trajectory_t	pos;	// for calculating position
	trajectory_t	apos;	// for calculating angles

	int		time;
	int		time2;

	vec3_t	angles;
	vec3_t	angles2;

	int		otherEntityNum;	// shotgun sources, etc
	int		otherEntityNum2;

	int		groundEntityNum;	// ENTITYNUM_NONE = in air

	int		constantLight;	// r + (g<<8) + (b<<16) + (intensity<<24)
	int		dl_intensity;	// used for coronas
	int		loopSound;		// constantly loop this sound

	int		modelindex2;
	int		playerNum;		// 0 to (MAX_CLIENTS - 1), for players and corpses
	int		frame;

	int		event;			// impulse events -- muzzle flashes, footsteps, etc
	int		eventParm;

	int		team;

	int		density;            // for particle effects

	// for players
	int		powerups;		// bit flags
	int		weapon;			// determines weapon and flash model, etc
	int		legsAnim;		// mask off ANIM_TOGGLEBIT
	int		torsoAnim;		// mask off ANIM_TOGGLEBIT
	int		tokens;			// harvester skulls
} entityState_t;


// bit limits
#define WEAPONNUM_BITS			4

// array limits (engine will only network arrays <= 1024 elements)
#define	MAX_STATS				16
#define	MAX_PERSISTANT			16
#define	MAX_POWERUPS			16 // entityState_t::powerups bit field limits this to <= 32.
#define	MAX_WEAPONS				(1<<WEAPONNUM_BITS) // playerState_t::stats[STAT_WEAPONS] bit field limits this to <= 16.

#define	MAX_PS_EVENTS			2

#define PS_PMOVEFRAMECOUNTBITS	6

// playerState_t is the information needed by both the client and server
// to predict player motion and actions
// nothing outside of pmove should modify these, or some degree of prediction error
// will occur

// you can't add anything to this without modifying the code in msg.c

// playerState_t is a full superset of entityState_t as it is used by players,
// so if a playerState_t is transmitted, the entityState_t can be fully derived
// from it.

typedef struct playerState_s {
	int			commandTime;	// cmd->serverTime of last executed command

	vec3_t		origin;

	qboolean	linked;			// set by server

	int			playerNum;		// ranges from 0 to MAX_CLIENTS-1

	vec3_t		viewangles;		// for fixed views
	int			viewheight;

	// ping is not communicated over the net at all
	int			ping;			// server to game info for scoreboard

	// ZTM: FIXME: make persistant private to game/cgame. Currently server accesses PERS_SCORE (0) in it.
	int			persistant[MAX_PERSISTANT];	// stats that aren't cleared on death

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
	//================================

	//int			commandTime;	// cmd->serverTime of last executed command
	int			pm_type;
	int			bobCycle;		// for view bobbing and footstep generation
	int			pm_flags;		// ducked, jump_held, etc
	int			pm_time;

	//vec3_t		origin;
	vec3_t		velocity;
	int			weaponTime;
	int			gravity;
	int			speed;
	int			delta_angles[3];	// add to command angles to get view direction
									// changed by spawns, rotating objects, and teleporters

	vec3_t		mins, maxs;		// bounding box size

	int			groundEntityNum;// ENTITYNUM_NONE = in air

	int			legsTimer;		// don't change low priority animations until this runs out
	int			legsAnim;		// mask off ANIM_TOGGLEBIT

	int			torsoTimer;		// don't change low priority animations until this runs out
	int			torsoAnim;		// mask off ANIM_TOGGLEBIT

	int			movementDir;	// a number 0 to 7 that represents the relative angle
								// of movement to the view angle (axial and diagonals)
								// when at rest, the value will remain unchanged
								// used to twist the legs during strafing

	vec3_t		grapplePoint;	// location of grapple to pull towards if PMF_GRAPPLE_PULL

	int			eFlags;				// copied to entityState_t->eFlags
	int			contents;			// copied to entityState_t->contents
	collisionType_t	collisionType;	// copied to entityState_t->capsule
	//qboolean	linked;				// set by server

	int			eventSequence;	// pmove generated events
	int			events[MAX_PS_EVENTS];
	int			eventParms[MAX_PS_EVENTS];

	int			externalEvent;	// events set on player from another source
	int			externalEventParm;

	//int			playerNum;		// ranges from 0 to MAX_CLIENTS-1
	int			weapon;			// copied to entityState_t->weapon
	int			weaponstate;

	//vec3_t		viewangles;		// for fixed views
	//int			viewheight;

	// damage feedback
	int			damageEvent;	// when it changes, latch the other parms
	int			damageYaw;
	int			damagePitch;
	int			damageCount;

	int			stats[MAX_STATS];
	//int			persistant[MAX_PERSISTANT];	// stats that aren't cleared on death
	int			powerups[MAX_POWERUPS];	// level.time that the powerup runs out
	int			ammo[MAX_WEAPONS];

	int			tokens;			// harvester skulls
	int			loopSound;
	int			jumppad_ent;	// jumppad entity hit this frame

	// not communicated over the net at all
	//int			ping;			// server to game info for scoreboard
	int			pmove_framecount;
	int			jumppad_frame;
	int			entityEventSequence;
} playerState_t;

extern vmNetField_t	bg_entityStateFields[];
extern int			bg_numEntityStateFields;

extern vmNetField_t	bg_playerStateFields[];
extern int			bg_numPlayerStateFields;


/*
===================================================================================

usercmd_t->button bits definitions

===================================================================================
*/

#define	BUTTON_ATTACK		(1<<0)

#define	BUTTON_USE_HOLDABLE	(1<<2)
#define	BUTTON_GESTURE		(1<<3)

#define	BUTTON_AFFIRMATIVE	(1<<5)
#define	BUTTON_NEGATIVE		(1<<6)
#define	BUTTON_GETFLAG		(1<<7)
#define	BUTTON_GUARDBASE	(1<<8)
#define	BUTTON_PATROL		(1<<9)
#define	BUTTON_FOLLOWME		(1<<10)

#define	BUTTON_TALK			(1<<13)		// displays talk balloon and disables actions

#define	BUTTON_WALKING		(1<<14)		// walking can't just be infered from MOVE_RUN
										// because a key pressed late in the frame will
										// only generate a small move value for that frame
										// walking will use different animations and
										// won't generate footsteps

#define	BUTTON_ANY			(1<<15)		// any key whatsoever

#define BUTTONS_GENERATED (BUTTON_TALK|BUTTON_WALKING|BUTTON_ANY)

#define	MOVE_RUN			120			// if forwardmove or rightmove are >= MOVE_RUN,
										// then BUTTON_WALKING should be set


/*
===================================================================================

PMOVE MODULE

The pmove code takes a player_state_t and a usercmd_t and generates a new player_state_t
and some other output data.  Used for local prediction on the client game and true
movement on the server game.
===================================================================================
*/

typedef enum {
	PM_NORMAL,		// can accelerate and turn
	PM_NOCLIP,		// noclip movement
	PM_SPECTATOR,	// still run into walls
	PM_DEAD,		// no acceleration or turning, but free falling
	PM_FREEZE,		// stuck in place with no control
	PM_INTERMISSION,	// no movement or status bar
	PM_SPINTERMISSION	// no movement or status bar
} pmtype_t;

typedef enum {
	WEAPON_READY, 
	WEAPON_RAISING,
	WEAPON_DROPPING,
	WEAPON_FIRING
} weaponstate_t;

// pmove->pm_flags
#define	PMF_DUCKED			1
#define	PMF_JUMP_HELD		2
#define	PMF_BACKWARDS_JUMP	8		// go into backwards land
#define	PMF_BACKWARDS_RUN	16		// coast down to backwards run
#define	PMF_TIME_LAND		32		// pm_time is time before rejump
#define	PMF_TIME_KNOCKBACK	64		// pm_time is an air-accelerate only time

#define	PMF_TIME_WATERJUMP	256		// pm_time is waterjump
#define	PMF_RESPAWNED		512		// clear after attack and jump buttons come up
#define	PMF_USE_ITEM_HELD	1024
#define PMF_GRAPPLE_PULL	2048	// pull towards grapple location
#define PMF_FOLLOW			4096	// spectate following another player
#define PMF_SCOREBOARD		8192	// spectate as a scoreboard
#define PMF_INVULEXPAND		16384	// invulnerability sphere set to full size

#define	PMF_ALL_TIMES	(PMF_TIME_WATERJUMP|PMF_TIME_LAND|PMF_TIME_KNOCKBACK)

#define	MAXTOUCH	32
typedef struct {
	// state (in / out)
	playerState_t	*ps;

	// command (in)
	usercmd_t	cmd;
	int			tracemask;			// collide against these types of surfaces
	int			debugLevel;			// if set, diagnostic output will be printed
	qboolean	noFootsteps;		// if the game is setup for no footsteps by the server
	qboolean	gauntletHit;		// true if a gauntlet attack would actually hit something

	int			framecount;

	// results (out)
	int			numtouch;
	int			touchents[MAXTOUCH];

	int			watertype;
	int			waterlevel;

	float		xyspeed;

	// enables overbounce bug
	qboolean	pmove_overbounce;

	// for fixed msec Pmove
	int			pmove_fixed;
	int			pmove_msec;

	// callbacks to test the world
	// these will be different functions during game and cgame
	void		(*trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask );
	int			(*pointcontents)( const vec3_t point, int passEntityNum );
} pmove_t;

// if a full pmove isn't done on the client, you can just update the angles
void PM_UpdateViewAngles( playerState_t *ps, const usercmd_t *cmd );
void Pmove (pmove_t *pmove);

//===================================================================================


// player_state->stats[] indexes
// NOTE: may not have more than MAX_STATS
typedef enum {
	STAT_HEALTH,
	STAT_HOLDABLE_ITEM,
	STAT_WEAPONS,					// 16 bit fields
	STAT_ARMOR,				
	STAT_DEAD_YAW,					// look this direction when dead (FIXME: get rid of?)
	STAT_MAX_HEALTH,				// health / armor limit, changeable by handicap
//#ifdef MISSIONPACK
	STAT_PERSISTANT_POWERUP
//#endif
} statIndex_t;


// player_state->persistant[] indexes
// these fields are the only part of player_state that isn't
// cleared on respawn
// NOTE: may not have more than MAX_PERSISTANT
typedef enum {
	PERS_SCORE,						// !!! MUST NOT CHANGE, SERVER AND GAME BOTH REFERENCE !!!
	PERS_HITS,						// total points damage inflicted so damage beeps can sound on change
	PERS_RANK,						// player rank or team rank
	PERS_TEAM,						// player team
	PERS_SPAWN_COUNT,				// incremented every respawn
	PERS_PLAYEREVENTS,				// 16 bits that can be flipped for events
	PERS_ATTACKER,					// playerNum of last damage inflicter
	PERS_ATTACKEE_ARMOR,			// health/armor of last person we attacked
	PERS_KILLED,					// count of the number of times you died
	// player awards tracking
	PERS_IMPRESSIVE_COUNT,			// two railgun hits in a row
	PERS_EXCELLENT_COUNT,			// two successive kills in a short amount of time
	PERS_DEFEND_COUNT,				// defend awards
	PERS_ASSIST_COUNT,				// assist awards
	PERS_GAUNTLET_FRAG_COUNT,		// kills with the guantlet
	PERS_CAPTURES					// captures
} persEnum_t;


// entityState_t->eFlags
#define	EF_DEAD				0x00000001		// don't draw a foe marker over players with EF_DEAD
#ifdef MISSIONPACK
#define EF_TICKING			0x00000002		// used to make players play the prox mine ticking sound
#endif
#define	EF_TELEPORT_BIT		0x00000004		// toggled every time the origin abruptly changes
#define	EF_AWARD_EXCELLENT	0x00000008		// draw an excellent sprite
#define EF_PLAYER_EVENT		0x00000010
#define	EF_BOUNCE			0x00000010		// for missiles
#define	EF_BOUNCE_HALF		0x00000020		// for missiles
#define	EF_AWARD_GAUNTLET	0x00000040		// draw a gauntlet sprite
#define	EF_NODRAW			0x00000080		// may have an event, but no model (unspawned items)
#define	EF_FIRING			0x00000100		// for lightning gun
#define	EF_KAMIKAZE			0x00000200
#define	EF_MOVER_STOP		0x00000400		// will push otherwise
#define EF_AWARD_CAP		0x00000800		// draw the capture sprite
#define	EF_TALK				0x00001000		// draw a talk balloon
#define	EF_CONNECTION		0x00002000		// draw a connection trouble sprite
#define	EF_VOTED			0x00004000		// already cast a vote
#define	EF_AWARD_IMPRESSIVE	0x00008000		// draw an impressive sprite
#define	EF_AWARD_DEFEND		0x00010000		// draw a defend sprite
#define	EF_AWARD_ASSIST		0x00020000		// draw a assist sprite
#define EF_AWARD_DENIED		0x00040000		// denied
#define EF_TEAMVOTED		0x00080000		// already cast a team vote
#define EF_GIBBED			0x00100000		// player has been gibbed, client only renders player if com_blood or cg_gibs is 0

// NOTE: may not have more than MAX_POWERUPS
typedef enum {
	PW_NONE,

	PW_QUAD,
	PW_BATTLESUIT,
	PW_HASTE,
	PW_INVIS,
	PW_REGEN,
	PW_FLIGHT,

	PW_REDFLAG,
	PW_BLUEFLAG,
	PW_NEUTRALFLAG,

	PW_SCOUT,
	PW_GUARD,
	PW_DOUBLER,
	PW_AMMOREGEN,
	PW_INVULNERABILITY,

	PW_NUM_POWERUPS

} powerup_t;

typedef enum {
	HI_NONE,

	HI_TELEPORTER,
	HI_MEDKIT,
	HI_KAMIKAZE,
	HI_PORTAL,
	HI_INVULNERABILITY,

	HI_NUM_HOLDABLE
} holdable_t;


// NOTE: may not have more than MAX_WEAPONS
typedef enum {
	WP_NONE,

	WP_GAUNTLET,
	WP_MACHINEGUN,
	WP_SHOTGUN,
	WP_GRENADE_LAUNCHER,
	WP_ROCKET_LAUNCHER,
	WP_LIGHTNING,
	WP_RAILGUN,
	WP_PLASMAGUN,
	WP_BFG,
	WP_GRAPPLING_HOOK,
#ifdef MISSIONPACK
	WP_NAILGUN,
	WP_PROX_LAUNCHER,
	WP_CHAINGUN,
#endif

	WP_NUM_WEAPONS
} weapon_t;


// reward sounds (stored in ps->persistant[PERS_PLAYEREVENTS])
#define	PLAYEREVENT_DENIEDREWARD		0x0001
#define	PLAYEREVENT_GAUNTLETREWARD		0x0002
#define PLAYEREVENT_HOLYSHIT			0x0004

// entityState_t->event values
// entity events are for effects that take place relative
// to an existing entities origin.  Very network efficient.

// two bits at the top of the entityState->event field
// will be incremented with each change in the event so
// that an identical event started twice in a row can
// be distinguished.  And off the value with ~EV_EVENT_BITS
// to retrieve the actual event number
#define	EV_EVENT_BIT1		0x00000100
#define	EV_EVENT_BIT2		0x00000200
#define	EV_EVENT_BITS		(EV_EVENT_BIT1|EV_EVENT_BIT2)

#define	EVENT_VALID_MSEC	300

typedef enum {
	EV_NONE,

	EV_FOOTSTEP,
	EV_FOOTSTEP_METAL,
	EV_FOOTSPLASH,
	EV_FOOTWADE,
	EV_SWIM,

	EV_STEP_4,
	EV_STEP_8,
	EV_STEP_12,
	EV_STEP_16,

	EV_FALL_SHORT,
	EV_FALL_MEDIUM,
	EV_FALL_FAR,

	EV_JUMP_PAD,			// boing sound at origin, jump sound on player

	EV_JUMP,
	EV_WATER_TOUCH,	// foot touches
	EV_WATER_LEAVE,	// foot leaves
	EV_WATER_UNDER,	// head touches
	EV_WATER_CLEAR,	// head leaves

	EV_ITEM_PICKUP,			// normal item pickups are predictable
	EV_GLOBAL_ITEM_PICKUP,	// powerup / team sounds are broadcast to everyone

	EV_NOAMMO,
	EV_CHANGE_WEAPON,
	EV_FIRE_WEAPON,

	EV_USE_ITEM0,
	EV_USE_ITEM1,
	EV_USE_ITEM2,
	EV_USE_ITEM3,
	EV_USE_ITEM4,
	EV_USE_ITEM5,
	EV_USE_ITEM6,
	EV_USE_ITEM7,
	EV_USE_ITEM8,
	EV_USE_ITEM9,
	EV_USE_ITEM10,
	EV_USE_ITEM11,
	EV_USE_ITEM12,
	EV_USE_ITEM13,
	EV_USE_ITEM14,
	EV_USE_ITEM15,

	EV_ITEM_RESPAWN,
	EV_ITEM_POP,
	EV_PLAYER_TELEPORT_IN,
	EV_PLAYER_TELEPORT_OUT,

	EV_GRENADE_BOUNCE,		// eventParm will be the soundindex

	EV_GENERAL_SOUND,
	EV_GLOBAL_SOUND,		// no attenuation
	EV_GLOBAL_TEAM_SOUND,

	EV_BULLET_HIT_FLESH,
	EV_BULLET_HIT_WALL,

	EV_MISSILE_HIT,
	EV_MISSILE_MISS,
	EV_MISSILE_MISS_METAL,
	EV_RAILTRAIL,
	EV_SHOTGUN,

	EV_PAIN,
	EV_DEATH1,
	EV_DEATH2,
	EV_DEATH3,
	EV_OBITUARY,

	EV_POWERUP_QUAD,
	EV_POWERUP_BATTLESUIT,
	EV_POWERUP_REGEN,

	EV_SCOREPLUM,			// score plum

//#ifdef MISSIONPACK
	EV_PROXIMITY_MINE_STICK,
	EV_PROXIMITY_MINE_TRIGGER,
	EV_KAMIKAZE,			// kamikaze explodes
	EV_OBELISKEXPLODE,		// obelisk explodes
	EV_OBELISKPAIN,			// obelisk is in pain
	EV_INVUL_IMPACT,		// invulnerability sphere impact
	EV_JUICED,				// invulnerability juiced effect
	EV_LIGHTNINGBOLT,		// lightning bolt bounced of invulnerability sphere
//#endif

	EV_DEBUG_LINE,
	EV_STOPLOOPINGSOUND,
	EV_TAUNT,
	EV_TAUNT_YES,
	EV_TAUNT_NO,
	EV_TAUNT_FOLLOWME,
	EV_TAUNT_GETFLAG,
	EV_TAUNT_GUARDBASE,
	EV_TAUNT_PATROL

} entity_event_t;


typedef enum {
	GTS_RED_CAPTURE,
	GTS_BLUE_CAPTURE,
	GTS_RED_RETURN,
	GTS_BLUE_RETURN,
	GTS_RED_TAKEN,
	GTS_BLUE_TAKEN,
	GTS_REDOBELISK_ATTACKED,
	GTS_BLUEOBELISK_ATTACKED,
	GTS_REDTEAM_SCORED,
	GTS_BLUETEAM_SCORED,
	GTS_REDTEAM_TOOK_LEAD,
	GTS_BLUETEAM_TOOK_LEAD,
	GTS_TEAMS_ARE_TIED,
	GTS_KAMIKAZE
} global_team_sound_t;

// animations
typedef enum {
	BOTH_DEATH1,
	BOTH_DEAD1,
	BOTH_DEATH2,
	BOTH_DEAD2,
	BOTH_DEATH3,
	BOTH_DEAD3,

	TORSO_GESTURE,

	TORSO_ATTACK,
	TORSO_ATTACK2,

	TORSO_DROP,
	TORSO_RAISE,

	TORSO_STAND,
	TORSO_STAND2,

	LEGS_WALKCR,
	LEGS_WALK,
	LEGS_RUN,
	LEGS_BACK,
	LEGS_SWIM,

	LEGS_JUMP,
	LEGS_LAND,

	LEGS_JUMPB,
	LEGS_LANDB,

	LEGS_IDLE,
	LEGS_IDLECR,

	LEGS_TURN,

	TORSO_GETFLAG,
	TORSO_GUARDBASE,
	TORSO_PATROL,
	TORSO_FOLLOWME,
	TORSO_AFFIRMATIVE,
	TORSO_NEGATIVE,

	MAX_ANIMATIONS,

	LEGS_BACKCR,
	LEGS_BACKWALK,
	FLAG_RUN,
	FLAG_STAND,
	FLAG_STAND2RUN,

	MAX_TOTALANIMATIONS
} animNumber_t;


typedef struct animation_s {
	int		firstFrame;
	int		numFrames;
	int		loopFrames;			// 0 to numFrames
	int		frameLerp;			// msec between frames
	int		initialLerp;		// msec to get to first frame
	int		reversed;			// true if animation is reversed
	int		flipflop;			// true if animation should flipflop back to base
} animation_t;


// flip the togglebit every time an animation
// changes so a restart of the same anim can be detected
#define	ANIM_TOGGLEBIT		128


#define DEFAULT_PLAYER_NAME		"UnnamedPlayer"
#define DEFAULT_PLAYER_COLOR1	4
#define DEFAULT_PLAYER_COLOR2	5

// Default player model names for the splitscreen players
#define DEFAULT_MODEL			"sarge"
#define DEFAULT_HEAD			"sarge"

#define DEFAULT_MODEL2			"grunt"
#define DEFAULT_HEAD2			"grunt"

#define DEFAULT_MODEL3			"major"
#define DEFAULT_HEAD3			"major"

#define DEFAULT_MODEL4			"visor"
#define DEFAULT_HEAD4			"visor"

// For fallback player and gender-specific fallback sounds
#define DEFAULT_MODEL_GENDER	"male"
#define DEFAULT_MODEL_MALE		"sarge"
#define DEFAULT_HEAD_MALE		"sarge"
#define DEFAULT_MODEL_FEMALE	"major"
#define DEFAULT_HEAD_FEMALE		"major"

#ifdef MISSIONPACK
// Default team player model names
#define DEFAULT_TEAM_MODEL		"james"
#define DEFAULT_TEAM_HEAD		"*james"

#define DEFAULT_TEAM_MODEL2		"janet"
#define DEFAULT_TEAM_HEAD2		"*janet"

#define DEFAULT_TEAM_MODEL3		"james"
#define DEFAULT_TEAM_HEAD3		"*james"

#define DEFAULT_TEAM_MODEL4		"janet"
#define DEFAULT_TEAM_HEAD4		"*janet"

// For fallback player and gender-specific fallback sounds
// Also used for Team Arena UI's character base model and CGame player pre-caching
#define DEFAULT_TEAM_MODEL_GENDER	"male"
#define DEFAULT_TEAM_MODEL_MALE		"james"
#define DEFAULT_TEAM_HEAD_MALE		"*james"
#define DEFAULT_TEAM_MODEL_FEMALE	"janet"
#define DEFAULT_TEAM_HEAD_FEMALE	"*janet"
#else
// Default team player model names
#define DEFAULT_TEAM_MODEL		DEFAULT_MODEL
#define DEFAULT_TEAM_HEAD		DEFAULT_HEAD

#define DEFAULT_TEAM_MODEL2		DEFAULT_MODEL2
#define DEFAULT_TEAM_HEAD2		DEFAULT_HEAD2

#define DEFAULT_TEAM_MODEL3		DEFAULT_MODEL3
#define DEFAULT_TEAM_HEAD3		DEFAULT_HEAD3

#define DEFAULT_TEAM_MODEL4		DEFAULT_MODEL4
#define DEFAULT_TEAM_HEAD4		DEFAULT_HEAD4

// For fallback player and gender-specific fallback sounds
#define DEFAULT_TEAM_MODEL_GENDER	DEFAULT_MODEL_GENDER
#define DEFAULT_TEAM_MODEL_MALE		DEFAULT_MODEL_MALE
#define DEFAULT_TEAM_HEAD_MALE		DEFAULT_HEAD_MALE
#define DEFAULT_TEAM_MODEL_FEMALE	DEFAULT_MODEL_FEMALE
#define DEFAULT_TEAM_HEAD_FEMALE	DEFAULT_HEAD_FEMALE
#endif


#define DEFAULT_REDTEAM_NAME		"Pagans"
#define DEFAULT_BLUETEAM_NAME		"Stroggs"
#define MAX_TEAMNAME		32

typedef enum {
	TEAM_FREE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SPECTATOR,

	TEAM_NUM_TEAMS
} team_t;

// Time between location updates
#define TEAM_LOCATION_UPDATE_TIME		1000

// How many players on the overlay
#define TEAM_MAXOVERLAY		32

//team task
typedef enum {
	TEAMTASK_NONE,
	TEAMTASK_OFFENSE, 
	TEAMTASK_DEFENSE,
	TEAMTASK_PATROL,
	TEAMTASK_FOLLOW,
	TEAMTASK_RETRIEVE,
	TEAMTASK_ESCORT,
	TEAMTASK_CAMP
} teamtask_t;

//flag status
typedef enum {
	FLAG_ATBASE = 0,
	FLAG_TAKEN,			// CTF
	FLAG_TAKEN_RED,		// One Flag CTF
	FLAG_TAKEN_BLUE,	// One Flag CTF
	FLAG_DROPPED
} flagStatus_t;

// means of death
typedef enum {
	MOD_UNKNOWN,
	MOD_SHOTGUN,
	MOD_GAUNTLET,
	MOD_MACHINEGUN,
	MOD_GRENADE,
	MOD_GRENADE_SPLASH,
	MOD_ROCKET,
	MOD_ROCKET_SPLASH,
	MOD_PLASMA,
	MOD_PLASMA_SPLASH,
	MOD_RAILGUN,
	MOD_LIGHTNING,
	MOD_BFG,
	MOD_BFG_SPLASH,
	MOD_WATER,
	MOD_SLIME,
	MOD_LAVA,
	MOD_CRUSH,
	MOD_TELEFRAG,
	MOD_FALLING,
	MOD_SUICIDE,
	MOD_TARGET_LASER,
	MOD_TRIGGER_HURT,
#ifdef MISSIONPACK
	MOD_NAIL,
	MOD_CHAINGUN,
	MOD_PROXIMITY_MINE,
	MOD_KAMIKAZE,
	MOD_JUICED,
#endif
	MOD_GRAPPLE,
	MOD_SUICIDE_TEAM_CHANGE
} meansOfDeath_t;


//---------------------------------------------------------

// gitem_t->type
typedef enum {
	IT_BAD,
	IT_WEAPON,				// EFX: rotate + upscale + minlight
	IT_AMMO,				// EFX: rotate
	IT_ARMOR,				// EFX: rotate + minlight
	IT_HEALTH,				// EFX: static external sphere + rotating internal
	IT_POWERUP,				// instant on, timer based
							// EFX: rotate + external ring that rotates
	IT_HOLDABLE,			// single use, holdable item
							// EFX: rotate + bob
	IT_PERSISTANT_POWERUP,
	IT_TEAM
} itemType_t;

#define MAX_ITEM_MODELS 4

typedef struct gitem_s {
	char		*classname;	// spawning name
	char		*pickup_sound;
	char		*world_model[MAX_ITEM_MODELS];

	char		*icon;
	char		*pickup_name;	// for printing on pickup

	int			quantity;		// for ammo how much, or duration of powerup
	itemType_t  giType;			// IT_* flags

	int			giTag;

	char		*sounds;		// string of all sounds this item will use
} gitem_t;

// included in both the game dll and the client
extern	gitem_t	bg_itemlist[];
extern	int		bg_numItems;

gitem_t	*BG_FindItem( const char *pickupName );
gitem_t	*BG_FindItemForWeapon( weapon_t weapon );
gitem_t	*BG_FindItemForAmmo( weapon_t weapon );
gitem_t	*BG_FindItemForPowerup( powerup_t pw );
gitem_t	*BG_FindItemForHoldable( holdable_t pw );
#define	BG_NumItems() (bg_numItems)
#define	BG_ItemForItemNum(x) (&bg_itemlist[(x)])
#define	BG_ItemNumForItem(x) ((x)-bg_itemlist)

qboolean	BG_CanItemBeGrabbed( int gametype, const entityState_t *ent, const playerState_t *ps );


// g_dmflags->integer flags
#define	DF_NO_FALLING			8
#define DF_FIXED_FOV			16
#define	DF_NO_FOOTSTEPS			32

// content masks
#define	MASK_ALL				(-1)
#define	MASK_SOLID				(CONTENTS_SOLID)
#define	MASK_PLAYERSOLID		(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BODY)
#define	MASK_DEADSOLID			(CONTENTS_SOLID|CONTENTS_PLAYERCLIP)
#define	MASK_WATER				(CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME)
#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA)
#define	MASK_SHOT				(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE)


//
// entityState_t->eType
//
typedef enum {
	ET_GENERAL,
	ET_PLAYER,
	ET_ITEM,
	ET_MISSILE,
	ET_MOVER,
	ET_BEAM,
	ET_PORTAL,
	ET_SPEAKER,
	ET_PUSH_TRIGGER,
	ET_TELEPORT_TRIGGER,
	ET_INVISIBLE,
	ET_GRAPPLE,				// grapple hooked on wall
	ET_TEAM,
	ET_CORONA,

	ET_EVENTS				// any of the EV_* events can be added freestanding
							// by setting eType to ET_EVENTS + eventNum
							// this avoids having to set eFlags and eventNum
} entityType_t;



void	BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
void	BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result );

void	BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps );

void	BG_TouchJumpPad( playerState_t *ps, entityState_t *jumppad );

void	BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap );
void	BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, int time, qboolean snap );

qboolean	BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime );

int		BG_ComposeUserCmdValue( int weapon );
void	BG_DecomposeUserCmdValue( int value, int *weapon );

void	BG_AddStringToList( char *list, size_t listSize, int *listLength, char *name );

void	SnapVectorTowards( vec3_t v, vec3_t to );

#define ARENAS_PER_TIER		4
#define MAX_ARENAS			1024
#define	MAX_ARENAS_TEXT		8192

#define MAX_BOTS			1024
#define MAX_BOTS_TEXT		8192


// Kamikaze

// 1st shockwave times
#define KAMI_SHOCKWAVE_STARTTIME		0
#define KAMI_SHOCKWAVEFADE_STARTTIME	1500
#define KAMI_SHOCKWAVE_ENDTIME			2000
// explosion/implosion times
#define KAMI_EXPLODE_STARTTIME			250
#define KAMI_IMPLODE_STARTTIME			2000
#define KAMI_IMPLODE_ENDTIME			2250
// 2nd shockwave times
#define KAMI_SHOCKWAVE2_STARTTIME		2000
#define KAMI_SHOCKWAVE2FADE_STARTTIME	2500
#define KAMI_SHOCKWAVE2_ENDTIME			3000
// radius of the models without scaling
#define KAMI_SHOCKWAVEMODEL_RADIUS		88
#define KAMI_BOOMSPHEREMODEL_RADIUS		72
// maximum radius of the models during the effect
#define KAMI_SHOCKWAVE_MAXRADIUS		1320
#define KAMI_BOOMSPHERE_MAXRADIUS		720
#define KAMI_SHOCKWAVE2_MAXRADIUS		704


// font rendering values used by ui and cgame

#define PROP_GAP_WIDTH			3
#define PROP_SPACE_WIDTH		8
#define PROP_HEIGHT				27
#define PROP_SMALL_SIZE_SCALE	0.75

#define PROPB_GAP_WIDTH			4
#define PROPB_SPACE_WIDTH		12
#define PROPB_HEIGHT			36

#define BLINK_DIVISOR			200
#define PULSE_DIVISOR			75.0f

// horizontal alignment
#define UI_LEFT			0x00000001	// default
#define UI_CENTER		0x00000002
#define UI_RIGHT		0x00000003
#define UI_FORMATMASK	0x0000000F

// vertical alignment
#define UI_VA_TOP			0x00000010	// default
#define UI_VA_CENTER		0x00000020
#define UI_VA_BOTTOM		0x00000030
#define UI_VA_FORMATMASK	0x000000F0

// font selection
#define UI_SMALLFONT	0x00000100
#define UI_BIGFONT		0x00000200	// default
#define UI_GIANTFONT	0x00000300
#define UI_TINYFONT		0x00000400
#define UI_NUMBERFONT	0x00000500
#define UI_CONSOLEFONT	0x00000600
#define UI_FONTMASK		0x00000F00

// other flags
#define UI_DROPSHADOW	0x00001000
#define UI_BLINK		0x00002000
#define UI_INVERSE		0x00004000
#define UI_PULSE		0x00008000
#define UI_FORCECOLOR	0x00010000
#define UI_GRADIENT		0x00020000
#define UI_NOSCALE		0x00040000 // fixed size with other UI elements, don't change it's scale
#define UI_INMOTION		0x00080000 // use for scrolling / moving text to fix uneven scrolling caused by aligning to pixel boundary


typedef struct
{
  const char *name;
} dummyCmd_t;
int cmdcmp( const void *a, const void *b );

//
typedef struct
{
	int gametype;

	// callbacks to test the entity
	// these will be different functions during game and cgame
	qboolean	(*spawnInt)( const char *key, const char *defaultString, int *out );
	qboolean	(*spawnString)( const char *key, const char *defaultString, char **out );

} bgEntitySpawnInfo_t;

qboolean BG_CheckSpawnEntity( const bgEntitySpawnInfo_t *info );


#define MAX_MAP_SIZE 65536

//
// bg_tracemap.c
//
typedef struct
{
	// callbacks to test the world
	// these will be different functions during game and cgame
	void		(*trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask );
	int			(*pointcontents)( const vec3_t point, int passEntityNum );
} bgGenTracemap_t;

void BG_GenerateTracemap(const char *mapname, vec2_t mapcoordsMins, vec2_t mapcoordsMaxs, bgGenTracemap_t *gen);

qboolean BG_LoadTraceMap( char *rawmapname, vec2_t world_mins, vec2_t world_maxs );
float BG_GetSkyHeightAtPoint( vec3_t pos );
float BG_GetSkyGroundHeightAtPoint( vec3_t pos );
float BG_GetGroundHeightAtPoint( vec3_t pos );
int BG_GetTracemapGroundFloor( void );
int BG_GetTracemapGroundCeil( void );
void etpro_FinalizeTracemapClamp( int *x, int *y );

void PC_SourceWarning(int handle, char *format, ...) __attribute__ ((format (printf, 2, 3)));
void PC_SourceError(int handle, char *format, ...) __attribute__ ((format (printf, 2, 3)));
int PC_CheckTokenString(int handle, char *string);
int PC_ExpectTokenString(int handle, char *string);
int PC_ExpectTokenType(int handle, int type, int subtype, pc_token_t *token);
int PC_ExpectAnyToken(int handle, pc_token_t *token);

#define MAX_STRINGFIELD				80
//field types
#define FT_CHAR						1			// char
#define FT_INT							2			// int
#define FT_FLOAT						3			// float
#define FT_STRING						4			// char [MAX_STRINGFIELD]
#define FT_STRUCT						6			// struct (sub structure)
//type only mask
#define FT_TYPE						0x00FF	// only type, clear subtype
//sub types
#define FT_ARRAY						0x0100	// array of type
#define FT_BOUNDED					0x0200	// bounded value
#define FT_UNSIGNED					0x0400

//structure field definition
typedef struct fielddef_s
{
	char *name;										//name of the field
	int offset;										//offset in the structure
	int type;										//type of the field
	//type specific fields
	int maxarray;									//maximum array size
	float floatmin, floatmax;					//float min and max
	struct structdef_s *substruct;			//sub structure
} fielddef_t;

//structure definition
typedef struct structdef_s
{
	int size;
	fielddef_t *fields;
} structdef_t;

qboolean PC_ReadStructure(int source, structdef_t *def, void *structure);

//
// System calls shared by game, cgame, and ui.
//
// print message on the local console
void	trap_Print( const char *text );

// abort the game
void	trap_Error( const char *text ) __attribute__((noreturn));

// milliseconds should only be used for performance tuning, never
// for anything game related.
int		trap_Milliseconds( void );

int		trap_RealTime( qtime_t *qtime );
void	trap_SnapVector( float *v );

// ServerCommand and ConsoleCommand parameter access
int		trap_Argc( void );
void	trap_Argv( int n, char *buffer, int bufferLength );
void	trap_Args( char *buffer, int bufferLength );
void	trap_LiteralArgs( char *buffer, int bufferLength );

// register a command name so the console can perform command completion.
void	trap_AddCommand( const char *cmdName );
void	trap_RemoveCommand( const char *cmdName );

void	trap_Cmd_ExecuteText( int exec_when, const char *text );

// console variable interaction
void	trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags );
void	trap_Cvar_Update( vmCvar_t *cvar );
void	trap_Cvar_Set( const char *var_name, const char *value );
void	trap_Cvar_SetValue( const char *var_name, float value );
void	trap_Cvar_Reset( const char *var_name );
int		trap_Cvar_VariableIntegerValue( const char *var_name );
float	trap_Cvar_VariableValue( const char *var_name );
void	trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_Cvar_LatchedVariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_Cvar_DefaultVariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_Cvar_InfoStringBuffer( int bit, char *buffer, int bufsize );
void	trap_Cvar_CheckRange( const char *var_name, float min, float max, qboolean integral );

// filesystem access
// returns length of file
int		trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
int		trap_FS_Read( void *buffer, int len, fileHandle_t f );
int		trap_FS_Write( const void *buffer, int len, fileHandle_t f );
int		trap_FS_Seek( fileHandle_t f, long offset, int origin ); // fsOrigin_t
int		trap_FS_Tell( fileHandle_t f );
void	trap_FS_FCloseFile( fileHandle_t f );
int		trap_FS_GetFileList( const char *path, const char *extension, char *listbuf, int bufsize );
int		trap_FS_Delete( const char *path );
int		trap_FS_Rename( const char *from, const char *to );

int		trap_PC_AddGlobalDefine( const char *define );
int		trap_PC_RemoveGlobalDefine( const char *define );
void	trap_PC_RemoveAllGlobalDefines( void );
int		trap_PC_LoadSource( const char *filename, const char *basepath );
int		trap_PC_FreeSource( int handle );
int		trap_PC_AddDefine( int handle, const char *define );
int		trap_PC_ReadToken( int handle, pc_token_t *pc_token );
void	trap_PC_UnreadToken( int handle );
int		trap_PC_SourceFileAndLine( int handle, char *filename, int *line );

void	*trap_HeapMalloc( int size );
int		trap_HeapAvailable( void );
void	trap_HeapFree( void *data );

// functions for console command argument completion
void	trap_Field_CompleteFilename( const char *dir, const char *ext, qboolean stripExt, qboolean allowNonPureFilesOnDisk );
void	trap_Field_CompleteCommand( const char *cmd, qboolean doCommands, qboolean doCvars );
void	trap_Field_CompleteList( const char *list );
