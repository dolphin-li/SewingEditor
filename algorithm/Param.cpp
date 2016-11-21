#include "Param.h"
#include "ldp_basic_mat.h"
Param::Param()
{
	m_smoothPolyThre = 0.2;
	m_coarse_mesh_ntri = 3000;
	m_detail_mesh_ntri = 20000;
}

Param::~Param()
{
}

void Param::logToStamp(ldp::TimeStamp& st)
{
	st.Prefix("param");
}