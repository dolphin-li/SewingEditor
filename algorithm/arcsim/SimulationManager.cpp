#include "SimulationManager.h"
#include "adaptiveCloth\conf.hpp"
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
		if (m_sim.get())
		{
			for (size_t i = 0; i < m_sim->handles.size(); i++)
				delete m_sim->handles[i];
			for (size_t i = 0; i < m_sim->cloth_meshes.size(); i++)
			{
				delete_mesh(*m_sim->cloth_meshes[i]);
				delete m_sim->cloth_meshes[i];
			}
			for (size_t i = 0; i < m_sim->obstacle_meshes.size(); i++)
			{
				delete_mesh(*m_sim->obstacle_meshes[i]);
				delete m_sim->obstacle_meshes[i];
			}
			m_sim.reset((Simulation*)nullptr);
		}
	}

	void SimulationManager::init(const svg::SvgManager& svgManager, std::string bodyMeshFileName)
	{
		clear();
		m_sim.reset(new Simulation);
		load_json(bodyMeshFileName, *m_sim.get());
	}
}