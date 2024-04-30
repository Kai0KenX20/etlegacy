/*
 * Wolfenstein: Enemy Territory GPL Source Code
 * Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
 *
 * ET: Legacy
 * Copyright (C) 2012-2024 ET:Legacy team <mail@etlegacy.com>
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
 * @file cm_trace.c
 */

#include "cm_local.h"
#include "cm_patch.h"

/// Always use bbox vs. bbox collision and never capsule vs. bbox or vice versa
#define ALWAYS_BBOX_VS_BBOX
/// Always use capsule vs. capsule collision and never capsule vs. bbox or vice versa
//#define ALWAYS_CAPSULE_VS_CAPSULE

//#define CAPSULE_DEBUG

/**
===============================================================================
BASIC MATH
===============================================================================
*/

#if !defined(ALWAYS_BBOX_VS_BBOX)

/**
 * @brief CM_ProjectPointOntoVector
 * @param point
 * @param vStart
 * @param vDir
 * @param vProj
 */
void CM_ProjectPointOntoVector(vec3_t point, vec3_t vStart, vec3_t vDir, vec3_t vProj)
{
	vec3_t pVec;
	float  dot;

	VectorSubtract(point, vStart, pVec);
	// project onto the directional vector for this segment
	Dot(pVec, vDir, dot);
	VectorMA(vStart, dot, vDir, vProj);
}

/**
 * @brief CM_DistanceFromLineSquared
 * @param[in] p
 * @param[in] lp1
 * @param[in] lp2
 * @param[in] dir
 * @return
 */
float CM_DistanceFromLineSquared(vec3_t p, vec3_t lp1, vec3_t lp2, vec3_t dir)
{
	vec3_t proj, t;
	int    j;

	CM_ProjectPointOntoVector(p, lp1, dir, proj);
	for (j = 0; j < 3; j++)
		if ((proj[j] > lp1[j] && proj[j] > lp2[j]) ||
		    (proj[j] < lp1[j] && proj[j] < lp2[j]))
		{
			break;
		}
	if (j < 3)
	{
		if (Q_fabs(proj[j] - lp1[j]) < Q_fabs(proj[j] - lp2[j]))
		{
			VectorSubtract(p, lp1, t);
		}
		else
		{
			VectorSubtract(p, lp2, t);
		}
		return VectorLengthSquared(t);
	}
	VectorSubtract(p, proj, t);
	return VectorLengthSquared(t);
}

/**
 * @brief SquareRootFloat
 * @param[in] number
 * @return
 */
float SquareRootFloat(float number)
{
	floatint_t  t;
	float       x, y;
	const float f = 1.5F;

	x   = number * 0.5F;
	t.f = number;
	t.i = 0x5f3759df - (t.i >> 1);
	y   = t.f;

	y = y * (f - (x * y * y));
	y = y * (f - (x * y * y));
	return number * y;
}

#endif

/**
===============================================================================
POSITION TESTING
===============================================================================
*/

/**
 * @brief CM_TestBoxInBrush
 * @param[in] tw
 * @param[in] brush
 */
void CM_TestBoxInBrush(traceWork_t *tw, cbrush_t *brush)
{
	int          i;
	cplane_t     *plane;
	float        dist, d1;
	cbrushside_t *side;

	if (!brush->numsides)
	{
		return;
	}

	// special test for axial
	if (VectorCompareGT(tw->bounds[0], brush->bounds[1]) || VectorCompareLT(tw->bounds[1], brush->bounds[0]))
	{
		return;
	}

	if (tw->sphere.use)
	{
		vec3_t startp;
		float  t;

		// the first six planes are the axial planes, so we only
		// need to test the remainder
		for (i = 6 ; i < brush->numsides ; i++)
		{
			side  = brush->sides + i;
			plane = side->plane;

			// adjust the plane distance apropriately for radius
			dist = plane->dist + tw->sphere.radius;
			// find the closest point on the capsule to the plane
			Dot(plane->normal, tw->sphere.offset, t);
			if (t > 0.0f)
			{
				VectorSubtract(tw->start, tw->sphere.offset, startp);
			}
			else
			{
				VectorAdd(tw->start, tw->sphere.offset, startp);
			}
			Dot(startp, plane->normal, d1);
			d1 -= dist;
			// if completely in front of face, no intersection
			if (d1 > 0.0f)
			{
				return;
			}
		}
	}
	else
	{
		// the first six planes are the axial planes, so we only
		// need to test the remainder
		for (i = 6 ; i < brush->numsides ; i++)
		{
			side  = brush->sides + i;
			plane = side->plane;

			// adjust the plane distance apropriately for mins/maxs
			Dot(tw->offsets[plane->signbits], plane->normal, d1);
			dist = plane->dist - d1;

			Dot(tw->start, plane->normal, d1);
			d1 -= dist;

			// if completely in front of face, no intersection
			if (d1 > 0.0f)
			{
				return;
			}
		}
	}

	// inside this brush
	tw->trace.startsolid = tw->trace.allsolid = qtrue;
	tw->trace.fraction   = 0.0f;
	tw->trace.contents   = brush->contents;
}

/**
 * @brief CM_TestInLeaf
 * @param[in,out] tw
 * @param[in] leaf
 */
void CM_TestInLeaf(traceWork_t *tw, cLeaf_t *leaf)
{
	int      k;
	int      brushnum;
	cbrush_t *b;

	// test box position against all brushes in the leaf
	for (k = 0 ; k < leaf->numLeafBrushes ; k++)
	{
		brushnum = cm.leafbrushes[leaf->firstLeafBrush + k];
		b        = &cm.brushes[brushnum];
		if (b->checkcount == cm.checkcount)
		{
			continue;   // already checked this brush in another leaf
		}
		b->checkcount = cm.checkcount;

		if (!(b->contents & tw->contents))
		{
			continue;
		}

		CM_TestBoxInBrush(tw, b);
		if (tw->trace.allsolid)
		{
			return;
		}
	}

	// test against all patches
	if (!cm_noCurves->integer)
	{
		cPatch_t *patch;

		for (k = 0 ; k < leaf->numLeafSurfaces ; k++)
		{
			patch = cm.surfaces[cm.leafsurfaces[leaf->firstLeafSurface + k]];
			if (!patch)
			{
				continue;
			}
			if (patch->checkcount == cm.checkcount)
			{
				continue;   // already checked this brush in another leaf
			}
			patch->checkcount = cm.checkcount;

			if (!(patch->contents & tw->contents))
			{
				continue;
			}

			if (CM_PositionTestInPatchCollide(tw, patch->pc))
			{
				tw->trace.startsolid = tw->trace.allsolid = qtrue;
				tw->trace.fraction   = 0.0f;

				return;
			}
		}
	}
}

#if !defined(ALWAYS_BBOX_VS_BBOX)
/**
 * @brief Capsule inside capsule check
 * @param[in,out] tw
 * @param[in] model
 */
void CM_TestCapsuleInCapsule(traceWork_t *tw, clipHandle_t model)
{
	int    i;
	vec3_t mins, maxs;
	vec3_t top, bottom;
	vec3_t p1, p2, tmp;
	vec3_t offset, symetricSize[2];
	float  radius, halfwidth, halfheight, offs, r;

	CM_ModelBounds(model, mins, maxs);

	VectorAdd(tw->start, tw->sphere.offset, top);
	VectorSubtract(tw->start, tw->sphere.offset, bottom);
	for (i = 0 ; i < 3 ; i++)
	{
		offset[i]          = (mins[i] + maxs[i]) * 0.5f;
		symetricSize[0][i] = mins[i] - offset[i];
		symetricSize[1][i] = maxs[i] - offset[i];
	}
	halfwidth  = symetricSize[1][0];
	halfheight = symetricSize[1][2];
	radius     = (halfwidth > halfheight) ? halfheight : halfwidth;
	offs       = halfheight - radius;

	r = Square(tw->sphere.radius + radius);
	// check if any of the spheres overlap
	VectorCopy(offset, p1);
	p1[2] += offs;
	VectorSubtract(p1, top, tmp);
	if (VectorLengthSquared(tmp) < r)
	{
		tw->trace.startsolid = tw->trace.allsolid = qtrue;
		tw->trace.fraction   = 0;
	}
	VectorSubtract(p1, bottom, tmp);
	if (VectorLengthSquared(tmp) < r)
	{
		tw->trace.startsolid = tw->trace.allsolid = qtrue;
		tw->trace.fraction   = 0;
	}
	VectorCopy(offset, p2);
	p2[2] -= offs;
	VectorSubtract(p2, top, tmp);
	if (VectorLengthSquared(tmp) < r)
	{
		tw->trace.startsolid = tw->trace.allsolid = qtrue;
		tw->trace.fraction   = 0;
	}
	VectorSubtract(p2, bottom, tmp);
	if (VectorLengthSquared(tmp) < r)
	{
		tw->trace.startsolid = tw->trace.allsolid = qtrue;
		tw->trace.fraction   = 0;
	}
	// if between cylinder up and lower bounds
	if ((top[2] >= p1[2] && top[2] <= p2[2]) ||
	    (bottom[2] >= p1[2] && bottom[2] <= p2[2]))
	{
		// 2d coordinates
		top[2] = p1[2] = 0;
		// if the cylinders overlap
		VectorSubtract(top, p1, tmp);
		if (VectorLengthSquared(tmp) < r)
		{
			tw->trace.startsolid = tw->trace.allsolid = qtrue;
			tw->trace.fraction   = 0;
		}
	}
}

/**
 * @brief Bounding box inside capsule check
 * @param[in,out] tw
 * @param model
 */
void CM_TestBoundingBoxInCapsule(traceWork_t *tw, clipHandle_t model)
{
	vec3_t       mins, maxs, offset, size[2];
	clipHandle_t h;
	cmodel_t     *cmod;
	int          i;

	// mins maxs of the capsule
	CM_ModelBounds(model, mins, maxs);

	// offset for capsule center
	for (i = 0 ; i < 3 ; i++)
	{
		offset[i]     = (mins[i] + maxs[i]) * 0.5f;
		size[0][i]    = mins[i] - offset[i];
		size[1][i]    = maxs[i] - offset[i];
		tw->start[i] -= offset[i];
		tw->end[i]   -= offset[i];
	}

	// replace the bounding box with the capsule
	tw->sphere.use        = qtrue;
	tw->sphere.radius     = (size[1][0] > size[1][2]) ? size[1][2] : size[1][0];
	tw->sphere.halfheight = size[1][2];
	VectorSet(tw->sphere.offset, 0, 0, size[1][2] - tw->sphere.radius);

	// replace the capsule with the bounding box
	h = CM_TempBoxModel(tw->size[0], tw->size[1], qfalse);
	// calculate collision
	cmod = CM_ClipHandleToModel(h);
	CM_TestInLeaf(tw, &cmod->leaf);
}
#endif

#define MAX_POSITION_LEAFS  1024

/**
 * @brief CM_PositionTest
 * @param[in,out] tw
 */
void CM_PositionTest(traceWork_t *tw)
{
	int        leafs[MAX_POSITION_LEAFS];
	int        i;
	leafList_t ll;

	// identify the leafs we are touching
	VectorAdd(tw->start, tw->size[0], ll.bounds[0]);
	VectorAdd(tw->start, tw->size[1], ll.bounds[1]);

	VectorAddConst(ll.bounds[0], -1.0f, ll.bounds[0]);
	VectorAddConst(ll.bounds[1], 1.0f, ll.bounds[1]);

	ll.count      = 0;
	ll.maxcount   = MAX_POSITION_LEAFS;
	ll.list       = leafs;
	ll.storeLeafs = CM_StoreLeafs;
	ll.lastLeaf   = 0;
	ll.overflowed = qfalse;

	cm.checkcount++;

	CM_BoxLeafnums_r(&ll, 0);

	cm.checkcount++;

	// test the contents of the leafs
	for (i = 0 ; i < ll.count ; i++)
	{
		CM_TestInLeaf(tw, &cm.leafs[leafs[i]]);
		if (tw->trace.allsolid)
		{
			break;
		}
	}
}

/**
===============================================================================
TRACING
===============================================================================
*/

/**
 * @brief CM_TraceThroughPatch
 * @param[in,out] tw
 * @param[in] patch
 */
static void CM_TraceThroughPatch(traceWork_t *tw, cPatch_t *patch)
{
	float oldFrac = tw->trace.fraction;

	c_patch_traces++;

	CM_TraceThroughPatchCollide(tw, patch->pc);

	if (tw->trace.fraction < oldFrac)
	{
		tw->trace.surfaceFlags = patch->surfaceFlags;
		tw->trace.contents     = patch->contents;
	}
}

/**
 * @brief CM_CalcTraceBounds
 * @param[in,out] tw
 * @param[in] expand
 */
static void CM_CalcTraceBounds(traceWork_t *tw, qboolean expand)
{
	int i, mask;
	vec3_t s, d, startd;
	VectorScale(tw->dir, tw->trace.fraction, d);
	VectorAdd(tw->start, d, startd);
	mask = VectorCompareLT(tw->start, tw->end);

	if (tw->sphere.use)
	{
		VectorAbs(tw->sphere.offset, s);
		VectorAddConst(s, tw->sphere.radius, s);
		for (i = 0; i < 3; i++)
		{
			//if (tw->start[i] < tw->end[i])
			if (mask & (1 << i))
			{
				tw->bounds[0][i] = tw->start[i] - s[i];
				tw->bounds[1][i] = startd[i] + s[i];
			}
			else
			{
				tw->bounds[0][i] = startd[i] - s[i];
				tw->bounds[1][i] = tw->start[i] + s[i];
			}
		}
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			if (mask & (1 << i))
			{
				tw->bounds[0][i] = tw->start[i] + tw->size[0][i];
				tw->bounds[1][i] = startd[i] + tw->size[1][i];
			}
			else
			{
				tw->bounds[0][i] = startd[i] + tw->size[0][i];
				tw->bounds[1][i] = tw->start[i] + tw->size[1][i];
			}
		}
	}

	if (expand)
	{
		// expand for epsilon
		VectorAddConst(tw->bounds[0], -1.0f, tw->bounds[0]);
		VectorAddConst(tw->bounds[1], 1.0f, tw->bounds[1]);
	}
}

/**
 * @brief CM_BoxDistanceFromPlane
 * @param[in] center
 * @param[in] extents
 * @param[in] plane
 * @return
 */
static float CM_BoxDistanceFromPlane(vec3_t center, vec3_t extents, cplane_t *plane)
{
	float d1, d2, d1d2;
	vec3_t v;

	Dot(center, plane->normal, d1);
	d1 -= plane->dist;

	VectorMultiply(extents, plane->normal, v);
	VectorAbs(v, v);
	d2 = v[0] + v[1] + v[2];

	d1d2 = d1 - d2;
	if (d1d2 > 0.0f)
	{
		return d1d2;
	}
	d1d2 = d1 + d2;
	if (d1d2 < 0.0f)
	{
		return d1d2;;
	}
	return 0.0f;
}

/**
 * @brief CM_TraceThroughBounds
 * @param[in] tw
 * @param[in] mins
 * @param[in] maxs
 * @return
 */
static int CM_TraceThroughBounds(traceWork_t *tw, vec3_t mins, vec3_t maxs)
{
	if (VectorCompareGT(mins, tw->bounds[1]) || VectorCompareLT(maxs, tw->bounds[0]))
	{
		return qfalse;
	}

	{
		vec3_t center, extents;

		VectorAdd(mins, maxs, center);
		VectorScale(center, 0.5f, center);
		VectorSubtract(maxs, center, extents);

		if (Q_fabs(CM_BoxDistanceFromPlane(center, extents, &tw->tracePlane1)) > tw->traceDist1)
		{
			return qfalse;
		}
		if (Q_fabs(CM_BoxDistanceFromPlane(center, extents, &tw->tracePlane2)) > tw->traceDist2)
		{
			return qfalse;
		}
	}
	// trace might go through the bounds
	return qtrue;
}

/**
 * @brief CM_TraceThroughBrush
 * @param[in,out] tw
 * @param[in] brush
 */
static void CM_TraceThroughBrush(traceWork_t *tw, cbrush_t *brush)
{
	int          i;
	cplane_t     *plane, *clipplane = NULL;
	float        dist;
	float        enterFrac = -1.f, leaveFrac = 1.f;
	float        d1, d2;
	qboolean     getout, startout;
	float        f;
	cbrushside_t *side, *leadside;

	if (!brush->numsides)
	{
		return;
	}

	c_brush_traces++;

	getout   = qfalse;
	startout = qfalse;

	leadside = NULL;

	if (tw->sphere.use)
	{
		vec3_t startp;
		vec3_t endp;
		float  t;

		// compare the trace against all planes of the brush
		// find the latest time the trace crosses a plane towards the interior
		// and the earliest time the trace crosses a plane towards the exterior
		for (i = 0; i < brush->numsides; i++)
		{
			side  = brush->sides + i;
			plane = side->plane;

			// adjust the plane distance apropriately for radius
			dist = plane->dist + tw->sphere.radius;

			// find the closest point on the capsule to the plane
			Dot(plane->normal, tw->sphere.offset, t);
			if (t > 0.0f)
			{
				VectorSubtract(tw->start, tw->sphere.offset, startp);
				VectorSubtract(tw->end, tw->sphere.offset, endp);
			}
			else
			{
				VectorAdd(tw->start, tw->sphere.offset, startp);
				VectorAdd(tw->end, tw->sphere.offset, endp);
			}

			Dot(startp, plane->normal, d1);
			d1 -= dist;
			Dot(endp, plane->normal, d2);
			d2 -= dist;

			if (d2 > 0.0f)
			{
				getout = qtrue; // endpoint is not in solid
			}
			if (d1 > 0.0f)
			{
				startout = qtrue;
			}

			// if completely in front of face, no intersection with the entire brush
			if (d1 > 0.0f && (d2 >= SURFACE_CLIP_EPSILON || d2 >= d1))
			{
				return;
			}

			// if it doesn't cross the plane, the plane isn't relevent
			if (d1 <= 0.0f && d2 <= 0.0f)
			{
				continue;
			}

			// crosses face
			if (d1 > d2)      // enter
			{
				f = (d1 - SURFACE_CLIP_EPSILON) / (d1 - d2);
				if (f < 0.0f)
				{
					f = 0.0f;
				}
				if (f > enterFrac)
				{
					enterFrac = f;
					clipplane = plane;
					leadside  = side;
				}
			}
			else        // leave
			{
				f = (d1 + SURFACE_CLIP_EPSILON) / (d1 - d2);
				if (f > 1.0f)
				{
					f = 1.0f;
				}
				if (f < leaveFrac)
				{
					leaveFrac = f;
				}
			}
		}
	}
	else
	{
		// compare the trace against all planes of the brush
		// find the latest time the trace crosses a plane towards the interior
		// and the earliest time the trace crosses a plane towards the exterior
		for (i = 0; i < brush->numsides; i++)
		{
			side  = brush->sides + i;
			plane = side->plane;

			// adjust the plane distance apropriately for mins/maxs
			Dot(tw->offsets[plane->signbits], plane->normal, d1);
			dist = plane->dist - d1;

			Dot(tw->start, plane->normal, d1);
			d1 -= dist;
			Dot(tw->end, plane->normal, d2);
			d2 -= dist;

			if (d2 > 0.0f)
			{
				getout = qtrue; // endpoint is not in solid
			}
			if (d1 > 0.0f)
			{
				startout = qtrue;
			}

			// if completely in front of face, no intersection with the entire brush
			if (d1 > 0.0f && (d2 >= SURFACE_CLIP_EPSILON || d2 >= d1))
			{
				return;
			}

			// if it doesn't cross the plane, the plane isn't relevent
			if (d1 <= 0.0f && d2 <= 0.0f)
			{
				continue;
			}

			// crosses face
			if (d1 > d2)      // enter
			{
				f = (d1 - SURFACE_CLIP_EPSILON) / (d1 - d2);
				if (f < 0.0f)
				{
					f = 0.0f;
				}
				if (f > enterFrac)
				{
					enterFrac = f;
					clipplane = plane;
					leadside  = side;
				}
			}
			else        // leave
			{
				f = (d1 + SURFACE_CLIP_EPSILON) / (d1 - d2);
				if (f > 1.0f)
				{
					f = 1.0f;
				}
				if (f < leaveFrac)
				{
					leaveFrac = f;
				}
			}
		}
	}

	// all planes have been checked, and the trace was not
	// completely outside the brush
	if (!startout)        // original point was inside brush
	{
		tw->trace.startsolid = qtrue;
		if (!getout)
		{
			tw->trace.allsolid = qtrue;
			tw->trace.fraction = 0.0f;
		}
		return;
	}

	if (enterFrac < leaveFrac)
	{
		if (enterFrac > -1 && enterFrac < tw->trace.fraction)
		{
			if (enterFrac < 0.0f)
			{
				enterFrac = 0.0f;
			}
			tw->trace.fraction     = enterFrac;
			tw->trace.plane        = *clipplane;
			tw->trace.surfaceFlags = leadside->surfaceFlags;
			tw->trace.contents     = brush->contents;
		}
	}
}

/**
 * @brief CM_TraceThroughLeaf
 * @param[in] tw
 * @param[in] leaf
 */
static void CM_TraceThroughLeaf(traceWork_t *tw, cLeaf_t *leaf)
{
	int      k;
	cbrush_t *brush;
	float    fraction;

	// trace line against all brushes in the leaf
	for (k = 0 ; k < leaf->numLeafBrushes ; k++)
	{
		// brushnum = cm.leafbrushes[leaf->firstLeafBrush + k];
		brush = &cm.brushes[cm.leafbrushes[leaf->firstLeafBrush + k]];
		if (brush->checkcount == cm.checkcount)
		{
			continue;   // already checked this brush in another leaf
		}
		brush->checkcount = cm.checkcount;

		if (!(brush->contents & tw->contents))
		{
			continue;
		}

		if (cm_optimize->integer)
		{
			if (!CM_TraceThroughBounds(tw, brush->bounds[0], brush->bounds[1]))
			{
				continue;
			}
		}

		fraction = tw->trace.fraction;

		CM_TraceThroughBrush(tw, brush);

		if (tw->trace.fraction == 0.f)
		{
			return;
		}

		if (tw->trace.fraction < fraction)
		{
			CM_CalcTraceBounds(tw, qtrue);
		}
	}

	// trace line against all patches in the leaf
	if (!cm_noCurves->integer)
	{
		cPatch_t *patch;

		for (k = 0 ; k < leaf->numLeafSurfaces ; k++)
		{
			patch = cm.surfaces[cm.leafsurfaces[leaf->firstLeafSurface + k]];
			if (!patch)
			{
				continue;
			}
			if (patch->checkcount == cm.checkcount)
			{
				continue;   // already checked this patch in another leaf
			}
			patch->checkcount = cm.checkcount;

			if (!(patch->contents & tw->contents))
			{
				continue;
			}

			if (cm_optimize->integer)
			{
				if (!CM_TraceThroughBounds(tw, patch->pc->bounds[0], patch->pc->bounds[1]))
				{
					continue;
				}
			}

			fraction = tw->trace.fraction;

			CM_TraceThroughPatch(tw, patch);

			if (tw->trace.fraction == 0.f)
			{
				return;
			}

			if (tw->trace.fraction < fraction)
			{
				CM_CalcTraceBounds(tw, qtrue);
			}
		}
	}
}
#if !defined(ALWAYS_BBOX_VS_BBOX)

#define RADIUS_EPSILON      1.0f

/**
 * @brief Get the first intersection of the ray with the sphere
 * @param[in,out] tw
 * @param[in] origin
 * @param[in] radius
 * @param[in] start
 * @param[in] end
 */
static void CM_TraceThroughSphere(traceWork_t *tw, vec3_t origin, float radius, vec3_t start, vec3_t end)
{
	float         l1, l2, length;
	float /*a, */ b, c, d;
	vec3_t        v1, dir;

	// if inside the sphere
	VectorSubtract(start, origin, dir);
	l1 = VectorLengthSquared(dir);
	if (l1 < Square(radius))
	{
		tw->trace.fraction   = 0;
		tw->trace.startsolid = qtrue;
		// test for allsolid
		VectorSubtract(end, origin, dir);
		l1 = VectorLengthSquared(dir);
		if (l1 < Square(radius))
		{
			tw->trace.allsolid = qtrue;
		}
		return;
	}

	VectorSubtract(end, start, dir);
	length = VectorNormalize(dir);

	l1 = CM_DistanceFromLineSquared(origin, start, end, dir);
	VectorSubtract(end, origin, v1);
	l2 = VectorLengthSquared(v1);
	// if no intersection with the sphere and the end point is at least an epsilon away
	if (l1 >= Square(radius) && l2 > Square(radius + SURFACE_CLIP_EPSILON))
	{
		return;
	}

	//  | origin - (start + t * dir) | = radius
	//  a = dir[0]^2 + dir[1]^2 + dir[2]^2;
	//  b = 2 * (dir[0] * (start[0] - origin[0]) + dir[1] * (start[1] - origin[1]) + dir[2] * (start[2] - origin[2]));
	//  c = (start[0] - origin[0])^2 + (start[1] - origin[1])^2 + (start[2] - origin[2])^2 - radius^2;

	VectorSubtract(start, origin, v1);
	// dir is normalized so a = 1
	//a = 1.0f; //dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2];
	b = 2.0f * (dir[0] * v1[0] + dir[1] * v1[1] + dir[2] * v1[2]);
	c = v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2] - (radius + RADIUS_EPSILON) * (radius + RADIUS_EPSILON);

	d = b * b - 4.0f * c; // * a;
	if (d > 0)
	{
		vec3_t intersection;
		float  sqrtd = SquareRootFloat(d);
		// = (- b + sqrtd) * 0.5f; // / (2.0f * a);
		float fraction = (-b - sqrtd) * 0.5f;   // / (2.0f * a);

		if (fraction < 0)
		{
			fraction = 0;
		}
		else
		{
			fraction /= length;
		}
		if (fraction < tw->trace.fraction)
		{
			float scale;

			tw->trace.fraction = fraction;
			VectorSubtract(end, start, dir);
			VectorMA(start, fraction, dir, intersection);
			VectorSubtract(intersection, origin, dir);
#ifdef CAPSULE_DEBUG
			l2 = VectorLength(dir);
			if (l2 < radius)
			{
				int bah = 1;
			}
#endif
			scale = 1.0f / (radius + RADIUS_EPSILON);
			VectorScale(dir, scale, dir);
			VectorCopy(dir, tw->trace.plane.normal);
			VectorAdd(tw->modelOrigin, intersection, intersection);
			Dot(tw->trace.plane.normal, intersection, tw->trace.plane.dist);
			tw->trace.contents = CONTENTS_BODY;
		}
	}
	/*
	else if (d == 0)
	{
	    //t1 = (- b ) / 2;
	    // slide along the sphere
	}
	*/
	// no intersection at all
}

/**
 * @brief get the first intersection of the ray with the cylinder
 * the cylinder extends halfheight above and below the origin
 * @param[in,out] tw
 * @param[in] origin
 * @param[in] radius
 * @param[in] halfheight
 * @param[in] start
 * @param[in] end
 */
static void CM_TraceThroughVerticalCylinder(traceWork_t *tw, vec3_t origin, float radius, float halfheight, vec3_t start, vec3_t end)
{
	float         length, l1, l2;
	float /*a, */ b, c, d;
	vec3_t        v1, dir, start2d, end2d, org2d;

	// 2d coordinates
	VectorSet(start2d, start[0], start[1], 0);
	VectorSet(end2d, end[0], end[1], 0);
	VectorSet(org2d, origin[0], origin[1], 0);
	// if between lower and upper cylinder bounds
	if (start[2] <= origin[2] + halfheight &&
	    start[2] >= origin[2] - halfheight)
	{
		// if inside the cylinder
		VectorSubtract(start2d, org2d, dir);
		l1 = VectorLengthSquared(dir);
		if (l1 < Square(radius))
		{
			tw->trace.fraction   = 0;
			tw->trace.startsolid = qtrue;
			VectorSubtract(end2d, org2d, dir);
			l1 = VectorLengthSquared(dir);
			if (l1 < Square(radius))
			{
				tw->trace.allsolid = qtrue;
			}
			return;
		}
	}
	//
	VectorSubtract(end2d, start2d, dir);
	length = VectorNormalize(dir);
	//
	l1 = CM_DistanceFromLineSquared(org2d, start2d, end2d, dir);
	VectorSubtract(end2d, org2d, v1);
	l2 = VectorLengthSquared(v1);
	// if no intersection with the cylinder and the end point is at least an epsilon away
	if (l1 >= Square(radius) && l2 > Square(radius + SURFACE_CLIP_EPSILON))
	{
		return;
	}

	// (start[0] - origin[0] - t * dir[0]) ^ 2 + (start[1] - origin[1] - t * dir[1]) ^ 2 = radius ^ 2
	// (v1[0] + t * dir[0]) ^ 2 + (v1[1] + t * dir[1]) ^ 2 = radius ^ 2;
	// v1[0] ^ 2 + 2 * v1[0] * t * dir[0] + (t * dir[0]) ^ 2 +
	//                      v1[1] ^ 2 + 2 * v1[1] * t * dir[1] + (t * dir[1]) ^ 2 = radius ^ 2
	// t ^ 2 * (dir[0] ^ 2 + dir[1] ^ 2) + t * (2 * v1[0] * dir[0] + 2 * v1[1] * dir[1]) +
	//                      v1[0] ^ 2 + v1[1] ^ 2 - radius ^ 2 = 0

	VectorSubtract(start, origin, v1);
	// dir is normalized so we can use a = 1
	//a = 1.0f; // * (dir[0] * dir[0] + dir[1] * dir[1]);
	b = 2.0f * (v1[0] * dir[0] + v1[1] * dir[1]);
	c = v1[0] * v1[0] + v1[1] * v1[1] - (radius + RADIUS_EPSILON) * (radius + RADIUS_EPSILON);

	d = b * b - 4.0f * c; // * a;
	if (d > 0)
	{
		float sqrtd = SquareRootFloat(d);
		// = (- b + sqrtd) * 0.5f;// / (2.0f * a);
		float fraction = (-b - sqrtd) * 0.5f;   // / (2.0f * a);

		if (fraction < 0)
		{
			fraction = 0;
		}
		else
		{
			fraction /= length;
		}
		if (fraction < tw->trace.fraction)
		{
			vec3_t intersection;

			VectorSubtract(end, start, dir);
			VectorMA(start, fraction, dir, intersection);
			// if the intersection is between the cylinder lower and upper bound
			if (intersection[2] <= origin[2] + halfheight &&
			    intersection[2] >= origin[2] - halfheight)
			{
				float scale;

				tw->trace.fraction = fraction;
				VectorSubtract(intersection, origin, dir);
				dir[2] = 0;
#ifdef CAPSULE_DEBUG
				l2 = VectorLength(dir);
				if (l2 <= radius)
				{
					int bah = 1;
				}
#endif
				scale = 1.0f / (radius + RADIUS_EPSILON);
				VectorScale(dir, scale, dir);
				VectorCopy(dir, tw->trace.plane.normal);
				VectorAdd(tw->modelOrigin, intersection, intersection);
				Dot(tw->trace.plane.normal, intersection, tw->trace.plane.dist);
				tw->trace.contents = CONTENTS_BODY;
			}
		}
	}
	/*
	else if (d == 0)
	{
	    //t[0] = (- b ) / 2 * a;
	    // slide along the cylinder
	}
	*/
	// no intersection at all
}

/**
 * @brief Capsule vs. capsule collision (not rotated)
 * @param[in] tw
 * @param[in] model
 */
static void CM_TraceCapsuleThroughCapsule(traceWork_t *tw, clipHandle_t model)
{
	int    i;
	vec3_t mins, maxs;
	vec3_t top, bottom, starttop, startbottom, endtop, endbottom;
	vec3_t offset, symetricSize[2];
	float  radius, halfwidth, halfheight, offs;

	CM_ModelBounds(model, mins, maxs);
	// test trace bounds vs. capsule bounds
	if (tw->bounds[0][0] > maxs[0] + RADIUS_EPSILON
	    || tw->bounds[0][1] > maxs[1] + RADIUS_EPSILON
	    || tw->bounds[0][2] > maxs[2] + RADIUS_EPSILON
	    || tw->bounds[1][0] < mins[0] - RADIUS_EPSILON
	    || tw->bounds[1][1] < mins[1] - RADIUS_EPSILON
	    || tw->bounds[1][2] < mins[2] - RADIUS_EPSILON
	    )
	{
		return;
	}
	// top origin and bottom origin of each sphere at start and end of trace
	VectorAdd(tw->start, tw->sphere.offset, starttop);
	VectorSubtract(tw->start, tw->sphere.offset, startbottom);
	VectorAdd(tw->end, tw->sphere.offset, endtop);
	VectorSubtract(tw->end, tw->sphere.offset, endbottom);

	// calculate top and bottom of the capsule spheres to collide with
	VectorAdd(mins, maxs, offset);
	VectorScale(offset, 0.5f, offset);
	VectorSubtract(mins, offset, symetricSize[0]);
	VectorSubtract(maxs, offset, symetricSize[1]);

	halfwidth  = symetricSize[1][0];
	halfheight = symetricSize[1][2];
	radius     = (halfwidth > halfheight) ? halfheight : halfwidth;
	offs       = halfheight - radius;
	VectorCopy(offset, top);
	top[2] += offs;
	VectorCopy(offset, bottom);
	bottom[2] -= offs;
	// expand radius of spheres
	radius += tw->sphere.radius;
	// if there is horizontal movement
	if (tw->start[0] != tw->end[0] || tw->start[1] != tw->end[1])
	{
		// height of the expanded cylinder is the height of both cylinders minus the radius of both spheres
		float h = halfheight + tw->sphere.halfheight - radius;

		// if the cylinder has a height
		if (h > 0)
		{
			// test for collisions between the cylinders
			CM_TraceThroughVerticalCylinder(tw, offset, radius, h, tw->start, tw->end);
		}
	}
	// test for collision between the spheres
	CM_TraceThroughSphere(tw, top, radius, startbottom, endbottom);
	CM_TraceThroughSphere(tw, bottom, radius, starttop, endtop);
}

/**
 * @brief Bounding box vs. capsule collision
 * @param[in] tw
 * @param[in] model
 */
static void CM_TraceBoundingBoxThroughCapsule(traceWork_t *tw, clipHandle_t model)
{
	vec3_t       mins, maxs, offset, size[2];
	clipHandle_t h;
	cmodel_t     *cmod;
	int          i;

	// mins maxs of the capsule
	CM_ModelBounds(model, mins, maxs);

	// offset for capsule center
	VectorAdd(mins, maxs, offset);
	VectorScale(offset, 0.5f, offset);
	VectorSubtract(mins, offset, size[0]);
	VectorSubtract(maxs, offset, size[1]);
	VectorSubtract(tw->start, offset, tw->start);
	VectorSubtract(tw->end, offset, tw->end);

	// replace the bounding box with the capsule
	tw->sphere.use        = qtrue;
	tw->sphere.radius     = (size[1][0] > size[1][2]) ? size[1][2] : size[1][0];
	tw->sphere.halfheight = size[1][2];
	VectorSet(tw->sphere.offset, 0, 0, size[1][2] - tw->sphere.radius);

	// replace the capsule with the bounding box
	h = CM_TempBoxModel(tw->size[0], tw->size[1], qfalse);
	// calculate collision
	cmod = CM_ClipHandleToModel(h);
	CM_TraceThroughLeaf(tw, &cmod->leaf);
}
#endif
//=========================================================================================

/**
 * @brief Traverse all the contacted leafs from the start to the end position.
 *
 * @details If the trace is a point, they will be exactly in order, but for larger
 * trace volumes it is possible to hit something in a later leaf with
 * a smaller intercept fraction.
 *
 * @param[in] tw
 * @param[in] num
 * @param[in] p1f
 * @param[in] p2f
 * @param[in] p1
 * @param[in] p2
 */
static void CM_TraceThroughTree(traceWork_t *tw, int num, float p1f, float p2f, vec3_t p1, vec3_t p2)
{
	cNode_t  *node;
	cplane_t *plane;
	float    t1, t2, offset;
	float    frac, frac2;
	float    idist;
	vec3_t   mid;
	int      side;
	float    midf;

	if (tw->trace.fraction <= p1f)
	{
		return;     // already hit something nearer
	}

	// if < 0, we are in a leaf node
	if (num < 0)
	{
		CM_TraceThroughLeaf(tw, &cm.leafs[-1 - num]);
		return;
	}

	// find the point distances to the seperating plane
	// and the offset for the size of the box

	node  = cm.nodes + num;
	plane = node->plane;

	// adjust the plane distance apropriately for mins/maxs
	if (plane->type < 3)
	{
		t1     = p1[plane->type] - plane->dist;
		t2     = p2[plane->type] - plane->dist;
		offset = tw->extents[plane->type];
	}
	else
	{
		Dot(plane->normal, p1, t1);
		t1 -= plane->dist;
		Dot(plane->normal, p2, t2);
		t2 -= plane->dist;
		if (tw->isPoint)
		{
			offset = 0;
		}
		else
		{
			/*
			// an axial brush right behind a slanted bsp plane
			// will poke through when expanded, so adjust
			// by sqrt(3)
			offset = Q_fabs(tw->extents[0]*plane->normal[0]) +
			    Q_fabs(tw->extents[1]*plane->normal[1]) +
			    Q_fabs(tw->extents[2]*plane->normal[2]);

			offset *= 2;
			offset = tw->maxOffset;
			*/

			offset = tw->maxOffset;
		}
	}

	// see which sides we need to consider
	if (t1 >= offset + 1.0f && t2 >= offset + 1.0f)
	{
		CM_TraceThroughTree(tw, node->children[0], p1f, p2f, p1, p2);
		return;
	}
	if (t1 < -offset - 1.0f && t2 < -offset - 1.0f)
	{
		CM_TraceThroughTree(tw, node->children[1], p1f, p2f, p1, p2);
		return;
	}

	// put the crosspoint SURFACE_CLIP_EPSILON pixels on the near side
	if (t1 < t2)
	{
		idist = rcp(t1 - t2);
		side  = 1;
		frac2 = (t1 + offset + SURFACE_CLIP_EPSILON) * idist;
		frac  = (t1 - offset + SURFACE_CLIP_EPSILON) * idist;
	}
	else if (t1 > t2)
	{
		idist = rcp(t1 - t2);
		side  = 0;
		frac2 = (t1 - offset - SURFACE_CLIP_EPSILON) * idist;
		frac  = (t1 + offset + SURFACE_CLIP_EPSILON) * idist;
	}
	else
	{
		side  = 0;
		frac  = 1.0f;
		frac2 = 0.0f;
	}

	// move up to the node
	if (frac < 0.0f)
	{
		frac = 0.0f;
	}
	if (frac > 1.0f)
	{
		frac = 1.0f;
	}

	midf = p1f + (p2f - p1f) * frac;

	VectorSubtract(p2, p1, mid);
	VectorScale(mid, frac, mid);
	VectorAdd(mid, p1, mid);

	CM_TraceThroughTree(tw, node->children[side], p1f, midf, p1, mid);

	// go past the node
	if (frac2 < 0.0f)
	{
		frac2 = 0.0f;
	}
	if (frac2 > 1.0f)
	{
		frac2 = 1.0f;
	}

	midf = p1f + (p2f - p1f) * frac2;

	VectorSubtract(p2, p1, mid);
	VectorScale(mid, frac2, mid);
	VectorAdd(mid, p1, mid);

	CM_TraceThroughTree(tw, node->children[side ^ 1], midf, p2f, mid, p2);
}

//======================================================================

/**
 * @brief CM_Trace
 * @param[out] results
 * @param[in] start
 * @param[in] end
 * @param[in] mins
 * @param[in] maxs
 * @param[in] model
 * @param[in] origin
 * @param[in] brushmask
 * @param[in] capsule
 * @param[in] sphere
 */
static void CM_Trace(trace_t *results, const vec3_t start, const vec3_t end,
                     const vec3_t mins, const vec3_t maxs,
                     clipHandle_t model, const vec3_t origin, int brushmask, qboolean capsule, sphere_t *sphere)
{
	int         i;
	traceWork_t tw;
	vec3_t      offset;
	cmodel_t    *cmod;
	qboolean    positionTest;

	cmod = CM_ClipHandleToModel(model);

	cm.checkcount++;        // for multi-check avoidance

	c_traces++;             // for statistics, may be zeroed

	// fill in a default trace
	Com_Memset(&tw, 0, sizeof(tw));
	tw.trace.fraction = 1.0f;   // assume it goes the entire distance until shown otherwise
	VectorCopy(origin, tw.modelOrigin);

	if (!cm.numNodes)
	{
		*results = tw.trace;

		return; // map not loaded, shouldn't happen
	}

	// allow NULL to be passed in for 0,0,0
	if (!mins)
	{
		mins = vec3_origin;
	}
	if (!maxs)
	{
		maxs = vec3_origin;
	}

	// set basic parms
	tw.contents = brushmask;

	// adjust so that mins and maxs are always symetric, which
	// avoids some complications with plane expanding of rotated
	// bmodels
	VectorAdd(mins, maxs, offset);
	VectorScale(offset, 0.5f, offset);
	VectorSubtract(mins, offset, tw.size[0]);
	VectorSubtract(maxs, offset, tw.size[1]);
	VectorAdd(start, offset, tw.start);
	VectorAdd(end, offset, tw.end);

	// if a sphere is already specified
	if (sphere)
	{
		tw.sphere = *sphere;
	}
	else
	{
		tw.sphere.use        = capsule;
		tw.sphere.radius     = (tw.size[1][0] > tw.size[1][2]) ? tw.size[1][2] : tw.size[1][0];
		tw.sphere.halfheight = tw.size[1][2];
		VectorSet(tw.sphere.offset, 0, 0, tw.size[1][2] - tw.sphere.radius);
	}

	positionTest = VectorCompare(start, end);

	tw.maxOffset = tw.size[1][0] + tw.size[1][1] + tw.size[1][2];

	// tw.offsets[signbits] = vector to apropriate corner from origin
	tw.offsets[0][0] = tw.size[0][0];
	tw.offsets[0][1] = tw.size[0][1];
	tw.offsets[0][2] = tw.size[0][2];

	tw.offsets[1][0] = tw.size[1][0];
	tw.offsets[1][1] = tw.size[0][1];
	tw.offsets[1][2] = tw.size[0][2];

	tw.offsets[2][0] = tw.size[0][0];
	tw.offsets[2][1] = tw.size[1][1];
	tw.offsets[2][2] = tw.size[0][2];

	tw.offsets[3][0] = tw.size[1][0];
	tw.offsets[3][1] = tw.size[1][1];
	tw.offsets[3][2] = tw.size[0][2];

	tw.offsets[4][0] = tw.size[0][0];
	tw.offsets[4][1] = tw.size[0][1];
	tw.offsets[4][2] = tw.size[1][2];

	tw.offsets[5][0] = tw.size[1][0];
	tw.offsets[5][1] = tw.size[0][1];
	tw.offsets[5][2] = tw.size[1][2];

	tw.offsets[6][0] = tw.size[0][0];
	tw.offsets[6][1] = tw.size[1][1];
	tw.offsets[6][2] = tw.size[1][2];

	tw.offsets[7][0] = tw.size[1][0];
	tw.offsets[7][1] = tw.size[1][1];
	tw.offsets[7][2] = tw.size[1][2];

	// check for point special case
	if (tw.size[0][0] == 0.0f && tw.size[0][1] == 0.0f && tw.size[0][2] == 0.0f)
	{
		tw.isPoint = qtrue;
		VectorClear(tw.extents);
	}
	else
	{
		tw.isPoint    = qfalse;
		VectorCopy(tw.size[1], tw.extents);
	}

	if (positionTest)
	{
		CM_CalcTraceBounds(&tw, qfalse);
	}
	else
	{
		vec3_t dir;

		VectorSubtract(tw.end, tw.start, dir);
		VectorCopy(dir, tw.dir);
		VectorNormalizeOnly(dir);
		MakeNormalVectors(dir, tw.tracePlane1.normal, tw.tracePlane2.normal);
		Dot(tw.tracePlane1.normal, tw.start, tw.tracePlane1.dist);
		Dot(tw.tracePlane2.normal, tw.start, tw.tracePlane2.dist);
		if (tw.isPoint)
		{
			tw.traceDist1 = tw.traceDist2 = 1.0f;
		}
		else
		{
			float dist, dot;

			tw.traceDist1 = tw.traceDist2 = 0.0f;
			for (i = 0; i < 8; i++)
			{
				Dot(tw.tracePlane1.normal, tw.offsets[i], dot);
				dist = Q_fabs(dot - tw.tracePlane1.dist);
				if (dist > tw.traceDist1)
				{
					tw.traceDist1 = dist;
				}
				Dot(tw.tracePlane2.normal, tw.offsets[i], dot);
				dist = Q_fabs(dot - tw.tracePlane2.dist);
				if (dist > tw.traceDist2)
				{
					tw.traceDist2 = dist;
				}
			}
			// expand for epsilon
			tw.traceDist1 += 1.0f;
			tw.traceDist2 += 1.0f;
		}

		CM_CalcTraceBounds(&tw, qtrue);
	}

	// check for position test special case
	if (positionTest)
	{
		if (model)
		{
#ifdef ALWAYS_BBOX_VS_BBOX
			if (model == BOX_MODEL_HANDLE || model == CAPSULE_MODEL_HANDLE)
			{
				tw.sphere.use = qfalse;
				CM_TestInLeaf(&tw, &cmod->leaf);
			}
			else
#elif defined(ALWAYS_CAPSULE_VS_CAPSULE)
			if (model == BOX_MODEL_HANDLE || model == CAPSULE_MODEL_HANDLE)
			{
				CM_TestCapsuleInCapsule(&tw, model);
			}
			else
#else // this is dead code when ALWAYS_BBOX_VS_BBOX or ALWAYS_CAPSULE_VS_CAPSULE are active
			if (model == CAPSULE_MODEL_HANDLE)
			{
				if (tw.sphere.use)
				{
					CM_TestCapsuleInCapsule(&tw, model);
				}
				else
				{
					CM_TestBoundingBoxInCapsule(&tw, model);
				}
			}
			else
#endif
			{
				CM_TestInLeaf(&tw, &cmod->leaf);
			}
		}
		else
		{
			CM_PositionTest(&tw);
		}
	}
	else
	{
		// general sweeping through world
		if (model)
		{
#ifdef ALWAYS_BBOX_VS_BBOX
			if (model == BOX_MODEL_HANDLE || model == CAPSULE_MODEL_HANDLE)
			{
				tw.sphere.use = qfalse;
				CM_TraceThroughLeaf(&tw, &cmod->leaf);
			}
			else
#elif defined(ALWAYS_CAPSULE_VS_CAPSULE)
			if (model == BOX_MODEL_HANDLE || model == CAPSULE_MODEL_HANDLE)
			{
				CM_TraceCapsuleThroughCapsule(&tw, model);
			}
			else
#else // this is dead code when ALWAYS_BBOX_VS_BBOX or ALWAYS_CAPSULE_VS_CAPSULE are active
			if (model == CAPSULE_MODEL_HANDLE)
			{
				if (tw.sphere.use)
				{
					CM_TraceCapsuleThroughCapsule(&tw, model);
				}
				else
				{
					CM_TraceBoundingBoxThroughCapsule(&tw, model);
				}
			}
			else
#endif
			{
				CM_TraceThroughLeaf(&tw, &cmod->leaf);
			}
		}
		else
		{
			CM_TraceThroughTree(&tw, 0, 0, 1, tw.start, tw.end);
		}
	}

	// generate endpos from the original, unmodified start/end
	if (tw.trace.fraction == 1.f)
	{
		VectorCopy(end, tw.trace.endpos);
	}
	else
	{
		VectorSubtract(end, start, tw.trace.endpos);
		VectorScale(tw.trace.endpos, tw.trace.fraction, tw.trace.endpos);
		VectorAdd(tw.trace.endpos, start, tw.trace.endpos);
	}

	*results = tw.trace;
}

/**
 * @brief CM_BoxTrace
 * @param[out] results
 * @param[in] start
 * @param[in] end
 * @param[in] mins
 * @param[in] maxs
 * @param[in] model
 * @param[in] brushmask
 * @param[in] capsule
 */
void CM_BoxTrace(trace_t *results, const vec3_t start, const vec3_t end,
                 const vec3_t mins, const vec3_t maxs,
                 clipHandle_t model, int brushmask, qboolean capsule)
{
	CM_Trace(results, start, end, mins, maxs, model, vec3_origin, brushmask, capsule, NULL);
}

/**
 * @brief Handles offseting and rotation of the end points for moving and
 * rotating entities
 * @param[out] results
 * @param[in] start
 * @param[in] end
 * @param[in] mins
 * @param[in] maxs
 * @param[in] model
 * @param[in] brushmask
 * @param[in] origin
 * @param[in] angles
 * @param[in] capsule
 */
void CM_TransformedBoxTrace(trace_t *results, const vec3_t start, const vec3_t end,
                            const vec3_t mins, const vec3_t maxs,
                            clipHandle_t model, int brushmask,
                            const vec3_t origin, const vec3_t angles, qboolean capsule)
{
	trace_t  trace;
	vec3_t   start_l, end_l, v;
	qboolean rotated;
	vec3_t   offset;
	vec3_t   symetricSize[2];
	vec3_t   matrix[3], transpose[3];
	float    halfwidth;
	float    halfheight;
	float    t;
	sphere_t sphere;

	if (!mins)
	{
		mins = vec3_origin;
	}
	if (!maxs)
	{
		maxs = vec3_origin;
	}

	// adjust so that mins and maxs are always symetric, which
	// avoids some complications with plane expanding of rotated bmodels
	VectorAdd(mins, maxs, offset);
	VectorScale(offset, 0.5f, offset);
	VectorSubtract(mins, offset, symetricSize[0]);
	VectorSubtract(maxs, offset, symetricSize[1]);
	VectorAdd(start, offset, start_l);
	VectorAdd(end, offset, end_l);

	// subtract origin offset
	VectorSubtract(start_l, origin, start_l);
	VectorSubtract(end_l, origin, end_l);

	// rotate start and end into the models frame of reference
	if (model != BOX_MODEL_HANDLE &&
	    (angles[0] != 0.f || angles[1] != 0.f || angles[2] != 0.f))
	{
		rotated = qtrue;
	}
	else
	{
		rotated = qfalse;
	}

	halfwidth  = symetricSize[1][0];
	halfheight = symetricSize[1][2];

	sphere.use        = capsule;
	sphere.radius     = (halfwidth > halfheight) ? halfheight : halfwidth;
	sphere.halfheight = halfheight;
	t                 = halfheight - sphere.radius;

	if (rotated)
	{
		// rotation on trace line (start-end) instead of rotating the bmodel
		// NOTE: This is still incorrect for bounding boxes because the actual bounding
		//       box that is swept through the model is not rotated. We cannot rotate
		//       the bounding box or the bmodel because that would make all the brush
		//       bevels invalid.
		//       However this is correct for capsules since a capsule itself is rotated too.
		CreateRotationMatrix(angles, matrix);
		VectorRotate(start_l, matrix, start_l);
		VectorRotate(end_l, matrix, end_l);
		// rotated sphere offset for capsule
		sphere.offset[0] = matrix[0][2] * t;
		sphere.offset[1] = -matrix[1][2] * t;
		sphere.offset[2] = matrix[2][2] * t;
	}
	else
	{
		VectorSet(sphere.offset, 0, 0, t);
	}

	// sweep the box through the model
	CM_Trace(&trace, start_l, end_l, symetricSize[0], symetricSize[1], model, origin, brushmask, capsule, &sphere);

	// if the bmodel was rotated and there was a collision
	if (rotated && trace.fraction != 1.0f)
	{
		// rotation of bmodel collision plane
		MatrixTranspose(matrix, transpose);
		VectorRotate(trace.plane.normal, transpose, trace.plane.normal);
	}

	// re-calculate the end position of the trace because the trace.endpos
	// calculated by CM_Trace could be rotated and have an offset
	VectorSubtract(end, start, v);
	VectorScale(v, trace.fraction, v);
	VectorAdd(v, start, trace.endpos);

	*results = trace;
}
