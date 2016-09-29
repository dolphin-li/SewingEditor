#pragma once
#include <memory>
#include <map>
#include <set>
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

		// given an index, find the largest group that contains this object.
		void selectGroupByIndex(int id, SelectOp op = SelectThis);

		// undo highlight for lastid and do highlight for this id
		void highlightShapeByIndex(int lastId, int thisId);

		// group may fail when the selected have been in different groups
		bool groupSelected();
		void ungroupSelected();
		void removeSelected();
	protected:
		void updateIndex(SvgAbstractObject* obj, int &idx);
		void updateBound(SvgAbstractObject* obj);
		void removeSelected(SvgAbstractObject* obj);
		bool groupSelected_findCommonParent(std::shared_ptr<SvgAbstractObject> obj,
			std::shared_ptr<SvgAbstractObject> objParent,
			std::shared_ptr<SvgAbstractObject>& commonParent);
		void ungroupSelected_collect(SvgAbstractObject* obj, std::set<SvgGroup*>& groups);
		void removeSingleNodeAndEmptyNode(std::shared_ptr<SvgAbstractObject>& obj,
			std::shared_ptr<SvgAbstractObject> parent);
	private:
		Camera* m_renderCam;
		std::shared_ptr<SvgAbstractObject> m_rootGroup;
		std::map<int, SvgAbstractObject*> m_idxMap;
		std::set<SvgAbstractObject*> m_groups_for_selection;
	};
}
