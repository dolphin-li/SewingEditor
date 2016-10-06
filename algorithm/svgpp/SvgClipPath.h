#pragma once

#include "SvgAbstractObject.h"
namespace svg
{
	class SvgClipPath : public SvgAbstractObject
	{
	public:
		SvgClipPath();
		~SvgClipPath();
		ObjectType objectType()const { return ObjectType::ClipPath; }

		virtual void render(PathUnitShapes shape = ShapeAll);
		virtual void renderId(PathUnitShapes shape = ShapeAll);
		virtual std::shared_ptr<SvgAbstractObject> clone()const;
		virtual void copyTo(SvgAbstractObject* obj)const{}
		virtual void toXML(TiXmlNode* parent)const{}
	private:

	};
}