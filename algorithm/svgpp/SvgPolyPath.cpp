#include "GL\glew.h"
#include "SvgPolyPath.h"
#include "SvgAttribute.h"
#include "SvgGroup.h"
#include "kdtree\PointTree.h"
namespace svg
{
#undef min
#undef max

	const static float CORNER_COS_ANGLE = cos(30.f * ldp::PI_S / 180.f);

	SvgPolyPath::SvgPolyPath() :SvgPath()
	{
		
	}

	SvgPolyPath::SvgPolyPath(int id)
	{
		m_id = id;
	}

	SvgPolyPath::~SvgPolyPath()
	{
	}

	void SvgPolyPath::render()
	{
		assert(m_gl_path_res->id);

		if (m_cornerPos.size() == 0)
			findCorners();

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

	void SvgPolyPath::renderId()
	{
		assert(m_gl_path_res->id);		
		if (m_cornerPos.size() == 0)
			findCorners();
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
			float sz = m_pathStyle.stroke_width;
			if (isHighlighted() && (isSelected() || ancestorSelected))
				sz *= 2;
			// render control points
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_STENCIL_TEST);
			glLineWidth(2);
			glBegin(GL_LINES);
			int posBegin = 0;
			for (size_t i = 0; i < m_cornerPos.size(); i++)
			{
				ldp::Float2 c = getCorner(i);

				glVertex2f(c[0] - sz, c[1] - sz);
				glVertex2f(c[0] + sz, c[1] - sz);

				glVertex2f(c[0] - sz, c[1] - sz);
				glVertex2f(c[0] - sz, c[1] + sz);

				glVertex2f(c[0] + sz, c[1] + sz);
				glVertex2f(c[0] + sz, c[1] - sz);

				glVertex2f(c[0] + sz, c[1] + sz);
				glVertex2f(c[0] - sz, c[1] + sz);
			}
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(1, 1, 1);
			posBegin = 0;
			for (size_t i = 0; i < m_cornerPos.size(); i++)
			{
				ldp::Float2 c = getCorner(i);
				glVertex2f(c[0] - sz, c[1] - sz);
				glVertex2f(c[0] + sz, c[1] - sz);
				glVertex2f(c[0] + sz, c[1] + sz);
				glVertex2f(c[0] - sz, c[1] + sz);
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
			newTptr->m_cornerPos = m_cornerPos;
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
		ele->SetAttribute("ldp_poly", m_id);
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

	void SvgPolyPath::findCorners()
	{
		m_cornerPos.clear();
		if (m_cmds.size() == 0)
			return;
		if (!isClosed())
			m_cornerPos.push_back(0);
		for (int icmd = !isClosed(); icmd < (int)m_cmds.size() - 1; icmd++)
		{
			int lasti = (icmd - 1 + int(m_cmds.size())) % int(m_cmds.size());
			int nexti = icmd + 1;
			ldp::Float2 lastp(m_coords[lasti * 2], m_coords[lasti * 2 + 1]);
			ldp::Float2 p(m_coords[icmd * 2], m_coords[icmd * 2 + 1]);
			ldp::Float2 dir = (p - lastp).normalize();
			ldp::Float2 nextp(m_coords[nexti * 2], m_coords[nexti * 2 + 1]);
			ldp::Float2 ndir = (nextp - p).normalize();
			if (dir.dot(ndir) < CORNER_COS_ANGLE)
				m_cornerPos.push_back(icmd);
		} // icmd
		if (!isClosed())
			m_cornerPos.push_back((int)m_cmds.size() - 1);
	}
}