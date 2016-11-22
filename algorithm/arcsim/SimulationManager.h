#pragma once

#include "adaptiveCloth\simulation.hpp"
namespace svg
{
	class SvgManager;
}
namespace arcsim
{
	class SimulationManager
	{
	public:
		void init(const svg::SvgManager& svgManager, std::string bodyMeshFileName);

		Simulation& getSimulator() { return m_sim; }
		const Simulation& getSimulator()const { return m_sim; }
	private:
		Simulation m_sim;
	};

}