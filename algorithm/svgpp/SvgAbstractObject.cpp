#include "SvgAbstractObject.h"
#include "SvgAttribute.h"
#include "SvgPath.h"
#include "SvgText.h"
#include "SvgClipPath.h"
#include "SvgGroup.h"

#undef min
#undef max

namespace svg
{
	SvgAbstractObject::SvgAbstractObject()
	{
		m_id = -1;
		m_parent = nullptr;
		m_bbox = ldp::Float4(FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX);
		m_attribute = std::shared_ptr<SvgAttribute>(new SvgAttribute());
	}

	SvgAbstractObject::~SvgAbstractObject()
	{
	}

	SvgAbstractObject* SvgAbstractObject::create(ObjectType type)
	{
		switch (type)
		{
		case svg::SvgAbstractObject::Path:
			return (SvgAbstractObject*)new SvgPath();
		case svg::SvgAbstractObject::Text:
			return (SvgAbstractObject*)new SvgText();
		case svg::SvgAbstractObject::ClipPath:
			return (SvgAbstractObject*)new SvgClipPath();
		case svg::SvgAbstractObject::Group:
			return (SvgAbstractObject*)new SvgGroup();
		default:
			return nullptr;
			break;
		}
	}

	const SvgGroup* SvgAbstractObject::parent()const
	{
		return m_parent;
	}
	SvgGroup* SvgAbstractObject::parent()
	{
		return m_parent;
	}
	void SvgAbstractObject::setParent(SvgGroup* p)
	{
		m_parent = p;
	}
	const SvgGroup* SvgAbstractObject::root()const
	{
		const SvgGroup* p = parent();
		while (!p) p = p->parent();
		return p;
	}
	SvgGroup* SvgAbstractObject::root()
	{
		SvgGroup* p = parent();
		while (!p) p = p->parent();
		return p;
	}


	ldp::Float4 SvgAbstractObject::unionBound(ldp::Float4 b)const
	{
		ldp::Float4 out;
		out[0] = std::min(m_bbox[0], b[0]);
		out[1] = std::max(m_bbox[1], b[1]);
		out[2] = std::min(m_bbox[2], b[2]);
		out[3] = std::max(m_bbox[3], b[3]);
		return out;
	}

	ldp::Float4 SvgAbstractObject::intersectBound(ldp::Float4 b)const
	{
		ldp::Float4 out;
		out[0] = std::max(m_bbox[0], b[0]);
		out[1] = std::min(m_bbox[1], b[1]);
		out[2] = std::max(m_bbox[2], b[2]);
		out[3] = std::min(m_bbox[3], b[3]);
		return out;
	}

}