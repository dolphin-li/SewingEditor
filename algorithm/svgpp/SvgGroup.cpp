#include "GL\glew.h"
#include "SvgGroup.h"

namespace svg
{
#undef min
#undef max
	SvgGroup::SvgGroup()
	{
		m_boxColor = ldp::Float3(0.8, 0.6, 0.4);
		m_boxStrokeWidth = 3;
	}

	SvgGroup::~SvgGroup()
	{
	}

	void SvgGroup::render()
	{
		if (ancestorAfterRoot() == this && isSelected())
			renderBounds(false);

		for (size_t i = 0; i < m_children.size(); i++)
			m_children[i]->render();
	}

	void SvgGroup::renderId()
	{
		for (size_t i = 0; i < m_children.size(); i++)
			m_children[i]->renderId();
		if (ancestorAfterRoot() == this && isSelected())
			renderBounds(true);
	}

	void SvgGroup::updateBoundFromGeometry()
	{
		ldp::Float4 box(FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX);

		for (auto child : m_children)
		{
			child->updateBoundFromGeometry();
			box = child->unionBound(box);
		}

		setBound(unionBound(box));
	}
}