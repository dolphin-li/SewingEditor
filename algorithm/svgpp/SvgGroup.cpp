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

	void SvgGroup::render(PathUnitShapes shapeToRender)
	{
		if (ancestorAfterRoot() == this && isSelected())
			renderBounds(false);

		for (size_t i = 0; i < m_children.size(); i++)
			m_children[i]->render(shapeToRender);
	}

	void SvgGroup::renderId(PathUnitShapes shapeToRender)
	{
		for (size_t i = 0; i < m_children.size(); i++)
			m_children[i]->renderId(shapeToRender);
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

	void SvgGroup::copyTo(SvgAbstractObject* obj)const
	{
		SvgAbstractObject::copyTo(obj);
		if (obj->objectType() == SvgAbstractObject::Group)
		{
			auto g = (SvgGroup*)obj;
			g->m_children = m_children;
		}
	}

	std::shared_ptr<SvgAbstractObject> SvgGroup::clone()const
	{
		std::shared_ptr<SvgAbstractObject> g(new SvgGroup());
		auto gptr = (SvgGroup*)g.get();
		copyTo(gptr);
		gptr->m_children.clear();
		for (auto c : m_children)
		{
			auto newC = c->clone();
			newC->setParent(gptr);
			gptr->m_children.push_back(newC);
		}
		return g;
	}
}