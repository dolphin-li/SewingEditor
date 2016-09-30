#include "SvgClipPath.h"

namespace svg
{
	SvgClipPath::SvgClipPath()
	{
	}

	SvgClipPath::~SvgClipPath()
	{
	}

	void SvgClipPath::render()
	{

	}

	void SvgClipPath::renderId()
	{

	}

	std::shared_ptr<SvgAbstractObject> SvgClipPath::clone()const
	{
		return std::shared_ptr<SvgAbstractObject>(new SvgClipPath());
	}
}