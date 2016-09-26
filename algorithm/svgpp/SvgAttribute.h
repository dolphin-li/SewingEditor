#pragma once
#include "ldp_basic_mat.h"
namespace svg
{
	class SvgAttribute
	{
	public:
		SvgAttribute();
		~SvgAttribute();

	public:
		ldp::Float4 m_color;
	};
}