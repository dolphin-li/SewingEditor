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
			for (auto& h : m_sim->handles)
			{
				delete h;
			}
			for (auto& c : m_sim->cloths)
			{
				delete_mesh(c.mesh);
				for (auto m : c.materials)
					delete m;
			}
			for (auto& m : m_sim->morphs)
			{
				for (auto& s : m.targets)
					delete_mesh(s);
			}
			for (auto& o : m_sim->obstacles)
			{
				delete_mesh(o.base_mesh);
				delete_mesh(o.curr_state_mesh);
			}
			for (auto& c : m_sim->cloth_meshes)
			{
				delete_mesh(*c);
				delete c;
			}
			for (auto& o : m_sim->obstacle_meshes)
			{
				delete_mesh(*o);
				delete o;
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