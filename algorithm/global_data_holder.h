#pragma once

#include "ldputil.h"
#include "Param.h"
#include <map>
namespace svg
{
class SvgManager;
};
namespace arcsim
{
	struct Simulation;
};
class CAnalysis2D_Cloth_Static;
class CDesigner2D_Cloth;
class FreeFormDeform;
class GlobalDataHolder
{
public:
	void init();

	void generateClothDebug();

	void svgToCloth();

	void svgToSim();
public:
	std::shared_ptr<svg::SvgManager> m_svgManager;
	std::shared_ptr<CAnalysis2D_Cloth_Static> m_clothManger;
	std::shared_ptr<FreeFormDeform> m_svgDefomer;
	std::shared_ptr<CDesigner2D_Cloth> m_clothUiListener;
	std::map<unsigned int, int> m_clothLoopId2svgIdMap;
	std::map<int, unsigned int> m_svgId2clothLoopIdMap;
	Param m_param;

	std::shared_ptr<arcsim::Simulation> m_simulator;
};

extern GlobalDataHolder g_dataholder;