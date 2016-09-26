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
	public:
		std::vector<GLubyte> m_cmds;
		std::vector<int> m_segmentPos;
		std::vector<GLfloat> m_coords;
		GLuint m_gl_path_id;
		GLenum m_gl_fill_rull;
		PathStyle m_pathStyle;
	};
}