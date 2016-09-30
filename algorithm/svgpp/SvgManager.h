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
			SlectionUnionInverse,
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
		void selectShapeByIndex(const std::set<int>& ids, SelectOp op = SelectThis);

		void selectPathBySimilarSelectedWidth();
		void selectPathByWidths(const std::set<float>& widths);

		// given an index, find the largest group that contains this object.
		void selectGroupByIndex(int id, SelectOp op = SelectThis);
		void selectGroupByIndex(const std::set<int>& ids, SelectOp op = SelectThis);

		// undo highlight for lastid and do highlight for this id
		void highlightShapeByIndex(int lastId, int thisId);

		// group may fail when the selected have been in different groups
		bool groupSelected();
		void ungroupSelected();
		void removeSelected();
		void removeSingleNodeAndEmptyNode();

		// path operations
		void splitSelectedPath(); // split selected, non-closed paths
		bool mergeSelectedPath(); // merge will fail if the paths have been in different groups
	protected:
		void updateIndex(SvgAbstractObject* obj, int &idx);
		void removeSelected(SvgAbstractObject* obj);
		bool groupSelected_findCommonParent(std::shared_ptr<SvgAbstractObject> obj,
			std::shared_ptr<SvgAbstractObject> objParent,
			std::shared_ptr<SvgAbstractObject>& commonParent, 
			int& cnt);
		void ungroupSelected_collect(SvgAbstractObject* obj, std::set<SvgGroup*>& groups);
		void removeSingleNodeAndEmptyNode(std::shared_ptr<SvgAbstractObject>& obj);
		void splitPath(std::shared_ptr<SvgAbstractObject>& obj);
	private:
		Camera* m_renderCam;
		std::shared_ptr<SvgAbstractObject> m_rootGroup;
		std::map<int, SvgAbstractObject*> m_idxMap;
		std::set<SvgAbstractObject*> m_groups_for_selection;
	};
}
