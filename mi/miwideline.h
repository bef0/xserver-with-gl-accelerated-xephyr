/*

Copyright 1988, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/

/* Author:  Keith Packard, MIT X Consortium */

#include "mispans.h"
#include "mifpoly.h" /* for ICEIL */

/*
 * Polygon edge description for integer wide-line routines
 */

typedef struct _PolyEdge {
    int	    height;	/* number of scanlines to process */
    int	    x;		/* starting x coordinate */
    int	    stepx;	/* fixed integral dx */
    int	    signdx;	/* variable dx sign */
    int	    e;		/* initial error term */
    int	    dy;
    int	    dx;
} PolyEdgeRec, *PolyEdgePtr;

#define SQSECANT 108.856472512142 /* 1/sin^2(11/2) - miter limit constant */

/*
 * types for general polygon routines
 */

typedef struct _PolyVertex {
    double  x, y;
} PolyVertexRec, *PolyVertexPtr;

typedef struct _PolySlope {
    int	    dx, dy;
    double  k;	    /* x0 * dy - y0 * dx */
} PolySlopeRec, *PolySlopePtr;

/*
 * Line face description for caps/joins
 */

typedef struct _LineFace {
    double  xa, ya;
    int	    dx, dy;
    int	    x, y;
    double  k;
} LineFaceRec, *LineFacePtr;

/*
 * macros for polygon fillers
 */

#define MILINESETPIXEL(pDrawable, pGC, pixel, oldPixel) { \
    oldPixel = pGC->fgPixel; \
    if (pixel != oldPixel) { \
	DoChangeGC (pGC, GCForeground, (XID *) &pixel, FALSE); \
	ValidateGC (pDrawable, pGC); \
    } \
}
#define MILINERESETPIXEL(pDrawable, pGC, pixel, oldPixel) { \
    if (pixel != oldPixel) { \
	DoChangeGC (pGC, GCForeground, (XID *) &oldPixel, FALSE); \
	ValidateGC (pDrawable, pGC); \
    } \
}

extern _X_EXPORT void miRoundJoinClip(
    LineFacePtr /*pLeft*/,
    LineFacePtr /*pRight*/,
    PolyEdgePtr /*edge1*/,
    PolyEdgePtr /*edge2*/,
    int * /*y1*/,
    int * /*y2*/,
    Bool * /*left1*/,
    Bool * /*left2*/
);

extern _X_EXPORT int miRoundCapClip(
    LineFacePtr /*face*/,
    Bool /*isInt*/,
    PolyEdgePtr /*edge*/,
    Bool * /*leftEdge*/
);

extern _X_EXPORT int miPolyBuildEdge(double x0, double y0, double k, int dx, int dy,
				int xi, int yi, int left, PolyEdgePtr edge);
extern _X_EXPORT int miPolyBuildPoly(PolyVertexPtr vertices, PolySlopePtr slopes,
				int count, int xi, int yi, PolyEdgePtr left,
				PolyEdgePtr right, int *pnleft, int *pnright,
				int *h);

