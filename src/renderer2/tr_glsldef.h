/*
 * Wolfenstein: Enemy Territory GPL Source Code
 * Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
 * Copyright (C) 2010-2011 Robert Beckebans <trebor_7@users.sourceforge.net>
 *
 * ET: Legacy
 * Copyright (C) 2012-2023 ET:Legacy team <mail@etlegacy.com>
 *
 * This file is part of ET: Legacy - http://www.etlegacy.com
 *
 * ET: Legacy is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ET: Legacy is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ET: Legacy. If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, Wolfenstein: Enemy Territory GPL Source Code is also
 * subject to certain additional terms. You should have received a copy
 * of these additional terms immediately following the terms and conditions
 * of the GNU General Public License which accompanied the source code.
 * If not, please request a copy in writing from id Software at the address below.
 *
 * id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
 */
/**
 * @file renderer2/tr_glsldef.h
 */

#if defined(UNIFORM_ENUM)
#define UNIFORM_DEF(e, n, t) e,
#elif defined(UNIFORM_TYPE)
#define UNIFORM_DEF(e, n, t) {#n, t },
#else
#define UNIFORM_DEF(e, n, t)
#endif

#if defined(MACRO_ENUM)
#define MACRO_DEF(n) n,
#elif defined(MACRO_NAME)
#define MACRO_DEF(n) #n,
#else
#define MACRO_DEF(n)
#endif

// These are not yet in use
#if defined(SHADER_PROTO)
#define SHADER_DEF(name) extern programInfo_t * name;
#elif defined(SHADER_DECL)
#define SHADER_DEF(name) programInfo_t * name;
#elif defined(SHADER_COMP)
#define SHADER_DEF(name) name = GLSL_GetShaderProgram(#name)
#else
#define SHADER_DEF(name)
#endif

#if defined(ATTR_DECL)
#define ATTR_DEF(name, val) name = val,
#elif defined(ATTR_MAP)
#define ATTR_DEF(name, val) { name, #name },
#else
#define ATTR_DEF(name, val)
#endif

#if defined(TEX_DECL)
#define TEX_DEF(name, uniform) name,
#elif defined(TEX_MAP)
#define TEX_DEF(name, uniform) uniform,
#else
#define TEX_DEF(name, uniform)
#endif

UNIFORM_DEF(UNIFORM_DIFFUSEMAP, u_DiffuseMap, GLSL_INT)
UNIFORM_DEF(UNIFORM_LIGHTMAP, u_LightMap, GLSL_INT)
UNIFORM_DEF(UNIFORM_NORMALMAP, u_NormalMap, GLSL_INT)
//UNIFORM_DEF(UNIFORM_DELUXEMAP, u_DeluxeMap, GLSL_INT)
UNIFORM_DEF(UNIFORM_SPECULARMAP, u_SpecularMap, GLSL_INT)
UNIFORM_DEF(UNIFORM_DEPTHMAP, u_DepthMap, GLSL_INT)

UNIFORM_DEF(UNIFORM_TEXTUREMAP, u_TextureMap, GLSL_INT)
UNIFORM_DEF(UNIFORM_LEVELSMAP, u_LevelsMap, GLSL_INT)

UNIFORM_DEF(UNIFORM_SCREENIMAGEMAP, u_ScreenImageMap, GLSL_INT)
UNIFORM_DEF(UNIFORM_SCREENDEPTHMAP, u_ScreenDepthMap, GLSL_INT)

UNIFORM_DEF(UNIFORM_ATTENUATIONXYMAP, u_AttenuationMapXY, GLSL_INT)
UNIFORM_DEF(UNIFORM_ATTENUATIONZMAP, u_AttenuationMapZ, GLSL_INT)

UNIFORM_DEF(UNIFORM_SHADOWMAP, u_ShadowMap, GLSL_INT)
UNIFORM_DEF(UNIFORM_SHADOWMAP0, u_ShadowMap0, GLSL_INT)
UNIFORM_DEF(UNIFORM_SHADOWMAP1, u_ShadowMap1, GLSL_INT)
UNIFORM_DEF(UNIFORM_SHADOWMAP2, u_ShadowMap2, GLSL_INT)
UNIFORM_DEF(UNIFORM_SHADOWMAP3, u_ShadowMap3, GLSL_INT)
UNIFORM_DEF(UNIFORM_SHADOWMAP4, u_ShadowMap4, GLSL_INT)

//UNIFORM_DEF(UNIFORM_RANDOMMAP, u_RandomMap, GLSL_INT)
UNIFORM_DEF(UNIFORM_PORTALMAP, u_PortalMap, GLSL_INT)

UNIFORM_DEF(UNIFORM_DEPTHMAPBACK, u_DepthMapBack, GLSL_INT)
UNIFORM_DEF(UNIFORM_DEPTHMAPFRONT, u_DepthMapFront, GLSL_INT)

UNIFORM_DEF(UNIFORM_CONTRASTMAP, u_ContrastMap, GLSL_INT)

UNIFORM_DEF(UNIFORM_ENVIRONMENTMAP0, u_EnvironmentMap0, GLSL_INT)
UNIFORM_DEF(UNIFORM_ENVIRONMENTMAP1, u_EnvironmentMap1, GLSL_INT)

UNIFORM_DEF(UNIFORM_GRAINMAP, u_GrainMap, GLSL_INT)
UNIFORM_DEF(UNIFORM_VIGNETTEMAP, u_VignetteMap, GLSL_INT)

// FIXME: these uniforms do not actually exist in the GLSL code!
UNIFORM_DEF(UNIFORM_TCGEN0VECTOR0, u_TCGen0Vector0, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_TCGEN0VECTOR1, u_TCGen0Vector1, GLSL_VEC3)

UNIFORM_DEF(UNIFORM_DEFORMGEN, u_DeformGen, GLSL_INT)
UNIFORM_DEF(UNIFORM_DEFORMPARAMS, u_DeformParams, GLSL_FLOATARR)

UNIFORM_DEF(UNIFORM_COLORGEN, u_ColorGen, GLSL_INT)
UNIFORM_DEF(UNIFORM_ALPHAGEN, u_AlphaGen, GLSL_INT)
UNIFORM_DEF(UNIFORM_COLOR, u_Color, GLSL_VEC4)

UNIFORM_DEF(UNIFORM_DLIGHTINFO, u_DlightInfo, GLSL_VEC4)
UNIFORM_DEF(UNIFORM_LIGHTFORWARD, u_LightForward, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_LIGHTUP, u_LightUp, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_LIGHTRIGHT, u_LightRight, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_LIGHTORIGIN, u_LightOrigin, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_LIGHTRADIUS, u_LightRadius, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_AMBIENTLIGHT, u_AmbientLight, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_DIRECTEDLIGHT, u_DirectedLight, GLSL_VEC3)

UNIFORM_DEF(UNIFORM_PORTALRANGE, u_PortalRange, GLSL_FLOAT)

UNIFORM_DEF(UNIFORM_FOGDISTANCE, u_FogDistance, GLSL_VEC4)
UNIFORM_DEF(UNIFORM_FOGDEPTH, u_FogDepth, GLSL_VEC4)
UNIFORM_DEF(UNIFORM_FOGEYET, u_FogEyeT, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_FOGCOLORMASK, u_FogColorMask, GLSL_VEC4)

UNIFORM_DEF(UNIFORM_MODELMATRIX, u_ModelMatrix, GLSL_MAT16)
UNIFORM_DEF(UNIFORM_MODELVIEWPROJECTIONMATRIX, u_ModelViewProjectionMatrix, GLSL_MAT16)

UNIFORM_DEF(UNIFORM_TIME, u_Time, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_VERTEXLERP, u_VertexLerp, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_MATERIALINFO, u_MaterialInfo, GLSL_VEC2)

// znear, zfar, width/2, height/2
UNIFORM_DEF(UNIFORM_VIEWINFO, u_ViewInfo, GLSL_VEC4)
UNIFORM_DEF(UNIFORM_VIEWORIGIN, u_ViewOrigin, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_VIEWFORWARD, u_ViewForward, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_VIEWLEFT, u_ViewLeft, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_VIEWUP, u_ViewUp, GLSL_VEC3)

UNIFORM_DEF(UNIFORM_INVTEXRES, u_InvTexRes, GLSL_VEC2)
UNIFORM_DEF(UNIFORM_AUTOEXPOSUREMINMAX, u_AutoExposureMinMax, GLSL_VEC2)
UNIFORM_DEF(UNIFORM_TONEMINAVGMAXLINEAR, u_ToneMinAvgMaxLinear, GLSL_VEC3)

UNIFORM_DEF(UNIFORM_PRIMARYLIGHTORIGIN, u_PrimaryLightOrigin, GLSL_VEC4)
UNIFORM_DEF(UNIFORM_PRIMARYLIGHTCOLOR, u_PrimaryLightColor, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_PRIMARYLIGHTAMBIENT, u_PrimaryLightAmbient, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_PRIMARYLIGHTRADIUS, u_PrimaryLightRadius, GLSL_FLOAT)

// FROM XREAL
UNIFORM_DEF(UNIFORM_COLORTEXTUREMATRIX, u_ColorTextureMatrix, GLSL_MAT16)
UNIFORM_DEF(UNIFORM_DIFFUSETEXTUREMATRIX, u_DiffuseTextureMatrix, GLSL_MAT16)
UNIFORM_DEF(UNIFORM_NORMALTEXTUREMATRIX, u_NormalTextureMatrix, GLSL_MAT16)
UNIFORM_DEF(UNIFORM_SPECULARTEXTUREMATRIX, u_SpecularTextureMatrix, GLSL_MAT16)
UNIFORM_DEF(UNIFORM_ALPHATEST, u_AlphaTest, GLSL_INT)
UNIFORM_DEF(UNIFORM_COLORMODULATE, u_ColorModulate, GLSL_VEC4)
UNIFORM_DEF(UNIFORM_BONEMATRIX, u_BoneMatrix, GLSL_MAT16ARR)
UNIFORM_DEF(UNIFORM_VERTEXINTERPOLATION, u_VertexInterpolation, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_PORTALPLANE, u_PortalPlane, GLSL_VEC4)
UNIFORM_DEF(UNIFORM_CURRENTMAP, u_CurrentMap, GLSL_INT)
UNIFORM_DEF(UNIFORM_COLORMAP, u_ColorMap, GLSL_INT)
UNIFORM_DEF(UNIFORM_AMBIENTCOLOR, u_AmbientColor, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_LIGHTDIR, u_LightDir, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_LIGHTCOLOR, u_LightColor, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_LIGHTSCALE, u_LightScale, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_LIGHTWRAPAROUND, u_LightWrapAround, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_LIGHTATTENUATIONMATRIX, u_LightAttenuationMatrix, GLSL_MAT16)
UNIFORM_DEF(UNIFORM_LIGHTFRUSTUM, u_LightFrustum, GLSL_VEC4ARR) // VEC4 [6]
UNIFORM_DEF(UNIFORM_SHADOWTEXELSIZE, u_ShadowTexelSize, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_SHADOWBLUR, u_ShadowBlur, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_SHADOWMATRIX, u_ShadowMatrix, GLSL_MAT16ARR) //MAT16 [5]
UNIFORM_DEF(UNIFORM_SHADOWPARALLELSPLITDISTANCES, u_ShadowParallelSplitDistances, GLSL_VEC4)
UNIFORM_DEF(UNIFORM_VIEWMATRIX, u_ViewMatrix, GLSL_MAT16)
UNIFORM_DEF(UNIFORM_MODELVIEWMATRIX, u_ModelViewMatrix, GLSL_MAT16)
UNIFORM_DEF(UNIFORM_MODELVIEWMATRIXTRANSPOSE, u_ModelViewMatrixTranspose, GLSL_MAT16)
UNIFORM_DEF(UNIFORM_PROJECTIONMATRIXTRANSPOSE, u_ProjectionMatrixTranspose, GLSL_MAT16)
UNIFORM_DEF(UNIFORM_UNPROJECTMATRIX, u_UnprojectMatrix, GLSL_MAT16)
UNIFORM_DEF(UNIFORM_DEPTHSCALE, u_DepthScale, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_ENVIRONMENTINTERPOLATION, u_EnvironmentInterpolation, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_DEFORMPARMS, u_DeformParms, GLSL_FLOATARR) // FLOAT [MAX_SHADER_DEFORM_PARMS]
UNIFORM_DEF(UNIFORM_FOGDISTANCEVECTOR, u_FogDistanceVector, GLSL_VEC4)
UNIFORM_DEF(UNIFORM_FOGDEPTHVECTOR, u_FogDepthVector, GLSL_VEC4)
UNIFORM_DEF(UNIFORM_DEFORMMAGNITUDE, u_DeformMagnitude, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_HDRKEY, u_HDRKey, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_HDRAVERAGELUMINANCE, u_HDRAverageLuminance, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_HDRMAXLUMINANCE, u_HDRMaxLuminance, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_REFRACTIONINDEX, u_RefractionIndex, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_FOGDENSITY, u_FogDensity, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_FOGCOLOR, u_FogColor, GLSL_VEC3)
UNIFORM_DEF(UNIFORM_FRESNELPOWER, u_FresnelPower, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_FRESNELSCALE, u_FresnelScale, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_FRESNELBIAS, u_FresnelBias, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_BLURMAGNITUDE, u_BlurMagnitude, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_NORMALSCALE, u_NormalScale, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_SHADOWCOMPARE, u_ShadowCompare, GLSL_FLOAT)
UNIFORM_DEF(UNIFORM_ETARATIO, u_EtaRatio, GLSL_VEC3)

// Booleans
UNIFORM_DEF(UNIFORM_B_SHOW_LIGHTMAP, SHOW_LIGHTMAP, GLSL_BOOL)
//UNIFORM_DEF(UNIFORM_B_SHOW_DELUXEMAP, SHOW_DELUXEMAP, GLSL_BOOL)
UNIFORM_DEF(UNIFORM_B_NORMALMAP, NORMALMAP, GLSL_BOOL)
UNIFORM_DEF(UNIFORM_B_PARALLAXMAP, PARALLAXMAP, GLSL_BOOL)

UNIFORM_DEF(UNIFORM_GAMMA, u_gamma, GLSL_FLOAT)

// Compile macros
MACRO_DEF(USE_ALPHA_TESTING)
MACRO_DEF(USE_PORTAL_CLIPPING)
MACRO_DEF(USE_FRUSTUM_CLIPPING)
MACRO_DEF(USE_VERTEX_SKINNING)
MACRO_DEF(USE_VERTEX_ANIMATION)
MACRO_DEF(USE_DEFORM_VERTEXES)
MACRO_DEF(USE_TCGEN_ENVIRONMENT)
MACRO_DEF(USE_TCGEN_LIGHTMAP)
MACRO_DEF(USE_NORMAL_MAPPING)
MACRO_DEF(USE_PARALLAX_MAPPING)
MACRO_DEF(USE_DELUXE_MAPPING)
MACRO_DEF(USE_REFLECTIONS)
MACRO_DEF(USE_SPECULAR)
MACRO_DEF(USE_WATER)
MACRO_DEF(USE_SHADOWING)
MACRO_DEF(TWOSIDED)
MACRO_DEF(EYE_OUTSIDE)
MACRO_DEF(BRIGHTPASS_FILTER)
MACRO_DEF(LIGHT_DIRECTIONAL)
MACRO_DEF(USE_GBUFFER)

// Attributes
ATTR_DEF(ATTR_POSITION, BIT(0))
ATTR_DEF(ATTR_TEXCOORD, BIT(1))
ATTR_DEF(ATTR_LIGHTCOORD, BIT(2))
ATTR_DEF(ATTR_TANGENT, BIT(3))
ATTR_DEF(ATTR_BINORMAL, BIT(4))
ATTR_DEF(ATTR_NORMAL, BIT(5))
ATTR_DEF(ATTR_COLOR, BIT(6))
//ATTR_DEF(ATTR_PAINTCOLOR, BIT(7))
//ATTR_DEF(ATTR_LIGHTDIRECTION, BIT(8)) // see u_LightDir
ATTR_DEF(ATTR_BONE_INDEXES, BIT(9))
ATTR_DEF(ATTR_BONE_WEIGHTS, BIT(10))

// for .md3 interpolation
ATTR_DEF(ATTR_POSITION2, BIT(11))
ATTR_DEF(ATTR_TANGENT2, BIT(12))
ATTR_DEF(ATTR_BINORMAL2, BIT(13))
ATTR_DEF(ATTR_NORMAL2, BIT(14))

// FIXME XBSP format with ATTR_LIGHTDIRECTION and ATTR_PAINTCOLOR
//ATTR_DEF(ATTR_DEFAULT, ATTR_POSITION | ATTR_TEXCOORD | ATTR_TANGENT | ATTR_BINORMAL | ATTR_COLOR)
ATTR_DEF(ATTR_BITS, ATTR_POSITION | ATTR_TEXCOORD | ATTR_LIGHTCOORD | ATTR_TANGENT | ATTR_BINORMAL | ATTR_NORMAL | ATTR_COLOR) //|ATTR_PAINTCOLOR | ATTR_LIGHTDIRECTION | ATTR_BONE_INDEXES | ATTR_BONE_WEIGHTS

// Texture unit definitions
TEX_DEF(TEX_CURRENT, UNIFORM_CURRENTMAP)
TEX_DEF(TEX_COLOR, UNIFORM_COLORMAP)
TEX_DEF(TEX_DIFFUSE, UNIFORM_DIFFUSEMAP)
TEX_DEF(TEX_NORMAL, UNIFORM_NORMALMAP)
TEX_DEF(TEX_SPECULAR, UNIFORM_SPECULARMAP)
TEX_DEF(TEX_DEPTH, UNIFORM_DEPTHMAP)
TEX_DEF(TEX_LIGHTMAP, UNIFORM_LIGHTMAP)
//TEX_DEF(TEX_DELUXE, UNIFORM_DELUXEMAP)
TEX_DEF(TEX_ATTEXY, UNIFORM_ATTENUATIONXYMAP)
TEX_DEF(TEX_ATTEZ, UNIFORM_ATTENUATIONZMAP)
TEX_DEF(TEX_SHADOWMAP, UNIFORM_SHADOWMAP)
TEX_DEF(TEX_SHADOWMAP0, UNIFORM_SHADOWMAP0)
TEX_DEF(TEX_SHADOWMAP1, UNIFORM_SHADOWMAP1)
TEX_DEF(TEX_SHADOWMAP2, UNIFORM_SHADOWMAP2)
TEX_DEF(TEX_SHADOWMAP3, UNIFORM_SHADOWMAP3)
TEX_DEF(TEX_SHADOWMAP4, UNIFORM_SHADOWMAP4)
//TEX_DEF(TEX_RANDOM, UNIFORM_RANDOMMAP)
TEX_DEF(TEX_PORTAL, UNIFORM_PORTALMAP)
TEX_DEF(TEX_DEPTHBACK, UNIFORM_DEPTHMAPBACK)
TEX_DEF(TEX_DEPTHFRONT, UNIFORM_DEPTHMAPFRONT)
TEX_DEF(TEX_CONTRAST, UNIFORM_CONTRASTMAP)
TEX_DEF(TEX_ENVMAP0, UNIFORM_ENVIRONMENTMAP0)
TEX_DEF(TEX_ENVMAP1, UNIFORM_ENVIRONMENTMAP1)
TEX_DEF(TEX_VIGNETTE, UNIFORM_VIGNETTEMAP)
TEX_DEF(TEX_GRAIN, UNIFORM_GRAINMAP)

#undef UNIFORM_DEF
#undef MACRO_DEF
#undef SHADER_DEF
#undef ATTR_DEF
#undef TEX_DEF
