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

#define MAX_TIKI_SURFACES MAX_CG_SKIN_SURFACES

typedef struct {
	char name[MAX_QPATH];
	char shader[MAX_QPATH];
	qboolean nodraw;
	qboolean nomipmaps;
} tikiSurface_t;

typedef struct {
	vec3_t offset;
	float scale;
	float radius;
	char path[MAX_QPATH];
	char allShader[MAX_QPATH];

	tikiSurface_t surfaces[MAX_TIKI_SURFACES];
	int numSurfaces;
} tikiModel_t;


// Load the TIKI file used by sky_edneskynoground (and other basic objects)
// TODO: Should create a new bg_*.c file for tiki file loading
// TODO?: support loading binary ".cik" files. It's probably the dtiki* structs in FAKK's qfiles.h.
char tikiText[2][120000]; // julie_base.tik is really long
int tikiTextIndex = 0;
qboolean CG_LoadTiki( const char *filename, tikiModel_t *modelInfo, qhandle_t *hModel ) {
	char		*text_p;
	int			len;
	int			i;
	char		*token;
	fileHandle_t	f;
	char		skelmodel[MAX_QPATH];
	char		animName[MAX_QPATH];
	char		animFile[MAX_QPATH];
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

	Com_Memset( modelInfo, 0, sizeof ( *modelInfo ) );

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		return qfalse;
	}
	if ( len >= sizeof( tikiText[0] ) - 1 ) {
		CG_Printf( "File %s too long\n", filename );
		trap_FS_FCloseFile( f );
		return qfalse;
	}
	trap_FS_Read( tikiText[tikiTextIndex], len, f );
	tikiText[tikiTextIndex][len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = tikiText[tikiTextIndex];

	tikiTextIndex ^= 1;

	Com_Memset( &parseStack, 0, sizeof ( parseStack ) );
	stackLevel = 0;

	section = TIKI_NONE;
	skelmodel[0] = 0;
	animName[0] = 0;
	animFile[0] = 0;

	COM_BeginParseSession( filename );

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

				if ( !CG_LoadTiki( include, modelInfo, hModel ) ) {
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
				tikiSurface_t *surface;
				char surfName[MAX_QPATH];
				char command[MAX_QPATH];
				char argument[MAX_QPATH];
				qboolean allSurfaces;

				// surface name
				// special handling for "all"
				// TODO: Special handling for filtered names "cap*" in see models/creeper.tik
				token = COM_Parse( &text_p );
				Q_strncpyz( surfName, token, sizeof ( surfName ) );

				// command
				token = COM_Parse( &text_p );
				Q_strncpyz( command, token, sizeof ( command ) );

				// if the command has an argument, read it
				if ( !Q_stricmp( command, "shader" ) || !Q_stricmp( command, "flags" ) ) {
					token = COM_Parse( &text_p );
					Q_strncpyz( argument, token, sizeof ( argument ) );
				} else {
					argument[0] = 0;
				}

				allSurfaces = Q_stricmp( surfName, "all" ) == 0;

				if ( allSurfaces && !Q_stricmp( command, "shader" ) ) {
					Q_strncpyz( modelInfo->allShader, argument, sizeof ( modelInfo->allShader ) );
				}

				// find surfaces
				for ( i = 0, surface = modelInfo->surfaces; i < modelInfo->numSurfaces + 1; i++, surface++ ) {
					// checked all surfaces, allocate surface if it did not exist
					if ( i == modelInfo->numSurfaces ) {
						if ( allSurfaces ) {
							break;
						}

						if ( modelInfo->numSurfaces >= MAX_TIKI_SURFACES ) {
							Com_Printf( "WARNING: Too many surfaces in '%s' (max is %d)\n", filename, MAX_TIKI_SURFACES );
							break;
						}

						Q_strncpyz( surface->name, surfName, sizeof ( surface->name ) );
						Q_strncpyz( surface->shader, "unset", sizeof ( surface->shader ) );
						//Com_Printf( "DEBUG: Create surface (%d) '%s' in '%s'\n", modelInfo->numSurfaces, surfName, filename );
						modelInfo->numSurfaces++;
					}

					if ( !allSurfaces && Q_stricmp( surfName, surface->name ) != 0 ) {
						continue;
					}

					if ( !Q_stricmp( command, "+nodraw" ) ) {
						surface->nodraw = qtrue;
					} else if ( !Q_stricmp( command, "-nodraw" ) ) {
						surface->nodraw = qfalse;
					} else if ( !Q_stricmp( command, "shader" ) ) {
						Q_strncpyz( surface->shader, argument, sizeof ( surface->shader ) );
					} else if ( !Q_stricmp( command, "flags" ) ) {
						// nomipmaps can be set after setting the shader names
						// "surface all flags nomipmaps"
						if ( !Q_stricmp( argument, "nomipmaps" ) ) {
							surface->nomipmaps = qtrue;
						} else {
							CG_Printf("WARNING: Unknown surface flag '%s' in '%s'\n", argument, filename );
							break;
						}
					} else {
						CG_Printf("WARNING: Unknown surface keyword '%s' in '%s'\n", command, filename );
						break;
					}

					// only a single surface
					if ( !allSurfaces ) {
						break;
					}
				}

				continue;
			}
		}

		// setup
		if ( section == TIKI_SETUP ) {
			// origin <float> <float> <float>
			if ( !Q_stricmp( token, "origin" ) ) {
				for ( i = 0; i < 3; i++ ) {
					token = COM_Parse( &text_p );
					modelInfo->offset[i] = atof( token );
				}
				continue;

			}
			// path <dirname>
			else if ( !Q_stricmp( token, "path" ) ) {
				token = COM_Parse( &text_p );
				Q_strncpyz( modelInfo->path, token, sizeof ( modelInfo->path ) );
				continue;
			}
			// scale <float>
			else if ( !Q_stricmp( token, "scale" ) ) {
				token = COM_Parse( &text_p );
				modelInfo->scale = atof( token );
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
			// radius <float>
			else if ( !Q_stricmp( token, "radius" ) ) {
				token = COM_Parse( &text_p );
				modelInfo->radius = atof( token );
				continue;
			}
			// skelmodel <filename>
			else if ( !Q_stricmp( token, "skelmodel" ) ) {
				token = COM_Parse( &text_p );

				// ZTM: This should probably be defered until done parsing because path might not be set yet?
				Com_sprintf( skelmodel, sizeof ( skelmodel ), "%s/%s", modelInfo->path, token );
				*hModel = trap_R_RegisterModel( skelmodel );

#if 0
				// ZTM: Currently the engine doesn't support skb models so this will always fail.
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
		// TODO: they are optionally followed by a braced section with client and server sections.
		// if there is a skelmodel than these are actually animations (.ska) files otherwise they're vertex models (.tan) files.
		if ( section == TIKI_ANIMATIONS ) {
			Q_strncpyz( animName, token, sizeof ( animName ) );

			if ( !Q_stricmp( token, "idle" ) ) {
				token = COM_Parse( &text_p );

				Com_sprintf( animFile, sizeof ( animFile ), "%s/%s", modelInfo->path, token );

				*hModel = trap_R_RegisterModel( animFile );
				continue;
			} else {
				token = COM_Parse( &text_p );

				// hack: some tiki files use .tan models but do not use idle animation
				if ( !*hModel && COM_CompareExtension( token, ".tan" ) ) {
					Com_sprintf( animFile, sizeof ( animFile ), "%s/%s", modelInfo->path, token );
					*hModel = trap_R_RegisterModel( animFile );
				}

				if ( cg_tikiDebug.integer ) {
					Com_Printf( "WARNING: Unknown tiki animation '%s' (filename '%s') in %s\n", animName, animFile, filename );
				}
				continue;
			}
		}

		// this currently causes tons of spam
		if ( cg_tikiDebug.integer ) {
			Com_Printf( "WARNING: Unknown tiki keyword '%s' in %s\n", token, filename );
		}
		SkipRestOfLine( &text_p );
	}

	return qtrue;
}

// convert tiki surface info into a skin
void CG_TikiSkin( const char *filename, const tikiModel_t *modelInfo, cgSkin_t *skin ) {
	const tikiSurface_t *surface;
	char shaderName[MAX_QPATH];
	int i;
	qhandle_t	hShader;

	skin->numSurfaces = 0;

	for ( i = 0, surface = modelInfo->surfaces; i < modelInfo->numSurfaces; i++, surface++ ) {
		if ( skin->numSurfaces >= MAX_CG_SKIN_SURFACES ) {
			Com_Printf( "WARNING: Ignoring surfaces in '%s', the max is %d surfaces!\n", filename, MAX_CG_SKIN_SURFACES );
			break;
		}

		if ( surface->nodraw ) {
			hShader = cgs.media.nodrawShader;
		} else {
#if 1
			// if there is an extension (.tga) it's in the path otherwise it's a shader name
			if ( modelInfo->path[0] && COM_CompareExtension( surface->shader, ".tga" ) ) {
				Com_sprintf( shaderName, sizeof ( shaderName ), "%s/%s", modelInfo->path, surface->shader );
			} else {
				Q_strncpyz( shaderName, surface->shader, sizeof ( shaderName ) );
			}
			hShader = trap_R_RegisterShaderEx( shaderName, LIGHTMAP_NONE, !surface->nomipmaps );
			//Com_Printf( "DEBUG: Tiki shader '%s' (handle %d) for surface '%s' in '%s'\n", shaderName, hShader, surface->name, filename );
#else
			// TODO: is there a way to get rid of the (currently needed) fallback case?
			//       can I just check if there is a '/' or '\\' to use without adding path?
			Q_strncpyz( shaderName, surface->shader, sizeof ( shaderName ) );
			hShader = trap_R_RegisterShaderEx( shaderName, LIGHTMAP_NONE, !surface->nomipmaps );
			//Com_Printf( "DEBUG: Tiki shader '%s' (handle %d) for surface '%s' in '%s'\n", shaderName, hShader, surface->name, filename );

			if ( !hShader && modelInfo->path[0] ) {
				Com_sprintf( shaderName, sizeof ( shaderName ), "%s/%s", modelInfo->path, surface->shader );
				hShader = trap_R_RegisterShaderEx( shaderName, LIGHTMAP_NONE, !surface->nomipmaps );
				//Com_Printf( "DEBUG: Tiki shader '%s' (handle %d) for surface '%s' in '%s'\n", shaderName, hShader, surface->name, filename );
			}
#endif

			// fallback to models
			//   ammo_gasplant.tik has path "models/ammo/gasplant" with shader "monster/gasyerass2/gasyerass2.tga"
			//   but shader path requires "models/" at the beginning
			if ( !hShader ) {
				Com_sprintf( shaderName, sizeof ( shaderName ), "models/%s", surface->shader );
				hShader = trap_R_RegisterShaderEx( shaderName, LIGHTMAP_NONE, !surface->nomipmaps );
				//Com_Printf( "DEBUG: Tiki shader '%s' (handle %d) for surface '%s' in '%s'\n", shaderName, hShader, surface->name, filename );
			}

			if ( !hShader ) {
				CG_Printf("WARNING: Failed to load shader '%s' for surface '%s' in '%s'\n", shaderName, surface->name, filename );
			}
		}

		skin->surfaces[skin->numSurfaces] = trap_R_AllocSkinSurface( surface->name, hShader );
		skin->numSurfaces++;
	}
}

// spawn 'idle' model specified by tiki file
// ZTM: TODO?: store info parsed from file instead of re-parsing it each time.
void CG_AddStaticTikiModel( const char *tikiFile, vec3_t origin, float scale, vec3_t angles ) {
	tikiModel_t modelInfo;
	vec3_t vScale;
	cg_gamemodel_t *gamemodel;
	int i;
	qhandle_t hModel;

	if ( cg_tikiDebug.integer ) {
		CG_Printf("DEBUG: Adding tiki model '%s' at %s, yaw %f\n", tikiFile, vtos( origin ), angles[YAW] );
	}

	if ( cg.numMiscGameModels >= MAX_STATIC_GAMEMODELS ) {
		CG_Error( "^1MAX_STATIC_GAMEMODELS(%i) hit", MAX_STATIC_GAMEMODELS );
	}

	gamemodel = &cgs.miscGameModels[cg.numMiscGameModels];

	hModel = 0;

	if ( !CG_LoadTiki( tikiFile, &modelInfo, &hModel ) ) {
		return;
	}
	cg.numMiscGameModels++;

	CG_TikiSkin( tikiFile, &modelInfo, &gamemodel->skin );

	// special handling for using a shader on "all" surfaces
	// used by models/lympthorn_f.tik on map gruff
	if ( modelInfo.allShader[0] ) {
		if ( gamemodel->skin.numSurfaces ) {
			// FIXME: The all shader would only be used on surfaces set in the skin,
			//        which presumably already have the shader name set.
			Com_Printf( "WARNING: Mix of explicit and implicit (\"all\") shader names in %s\n", tikiFile );
		} else {
			char shaderName[MAX_QPATH];

			// if there is an extension (.tga) it's in the path otherwise it's a shader name
			if ( modelInfo.path[0] && COM_CompareExtension( modelInfo.allShader, ".tga" ) ) {
				Com_sprintf( shaderName, sizeof ( shaderName ), "%s/%s", modelInfo.path, modelInfo.allShader );
			} else {
				Q_strncpyz( shaderName, modelInfo.allShader, sizeof ( shaderName ) );
			}

			// FIXME: there can be a nomipmaps flag set
			gamemodel->customShader = trap_R_RegisterShaderEx( shaderName, LIGHTMAP_NONE, qtrue );
		}
	}

	VectorAdd( origin, modelInfo.offset, gamemodel->org );

	// multiply entity scale by scale from tiki file
	scale *= modelInfo.scale;
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
		// using radius causes cull issues
		// also, I don't know if tiki radius is suppose to be used for culling
		gamemodel->radius = 0;//modelInfo.radius;
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
	if ( !trap_GetEntityToken( &cg.spawnEntityOffset, com_token, sizeof( com_token ) ) ) {
		// end of spawn string
		return qfalse;
	}
	if ( com_token[0] != '{' ) {
		CG_Error( "CG_ParseSpawnVars: found %s when expecting {",com_token );
	}

	// go through all the key / value pairs
	while ( 1 ) {
		// parse key
		if ( !trap_GetEntityToken( &cg.spawnEntityOffset, keyname, sizeof( keyname ) ) ) {
			CG_Error( "CG_ParseSpawnVars: EOF without closing brace" );
		}

		if ( keyname[0] == '}' ) {
			break;
		}

		// parse value
		if ( !trap_GetEntityToken( &cg.spawnEntityOffset, com_token, sizeof( com_token ) ) ) {
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

	CG_SpawnString( "enableDust", "0", &s );
	trap_Cvar_Set( "cg_enableDust", s );

	CG_SpawnString( "enableBreath", "0", &s );
	trap_Cvar_Set( "cg_enableBreath", s );

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
	cg.spawnEntityOffset = 0;
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
