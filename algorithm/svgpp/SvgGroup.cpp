#include "SvgGroup.h"

namespace svg
{
	SvgGroup::SvgGroup()
	{
	}

	SvgGroup::~SvgGroup()
	{
	}

	void SvgGroup::render()
	{
		for (size_t i = 0; i < m_children.size(); i++)
			m_children[i]->render();
	}

	void SvgGroup::renderId()
	{
		for (size_t i = 0; i < m_children.size(); i++)
			m_children[i]->renderId();
	}
}