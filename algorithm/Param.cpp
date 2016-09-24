#include "Param.h"
#include "ldp_basic_mat.h"

Param::Param()
{

}

Param::~Param()
{
}

void Param::logToStamp(ldp::TimeStamp& st)
{
	st.Prefix("param");
}