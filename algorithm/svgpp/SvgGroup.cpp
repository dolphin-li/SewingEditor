#include "GL\glew.h"
#include "SvgGroup.h"

namespace svg
{
#undef min
#undef max
	SvgGroup::SvgGroup()
	{
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
}