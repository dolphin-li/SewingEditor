#pragma once

#include "glsl.h"
#include <string>
class CShaderManager
{
public:
	enum ShaderType
	{
		none,
		orennayar,
		glass,
		xray,
	};
	enum{ nShaders = 4};
public:
	CShaderManager();
	~CShaderManager();
	void create(const char* folder_of_shaders);
	void bind(ShaderType type);
	void unbind();
	void release();
	cwc::glShader* getCurShader();
	ShaderType getCurType()const{return m_type;}
protected:
	void bind_orennayar();
	void bind_xray();
	void bind_glass();
private:
	cwc::glShader* m_shader[nShaders];
	cwc::glShaderObject* m_vshader[nShaders];
	cwc::glShaderObject* m_fshader[nShaders];
	std::string m_dir;
	ShaderType m_type;
};