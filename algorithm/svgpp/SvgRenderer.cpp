#include "glew.h"
#include "freeglut.h"
#include "SvgRenderer.h"
#pragma comment(lib, "glew32.lib")

SvgRenderer::SvgRenderer()
{


}

SvgRenderer::~SvgRenderer()
{

}
int hasPathRendering = 0;
int hasDirectStateAccess = 0;
const char *programName = "nvpr_basic";
GLuint pathObj = 42;
int path_specification_mode = 0;
int filling = 1;
int stroking = 1;
int even_odd = 0;
void SvgRenderer::init()
{    
	/* The PostScript path syntax also supports compact and precise binary
	 encoding and includes PostScript-style circular arcs.

	 Or the path's command and coordinates can be specified explicitly: */
	static const GLubyte pathCommands[10] =
	{ GL_MOVE_TO_NV, GL_LINE_TO_NV, GL_LINE_TO_NV, GL_LINE_TO_NV,
	GL_LINE_TO_NV, GL_CLOSE_PATH_NV,
	'M', 'C', 'C', 'Z' };  // character aliases
	static const GLshort pathCoords[12][2] =
	{ { 100, 180 }, { 40, 10 }, { 190, 120 }, { 10, 120 }, { 160, 10 },
	{ 300, 300 }, { 100, 400 }, { 100, 200 }, { 300, 100 },
	{ 500, 200 }, { 500, 400 }, { 300, 300 } };

	glPathCommandsNV(pathObj, 10, pathCommands, 24, GL_SHORT, pathCoords);

	/* Before rendering, configure the path object with desirable path
	parameters for stroking.  Specify a wider 6.5-unit stroke and
	the round join style: */

	glPathParameteriNV(pathObj, GL_PATH_JOIN_STYLE_NV, GL_ROUND_NV);
	glPathParameterfNV(pathObj, GL_PATH_STROKE_WIDTH_NV, 6.5);


}

void SvgRenderer::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	/* Before rendering to a window with a stencil buffer, clear the stencil
	buffer to zero and the color buffer to black: */

	glClearStencil(0);
	glClearColor(0, 0, 0, 0);
	glStencilMask(~0);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	/* Use an orthographic path-to-clip-space transform to map the
	[0..500]x[0..400] range of the star's path coordinates to the [-1..1]
	clip space cube: */

	glMatrixLoadIdentityEXT(GL_PROJECTION);
	glMatrixLoadIdentityEXT(GL_MODELVIEW);
	glMatrixOrthoEXT(GL_MODELVIEW, 0, 500, 0, 400, -1, 1);

	if (filling) {

		/* Stencil the path: */

		glStencilFillPathNV(pathObj, GL_COUNT_UP_NV, 0x1F);

		/* The 0x1F mask means the counting uses modulo-32 arithmetic. In
		principle the star's path is simple enough (having a maximum winding
		number of 2) that modulo-4 arithmetic would be sufficient so the mask
		could be 0x3.  Or a mask of all 1's (~0) could be used to count with
		all available stencil bits.

		Now that the coverage of the star and the heart have been rasterized
		into the stencil buffer, cover the path with a non-zero fill style
		(indicated by the GL_NOTEQUAL stencil function with a zero reference
		value): */

		glEnable(GL_STENCIL_TEST);
		if (even_odd) {
			glStencilFunc(GL_NOTEQUAL, 0, 0x1);
		}
		else {
			glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
		}
		glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
		glColor3f(0, 1, 0); // green
		glCoverFillPathNV(pathObj, GL_BOUNDING_BOX_NV);

	}

	/* The result is a yellow star (with a filled center) to the left of
	a yellow heart.

	The GL_ZERO stencil operation ensures that any covered samples
	(meaning those with non-zero stencil values) are zero'ed when
	the path cover is rasterized. This allows subsequent paths to be
	rendered without clearing the stencil buffer again.

	A similar two-step rendering process can draw a white outline
	over the star and heart. */

	/* Now stencil the path's stroked coverage into the stencil buffer,
	setting the stencil to 0x1 for all stencil samples within the
	transformed path. */

	if (stroking) {

		glStencilStrokePathNV(pathObj, 0x1, ~0);

		/* Cover the path's stroked coverage (with a hull this time instead
		of a bounding box; the choice doesn't really matter here) while
		stencil testing that writes white to the color buffer and again
		zero the stencil buffer. */

		glColor3f(1, 1, 0); // yellow
		glCoverStrokePathNV(pathObj, GL_CONVEX_HULL_NV);

		/* In this example, constant color shading is used but the application
		can specify their own arbitrary shading and/or blending operations,
		whether with Cg compiled to fragment program assembly, GLSL, or
		fixed-function fragment processing.

		More complex path rendering is possible such as clipping one path to
		another arbitrary path.  This is because stencil testing (as well
		as depth testing, depth bound test, clip planes, and scissoring)
		can restrict path stenciling. */
	}
}