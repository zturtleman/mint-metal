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

/*
 * name:		cg_spawn.c
 *
 * desc:		Client sided only map entities
*/

#include "cg_local.h"

qboolean CG_SpawnString( const char *key, const char *defaultString, char **out ) {
	int i;

	if ( !cg.spawning ) {
		*out = (char *)defaultString;
		CG_Error( "CG_SpawnString() called while not spawning" );
	}

	for ( i = 0 ; i < cg.numSpawnVars ; i++ ) {
		if ( !strcmp( key, cg.spawnVars[i][0] ) ) {
			*out = cg.spawnVars[i][1];
			return qtrue;
		}
	}

	*out = (char *)defaultString;
	return qfalse;
}

qboolean CG_SpawnFloat( const char *key, const char *defaultString, float *out ) {
	char        *s;
	qboolean present;

	present = CG_SpawnString( key, defaultString, &s );
	*out = atof( s );
	return present;
}

qboolean CG_SpawnInt( const char *key, const char *defaultString, int *out ) {
	char        *s;
	qboolean present;

	present = CG_SpawnString( key, defaultString, &s );
	*out = atoi( s );
	return present;
}

qboolean CG_SpawnVector( const char *key, const char *defaultString, float *out ) {
	char        *s;
	qboolean present;

	present = CG_SpawnString( key, defaultString, &s );
	sscanf( s, "%f %f %f", &out[0], &out[1], &out[2] );
	return present;
}

qboolean CG_SpawnVector2D( const char *key, const char *defaultString, float *out ) {
	char        *s;
	qboolean present;

	present = CG_SpawnString( key, defaultString, &s );
	sscanf( s, "%f %f", &out[0], &out[1] );
	return present;
}

/*
=============
VectorToString

This is just a convenience function
for printing vectors
=============
*/
char    *vtos( const vec3_t v ) {
	static int index;
	static char str[8][32];
	char    *s;

	// use an array so that multiple vtos won't collide
	s = str[index];
	index = ( index + 1 ) & 7;

	Com_sprintf( s, 32, "(%i %i %i)", (int)v[0], (int)v[1], (int)v[2] );

	return s;
}

void SP_misc_gamemodel( void ) {
	char* model;
	char* skin;
	vec_t angle;
	vec3_t angles;

	vec_t scale;
	vec3_t vScale;

	vec3_t org;

	cg_gamemodel_t* gamemodel;

	int i;

#if 0 // ZTM: Note: Spearmint's game always drops misc_gamemodels. Also, RTCW has targetname set though I'm not sure what, if anything, it's used for.
	if ( CG_SpawnString( "targetname", "", &model ) || CG_SpawnString( "scriptname", "", &model ) || CG_SpawnString( "spawnflags", "", &model ) ) {
		// Gordon: this model may not be static, so let the server handle it
		return;
	}
#endif

	if ( cg.numMiscGameModels >= MAX_STATIC_GAMEMODELS ) {
		CG_Error( "^1MAX_STATIC_GAMEMODELS(%i) hit", MAX_STATIC_GAMEMODELS );
	}

	CG_SpawnString( "model", "", &model );

	CG_SpawnString( "skin", "", &skin );

	CG_SpawnVector( "origin", "0 0 0", org );

	if ( !CG_SpawnVector( "angles", "0 0 0", angles ) ) {
		if ( CG_SpawnFloat( "angle", "0", &angle ) ) {
			angles[YAW] = angle;
		}
	}

	if ( !CG_SpawnVector( "modelscale_vec", "1 1 1", vScale ) ) {
		if ( CG_SpawnFloat( "modelscale", "1", &scale ) ) {
			VectorSet( vScale, scale, scale, scale );
		}
	}

	gamemodel = &cgs.miscGameModels[cg.numMiscGameModels++];
	gamemodel->model = trap_R_RegisterModel( model );

	if ( *skin ) {
		CG_RegisterSkin( skin, &gamemodel->skin, qfalse );
	}

	AnglesToAxis( angles, gamemodel->axes );
	for ( i = 0; i < 3; i++ ) {
		VectorScale( gamemodel->axes[i], vScale[i], gamemodel->axes[i] );
	}
	VectorCopy( org, gamemodel->org );

	if ( gamemodel->model ) {
		vec3_t mins, maxs;

		trap_R_ModelBounds( gamemodel->model, mins, maxs, 0, 0, 0 );

		for ( i = 0; i < 3; i++ ) {
			mins[i] *= vScale[i];
			maxs[i] *= vScale[i];
		}

		gamemodel->radius = RadiusFromBounds( mins, maxs );
	} else {
		gamemodel->radius = 0;
	}
}

/*QUAKED props_skyportal (.6 .7 .7) (-8 -8 0) (8 8 16)
"fov" for the skybox default is 90
To have the portal sky fogged, enter any of the following values:
"fogcolor" (r g b) (values 0.0-1.0)
"fognear" distance from entity to start fogging (FIXME? Supported by RTCW, but not Spearmint)
"fogfar" distance from entity that fog is opaque

*/
void SP_skyportal( void ) {
	int fogn;
	int isfog;

	cg.hasSkyPortal = qtrue;

	CG_SpawnVector( "origin", "0 0 0", cg.skyPortalOrigin );
	isfog = CG_SpawnVector( "fogcolor", "0 0 0", cg.skyPortalFogColor );
	isfog += CG_SpawnInt( "fognear", "0", &fogn );
	isfog += CG_SpawnInt( "fogfar", "300", &cg.skyPortalFogDepthForOpaque );
	if ( !isfog ) {
		cg.skyPortalFogDepthForOpaque = 0;
	}
}

// ZTM: A complex scripting entity from FAKK. Only sky portal origin is supported.
// It works on fakkhouse map, portal sky doesn't work on all maps though.
// Might set skyorigin entity via FAKK's scripting files...
void SP_script_object( void ) {
	char *s;

	if (CG_SpawnString("script", "", &s)) {
		//CG_Printf("DEBUG: script_object: %s\n", s );
		// "rendereffects +skyorigin\n"
		if ( strstr( s, "+skyorigin" ) ) {
			CG_SpawnVector( "origin", "0 0 0", cg.skyPortalOrigin );
			cg.hasSkyPortal = qtrue;
		}
	}
}


// Load the TIKI file used by sky_edneskynoground (and other basic objects)
// TODO: Should create a new bg_*.c file for tiki file loading
// TODO?: support loading binary ".cik" files. It's probably the dtiki* structs in FAKK's qfiles.h.
char tikiText[120000]; // julie_base.tik is really long
qboolean CG_LoadTiki( const char *filename, cgSkin_t *skin, qhandle_t *hModel, vec3_t offset, float *scale, char *outPath ) {
	char		*text_p;
	int			len;
	int			i;
	char		*token;
	
	fileHandle_t	f;
	char		path[MAX_QPATH];
	char		skelmodel[MAX_QPATH];
	char		animName[MAX_QPATH];
	char		animFile[MAX_QPATH];
	char		surfName[MAX_QPATH];
	enum {
		TIKI_NONE,
		TIKI_SERVER,
		TIKI_CLIENT,
		TIKI_SETUP,
		TIKI_INIT,
		TIKI_INIT_SERVER,
		TIKI_INIT_CLIENT,
		TIKI_ANIMATIONS,
		TIKI_ANIMATIONS_SERVER,
		TIKI_ANIMATIONS_CLIENT,
		TIKI_END
	};
	int section;

	struct {
		//int section;
		int braceLevel; // unnamed brace levels
	} parseStack[32];
	int stackLevel;

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		return qfalse;
	}
	if ( len >= sizeof( tikiText ) - 1 ) {
		CG_Printf( "File %s too long\n", filename );
		trap_FS_FCloseFile( f );
		return qfalse;
	}
	trap_FS_Read( tikiText, len, f );
	tikiText[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = tikiText;

	Com_Memset( &parseStack, 0, sizeof ( parseStack ) );
	stackLevel = 0;

	section = TIKI_NONE;
	path[0] = 0;
	skelmodel[0] = 0;
	animName[0] = 0;
	animFile[0] = 0;
	surfName[0] = 0;

	// read optional parameters
	while ( 1 ) {
		token = COM_Parse( &text_p );
		if ( !*token ) {
			break;
		}

		// usually the file starts with this as an identifier
		// but models/item_bighealthfruit1.tik does not and some
		// included files do not either
		if ( !Q_stricmp( token, "TIKI" ) ) {
			continue;
		}

		//
		// section handling
		//
		if ( !Q_stricmp( token, "setup" ) ) {
			token = COM_Parse( &text_p );
			if ( Q_stricmp( token, "{" ) ) {
				CG_Printf("WARNING: Missing '{' for setup in '%s'\n", filename );
				return qfalse;
			}
			stackLevel++;
			section = TIKI_SETUP;
			continue;
		}
		if ( !Q_stricmp( token, "init" ) ) {
			token = COM_Parse( &text_p );
			if ( Q_stricmp( token, "{" ) ) {
				CG_Printf("WARNING: Missing '{' for init in '%s'\n", filename );
				return qfalse;
			}
			stackLevel++;
			section = TIKI_INIT;
			continue;
		}
		if ( !Q_stricmp( token, "animations" ) ) {
			token = COM_Parse( &text_p );
			if ( Q_stricmp( token, "{" ) ) {
				CG_Printf("WARNING: Missing '{' for animations in '%s'\n", filename );
				return qfalse;
			}
			stackLevel++;
			section = TIKI_ANIMATIONS;
			continue;
		}
		if ( !Q_stricmp( token, "server" ) ) {
			token = COM_Parse( &text_p );
			if ( Q_stricmp( token, "{" ) ) {
				CG_Printf("WARNING: Missing '{' for server in '%s'\n", filename );
				return qfalse;
			}
			if ( section == TIKI_NONE ) {
				stackLevel++;
				section = TIKI_SERVER;
			} else if ( section == TIKI_INIT ) {
				stackLevel++;
				section = TIKI_INIT_SERVER;
			} else if ( section == TIKI_ANIMATIONS ) {
				stackLevel++;
				section = TIKI_ANIMATIONS_SERVER;
			} else {
				CG_Printf("DEBUG: Found 'server' in unsupported section (internal #%d) in '%s'\n", section, filename );
			}
			continue;
		}
		if ( !Q_stricmp( token, "client" ) ) {
			token = COM_Parse( &text_p );
			if ( Q_stricmp( token, "{" ) ) {
				CG_Printf("WARNING: Missing '{' for client in '%s'\n", filename );
				return qfalse;
			}
			if ( section == TIKI_NONE ) {
				stackLevel++;
				section = TIKI_CLIENT;
			} else if ( section == TIKI_INIT ) {
				stackLevel++;
				section = TIKI_INIT_CLIENT;
			} else if ( section == TIKI_ANIMATIONS ) {
				stackLevel++;
				section = TIKI_ANIMATIONS_CLIENT;
			} else {
				CG_Printf("DEBUG: Found 'client' in unsupported section (internal #%d) in '%s'\n", section, filename );
			}
			continue;
		}
		// unnamed { } section
		if ( !Q_stricmp( token, "{" ) ) {
			parseStack[stackLevel].braceLevel++;
			continue;
		}
		if ( !Q_stricmp( token, "}" ) ) {
			if ( parseStack[stackLevel].braceLevel ) {
				parseStack[stackLevel].braceLevel--;
				continue;
			}

			if ( section == TIKI_NONE ) {
				CG_Printf("WARNING: Unexpected '}' in '%s'\n", filename );
			} else if ( section == TIKI_INIT_SERVER || section == TIKI_INIT_CLIENT ) {
				stackLevel--;
				section = TIKI_INIT;
			} else if ( section == TIKI_ANIMATIONS_SERVER || section == TIKI_ANIMATIONS_CLIENT ) {
				stackLevel--;
				section = TIKI_ANIMATIONS;
			} else {
				stackLevel--;
				section = TIKI_NONE;
			}
			continue;
		}

		//
		// keyword handling
		//
		if ( section == TIKI_NONE ) {
			// $include <filename>
			// ZTM: This isn't fully supported, more state should be kept than the few vars passed to CG_LoadTiki
			if ( !Q_stricmp( token, "$include" ) ) {
				char include[MAX_QPATH];

				token = COM_Parse( &text_p );
				Q_strncpyz( include, token, sizeof ( include ) );

				if ( !CG_LoadTiki( include, skin, hModel, offset, scale, path ) ) {
					CG_Printf("WARNING: Failed to load '%s' included in '%s'\n", include, filename );
				}
				continue;
			}
			// UNIMPLIMENTED: $define <name> <value>
			else if ( !Q_stricmp( token, "$define" ) ) {
				token = COM_Parse( &text_p );
				token = COM_Parse( &text_p );
				continue;
			}
		}

		// setup and init-server
		if ( section == TIKI_SETUP || section == TIKI_INIT_SERVER ) {
			if ( !Q_stricmp( token, "surface" ) ) {
				qhandle_t hShader;
				char shaderName[MAX_QPATH];

				// surface name
				token = COM_Parse( &text_p );
				Q_strncpyz( surfName, token, sizeof ( surfName ) );

				// command
				token = COM_Parse( &text_p );
				if ( !Q_stricmp( token, "+nodraw" ) ) {
					hShader = cgs.media.nodrawShader;
				} else if ( !Q_stricmp( token, "-nodraw" ) ) {
					// NOTE: There can be a included tiki file that sets no draw and then this file wants to enable it.
					//       Will need to rework how skins are handled.
				} else if ( !Q_stricmp( token, "shader" ) ) {
					token = COM_Parse( &text_p );

					// TODO: is there a way to get rid of the (currently needed) fallback case?
					//       can I just check if there is a '/' or '\\' to use without adding path?
					Q_strncpyz( shaderName, token, sizeof ( shaderName ) );
					hShader = trap_R_RegisterShaderEx( shaderName, LIGHTMAP_NONE, qtrue );
					if ( !hShader && path[0] ) {
						Com_sprintf( shaderName, sizeof ( shaderName ), "%s/%s", path, token );
						hShader = trap_R_RegisterShaderEx( shaderName, LIGHTMAP_NONE, qtrue );
					}

					if ( !hShader ) {
						CG_Printf("WARNING: Failed to load shader '%s' for surface '%s' in '%s'\n", shaderName, surfName, filename );
					}
				} else if ( !Q_stricmp( token, "flags" ) ) {
					token = COM_Parse( &text_p );
					// NOTE: nomipmaps can be set _after_ setting the shader names,
					//       which doesn't work with my current code.
					// "surface all flags nomipmaps"
					if ( !Q_stricmp( token, "nomipmaps" ) ) {
						// TODO
						//CG_Printf("WARNING: nomipmaps not supported yet, in '%s'\n", token, filename );
					} else {
						CG_Printf("WARNING: Unknown surface flag '%s' in '%s'\n", token, filename );
					}
					continue;
				} else {
					CG_Printf("WARNING: Unknown surface keyword '%s' in '%s'\n", token, filename );
					continue;
				}

				if ( skin->numSurfaces >= MAX_CG_SKIN_SURFACES ) {
					Com_Printf( "WARNING: Ignoring surfaces in '%s', the max is %d surfaces!\n", filename, MAX_CG_SKIN_SURFACES );
					break;
				}

				skin->surfaces[skin->numSurfaces] = trap_R_AllocSkinSurface( surfName, hShader );
				skin->numSurfaces++;
				continue;
			}
		}

		// setup
		if ( section == TIKI_SETUP ) {
			// origin <float> <float> <float>
			if ( !Q_stricmp( token, "origin" ) ) {
				for ( i = 0; i < 3; i++ ) {
					token = COM_Parse( &text_p );
					offset[i] = atof( token );
				}
				continue;

			}
			// path <dirname>
			else if ( !Q_stricmp( token, "path" ) ) {
				token = COM_Parse( &text_p );
				Q_strncpyz( path, token, sizeof ( path ) );
				continue;
			}
			// scale <float>
			else if ( !Q_stricmp( token, "scale" ) ) {
				token = COM_Parse( &text_p );
				*scale = atof( token );
				continue;
			}
			// UNIMPLIMENTED: lod_scale <float>
			else if ( !Q_stricmp( token, "lod_scale" ) ) {
				token = COM_Parse( &text_p );
				continue;
			}
			// UNIMPLIMENTED: lod_bias <float>
			else if ( !Q_stricmp( token, "lod_bias" ) ) {
				token = COM_Parse( &text_p );
				continue;
			}
			// UNIMPLIMENTED: radius <float>
			else if ( !Q_stricmp( token, "radius" ) ) {
				token = COM_Parse( &text_p );
				continue;
			}
			// skelmodel <filename>
			// Example: path models/example ; skelmodel example.skb
			else if ( !Q_stricmp( token, "skelmodel" ) ) {
				token = COM_Parse( &text_p );

				Com_sprintf( skelmodel, sizeof ( skelmodel ), "%s/%s", path, token );
				*hModel = trap_R_RegisterModel( skelmodel );

				// ZTM: Currently the engine doesn't support skb models so this will always fail.
#if 0
				if ( !*hModel ) {
					CG_Printf("WARNING: Failed to load skelmodel '%s' in '%s'\n", skelmodel, filename );
				}
#endif
				continue;
			} else {
				Com_Printf( "WARNING: Unknown setup keyword '%s' in %s\n", token, filename );
				SkipRestOfLine( &text_p );
				continue;
			}
		}

		// animations
		// they are optionally followed by a braced section with client and server sections.
		// if there is a skelmodel than these are actually animations (.ska) files otherwise they're vertex models (.tan) files.
		if ( section == TIKI_ANIMATIONS ) {
			Q_strncpyz( animName, token, sizeof ( animName ) );

			if ( !Q_stricmp( token, "idle" ) ) {
				token = COM_Parse( &text_p );

				Com_sprintf( animFile, sizeof ( animFile ), "%s/%s", path, token );

				*hModel = trap_R_RegisterModel( animFile );
				continue;
			} else {
				token = COM_Parse( &text_p );
				Com_sprintf( animFile, sizeof ( animFile ), "%s/%s", path, token );

				// some tiki files use .tan models but do not use idle animation
				if ( !*hModel && COM_CompareExtension( animFile, ".tan" ) ) {
					*hModel = trap_R_RegisterModel( animFile );
				}

				if ( cg_tikiDebug.integer ) {
					Com_Printf( "WARNING: Unknown tiki animation '%s' (filename '%s') in %s\n", animName, animFile, filename );
				}
				SkipRestOfLine( &text_p );
				continue;
			}
		}

		// this currently causes tons of spam
		if ( cg_tikiDebug.integer ) {
			Com_Printf( "WARNING: Unknown tiki keyword '%s' in %s\n", token, filename );
		}
		SkipRestOfLine( &text_p );
	}

	if ( outPath && path[0] ) {
		Q_strncpyz( outPath, path, MAX_QPATH );
	}

	return qtrue;
}

// spawn 'idle' model specified by tiki file
// ZTM: TODO?: store info parsed from file instead of re-parsing it each time.
void CG_AddStaticTikiModel( const char *tikiFile, vec3_t origin, float scale, vec3_t angles ) {
	float tikiScale;
	vec3_t vScale;
	vec3_t offset;
	cg_gamemodel_t *gamemodel;
	int i;
	qhandle_t hModel;

	if ( cg_tikiDebug.integer ) {
		CG_Printf("DEBUG: Adding tiki model '%s' at %s, yaw %f\n", tikiFile, vtos( origin ), angles[YAW] );
	}

	if ( cg.numMiscGameModels >= MAX_STATIC_GAMEMODELS ) {
		CG_Error( "^1MAX_STATIC_GAMEMODELS(%i) hit", MAX_STATIC_GAMEMODELS );
	}

	VectorSet( offset, 0, 0, 0 );

	gamemodel = &cgs.miscGameModels[cg.numMiscGameModels];

	hModel = 0;

	if ( !CG_LoadTiki( tikiFile, &gamemodel->skin, &hModel, offset, &tikiScale, NULL ) ) {
		return;
	}
	cg.numMiscGameModels++;

	VectorAdd( origin, offset, gamemodel->org );

	// multiply entity scale by scale from tiki file
	scale *= tikiScale;
	VectorSet( vScale, scale, scale, scale );

	if ( !hModel ) {
		CG_Printf( "WARNING: no model loaded for '%s'\n", tikiFile );
	}

	gamemodel->model = hModel;

	AnglesToAxis( angles, gamemodel->axes );
	for ( i = 0; i < 3; i++ ) {
		VectorScale( gamemodel->axes[i], vScale[i], gamemodel->axes[i] );
	}

	// Disabled because it causes issues drawing sky_ednesky / sky_edneskynoground
	if ( 0 && gamemodel->model ) {
		vec3_t mins, maxs;

		trap_R_ModelBounds( gamemodel->model, mins, maxs, 0, 0, 0 );

		for ( i = 0; i < 3; i++ ) {
			mins[i] *= vScale[i];
			maxs[i] *= vScale[i];
		}

		gamemodel->radius = RadiusFromBounds( mins, maxs );
	} else {
		gamemodel->radius = 0;
	}

}

// handle spawning the various tiki object entities
// there are many different class names, but they all include 'model' and other settings.
// I don't think the class name itself is important, not entirely sure though.
// Not sure what spawnflags is suppose to do.
void SP_generic_tiki_model( void ) {
	char filename[MAX_QPATH];
	char *classname;
	char *model;
	vec3_t origin;
	float scale;
	vec3_t angles;
	float angle;
	int make_static;

	CG_SpawnString( "classname", "", &classname );
	CG_SpawnString( "model", "", &model );
	CG_SpawnVector( "origin", "0 0 0", origin );
	CG_SpawnFloat( "scale", "1", &scale );
	CG_SpawnInt( "make_static", "0", &make_static );

	if ( !CG_SpawnVector( "angles", "0 0 0", angles ) ) {
		if ( CG_SpawnFloat( "angle", "0", &angle ) ) {
			angles[YAW] = angle;
		}
	}

	Com_sprintf( filename, sizeof ( filename ), "models/%s", model );

	// static means it is baked into the BSP, useful for seeing if my code matches the intended behavior though
	if ( make_static ) {
		//CG_Printf("DEBUG: Entity '%s': ignoring static tiki model '%s',\n", classname, filename );
		return;
	} else {
		if ( cg_tikiDebug.integer ) {
			CG_Printf("DEBUG: Entity '%s': spawning tiki model '%s'\n", classname, filename );
		}
	}

	CG_AddStaticTikiModel( filename, origin, scale, angles );
}


typedef struct {
	char    *name;
	void ( *spawn )( void );
} spawn_t;

spawn_t spawns[] = {
	{0, 0},
	{"misc_gamemodel",               SP_misc_gamemodel},
	{"props_skyportal",              SP_skyportal},

	// fakk entities
	{"script_object",                SP_script_object},
};

int numSpawns = ARRAY_LEN( spawns );

/*
===================
CG_ParseEntityFromSpawnVars

Spawn an entity and fill in all of the level fields from
cg.spawnVars[], then call the class specfic spawn function
===================
*/
void CG_ParseEntityFromSpawnVars( void ) {
	int i;
	char    *classname;
	bgEntitySpawnInfo_t spawnInfo;

	spawnInfo.gametype = cgs.gametype;
	spawnInfo.spawnInt = CG_SpawnInt;
	spawnInfo.spawnString = CG_SpawnString;

	// check "notsingle", "notfree", "notteam", etc
	if ( !BG_CheckSpawnEntity( &spawnInfo ) ) {
		return;
	}

	if ( CG_SpawnString( "classname", "", &classname ) ) {
		for ( i = 0; i < numSpawns; i++ ) {
			if ( !Q_stricmp( spawns[i].name, classname ) ) {
				spawns[i].spawn();
				break;
			}
		}

		// Try generic tiki object (no idea if this is a 'proper' thing to do)
		if ( i == numSpawns ) {
			char *model;
			if ( CG_SpawnString( "model", "", &model ) && COM_CompareExtension( model, ".tik" ) ) {
				SP_generic_tiki_model();
			}
		}
	}

}

/*
====================
CG_AddSpawnVarToken
====================
*/
char *CG_AddSpawnVarToken( const char *string ) {
	int l;
	char    *dest;

	l = strlen( string );
	if ( cg.numSpawnVarChars + l + 1 > MAX_SPAWN_VARS_CHARS ) {
		CG_Error( "CG_AddSpawnVarToken: MAX_SPAWN_VARS" );
	}

	dest = cg.spawnVarChars + cg.numSpawnVarChars;
	memcpy( dest, string, l + 1 );

	cg.numSpawnVarChars += l + 1;

	return dest;
}

/*
====================
CG_ParseSpawnVars

Parses a brace bounded set of key / value pairs out of the
level's entity strings into cg.spawnVars[]

This does not actually spawn an entity.
====================
*/
qboolean CG_ParseSpawnVars( void ) {
	char keyname[MAX_TOKEN_CHARS];
	char com_token[MAX_TOKEN_CHARS];

	cg.numSpawnVars = 0;
	cg.numSpawnVarChars = 0;

	// parse the opening brace
	if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) {
		// end of spawn string
		return qfalse;
	}
	if ( com_token[0] != '{' ) {
		CG_Error( "CG_ParseSpawnVars: found %s when expecting {",com_token );
	}

	// go through all the key / value pairs
	while ( 1 ) {
		// parse key
		if ( !trap_GetEntityToken( keyname, sizeof( keyname ) ) ) {
			CG_Error( "CG_ParseSpawnVars: EOF without closing brace" );
		}

		if ( keyname[0] == '}' ) {
			break;
		}

		// parse value
		if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) {
			CG_Error( "CG_ParseSpawnVars: EOF without closing brace" );
		}

		if ( com_token[0] == '}' ) {
			CG_Error( "CG_ParseSpawnVars: closing brace without data" );
		}
		if ( cg.numSpawnVars == MAX_SPAWN_VARS ) {
			CG_Error( "CG_ParseSpawnVars: MAX_SPAWN_VARS" );
		}
		cg.spawnVars[ cg.numSpawnVars ][0] = CG_AddSpawnVarToken( keyname );
		cg.spawnVars[ cg.numSpawnVars ][1] = CG_AddSpawnVarToken( com_token );
		cg.numSpawnVars++;
	}

	return qtrue;
}

void SP_worldspawn( void ) {
	char    *s;
	int		spawnflags;

	CG_SpawnString( "classname", "", &s );
	if ( Q_stricmp( s, "worldspawn" ) ) {
		CG_Error( "SP_worldspawn: The first entity isn't 'worldspawn'" );
	}

	if ( CG_SpawnVector2D( "mapcoordsmins", "-128 128", cg.mapcoordsMins ) &&  // top left
		 CG_SpawnVector2D( "mapcoordsmaxs", "128 -128", cg.mapcoordsMaxs ) ) { // bottom right
		cg.mapcoordsValid = qtrue;
	} else {
		cg.mapcoordsValid = qfalse;
	}

#if 0
	cg.mapcoordsScale[0] = 1 / ( cg.mapcoordsMaxs[0] - cg.mapcoordsMins[0] );
	cg.mapcoordsScale[1] = 1 / ( cg.mapcoordsMaxs[1] - cg.mapcoordsMins[1] );

	BG_InitLocations( cg.mapcoordsMins, cg.mapcoordsMaxs );
#endif

	CG_SpawnString( "atmosphere", "", &s );
	CG_EffectParse( s );

	CG_SpawnFloat( "skyalpha", "1", &cg.skyAlpha );

	// NEW IN FAKK
	if ( CG_SpawnInt( "spawnflags", "0", &spawnflags ) ) {
		Com_Printf( "spawnflags: %d\n", spawnflags);
	}

	// CINEMATIC
	if ( spawnflags & 1 ) {
		Com_Printf("spawnflags: CINEMATIC\n");
	}

	if ( CG_SpawnString( "soundtrack", "", &s ) ) {
		Com_Printf( "soundtrack: %s\n", s);
	}

	if ( CG_SpawnString( "skipthread", "", &s ) ) {
		Com_Printf( "skipthread: %s\n", s);
	}

	if ( CG_SpawnString( "script", "", &s ) ) {
		Com_Printf( "script: %s\n", s);
	}

	if ( CG_SpawnString( "watercolor", "", &s ) ) {
		Com_Printf( "watercolor: %s\n", s);
	}

	if ( CG_SpawnString( "wateralpha", "", &s ) ) {
		Com_Printf( "wateralpha: %s\n", s);
	}

	if ( CG_SpawnString( "lavacolor", "", &s ) ) {
		Com_Printf( "lavacolor: %s\n", s);
	}

	if ( CG_SpawnString( "lavaalpha", "", &s ) ) {
		Com_Printf( "lavaalpha: %s\n", s);
	}

	if ( CG_SpawnString( "farplane_color", "", &s ) ) {
		Com_Printf( "farplane_color: %s\n", s);
	}

	// default is yes
	if ( CG_SpawnString( "farplane_cull", "yes", &s ) ) {
		Com_Printf( "farplane_cull: %s\n", s);
	}

	if ( CG_SpawnString( "farplane", "", &s ) ) {
		Com_Printf( "farplane: %s\n", s);
	}
}

/*
==============
CG_ParseEntitiesFromString

Parses textual entity definitions out of an entstring and spawns gentities.
==============
*/
void CG_ParseEntitiesFromString( void ) {
	// allow calls to CG_Spawn*()
	cg.spawning = qtrue;
	cg.numSpawnVars = 0;
	cg.numMiscGameModels = 0;

	// the worldspawn is not an actual entity, but it still
	// has a "spawn" function to perform any global setup
	// needed by a level (setting configstrings or cvars, etc)
	if ( !CG_ParseSpawnVars() ) {
		CG_Error( "ParseEntities: no entities" );
	}
	SP_worldspawn();

	// parse ents
	while ( CG_ParseSpawnVars() ) {
		CG_ParseEntityFromSpawnVars();
	}

	CG_DPrintf( "CGame loaded %d misc_gamemodels\n", cg.numMiscGameModels );

	cg.spawning = qfalse;           // any future calls to CG_Spawn*() will be errors
}
