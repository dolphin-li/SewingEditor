#pragma once

class Camera;
class SvgRenderer
{
public:
	SvgRenderer();
	~SvgRenderer();

	void init(Camera* cam);
	void render();
private:
	Camera* m_renderCam;
};