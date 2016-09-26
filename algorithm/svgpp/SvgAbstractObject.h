#pragma once

#include <string>
#include <vector>
#include <memory>
#include "ldp_basic_mat.h"

namespace svg
{
	class SvgAttribute;
	class SvgGroup;
	class SvgAbstractObject
	{
	public:
		enum ObjectType
		{
			Path,
			Text,
			ClipPath,
			Group
		};
		enum
		{
			INDEX_BEGIN = 1
		};
	public:
		SvgAbstractObject();
		~SvgAbstractObject();

		static SvgAbstractObject* create(ObjectType type);

		virtual ObjectType objectType()const = 0;

		void setId(int id) { m_id = id; }
		int getId()const { return m_id; }

		SvgGroup* parent();
		const SvgGroup* parent()const;
		void setParent(SvgGroup* p);
		SvgGroup* root();
		const SvgGroup* root()const;

		ldp::Float4 getBound()const{ return m_bbox; }
		void setBound(ldp::Float4 b){ m_bbox = b; }
		ldp::Float4 unionBound(ldp::Float4 b)const;
		ldp::Float4 intersectBound(ldp::Float4 b)const;
		ldp::Float2 getOrigion()const{ return ldp::Float2(m_bbox[0], m_bbox[1]); }
		ldp::Float2 getSize()const { return ldp::Float2(width(), height()); }
		float width()const{ return m_bbox[1] - m_bbox[0]; }
		float height()const{ return m_bbox[3] - m_bbox[2]; }

		virtual void render() = 0;
		virtual void renderId() = 0;
	protected:
		int m_id;
		SvgGroup* m_parent;
		std::shared_ptr<SvgAttribute> m_attribute;
		ldp::Float4 m_bbox;
	};
}