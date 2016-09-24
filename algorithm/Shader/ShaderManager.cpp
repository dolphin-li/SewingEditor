#include "ShaderManager.h"
CShaderManager::CShaderManager()
{
	for(int i=0; i<nShaders; i++)
	{
		m_shader[i] = NULL;
		m_vshader[i] = NULL;
		m_fshader[i] = NULL;
	}
	m_type = none;
}

CShaderManager::~CShaderManager()
{
	
}

void CShaderManager::release()
{
	for(int i=0; i<nShaders; i++)
	{
		if(m_vshader[i]) delete m_vshader[i];
		if(m_fshader[i]) delete m_fshader[i];
		if(m_shader[i]) delete m_shader[i];
	}
}

inline std::string fullfile(std::string path, std::string name)
{
	if (path == "")
		return name;
	if (path.back() != '/' && path.back() != '\\')
		path.append("/");
	return path + name;
}

void CShaderManager::create(const char* folder_of_shaders)
{
	release();

	printf("%s\n", gluErrorString(glGetError()));

	const static std::string names[nShaders] ={
		"none", "Oren-Nayar", "glass", "xray"
	};

	for(int i=1; i<nShaders; i++)
	{
		std::string fname = fullfile(folder_of_shaders, "../Shader/"+names[i]+".frag");
		std::string vname = fullfile(folder_of_shaders, "../Shader/"+names[i]+".vert");
		m_vshader[i] = new cwc::aVertexShader();
		m_fshader[i] = new cwc::aFragmentShader();
		if(m_fshader[i]->load(fname.c_str())|| m_vshader[i]->load(vname.c_str()))
		{
			printf("cound not load shader: %s \n or \n %s\n", fname.c_str(), vname.c_str());
			delete m_vshader[i];
			delete m_fshader[i];
			m_vshader[i] = NULL;
			m_fshader[i] = NULL;
			continue;
		}
		m_fshader[i]->compile();
		m_vshader[i]->compile();
		
		m_shader[i] = new cwc::glShader();
		m_shader[i]->addShader(m_fshader[i]);
		m_shader[i]->addShader(m_vshader[i]);
		m_shader[i]->link();
		printf("[shader log %d]: %s\n", i, m_shader[i]->getLinkerLog());
	}// i	
}

void CShaderManager::bind(ShaderType type)
{
	m_type = type;
	if(!m_shader[m_type])
		return;
	m_shader[m_type]->begin();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	switch(m_type)
	{
	default:
		printf("error: non-supported shader type");
		return;
	case orennayar:
		bind_orennayar();
		break;
	case xray:
		bind_xray();
		break;
	case glass:
		bind_glass();
		break;
	}
}

void CShaderManager::unbind()
{
	if(!m_shader[m_type])
		return;
	glPopAttrib();
	m_shader[m_type]->end();
}

void CShaderManager::bind_orennayar()
{
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColor4f(1,1,1,1);
	glDisable(GL_COLOR_MATERIAL);
	glPolygonOffset(1.f, 1.f);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_MULTISAMPLE);

	float light_amb[] = {0.05, 0.05, 0.05, 1};
	float light_dif[] = {1, 1, 1, 1};
	float mat_amb[] = {0.9, 0.8, 0.7, 1};
	float mat_dif[] = {0.9, 0.8, 0.7, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);

	m_shader[orennayar]->setUniform1f("roughnessSquared", 0);
	m_shader[orennayar]->setUniform4f("mat_ambient", mat_amb[0], mat_amb[1], mat_amb[2], mat_amb[3]);
	m_shader[orennayar]->setUniform4f("mat_diffuse", mat_dif[0], mat_dif[1], mat_dif[2], mat_dif[3]);
}

cwc::glShader* CShaderManager::getCurShader()
{
	return m_shader[m_type];
}

void CShaderManager::bind_xray()
{
	glClearColor(0.5,0.5,0.5,1);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(770, 770);
	glBlendEquation(32774);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
	glPolygonOffset(2.f, 2.f);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_MULTISAMPLE);
	
	m_shader[xray]->setUniform1f("edgefalloff", 1.f);
	m_shader[xray]->setUniform1f("intensity", 0.5f);
	m_shader[xray]->setUniform1f("ambient", 0.31f);
}

void CShaderManager::bind_glass()
{
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(770, 770);
	glBlendEquation(32774);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
	glPolygonOffset(2.f, 2.f);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_MULTISAMPLE);
	
	m_shader[glass]->setUniform4f("GlassColor", 0.9, 0.9, 1.0, 0.35);
	m_shader[glass]->setUniform3f("LightPosition", 0.f, 0.f, 5.f, 1.f);
	m_shader[glass]->setUniform4f("SpecularColor2", 0.1f, 0.08f, 0.05f, 1.f);
	m_shader[glass]->setUniform4f("SpecularColor2", 0.1f, 0.1f, 0.05f, 1.f);
	m_shader[glass]->setUniform1f("SpecularFactor1", 2.f);
	m_shader[glass]->setUniform1f("SpecularFactor2", 2.f);
}