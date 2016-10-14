#pragma once
#include "GL\glew.h"
#include "SvgPath.h"
#include "PathStyle.hpp"
namespace svg
{
	class SvgPolyPath : public SvgPath
	{
	public:
		SvgPolyPath();
		~SvgPolyPath();
		ObjectType objectType()const { return ObjectType::PolyPath; }

		virtual void render(PathUnitShapes shapeToRender = ShapeAll);
		virtual void renderId(PathUnitShapes shapeToRender = ShapeAll);
		virtual std::shared_ptr<SvgAbstractObject> clone(bool selectedOnly = false)const;
		virtual TiXmlElement* toXML(TiXmlNode* parent)const;
		virtual void copyTo(SvgAbstractObject* obj)const;

		bool isClosed()const;

		// given this polygon (closed or non-closed), we find all the corners
		void findCorners();

		int numCorners()const{ return (int)m_cornerPos.size(); }
		ldp::Float2 getCorner(int i)const{ return ldp::Float2(m_coords[m_cornerPos[i] * 2], 
			m_coords[m_cornerPos[i] * 2 + 1]); }
	protected:
		void renderSelection();
	protected:
		std::vector<int> m_cornerPos;
	};
}