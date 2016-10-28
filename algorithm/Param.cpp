#include "Param.h"
#include "ldp_basic_mat.h"
#include "analysis2d_cloth_static.h"
#include "SC\cloth_utility.h"
Param::Param()
{
	m_clothParam.reset(new CClothParam);
	m_smoothPolyThre = 0.2;
}

Param::~Param()
{
}

void Param::changeClothThickness(ClothThickness ct)
{
	switch (ct)
	{
	case Param::Thick:
		m_clothParam->stiff_bend = 1.0e-2;
		m_clothParam->rho = 20;
		break;
	case Param::Medium:
		m_clothParam->stiff_bend = 1.0e-5;
		m_clothParam->rho = 0.02;
		break;
	case Param::Thin:
		m_clothParam->stiff_bend = 1.0e-10;
		m_clothParam->rho = 0.02;
		break;
	case Param::VeryThin:
		m_clothParam->stiff_bend = 0;
		m_clothParam->rho = 0.02;
		break;
	default:
		break;
	}
}

void Param::logToStamp(ldp::TimeStamp& st)
{
	st.Prefix("param");
}