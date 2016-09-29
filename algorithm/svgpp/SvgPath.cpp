#include "GL\glew.h"
#include "SvgPath.h"
#include "SvgAttribute.h"
#include "SvgGroup.h"
namespace svg
{
#undef min
#undef max
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
		m_gl_path_id = glGenPathsNV(1);
	}

	SvgPath::~SvgPath()
	{
		if (m_gl_path_id)
			glDeletePathsNV(m_gl_path_id, 1);
	}

	void SvgPath::render()
	{
		assert(m_gl_path_id);

		bool ancestorSelected = false;
		if (ancestorAfterRoot())
			ancestorSelected = ancestorAfterRoot()->isSelected();

		glColor3fv(attribute()->m_color.ptr());
		if (isHighlighted() || isSelected() || ancestorSelected)
			glColor3f(0, 0, 1);
		if (isSelected())
			renderBounds(false);

		if (m_invalid)
		{
			glPathCommandsNV(m_gl_path_id,
				GLsizei(m_cmds.size()), &m_cmds[0],
				GLsizei(m_coords.size()), GL_FLOAT, &m_coords[0]);
			m_invalid = false;
		}// end if invalid

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

		glStencilStrokePathNV(m_gl_path_id, 1, ~0);
		glCoverStrokePathNV(m_gl_path_id, GL_BOUNDING_BOX_NV);	

		// render selected or highlighted
		if (isHighlighted() || isSelected() || ancestorSelected)
		{
			float sz = m_pathStyle.stroke_width / 5;
			if (isHighlighted() && (isSelected() || ancestorSelected))
				sz *= 2;
			// render control points
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_STENCIL_TEST);
			glBegin(GL_LINES);
			for (int i = 0; i < (int)m_segmentPos.size()-1; i++)
			{
				int bg = m_segmentPos[i];
				int ed = m_segmentPos[i + 1];
				for (int j = bg; j < ed-1; j += 2)
				{
					ldp::Float2 c(m_coords[j], m_coords[j+1]);
					glVertex2f(c[0] - sz, c[1] - sz);
					glVertex2f(c[0] + sz, c[1] - sz);

					glVertex2f(c[0] - sz, c[1] - sz);
					glVertex2f(c[0] - sz, c[1] + sz);

					glVertex2f(c[0] + sz, c[1] + sz);
					glVertex2f(c[0] + sz, c[1] - sz);

					glVertex2f(c[0] + sz, c[1] + sz);
					glVertex2f(c[0] - sz, c[1] + sz);
				}
			}
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(1, 1, 1);
			for (int i = 0; i < (int)m_segmentPos.size() - 1; i++)
			{
				int bg = m_segmentPos[i];
				int ed = m_segmentPos[i + 1];
				for (int j = bg; j < ed - 1; j += 2)
				{
					ldp::Float2 c(m_coords[j], m_coords[j + 1]);
					glVertex2f(c[0] - sz, c[1] - sz);
					glVertex2f(c[0] + sz, c[1] - sz);
					glVertex2f(c[0] + sz, c[1] + sz);
					glVertex2f(c[0] - sz, c[1] + sz);
				}
			}
			glEnd();
			glPopAttrib();
		}
	}

	void SvgPath::renderId()
	{
		assert(m_gl_path_id);
		glColor4fv(color_from_index(m_id).ptr());

		if (m_invalid)
		{
			glPathCommandsNV(m_gl_path_id,
				GLsizei(m_cmds.size()), &m_cmds[0],
				GLsizei(m_coords.size()), GL_FLOAT, &m_coords[0]);
			m_invalid = false;
		}// end if invalid

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

		glStencilStrokePathNV(m_gl_path_id, 1, ~0);
		glCoverStrokePathNV(m_gl_path_id, GL_BOUNDING_BOX_NV);
	}
}