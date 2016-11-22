#pragma once
#include "ldp_basic_mat.h"
#include "ldputil.h"
class CClothParam;
class Param
{
public:
	Param();
	~Param();
	void logToStamp(ldp::TimeStamp& st);
public:
	double m_smoothPolyThre;

	int m_coarse_mesh_ntri;
	int m_detail_mesh_ntri;
};
