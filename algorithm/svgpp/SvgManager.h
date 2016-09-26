#pragma once
#include <memory>

class Camera;
namespace svg
{
	class SvgGroup;
	class SvgAbstractObject;
	class SvgManager
	{
	public:
		SvgManager();
		~SvgManager();

		void init(Camera* cam);
		void load(const char* svg_file);
		void render();

		int width()const;
		int height()const;

		// should be called after adding/removing units or groups
		void updateIndex();

		// should be called after geometric changes
		void updateBound();
	protected:
		void updateIndex(SvgAbstractObject* obj, int &idx);
		void updateBound(SvgAbstractObject* obj);
	private:
		Camera* m_renderCam;
		std::shared_ptr<SvgGroup> m_rootGroup;
	};
}
