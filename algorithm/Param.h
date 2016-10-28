#pragma once
#include "ldp_basic_mat.h"
#include "util.h"
class CClothParam;
class Param
{
public:
	enum ClothThickness
	{
		Thick,
		Medium,
		Thin,
		VeryThin
	};
public:
	Param();
	~Param();
	void logToStamp(ldp::TimeStamp& st);

	void changeClothThickness(ClothThickness ct);
public:
	std::shared_ptr<CClothParam> m_clothParam;
	double m_smoothPolyThre;
};
