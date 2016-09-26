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
	private:

	};
}