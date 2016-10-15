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
		m_highlightedCorner_arrayId = -1;
		m_selectedCorner_arrayId = -1;
	}

	SvgPolyPath::SvgPolyPath(int id)
	{
		m_id = id;
	}

	SvgPolyPath::~SvgPolyPath()
	{
	}

	int SvgPolyPath::numId()const
	{ 
		int nVerts = (int)m_cornerPos.size();
		int nEdges = (int)m_cornerPos.size() - (!isClosed());
		return nVerts + nEdges + 1; // 1 for the path itself
	}

	void SvgPolyPath::setSelected(bool s, int idx)
	{
		SvgPath::setSelected(s, idx);
		m_selectedCorner_arrayId = idx - m_id;
		if (m_selectedCorner_arrayId < 0 || m_selectedCorner_arrayId >= numId())
			m_selectedCorner_arrayId = -1;
	}

	void SvgPolyPath::setHighlighted(bool s, int idx)
	{
		SvgPath::setHighlighted(s, idx);
		m_highlightedCorner_arrayId = idx - m_id;
		if (m_highlightedCorner_arrayId < 0 || m_highlightedCorner_arrayId >= numId())
			m_highlightedCorner_arrayId = -1;
	}

	void SvgPolyPath::render()
	{
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
		for (auto glRes : m_edgeGLIds)
		{
			glStencilStrokePathNV(glRes->id, 1, ~0);
			glCoverStrokePathNV(glRes->id, GL_BOUNDING_BOX_NV);
		}
		renderSelection();
	}

	void SvgPolyPath::renderId()
	{
		assert(m_gl_path_res->id);
		if (m_invalid)
		{
			cacheNvPaths();
			m_invalid = false;
		}// end if invalid

		configNvParams();
		int id = m_id;
		for (auto glRes : m_edgeGLIds)
		{
			glColor4fv(color_from_index(id++).ptr());
			glStencilStrokePathNV(glRes->id, 1, ~0);
			glCoverStrokePathNV(glRes->id, GL_BOUNDING_BOX_NV);
		}
	}

	void SvgPolyPath::renderSelection()
	{
		const int nVerts = (int)m_cornerPos.size();
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
			for (size_t i = 0; i < nVerts; i++)
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
			for (size_t i = 0; i < nVerts; i++)
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

	void SvgPolyPath::cacheNvPaths()
	{
		updateEdgeRenderData();
		for (size_t i = 0; i < m_edgeGLIds.size(); i++)
		{
			glPathCommandsNV(m_edgeGLIds[i]->id,
				GLsizei(m_edgeCmds[i].size()), m_edgeCmds[i].data(),
				GLsizei(m_edgeCoords[i].size()), GL_FLOAT, m_edgeCoords[i].data());
		}
	}

	void SvgPolyPath::configNvParams()
	{
		for (auto glRes : m_edgeGLIds)
		{
			glPathParameteriNV(glRes->id, GL_PATH_JOIN_STYLE_NV, lineJoinConverter(this));
			glPathParameteriNV(glRes->id, GL_PATH_END_CAPS_NV, lineCapConverter(this));
			glPathParameterfNV(glRes->id, GL_PATH_STROKE_WIDTH_NV, m_pathStyle.stroke_width);
			glPathParameterfNV(glRes->id, GL_PATH_MITER_LIMIT_NV, m_pathStyle.miter_limit);
			if (m_pathStyle.dash_array.size())
			{
				glPathDashArrayNV(glRes->id, GLsizei(m_pathStyle.dash_array.size()), &m_pathStyle.dash_array[0]);
				glPathParameteriNV(glRes->id, GL_PATH_DASH_CAPS_NV, lineCapConverter(this));
				glPathParameterfNV(glRes->id, GL_PATH_DASH_OFFSET_NV, m_pathStyle.dash_offset);
				glPathParameteriNV(glRes->id, GL_PATH_DASH_OFFSET_RESET_NV, m_pathStyle.dash_phase);
			}
			else
			{
				glPathDashArrayNV(glRes->id, 0, NULL);
			}
		}
	}

	void SvgPolyPath::copyTo(SvgAbstractObject* obj)const
	{
		SvgPath::copyTo(obj);
		if (obj->objectType() == SvgAbstractObject::PolyPath)
		{
			auto newTptr = (SvgPolyPath*)obj;
			newTptr->m_cornerPos = m_cornerPos;
			newTptr->m_highlightedCorner_arrayId = -1;
			newTptr->m_selectedCorner_arrayId = m_selectedCorner_arrayId;
			newTptr->m_edgeCmds = m_edgeCmds;
			newTptr->m_edgeCoords = m_edgeCoords;
			newTptr->m_edgeGLIds = m_edgeGLIds;
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
		const bool closed = isClosed();
		if (!closed)
			m_cornerPos.push_back(0);
		for (int icmd = !closed; icmd < (int)m_cmds.size() - 1; icmd++)
		{
			int lasti = (icmd - 1 + int(m_cmds.size())) % int(m_cmds.size());
			if (closed && lasti == (int)m_cmds.size() - 1)
				lasti--;
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

		invalid();
	}

	void SvgPolyPath::updateEdgeRenderData()
	{
		const bool closed = isClosed();
		const int nCorners = m_cornerPos.size();
		const int nEdges = nCorners - !closed;
		m_edgeCmds.clear();
		m_edgeCoords.clear();
		m_edgeCmds.resize(nEdges);
		m_edgeCoords.resize(nEdges);
		m_edgeGLIds.resize(nEdges);

		// gl resource
		for (int iedge = 0; iedge < nEdges; iedge++)
		if (m_edgeGLIds[iedge].get() == nullptr)
			m_edgeGLIds[iedge] = std::shared_ptr<GLPathResource>(new GLPathResource());

		// path data
		for (int iedge = 0; iedge < nEdges; iedge++)
		{
			auto& edgeCmd = m_edgeCmds[iedge];
			auto& edgeCoord = m_edgeCoords[iedge];
			int cb = m_cornerPos[iedge];
			int ce = iedge + 1 < nCorners ? m_cornerPos[iedge + 1] + 1 : m_cmds.size();
			edgeCmd.insert(edgeCmd.end(), m_cmds.begin() + cb, m_cmds.begin() + ce);
			edgeCoord.insert(edgeCoord.end(), m_coords.begin() + cb * 2, m_coords.begin() + ce * 2);
			if (iedge + 1 == nCorners){
				edgeCmd.insert(edgeCmd.end(), m_cmds.begin(), m_cmds.begin() + m_cornerPos[0] + 1);
				edgeCoord.insert(edgeCoord.end(), m_coords.begin(), m_coords.begin() + m_cornerPos[0] * 2 + 2);
			}
			edgeCmd[0] = GL_MOVE_TO_NV;
		} // end for iedge
	}
}