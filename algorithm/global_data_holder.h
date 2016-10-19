#pragma once

#include "util.h"
#include "Param.h"
#include <map>
namespace svg{
class SvgManager;
}
class CAnalysis2D_Cloth_Static;
class CDesigner2D_Cloth;
class GlobalDataHolder
{
public:
	void init();

	void generateClothDebug();

	void svgToCloth();
public:
	std::shared_ptr<svg::SvgManager> m_svgManager;
	std::shared_ptr<CAnalysis2D_Cloth_Static> m_clothManger;
	std::shared_ptr<CDesigner2D_Cloth> m_clothUiListener;
	std::map<unsigned int, int> m_clothLoopId2svgIdMap;
};

extern GlobalDataHolder g_dataholder;