#include "SvgClipPath.h"

namespace svg
{
	SvgClipPath::SvgClipPath()
	{
	}

	SvgClipPath::~SvgClipPath()
	{
	}

	void SvgClipPath::render(PathUnitShapes shape)
	{
		if (!isVisible(shape))
			return;
	}

	void SvgClipPath::renderId(PathUnitShapes shape)
	{
		if (!isVisible(shape))
			return;

	}

	std::shared_ptr<SvgAbstractObject> SvgClipPath::clone()const
	{
		return std::shared_ptr<SvgAbstractObject>(new SvgClipPath());
	}
}