#include "GL\glew.h"
#include "SvgPath.h"
#include "SvgAttribute.h"
namespace svg
{
	static GLenum lineJoinConverter(const SvgPath *path)
	{
		switch (path->m_pathStyle.line_join) {
		default:
			assert(!"bad line_join");
		case PathStyle::MITER_TRUNCATE_JOIN:
			return GL_MITER_TRUNCATE_NV;
		case PathStyle::MITER_REVERT_JOIN:
			return GL_MITER_REVERT_NV;
		case PathStyle::ROUND_JOIN:
			return GL_ROUND_NV;
		case PathStyle::BEVEL_JOIN:
			return GL_BEVEL_NV;
		case PathStyle::NONE_JOIN:
			return GL_NONE;
		}
	}

	static GLenum lineCapConverter(const SvgPath *path)
	{
		switch (path->m_pathStyle.line_cap) {
		default:
			assert(!"bad line_cap");
		case PathStyle::BUTT_CAP:
			return GL_FLAT;
		case PathStyle::ROUND_CAP:
			return GL_ROUND_NV;
		case PathStyle::SQUARE_CAP:
			return GL_SQUARE_NV;
		case PathStyle::TRIANGLE_CAP:
			return GL_TRIANGULAR_NV;
		}
	}

	SvgPath::SvgPath() :SvgAbstractObject()
	{
		m_gl_path_id = 0;
	}

	SvgPath::~SvgPath()
	{
	}

	void SvgPath::render()
	{
		assert(m_gl_path_id);
		glPathCommandsNV(m_gl_path_id,
			GLsizei(m_cmds.size()), &m_cmds[0],
			GLsizei(m_coords.size()), GL_FLOAT, &m_coords[0]);

		glPathParameteriNV(m_gl_path_id, GL_PATH_JOIN_STYLE_NV, lineJoinConverter(this));
		glPathParameteriNV(m_gl_path_id, GL_PATH_END_CAPS_NV, lineCapConverter(this));
		glPathParameterfNV(m_gl_path_id, GL_PATH_STROKE_WIDTH_NV, m_pathStyle.stroke_width);
		glPathParameterfNV(m_gl_path_id, GL_PATH_MITER_LIMIT_NV, m_pathStyle.miter_limit);
		if (m_pathStyle.dash_array.size()) 
		{
			glPathDashArrayNV(m_gl_path_id, GLsizei(m_pathStyle.dash_array.size()), &m_pathStyle.dash_array[0]);
			glPathParameteriNV(m_gl_path_id, GL_PATH_DASH_CAPS_NV, lineCapConverter(this));
			glPathParameterfNV(m_gl_path_id, GL_PATH_DASH_OFFSET_NV, m_pathStyle.dash_offset);
			glPathParameteriNV(m_gl_path_id, GL_PATH_DASH_OFFSET_RESET_NV, m_pathStyle.dash_phase);
		}
		else 
		{
			glPathDashArrayNV(m_gl_path_id, 0, NULL);
		}

		glColor4fv(m_attribute->m_color.ptr());
		glStencilStrokePathNV(m_gl_path_id, 1, ~0);
		glCoverStrokePathNV(m_gl_path_id, GL_BOUNDING_BOX_NV);
	}

	void SvgPath::renderId()
	{

	}

}