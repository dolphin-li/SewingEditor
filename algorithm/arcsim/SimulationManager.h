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
		SimulationManager();
		void init(const svg::SvgManager& svgManager, std::string configFileName);
		void clear();
		Simulation& getSimulator() { return *m_sim.get(); }
		const Simulation& getSimulator()const { return *m_sim.get(); }
	private:
		std::shared_ptr<Simulation> m_sim;
	};

}