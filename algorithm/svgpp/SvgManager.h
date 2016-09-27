#pragma once
#include <memory>
#include <map>
class Camera;
namespace svg
{
	class SvgGroup;
	class SvgAbstractObject;
	class SvgManager
	{
	public:
		enum SelectOp{
			SelectThis,
			SelectUnion,
			SelectAll,
			SelectNone,
			SelectInverse,
		};
	public:
		SvgManager();
		~SvgManager();

		void init(Camera* cam);
		void load(const char* svg_file);
		void render();
		void renderIndex();

		int width()const;
		int height()const;

		// should be called after adding/removing units or groups
		void updateIndex();

		// should be called after geometric changes
		void updateBound();

		// given an index, select shapes
		void selectShapeByIndex(int id, SelectOp op = SelectThis);

		// undo highlight for lastid and do highlight for this id
		void highlightShapeByIndex(int lastId, int thisId);
	protected:
		void updateIndex(SvgAbstractObject* obj, int &idx);
		void updateBound(SvgAbstractObject* obj);
		void selectShapeByIndex(SvgAbstractObject* obj, int id, SelectOp op);
	private:
		Camera* m_renderCam;
		std::shared_ptr<SvgGroup> m_rootGroup;
		std::map<int, SvgAbstractObject*> m_idxMap;
	};
}
