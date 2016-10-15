#pragma once
#include "GL\glew.h"
#include "SvgPath.h"
#include "PathStyle.hpp"
#include <set>
namespace svg
{
	class SvgPolyPath : public SvgPath
	{
	public:
		SvgPolyPath();
		SvgPolyPath(int id);
		virtual ~SvgPolyPath();
		ObjectType objectType()const { return ObjectType::PolyPath; }

		virtual void render();
		virtual void renderId();
		virtual std::shared_ptr<SvgAbstractObject> clone(bool selectedOnly = false)const;
		virtual TiXmlElement* toXML(TiXmlNode* parent)const;
		virtual void copyTo(SvgAbstractObject* obj)const;
		virtual int numId()const;
		virtual void setSelected(bool s, int idx = -1);
		virtual void setHighlighted(bool s, int idx = -1);

		// given this polygon (closed or non-closed), we find all the corners
		void findCorners();

		bool isClosed()const;
		int numCorners()const{ return (int)m_cornerPos.size(); }
		ldp::Float2 getCorner(int i)const{ return ldp::Float2(m_coords[m_cornerPos[i] * 2], 
			m_coords[m_cornerPos[i] * 2 + 1]); }
	protected:
		void cacheNvPaths();
		void renderSelection();
		void configNvParams();
		void updateEdgeRenderData();
	protected:
		std::vector<int> m_cornerPos;
		int m_selectedCorner_arrayId;
		int m_highlightedCorner_arrayId;

		// corner-splitted path data, for rendering
		std::vector<std::vector<GLubyte>> m_edgeCmds;
		std::vector<std::vector<float>> m_edgeCoords;
		std::vector<std::shared_ptr<GLPathResource>> m_edgeGLIds;
	};
}