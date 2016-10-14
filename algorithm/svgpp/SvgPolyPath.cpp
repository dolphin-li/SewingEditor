#include "GL\glew.h"
#include "SvgPolyPath.h"
#include "SvgAttribute.h"
#include "SvgGroup.h"
#include "kdtree\PointTree.h"
namespace svg
{
#undef min
#undef max

	SvgPolyPath::SvgPolyPath() :SvgPath()
	{

	}

	SvgPolyPath::~SvgPolyPath()
	{
	}

	void SvgPolyPath::render(PathUnitShapes shapeToRender)
	{
		if (!isVisible(shapeToRender))
			return;

		assert(m_gl_path_res->id);

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
			cacheNvPaths();
			m_invalid = false;
		}// end if invalid

		configNvParams();
		glStencilStrokePathNV(m_gl_path_res->id, 1, ~0);
		glCoverStrokePathNV(m_gl_path_res->id, GL_BOUNDING_BOX_NV);	
		renderSelection();
	}

	void SvgPolyPath::renderId(PathUnitShapes shapeToRender)
	{
		if (!isVisible(shapeToRender))
			return;

		assert(m_gl_path_res->id);
		glColor4fv(color_from_index(m_id).ptr());

		if (m_invalid)
		{
			cacheNvPaths();
			m_invalid = false;
		}// end if invalid

		configNvParams();
		glStencilStrokePathNV(m_gl_path_res->id, 1, ~0);
		glCoverStrokePathNV(m_gl_path_res->id, GL_BOUNDING_BOX_NV);
	}

	void SvgPolyPath::renderSelection()
	{
		bool ancestorSelected = false;
		if (ancestorAfterRoot())
			ancestorSelected = ancestorAfterRoot()->isSelected();
		if (isHighlighted() || isSelected() || ancestorSelected)
		{
			float sz = m_pathStyle.stroke_width / 5;
			if (isHighlighted() && (isSelected() || ancestorSelected))
				sz *= 2;
			// render control points
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_STENCIL_TEST);
			glBegin(GL_LINES);
			int posBegin = 0;
			for (size_t i = 0; i < m_cmds.size(); i++)
			{
				int posEnd = posBegin + numCoords(m_cmds[i]);
				for (int j = posBegin; j < posEnd - 1; j += 2)
				{
					ldp::Float2 c(m_coords[j], m_coords[j + 1]);
					glVertex2f(c[0] - sz, c[1] - sz);
					glVertex2f(c[0] + sz, c[1] - sz);

					glVertex2f(c[0] - sz, c[1] - sz);
					glVertex2f(c[0] - sz, c[1] + sz);

					glVertex2f(c[0] + sz, c[1] + sz);
					glVertex2f(c[0] + sz, c[1] - sz);

					glVertex2f(c[0] + sz, c[1] + sz);
					glVertex2f(c[0] - sz, c[1] + sz);
				}
				posBegin = posEnd;
			}
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(1, 1, 1);
			posBegin = 0;
			for (size_t i = 0; i < m_cmds.size(); i++)
			{
				int posEnd = posBegin + numCoords(m_cmds[i]);
				for (int j = posBegin; j < posEnd - 1; j += 2)
				{
					ldp::Float2 c(m_coords[j], m_coords[j + 1]);
					glVertex2f(c[0] - sz, c[1] - sz);
					glVertex2f(c[0] + sz, c[1] - sz);
					glVertex2f(c[0] + sz, c[1] + sz);
					glVertex2f(c[0] - sz, c[1] + sz);
				}
				posBegin = posEnd;
			}
			glEnd();
			glPopAttrib();
		}
	}

	void SvgPolyPath::copyTo(SvgAbstractObject* obj)const
	{
		SvgPath::copyTo(obj);
		if (obj->objectType() == SvgAbstractObject::PolyPath)
		{
			auto newTptr = (SvgPolyPath*)obj;
		}
	}

	std::shared_ptr<SvgAbstractObject> SvgPolyPath::clone(bool selectedOnly)const
	{
		if (selectedOnly)
		{
			if (!(hasSelectedChildren() || isSelected()))
				throw std::exception("ERROR: SvgPolyPath::clone(), mis-called");
		}
		std::shared_ptr<SvgAbstractObject> newT(new SvgPolyPath());
		auto newTptr = (SvgPolyPath*)newT.get();

		copyTo(newTptr);

		return newT;
	}

	TiXmlElement* SvgPolyPath::toXML(TiXmlNode* parent)const
	{
		std::string cmdStr;
		char buffer[1014];
		std::vector<int> cmdPos;
		cmdPos.push_back(0);
		for (auto c : m_cmds)
			cmdPos.push_back(cmdPos.back() + numCoords(c));
		for (size_t i_cmd = 0; i_cmd < m_cmds.size(); i_cmd++)
		{
			auto str = svgCmd(m_cmds[i_cmd]);
			if (str == 'A')
				throw std::exception("arc export not implemented!");
			cmdStr += str;
			int bg = cmdPos[i_cmd], ed = cmdPos[i_cmd + 1];
			for (int i = bg; i < ed; i++)
			{
				sprintf_s(buffer, "%.2f", m_coords[i]);
				cmdStr += buffer;
				if (i < ed - 1)
					cmdStr += ',';
			}
		}

		TiXmlElement* ele = new TiXmlElement("path");
		parent->LinkEndChild(ele);
		ele->SetAttribute("fill", strokeFillMap(m_pathStyle.fill_rule));
		ele->SetAttribute("stroke", "#231F20");
		ele->SetDoubleAttribute("stroke-width", m_pathStyle.stroke_width);
		ele->SetAttribute("stroke-linecap", strokeLineCapMap(m_pathStyle.line_cap));
		ele->SetAttribute("stroke-linejoin", strokeLineJoinMap(m_pathStyle.line_join));
		ele->SetDoubleAttribute("stroke-miterlimit", m_pathStyle.miter_limit);
		ele->SetAttribute("d", cmdStr.c_str());
		return ele;
	}

	bool SvgPolyPath::isClosed()const
	{
		if (m_coords.size() < 4)
			return false;
		return m_coords[0] == m_coords[m_coords.size() - 2] && 
			m_coords[1] == m_coords[m_coords.size()-1];
	}
}