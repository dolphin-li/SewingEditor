#include "global_data_holder.h"
#include "SvgManager.h"

GlobalDataHolder g_dataholder;

void GlobalDataHolder::init()
{
	m_svgManager.reset(new svg::SvgManager);
}