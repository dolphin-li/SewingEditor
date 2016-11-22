#include "SimulationManager.h"

#include "SvgManager.h"
#include "SvgPolyPath.h"

#pragma comment(lib, "blas.lib")
#pragma comment(lib, "clapack.lib")
#pragma comment(lib, "libmetis.lib")
#pragma comment(lib, "libtaucs.lib")
#pragma comment(lib, "vcf2c.lib")

namespace arcsim
{
	SimulationManager::SimulationManager()
	{

	}

	void SimulationManager::clear()
	{
		m_sim.reset((Simulation*)nullptr);
	}

	void SimulationManager::init(const svg::SvgManager& svgManager, std::string bodyMeshFileName)
	{
		clear();
	}
}