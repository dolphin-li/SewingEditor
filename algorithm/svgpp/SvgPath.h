#pragma once
#include "GL\glew.h"
#include "SvgAbstractObject.h"
#include "PathStyle.hpp"
namespace svg
{
	class SvgPath : public SvgAbstractObject
	{
	public:
		SvgPath();
		~SvgPath();
		ObjectType objectType()const { return ObjectType::Path; }

		virtual void render();
		virtual void renderId();

		virtual void updateBoundFromGeometry();

		// split all segments to individual paths
		// if there is only one segment, we do not split and return an nullptr
		// else we return grouped splittings
		std::shared_ptr<SvgAbstractObject> splitToSegments()const;
	protected:
		void cacheNvPaths();
		void renderSelection();
		void configNvParams();
	public:
		std::vector<GLubyte> m_cmds;
		std::vector<int> m_segmentPos;
		std::vector<GLfloat> m_coords;
		GLuint m_gl_path_id;
		GLenum m_gl_fill_rull;
		PathStyle m_pathStyle;
	};
}