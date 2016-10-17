#pragma once

#include "util.h"
#include "Param.h"
namespace svg{
class SvgManager;
}
class GlobalDataHolder
{
public:
	void init();
public:
	std::shared_ptr<svg::SvgManager> m_svgManager;
};

extern GlobalDataHolder g_dataholder;