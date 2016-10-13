#pragma once
#include <memory>
#include <map>
#include <set>
#include "ldp_basic_mat.h"
class Camera;
#undef min
#undef max
namespace svg
{
	class SvgGroup;
	class SvgAbstractObject;
	class SvgManager
	{
	public:
		struct Layer
		{
			Layer();

			bool operator < (const Layer& rhs)
			{
				return name < rhs.name;
			}
			bool operator == (const Layer& rhs)
			{
				return name == rhs.name;
			}
			void updateIndex(SvgAbstractObject* obj, int &idx);

			std::shared_ptr<SvgAbstractObject> root;
			std::map<int, SvgAbstractObject*> idxMap;
			std::set<SvgAbstractObject*> groups_for_selection;
			std::string name;
			bool selected;
		};
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
		void load(const char* svg_file, bool clearOld = true);
		void save(const char* svg_file, bool selectionOnly=false);
		void render(int shapes);
		void renderIndex(int shapes);
		std::shared_ptr<SvgManager> clone()const;

		Layer* addLayer(std::string name = "");
		Layer* getLayer(std::string name);
		const Layer* getLayer(std::string name)const;
		Layer* getCurrentLayer();
		const Layer* getCurrentLayer()const;
		void setCurrentLayer(std::string name);
		void removeLayer(std::string name);
		void renameLayer(std::string oldname, std::string newname);
		void mergeSelectedLayers();
		Layer* selectedToNewLayer();
		const std::map<std::string, std::shared_ptr<Layer>>& layers()const { return m_layers; }
		

		int width()const;
		int height()const;

		// should be called after adding/removing units or groups
		void updateIndex();

		// should be called after geometric changes
		void updateBound();
		ldp::Float4 getBound()const;

		// given an index, select shapes
		void selectShapeByIndex(int id, SelectOp op = SelectThis);
		void selectShapeByIndex(const std::set<int>& ids, SelectOp op = SelectThis);

		void selectPathBySimilarSelectedWidth();
		void selectPathByWidths(const std::set<float>& widths);

		// not finished
		void selectPathConnected();
		// not finished
		void selectPathSimilarShape();
		// not finished
		void selectPathClosed();

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
		// not finished, for burdastyle data, see SvgPath::PathUnitShapes
		void splitSelectedPathByShape();
		// not finished
		void convertSelectedPathToConnectedGroups();
	protected:
		void removeSelected(SvgAbstractObject* obj);
		bool groupSelected_findCommonParent(std::shared_ptr<SvgAbstractObject> obj,
			std::shared_ptr<SvgAbstractObject> objParent,
			std::shared_ptr<SvgAbstractObject>& commonParent, 
			int& cnt);
		void ungroupSelected_collect(SvgAbstractObject* obj, std::set<SvgGroup*>& groups);
		void removeSingleNodeAndEmptyNode(std::shared_ptr<SvgAbstractObject>& obj);
		void splitPath(std::shared_ptr<SvgAbstractObject>& obj);
		void splitPathByShape(std::shared_ptr<SvgAbstractObject>& obj);
	private:
		Camera* m_renderCam;
		std::map<std::string, std::shared_ptr<Layer>> m_layers;
		std::string m_currentLayerName;
	};
}
