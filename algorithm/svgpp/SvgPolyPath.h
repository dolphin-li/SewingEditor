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
	protected:
		void renderSelection();
	};
}