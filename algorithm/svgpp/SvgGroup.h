#pragma once

#include "SvgAbstractObject.h"
namespace svg
{
	class SvgGroup : public SvgAbstractObject
	{
	public:
		SvgGroup();
		~SvgGroup();
		ObjectType objectType()const { return ObjectType::Group; }

		virtual void render(PathUnitShapes shapeToRender = ShapeAll);
		virtual void renderId(PathUnitShapes shapeToRender = ShapeAll);
		virtual std::shared_ptr<SvgAbstractObject> clone(bool selectedOnly = false)const;
		virtual void toXML(TiXmlNode* parent)const;
		virtual void copyTo(SvgAbstractObject* obj)const;
		virtual bool hasSelectedChildren()const;

		virtual void updateBoundFromGeometry();
	public:
		std::vector<std::shared_ptr<SvgAbstractObject>> m_children;
	};
}