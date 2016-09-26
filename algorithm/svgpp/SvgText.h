#pragma once

#include "SvgAbstractObject.h"
namespace svg
{
	class SvgText : public SvgAbstractObject
	{
	public:
		SvgText();
		~SvgText();
		ObjectType objectType()const { return ObjectType::Text; }

		virtual void render();
		virtual void renderId();
	private:

	};
}