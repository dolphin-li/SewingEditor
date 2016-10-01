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

		virtual void render();
		virtual void renderId();
		virtual std::shared_ptr<SvgAbstractObject> clone()const;
		virtual void copyTo(SvgAbstractObject* obj)const{}
	private:

	};
}