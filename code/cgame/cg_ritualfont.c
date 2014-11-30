/*
===========================================================================
Copyright (C) 2014 Zack Middleton

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
// Ritual font loader (partially based on OpenMOHAA tr_font.c)

#include "cg_local.h"

/*
=================
CG_LoadRitualFont
=================
*/
qboolean CG_LoadRitualFont( const char *fontname, fontInfo_t *font ) {
	char			shadername[MAX_QPATH];
	qhandle_t		fontShader;
	char			filename[MAX_QPATH];
	char			text[20000];
	fileHandle_t	f;
	int				i, len;
	char			*p, *token;
	qboolean		foundIndirections;
	float			height;
	float			aspect;
	int				indirections[256];
	int				numIndirections;
	vec4_t			locations;
	int				locationNum;
	float			imageWidth, imageHeight;

	Com_Memset( font, 0, sizeof ( fontInfo_t ) );

	// bitmap image
	Com_sprintf( shadername, sizeof(shadername), "gfx/fonts/%s.ftx", fontname );
	fontShader = trap_R_RegisterShaderNoMip( shadername );

	if ( !fontShader ) {
		return qfalse;
	}

	// glyph info filename
	Com_sprintf( filename, sizeof(filename), "fonts/%s.ritualfont", fontname );

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		return qfalse;
	}
	if ( len >= sizeof( text ) - 1 ) {
		CG_Printf( S_COLOR_YELLOW "WARNING: File %s too long\n", filename );
		trap_FS_FCloseFile( f );
		return qfalse;
	}
	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	p = text;
	token = COM_Parse(&p);
	if (strcmp(token, "RitFont")) {
		CG_Printf( S_COLOR_YELLOW "WARNING: Failed loading %s: Expected \"RitFont\" found \"%s\"\n", filename, token );
		return qfalse;
	}

	foundIndirections = qfalse;
	imageWidth = imageHeight = 256;

	while ( 1 ) {
		token = COM_Parse(&p);

		if ( !*token ) {
			break;
		}

		if ( Q_stricmp( token, "height" ) == 0 ) {
			// height of glyphs
			token = COM_Parse(&p);

			if ( !*token ) {
				CG_Printf( S_COLOR_YELLOW "WARNING: Failed loading %s: Missing value for height\n", filename );
				return qfalse;
			}

			height = atof( token );
		} else if ( Q_stricmp( token, "aspect" ) == 0 ) {
			// aspect of font image
			token = COM_Parse(&p);

			if ( !*token ) {
				CG_Printf( S_COLOR_YELLOW "WARNING: Failed loading %s: Missing value for aspect\n", filename );
				return qfalse;
			}

			aspect = atof( token );

			// width is always 256. aspects specifies height indirectly...
			imageHeight = imageWidth / aspect;
		} else if ( Q_stricmp( token, "indirections" ) == 0 ) {
			token = COM_Parse(&p);

			if ( !*token || token[0] != '{' ) {
				CG_Printf( S_COLOR_YELLOW "WARNING: Failed loading %s: Missing '{' for indirections\n", filename );
				return qfalse;
			}

			numIndirections = 0;
			while ( 1 ) {
				token = COM_Parse(&p);

				if ( !*token ) {
					CG_Printf( S_COLOR_YELLOW "WARNING: Failed loading %s: Missing '}' for indirections\n", filename );
					return qfalse;
				}

				if ( token[0] == '}' ) {
					break;
				}

				if ( numIndirections < 256 ) {
					indirections[numIndirections] = atoi( token );
					numIndirections++;
				}
			}

			foundIndirections = qtrue;
		} else if ( Q_stricmp( token, "locations" ) == 0 ) {
			if ( !foundIndirections ) {
				CG_Printf( S_COLOR_YELLOW "WARNING: Failed loading %s: Expected indirections before locations\n", filename );
				return qfalse;
			}

			token = COM_Parse(&p);

			if ( !*token || token[0] != '{' ) {
				CG_Printf( S_COLOR_YELLOW "WARNING: Failed loading %s: Missing '{' for locations\n", filename);
				return qfalse;
			}

			locationNum = 0;
			while ( 1 ) {
				token = COM_Parse(&p);

				if ( !*token ) {
					CG_Printf( S_COLOR_YELLOW "WARNING: Failed loading %s: Missing '}' for locations\n", filename);
					return qfalse;
				}

				if ( token[0] == '}' ) {
					break;
				}

				if ( !*token || token[0] != '{' ) {
					CG_Printf( S_COLOR_YELLOW "WARNING: Failed loading %s: Missing '{' for locations\n", filename);
					return qfalse;
				}

				// read x, y, width, height in pixels
				for ( i = 0; i < 4; i++ ) {
					token = COM_Parse(&p);
					if ( !*token ) {
						return qfalse;
					}
					locations[i] = atof( token );
				}

				token = COM_Parse(&p);
				if ( !*token || token[0] != '}' ) {
					CG_Printf( S_COLOR_YELLOW "WARNING: Failed loading %s: Missing '}' for a location\n", filename);
					return qfalse;
				}

				for ( i = 0; i < numIndirections; i++ ) {
					if ( indirections[i] == locationNum ) {
						break;
					}
				}

				locationNum++;

				if ( i == numIndirections ) {
					// glyph does not exist in font
					continue;
				}

				// setup glyph info
				font->glyphs[i].height = locations[3];
				font->glyphs[i].top = locations[3];
				font->glyphs[i].left = 0;
				font->glyphs[i].pitch = locations[2];
				font->glyphs[i].xSkip = locations[2];
				font->glyphs[i].imageWidth = locations[2];
				font->glyphs[i].imageHeight = locations[3];
				font->glyphs[i].s = locations[0] / imageWidth;
				font->glyphs[i].t = locations[1] / imageHeight;
				font->glyphs[i].s2 = (locations[0] + locations[2]) / imageWidth;
				font->glyphs[i].t2 = (locations[1] + locations[3]) / imageHeight;
				font->glyphs[i].glyph = fontShader;
				Q_strncpyz(font->glyphs[i].shaderName, shadername, sizeof(font->glyphs[0].shaderName));
			}
		} else {
			CG_Printf( S_COLOR_YELLOW "WARNING: Loading %s: Unknown keyword %s\n", filename, token);
		}
	}

	if ( !foundIndirections ) {
		CG_Printf( S_COLOR_YELLOW "WARNING: Failed loading %s: Missing indirections and locations\n", filename);
		return qfalse;
	}

	font->glyphScale = 48.0f / height;
	Q_strncpyz(font->name, filename, sizeof(font->name));
	return qtrue;
}
