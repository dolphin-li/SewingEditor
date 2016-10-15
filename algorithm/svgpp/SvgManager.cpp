#include "glew.h"
#include "freeglut.h"

#include "SvgManager.h"

#include "Camera.h"
#include "util.h"
#include "kdtree\PointTree.h"

#include "nvprsvg\svg_loader.hpp"
#include "stc\scene_stc.hpp"
#include "tinyxml\tinyxml.h"

#include "SvgAbstractObject.h"
#include "SvgAttribute.h"
#include "SvgPath.h"
#include "SvgPolyPath.h"
#include "SvgText.h"
#include "SvgGroup.h"

namespace svg
{
	const static float PATH_CONTACT_DIST_THRE = 0.2f;
	const static float PATH_COS_ANGLE_DIST_THRE = cos(15.f * ldp::PI_S / 180.f);
#pragma region --helper

	struct ConvertPathProcessor : PathSegmentProcessor
	{
		Path *p;

		GLenum &fill_rule;

		vector<GLubyte> cmds;
		vector<GLfloat> coords;
		vector<int> cmdPos;

		ConvertPathProcessor(Path *p_, GLenum &fill_rule_)
			: p(p_)
			, fill_rule(fill_rule_)
			, cmds(p->cmd.size())
			, coords(p->coord.size())
		{
			cmds.clear();
			coords.clear();
			cmdPos.clear();
		}

		void beginPath(PathPtr p) {
			switch (p->style.fill_rule) {
			case PathStyle::EVEN_ODD:
				fill_rule = GL_INVERT;
				break;
			default:
				assert(!"bogus style.fill_rule");
				break;
			case PathStyle::NON_ZERO:
				fill_rule = GL_COUNT_UP_NV;
				break;
			}
		}
		void moveTo(const float2 plist[2], size_t coord_index, char cmd) {
			cmds.push_back(GL_MOVE_TO_NV);
			cmdPos.push_back(coords.size());
			coords.push_back(plist[1].x);
			coords.push_back(plist[1].y);
		}
		void lineTo(const float2 plist[2], size_t coord_index, char cmd) {
			cmds.push_back(GL_LINE_TO_NV);
			cmdPos.push_back(coords.size());
			coords.push_back(plist[1].x);
			coords.push_back(plist[1].y);
		}
		void quadraticCurveTo(const float2 plist[3], size_t coord_index, char cmd) {
			cmds.push_back(GL_QUADRATIC_CURVE_TO_NV);
			cmdPos.push_back(coords.size());
			coords.push_back(plist[1].x);
			coords.push_back(plist[1].y);
			coords.push_back(plist[2].x);
			coords.push_back(plist[2].y);
		}
		void cubicCurveTo(const float2 plist[4], size_t coord_index, char cmd) {
			cmds.push_back(GL_CUBIC_CURVE_TO_NV);
			cmdPos.push_back(coords.size());
			coords.push_back(plist[1].x);
			coords.push_back(plist[1].y);
			coords.push_back(plist[2].x);
			coords.push_back(plist[2].y);
			coords.push_back(plist[3].x);
			coords.push_back(plist[3].y);
		}
		void arcTo(const EndPointArc &arc, size_t coord_index, char cmd) {
			if (arc.large_arc_flag) {
				if (arc.sweep_flag) {
					cmds.push_back(GL_LARGE_CCW_ARC_TO_NV);
				}
				else {
					cmds.push_back(GL_LARGE_CW_ARC_TO_NV);
				}
			}
			else {
				if (arc.sweep_flag) {
					cmds.push_back(GL_SMALL_CCW_ARC_TO_NV);
				}
				else {
					cmds.push_back(GL_SMALL_CW_ARC_TO_NV);
				}
			}
			cmdPos.push_back(coords.size());
			coords.push_back(arc.radii.x);
			coords.push_back(arc.radii.y);
			coords.push_back(arc.x_axis_rotation);
			coords.push_back(arc.p[1].x);
			coords.push_back(arc.p[1].y);
		}
		void close(char cmd) {
			cmds.push_back(GL_CLOSE_PATH_NV);
			cmdPos.push_back(coords.size());
		}
		void endPath(PathPtr p) {
		}
	};

	class ConvertVisitor : public ClipSaveVisitor, public enable_shared_from_this<ConvertVisitor>
	{
	public:
		ConvertVisitor(SvgManager* manager)
		{
			m_manager = manager;
			m_layer = nullptr;
			m_group = nullptr;
		}
		virtual void visit(ShapePtr shape)
		{
			GLenum gl_fill_rull = 0;
			ConvertPathProcessor processor(shape->getPath().get(), gl_fill_rull);
			shape->getPath()->processSegments(processor);
			Cg::float4 bd = shape->getPath()->getBounds();
			SvgPath* path = nullptr;
			if (shape->getPath()->ldp_poly_id >= 0)
				path = new SvgPolyPath(shape->getPath()->ldp_poly_id);
			else
				path = new SvgPath();
			path->m_gl_fill_rull = gl_fill_rull;
			path->m_pathStyle = shape->getPath()->style;
			path->m_cmds.insert(path->m_cmds.end(), processor.cmds.begin(), processor.cmds.end());
			path->m_coords.insert(path->m_coords.end(), processor.coords.begin(), processor.coords.end());
			path->setBound(ldp::Float4(bd.x, bd.z, bd.y, bd.w));
			path->setParent(m_group);
			if (path->objectType() == SvgAbstractObject::PolyPath)
				((SvgPolyPath*)path)->findCorners();
			m_group->m_children.push_back(std::shared_ptr<SvgAbstractObject>(path));
		}
		virtual void visit(TextPtr text)
		{
			SvgText* t = new SvgText();
			double3x3 M = text->transform;
			ldp::Mat3f& tM = t->attribute()->m_transfrom;
			for (int r = 0; r < 2; r++)
			for (int c = 0; c < 2; c++)
				tM(r, c) = M[c][r]; // a transpose here
			tM(0, 2) = M[0][2];
			tM(1, 2) = M[1][2];
			t->m_font = text->font_family;
			t->m_text = text->text;
			t->m_font_size = text->font_size;
			t->updateText();
			t->setParent(m_group);
			m_group->m_children.push_back(std::shared_ptr<SvgText>(t));
		}
		virtual void apply(GroupPtr group)
		{
			if (group->ldp_layer_name != "")
			{
				m_layer = m_manager->getLayer(group->ldp_layer_name);
				if (m_layer == nullptr)
					m_layer = m_manager->addLayer(group->ldp_layer_name);
				m_group = (SvgGroup*)m_layer->root.get();
				m_layer->selected = true;
			}
			if (m_layer == nullptr)
			{
				m_layer = m_manager->addLayer("default");
				m_group = (SvgGroup*)m_layer->root.get();
				m_layer->selected = true;
			}
			else
			{
				SvgGroup* g = new SvgGroup;
				m_group->m_children.push_back(std::shared_ptr<SvgGroup>(g));
				g->setParent(m_group);
				m_group = g;
			}
		}
		virtual void unapply(GroupPtr group)
		{
			assert(m_group);
			m_group = m_group->parent();
		}
		virtual void fill(StCShapeRendererPtr shape_renderer_renderer) { }
		virtual void stroke(StCShapeRendererPtr shape_renderer_renderer) { }
	private:
		SvgGroup* m_group;
		SvgManager* m_manager;
		SvgManager::Layer* m_layer;
	};

	class ConvertTraversal : public GenericTraversal
	{
	public:
		void traverse(GroupPtr group, VisitorPtr visitor)
		{
			visitor->apply(group);
			for (size_t i = 0; i != group->list.size(); i++)
				group->list[i]->traverse(visitor, *this);
			visitor->unapply(group);
		}
	};

#pragma endregion

	SvgManager::Layer::Layer()
	{
		name = "";
		selected = false;
	}

	SvgManager::SvgManager()
	{
		m_renderCam = nullptr;
	}

	SvgManager::~SvgManager()
	{

	}

	std::shared_ptr<SvgManager> SvgManager::clone()const
	{
		std::shared_ptr<SvgManager> manager(new SvgManager());
		manager->m_renderCam = m_renderCam;
		manager->m_currentLayerName = m_currentLayerName;
		for (auto iter : m_layers)
		{
			auto layer = manager->addLayer(iter.second->name);
			layer->root = iter.second->root->clone();
			layer->name = iter.second->name;
			layer->selected = iter.second->selected;
		}
		manager->updateIndex();
		manager->updateBound();
		return manager;
	}

	void SvgManager::init(Camera* cam)
	{
		m_renderCam = cam;
	}

	void SvgManager::load(const char* svg_file, bool clearOld)
	{
		// load scene using nvpr implementation
		SvgScenePtr svg_scene = svg_loader(svg_file);
		if (svg_scene.get() == nullptr)
			throw std::exception(("file not exist:" + std::string(svg_file)).c_str());

		// convert to my data structure
		if (clearOld)
			m_layers.clear();
		ConvertTraversal traversal;
		svg_scene->traverse(VisitorPtr(new ConvertVisitor(this)), traversal);
		if (m_layers.size())
		{
			m_currentLayerName = m_layers.begin()->second->name;
			removeSingleNodeAndEmptyNode();
			updateIndex();
			updateBound();
			if (m_renderCam)
			{
				ldp::Float4 b = getBound();
				m_renderCam->setFrustum(b[0], b[1], b[2], b[3], -1, 1);
			}
		}
	}

	void SvgManager::save(const char* svg_file, bool selectionOnly)
	{
		TiXmlDocument doc;
		TiXmlDeclaration *dec = new TiXmlDeclaration("1.0", "utf-8", "");
		doc.LinkEndChild(dec);
		TiXmlElement *dec_ele = new TiXmlElement("!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\"");
		doc.LinkEndChild(dec_ele);
		dec_ele->ldp_hack_ignore_last_dash = true;
		// header info-------------------------------------------------------
		TiXmlElement *root_ele = new TiXmlElement("svg");
		doc.LinkEndChild(root_ele);
		root_ele->SetAttribute("version", "1.1");
		root_ele->SetAttribute("id", "svg_base");
		std::string x, y, w, h;
		ldp::Float4 bound = getBound();
		x = std::to_string(bound[0]);
		y = std::to_string(bound[2]);
		w = std::to_string(bound[1] - bound[0]);
		h = std::to_string(bound[3] - bound[2]);
		root_ele->SetAttribute("x", (x + "px").c_str());
		root_ele->SetAttribute("y", (y + "px").c_str());
		root_ele->SetAttribute("width", (w + "px").c_str());
		root_ele->SetAttribute("height", (h + "px").c_str());
		root_ele->SetAttribute("viewBox", (x + " " + y + " " + w + " " + h).c_str());
		root_ele->SetAttribute("enable-background", ("new " + x + " " + y + " " + w + " " + h).c_str());
		root_ele->SetAttribute("xml:space", "preserve");

		for (auto iter : m_layers)
		{
			if (selectionOnly && !iter.second->selected)
				continue;
			auto root = iter.second->root;
			if (root.get())
			{
				TiXmlElement* ele = root->toXML(root_ele);
				ele->SetAttribute("ldp_layer_name", iter.second->name.c_str());
			}
		} // end for layers
		if (!doc.SaveFile(svg_file))
			throw std::exception(("error writing svg file: " + std::string(svg_file)).c_str());
	}

	int SvgManager::width()const
	{
		float w = 0;
		for (auto iter : m_layers)
		{
			w = std::max(w, iter.second->root->width());
		}
		return std::lroundf(w);
	}

	int SvgManager::height()const
	{
		float h = 0;
		for (auto iter : m_layers)
		{
			h = std::max(h, iter.second->root->height());
		}
		return std::lroundf(h);
	}

	void SvgManager::updateIndex()
	{
		int idx = SvgAbstractObject::INDEX_BEGIN;
		for (auto iter : m_layers)
		{
			iter.second->idxMap.clear();
			iter.second->groups_for_selection.clear();
			iter.second->updateIndex(iter.second->root.get(), idx);
		}
	}

	void SvgManager::Layer::updateIndex(SvgAbstractObject* obj, int& idx)
	{
		if (obj == nullptr)
			return;
		SvgAbstractObject* ogp = obj->ancestorAfterRoot();
		if (ogp)
			groups_for_selection.insert(ogp);

		obj->setId(idx);
		const int n = obj->numId();
		for (int i = 0; i < n; i++)
			idxMap.insert(std::make_pair(idx++, obj));
		if (obj->objectType() == SvgAbstractObject::Group)
		{
			SvgGroup* g = (SvgGroup*)obj;
			for (size_t i = 0; i < g->m_children.size(); i++)
				updateIndex(g->m_children[i].get(), idx);
		}
	}

	void SvgManager::updateBound()
	{
		if (m_layers.empty())
			return;
		for (auto iter : m_layers)
		{
			iter.second->root->updateBoundFromGeometry();
		}
	}

	ldp::Float4 SvgManager::getBound()const
	{
		ldp::Float4 box;
		for (auto iter : m_layers)
		{
			box = iter.second->root->unionBound(box);
		}
		return box;
	}

	void SvgManager::render()
	{
		if (m_layers.empty())
			return;
		for (auto iter : m_layers)
		{
			if (iter.second->root && iter.second->selected)
				iter.second->root->render();
		}
	}

	void SvgManager::renderIndex()
	{
		if (m_layers.empty())
			return;
		for (auto iter : m_layers)
		{
			if (iter.second->root && iter.second->selected)
				iter.second->root->renderId();
		}
	}

	void SvgManager::selectShapeByIndex(int id, SelectOp op)
	{
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			for (auto iter : layer->idxMap)
			{
				SvgAbstractObject* obj = iter.second;
				bool firstVisit = (iter.first == obj->getId());
				switch (op)
				{
				case svg::SvgManager::SelectThis:
					if (id >= SvgAbstractObject::INDEX_BEGIN)
						obj->setSelected(obj->isMyValidIdRange(id), id);
					break;
				case svg::SvgManager::SelectUnion:
					if (obj->isMyValidIdRange(id))
						obj->setSelected(true, id);
					break;
				case svg::SvgManager::SlectionUnionInverse:
					if (obj->isMyValidIdRange(id) && firstVisit)
						obj->setSelected(!obj->isSelected(), id);
					break;
				case svg::SvgManager::SelectAll:
					obj->setSelected(true);
					break;
				case svg::SvgManager::SelectNone:
					obj->setSelected(false);
					break;
				case svg::SvgManager::SelectInverse:
					if (obj->objectType() != obj->Group && firstVisit)
						obj->setSelected(!obj->isSelected(), id);
					break;
				default:
					break;
				}
			}
		}
	}

	void SvgManager::selectShapeByIndex(const std::set<int>& ids, SelectOp op)
	{
		std::set<int> validIds;
		for (auto id : ids)
		{
			if (id >= SvgAbstractObject::INDEX_BEGIN)
				validIds.insert(id);
		}

		if (validIds.size() == 1)
		{
			selectShapeByIndex(*validIds.begin(), op);
			return;
		}

		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			for (auto iter : layer->idxMap)
			{
				SvgAbstractObject* obj = iter.second;
				bool firstVisit =  (iter.first == obj->getId());
				bool found = false;
				const int n = obj->numId();
				for (int i = 0; i < n; i++)
				if (validIds.find(obj->getId() + i) != validIds.end()){
					found = true;
					break;
				}
				switch (op)
				{
				case svg::SvgManager::SelectThis:
					if (validIds.size())
						obj->setSelected(found);
					break;
				case svg::SvgManager::SelectUnion:
					if (found)
						obj->setSelected(true);
					break;
				case svg::SvgManager::SlectionUnionInverse:
					if (found && firstVisit)
						obj->setSelected(!obj->isSelected());
					break;
				case svg::SvgManager::SelectAll:
					obj->setSelected(true);
					break;
				case svg::SvgManager::SelectNone:
					obj->setSelected(false);
					break;
				case svg::SvgManager::SelectInverse:
					if (obj->objectType() != obj->Group && firstVisit)
						obj->setSelected(!obj->isSelected());
					break;
				default:
					break;
				}
			}
		}
	}

	void SvgManager::selectGroupByIndex(int id_, SelectOp op)
	{
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			SvgAbstractObject* target = nullptr;
			auto it = layer->idxMap.find(id_);
			if (it != layer->idxMap.end())
				target = it->second->ancestorAfterRoot();

			for (auto giter : layer->groups_for_selection)
			{
				SvgAbstractObject* ans = giter;
				switch (op)
				{
				case svg::SvgManager::SelectThis:
					if (target)
						ans->setSelected(ans == target);
					break;
				case svg::SvgManager::SelectUnion:
					if (ans == target)
						ans->setSelected(true);
					break;
				case svg::SvgManager::SlectionUnionInverse:
					if (ans == target)
						ans->setSelected(!ans->isSelected());
					break;
				case svg::SvgManager::SelectAll:
					ans->setSelected(true);
					break;
				case svg::SvgManager::SelectNone:
					ans->setSelected(false);
					break;
				case svg::SvgManager::SelectInverse:
					ans->setSelected(!ans->isSelected());
					break;
				default:
					break;
				}
			}
		}
	}

	void SvgManager::selectGroupByIndex(const std::set<int>& ids, SelectOp op)
	{
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			std::set<SvgAbstractObject*> targets;
			for (auto iter : layer->idxMap)
			{
				auto it = ids.find(iter.first);
				if (it != ids.end())
					targets.insert(iter.second->ancestorAfterRoot());
			}

			for (auto giter : layer->groups_for_selection)
			{
				SvgAbstractObject* ans = giter;
				switch (op)
				{
				case svg::SvgManager::SelectThis:
					if (targets.find(ans) != targets.end())
						ans->setSelected(true);
					else
						ans->setSelected(false);
					break;
				case svg::SvgManager::SelectUnion:
					if (targets.find(ans) != targets.end())
						ans->setSelected(true);
					break;
				case svg::SvgManager::SlectionUnionInverse:
					if (targets.find(ans) != targets.end())
						ans->setSelected(!ans->isSelected());
					break;
				case svg::SvgManager::SelectAll:
					ans->setSelected(true);
					break;
				case svg::SvgManager::SelectNone:
					ans->setSelected(false);
					break;
				case svg::SvgManager::SelectInverse:
					ans->setSelected(!ans->isSelected());
					break;
				default:
					break;
				}
			}
		}
	}

	void SvgManager::highlightShapeByIndex(int lastId, int thisId)
	{
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			auto lastIt = layer->idxMap.find(lastId);
			if (lastIt != layer->idxMap.end())
				lastIt->second->setHighlighted(false);
			auto thisIt = layer->idxMap.find(thisId);
			if (thisIt != layer->idxMap.end())
				thisIt->second->setHighlighted(true);
		}
	}

	bool SvgManager::groupSelected()
	{
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			std::shared_ptr<SvgAbstractObject> commonParent;
			int cnt = 0;
			bool ret = groupSelected_findCommonParent(layer->root,
				std::shared_ptr<SvgAbstractObject>(), commonParent, cnt);
			if (commonParent.get() == nullptr)
				continue;

			if (ret == false)
			{
				printf("error in grouping: all selected must be not in different groups previously!\n");
				return false;
			}

			assert(commonParent->objectType() == SvgAbstractObject::Group);
			auto commonParentG = (SvgGroup*)commonParent.get();
			assert(commonParentG->m_children.size() >= cnt);
			if (commonParentG->m_children.size() == cnt || 0 == cnt)
				return true;

			auto tmp = commonParentG->m_children;
			commonParentG->m_children.clear();
			std::shared_ptr<SvgAbstractObject> newGroup(new SvgGroup());
			newGroup->setSelected(true);
			newGroup->setParent(commonParentG);
			for (auto c : tmp)
			{
				if (c->isSelected())
				{
					c->setParent((SvgGroup*)newGroup.get());
					c->setSelected(false);
					((SvgGroup*)newGroup.get())->m_children.push_back(c);
				}
				else
					commonParentG->m_children.push_back(c);
			}
			commonParentG->m_children.push_back(newGroup);
		}

		updateIndex();
		updateBound();
		return true;
	}

	void SvgManager::ungroupSelected()
	{
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			std::set<SvgGroup*> groups;
			ungroupSelected_collect(layer->root.get(), groups);

			for (auto g : groups)
			{
				assert(g->parent()->objectType() == g->Group);
				SvgGroup* pg = (SvgGroup*)g->parent();
				for (auto child : g->m_children)
				{
					child->setParent(g->parent());
					child->setSelected(true);
					pg->m_children.push_back(child);
				}
				for (auto it = pg->m_children.begin(); it != pg->m_children.end(); ++it)
				{
					if (it->get() == g)
					{
						pg->m_children.erase(it);
						break;
					}
				}
			}
		}
		updateIndex();
		updateBound();
	}

	void SvgManager::removeSingleNodeAndEmptyNode()
	{
		for (auto iter : m_layers)
			removeInvalidPaths(iter.second->root.get());
		for (auto iter : m_layers)
			removeSingleNodeAndEmptyNode(iter.second->root);
		updateIndex();
		updateBound();
	}

	void SvgManager::removeSelected()
	{
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			removeSelected(layer->root.get());
		}
		updateIndex();
		updateBound();
	}

	void SvgManager::removeSelected(SvgAbstractObject* obj)
	{
		if (obj == nullptr)
			return;
		if (obj->objectType() == obj->Group)
		{
			SvgGroup* g = (SvgGroup*)obj;
			auto tmp = g->m_children;
			g->m_children.clear();
			for (auto iter : tmp)
			{
				if (!iter->isSelected())
					g->m_children.push_back(iter);
			}
			for (auto iter : tmp)
			{
				removeSelected(iter.get());
			}
		}
	}

	void SvgManager::removeInvalidPaths(SvgAbstractObject* obj)
	{
		if (obj == nullptr)
			return;
		if (obj->objectType() == obj->Group)
		{
			SvgGroup* g = (SvgGroup*)obj;
			auto tmp = g->m_children;
			g->m_children.clear();
			for (auto iter : tmp)
			{
				if (iter->objectType() == SvgAbstractObject::Path)
				{
					auto pc = (SvgPath*)iter.get();
					if (pc->m_cmds.size())
						g->m_children.push_back(iter);
				}
				else if (iter->objectType() == SvgAbstractObject::PolyPath)
				{
					auto pc = (SvgPolyPath*)iter.get();
					if (pc->m_cmds.size())
						g->m_children.push_back(iter);
				}
				else
					g->m_children.push_back(iter);
			}
			for (auto iter : tmp)
			{
				removeInvalidPaths(iter.get());
			}
		}
	}

	bool SvgManager::groupSelected_findCommonParent(
		std::shared_ptr<SvgAbstractObject> obj,
		std::shared_ptr<SvgAbstractObject> objParent,
		std::shared_ptr<SvgAbstractObject>& commonParent,
		int& cnt)
	{
		if (obj.get() == nullptr)
			return true;
		if (obj->isSelected() && objParent.get() != nullptr)
		{
			if (commonParent.get() == nullptr)
			{
				commonParent = objParent;
				cnt = 1;
			}
			else if (commonParent.get() != objParent.get())
				return false;
			else
				cnt++;
		}

		if (obj->objectType() == obj->Group)
		{
			auto g = (SvgGroup*)obj.get();
			for (auto child : g->m_children)
			{
				if (!groupSelected_findCommonParent(child, obj, commonParent, cnt))
					return false;
			}
		}

		return true;
	}

	void SvgManager::ungroupSelected_collect(SvgAbstractObject* obj, std::set<SvgGroup*>& groups)
	{
		if (obj == nullptr)
			return;
		if (obj->objectType() == obj->Group)
		{
			SvgGroup* g = (SvgGroup*)obj;
			if (g->isSelected() && g->parent())
			{
				groups.insert(g);
			}
			else
			{
				for (auto child : g->m_children)
					ungroupSelected_collect(child.get(), groups);
			}
		}
	}

	void SvgManager::removeSingleNodeAndEmptyNode(std::shared_ptr<SvgAbstractObject>& obj)
	{
		if (obj == nullptr)
			return;
		if (obj->objectType() != obj->Group)
			return;
		auto objGroup = (SvgGroup*)obj.get();
		if (objGroup->m_children.size() == 1)
		{
			if (obj->parent() || objGroup->m_children[0]->objectType() == obj->Group)
			{
				objGroup->m_children[0]->setParent(obj->parent());
				obj = objGroup->m_children[0];
				removeSingleNodeAndEmptyNode(obj);
			}
		}
		else if (objGroup->m_children.size() == 0)
		{
			if (obj->parent())
				obj->setId(obj->INDEX_BEGIN - 1);
		}
		else
		{
			for (auto& c : objGroup->m_children)
				removeSingleNodeAndEmptyNode(c);
			auto tmp = objGroup->m_children;
			objGroup->m_children.clear();
			for (auto c : tmp)
			if (c.get())
			{
				if (c->getId() != obj->INDEX_BEGIN - 1)
					objGroup->m_children.push_back(c);
			}
		}
	}

	void SvgManager::splitSelectedPath()
	{
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			splitPath(layer->root, false);
		}
		updateIndex();
		updateBound();
	}

	bool SvgManager::mergeSelectedPath()
	{
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			std::shared_ptr<SvgAbstractObject> commonParent;
			int cnt = 0;
			bool ret = groupSelected_findCommonParent(layer->root,
				std::shared_ptr<SvgAbstractObject>(), commonParent, cnt);
			if (commonParent.get() == nullptr)
				continue;
			if (ret == false)
			{
				printf("error in grouping: all selected must be not in different groups previously!\n");
				return false;
			}
			assert(commonParent->objectType() == SvgAbstractObject::Group);

			auto parentPtr = (SvgGroup*)commonParent.get();
			auto tmpChildren = parentPtr->m_children;
			parentPtr->m_children.clear();
			std::shared_ptr<SvgAbstractObject> path;
			for (auto c : tmpChildren)
			{
				if ((c->objectType() != SvgAbstractObject::Path
					&& c->objectType() != SvgAbstractObject::PolyPath)
					|| !c->isSelected())
					parentPtr->m_children.push_back(c);
				else
				{
					if (path.get() == nullptr)
						path = c;
					else
					{
						auto pathPtr = (SvgPath*)path.get();
						auto curPtr = (SvgPath*)c.get();

						// cmds
						pathPtr->m_cmds.insert(pathPtr->m_cmds.end(),
							curPtr->m_cmds.begin(), curPtr->m_cmds.end());

						// coords
						pathPtr->m_coords.insert(pathPtr->m_coords.end(),
							curPtr->m_coords.begin(), curPtr->m_coords.end());
					}
				}
			}// auto c : tmpChildren

			if (path.get() == nullptr)
				continue;

			path->invalid();
			parentPtr->m_children.push_back(path);
			if (parentPtr->m_children.size() == 1)
				removeSingleNodeAndEmptyNode(commonParent);
		}
		updateIndex();
		updateBound();

		return true;
	}

	void SvgManager::splitPath(std::shared_ptr<SvgAbstractObject>& obj, bool to_single_seg)
	{
		if (obj.get() == nullptr)
			return;
		if (obj->objectType() == obj->Group)
		{
			SvgGroup* g = (SvgGroup*)obj.get();
			for (auto& c : g->m_children)
				splitPath(c, to_single_seg);
		}
		else if ((obj->objectType() == obj->Path || obj->objectType() ==obj->PolyPath) && obj->isSelected())
		{
			SvgPath* p = (SvgPath*)obj.get();
			auto newGroup = p->splitToSegments(to_single_seg);
			if (newGroup.get())
			{
				newGroup->setParent(obj->parent());
				newGroup->setSelected(obj->isSelected());
				obj = newGroup;
			}
		}
	}

	void SvgManager::selectPathBySimilarSelectedWidth()
	{
		std::set<float> widths;
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			for (auto iter : layer->idxMap)
			{
				if (iter.second->objectType() == SvgAbstractObject::Path && iter.second->isSelected())
				{
					auto p = (SvgPath*)iter.second;
					widths.insert(p->m_pathStyle.stroke_width);
				}
				if (iter.second->objectType() == SvgAbstractObject::PolyPath && iter.second->isSelected())
				{
					auto p = (SvgPolyPath*)iter.second;
					widths.insert(p->m_pathStyle.stroke_width);
				}
			}
		}
		if (widths.size())
			selectPathByWidths(widths);
	}

	void SvgManager::selectPathByWidths(const std::set<float>& widths)
	{
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			for (auto iter : layer->idxMap)
			{
				if (iter.second->objectType() == SvgAbstractObject::Path)
				{
					auto p = (SvgPath*)iter.second;
					p->setSelected(widths.find(p->m_pathStyle.stroke_width) != widths.end());
				}
				else if (iter.second->objectType() == SvgAbstractObject::PolyPath)
				{
					auto p = (SvgPolyPath*)iter.second;
					p->setSelected(widths.find(p->m_pathStyle.stroke_width) != widths.end());
				}
			}
		}
	}

#pragma region --graph related
	typedef kdtree::PointTree<float, 2> Tree;
	typedef Tree::Point Point;
	struct GraphNode
	{
		int idx;
		std::vector<GraphNode*> nbNodes;
		GraphNode* father;
		bool visited;
		GraphNode()
		{
			idx = -1;
			father = nullptr;
			visited = false;
		}

		bool operator < (const GraphNode& r)const
		{
			return nbNodes.size() < r.nbNodes.size();
		}
	};
	struct GraphNodePtrLess
	{
		bool operator () (std::shared_ptr<GraphNode> a, std::shared_ptr<GraphNode> b)
		{
			return *a < *b;
		}
	};
	struct PolyPath
	{
		bool closed;
		std::vector<int> nodes;
		std::set<int> nodeSet;
		std::vector<ldp::Float2> points;

		PolyPath()
		{
			closed = false;
		}

		void prepareNodeset()
		{
			nodeSet.clear();
			for (auto n : nodes)
				nodeSet.insert(n);
		}

		void collectPoints(const std::vector<Point>& pts)
		{
			points.clear();
			for (auto id : nodes)
				points.push_back(pts[id].p);
		}

		bool operator == (const PolyPath& r)const
		{
			if (nodes.size() != r.nodes.size())
				return false;
			auto r0_iter = std::find(r.nodes.begin(), r.nodes.end(), nodes[0]);
			if (r0_iter == r.nodes.end())
				return false;
			if (nodes.size() == 1)
				return true;

			// forward matching
			auto my_iter = nodes.begin();
			for (auto r_iter = r0_iter;; )
			{
				if (*r_iter != *my_iter)
					break;
				r_iter++;
				my_iter++;
				if (r_iter == r.nodes.end())
					r_iter = r.nodes.begin();
				if (r_iter == r0_iter)
					return true;
			}

			// backward matching
			my_iter = nodes.begin();
			for (auto r_iter = r0_iter;;)
			{
				if (*r_iter != *my_iter)
					return false;
				if (r_iter == r.nodes.begin())
					r_iter = r.nodes.end() - 1;
				else
					r_iter--;
				my_iter++;
				if (r_iter == r0_iter)
					return true;
			}

			return false;
		}

		bool operator < (const PolyPath& r)const
		{
			for (auto n : nodes)
			if (r.nodeSet.find(n) == r.nodeSet.end())
				return false;
			return true;
		}

		bool operator > (const PolyPath& r)
		{
			return r < *this;
		}

		bool containedIn(const std::vector<PolyPath>& paths)
		{
			for (auto& path : paths)
				if (path == *this)
					return true;
			return false;
		}

		bool intersected(const PolyPath& r)const
		{
			for (auto n : nodes)
			if (r.nodeSet.find(n) != r.nodeSet.end())
				return true;
			return false;
		}
	};

	static void graph_find_cycles(GraphNode* node, std::vector<PolyPath>& groups, 
		std::vector<int>& global_visit_flag)
	{
		node->visited = true;
		global_visit_flag[node->idx] = true;
		for (auto nbNode : node->nbNodes){
			if (nbNode == node->father) continue;
			if (nbNode->visited){
				// found a loop
				PolyPath group;
				group.closed = true;
				for (auto pn = node; ; pn = pn->father){
					assert(pn);
					group.nodes.push_back(pn->idx);
					if (pn == nbNode) break;
				} // end for pn
				if (!group.containedIn(groups))
					groups.push_back(group);
			} // end if found a group
			else{
				nbNode->father = node;
				graph_find_cycles(nbNode, groups, global_visit_flag);
				nbNode->father = nullptr;
			}
		} // end for nbNode
		node->visited = false;
	}

	static void graph_find_non_cycles(GraphNode* node, std::vector<PolyPath>& groups,
		std::vector<int>& global_visit_flag)
	{
		PolyPath path;
		for (auto nbNode = node;;){
			path.nodes.push_back(nbNode->idx);
			if (global_visit_flag[nbNode->idx]) break;
			global_visit_flag[nbNode->idx] = true;
			for (auto n : nbNode->nbNodes){
				if (n != node)
				{
					node = nbNode;
					nbNode = n;
					break;
				}
			} // end for n
			if (nbNode == node) break;
		} // end for nbNode

		if (path.nodes.size() > 1)
		{
			if (!path.containedIn(groups))
				groups.push_back(path);
		}
	}

	static void graph_to_connected_components(
		std::set<ldp::Int2>& graphSet, int nNodes,
		std::vector<PolyPath>& groups)
	{
		groups.clear();
		if (nNodes == 0)
			return;

		// 1. produce a graph representation
		std::vector<GraphNode> graph(nNodes);
		std::vector<int> node_visited(nNodes, 0);
		for (int i = 0; i < nNodes; i++)
			graph[i].idx = i;
		for (auto c : graphSet)
			graph[c[0]].nbNodes.push_back(&graph[c[1]]);

		// 2. find cycles in this step
		for (int nodeId = 0; nodeId < node_visited.size(); nodeId++)
		{
			if (node_visited[nodeId] || graph[nodeId].nbNodes.size() != 2) continue;
			graph[nodeId].father = nullptr;
			graph_find_cycles(&graph[nodeId], groups, node_visited);
		} // end while

		// 2.1 re-calc visited nodes
		std::fill(node_visited.begin(), node_visited.end(), 0);
		for (auto g : groups)
			for (auto idx : g.nodes)
				node_visited[idx] = 1;

		// 3. find non-cycles in this step
		for (int nodeId = 0; nodeId < node_visited.size(); nodeId++)
		{
			if (node_visited[nodeId] || graph[nodeId].nbNodes.size() != 1) continue;
			graph_find_non_cycles(&graph[nodeId], groups, node_visited);
		} // end while
	}

	static void graph_convert_inner_loops_to_segs(std::vector<PolyPath>& groups,
		const std::vector<Point>& points)
	{
		for (auto& group : groups)
		{
			group.prepareNodeset();
			group.collectPoints(points);
		}

		for (const auto& groupi : groups)
		{
			if (!groupi.closed) continue;
#pragma omp parallel for
			for (int j_group = 0; j_group < (int)groups.size(); j_group++)
			{
				PolyPath& groupj = groups[j_group];
				if (&groupi == &groupj) continue;
				if (!groupj.closed) continue;
				if (!groupi.intersected(groupj))
					continue;

				std::vector<int> tmpIds;
				int startPos = 0, numBreaks = 0;
				for (int groupj_pid = 0; groupj_pid < groupj.nodes.size(); groupj_pid++)
				{
					int v = groupj.nodes[groupj_pid];
					if (groupi.nodeSet.find(v) != groupi.nodeSet.end()) continue;
					ldp::Float2 p = groupj.points[groupj_pid];
					if (ldp::PointInPolygon(groupi.points.size() - 1, groupi.points.data(), p))
					{
						if (tmpIds.size()){
							if (groupj_pid - tmpIds.back() > 1){
								startPos = tmpIds.size();
								numBreaks++;
							}
						}
						tmpIds.push_back(groupj_pid);
					}
				} // end for groupj_pid

				// we only handle continuous segments
				if (numBreaks > 1 || tmpIds.size() == 0) continue;

				PolyPath tmpPath;
				int prePos = (tmpIds[startPos] + groupj.nodes.size() - 1) % groupj.nodes.size();
				tmpPath.nodes.push_back(groupj.nodes[prePos]);
				for (int i = 0; i < tmpIds.size(); i++)
				{
					int pos = (i + startPos) % tmpIds.size();
					tmpPath.nodes.push_back(groupj.nodes[tmpIds[pos]]);
				}
				int postPos = (tmpIds[(startPos - 1 + tmpIds.size()) % tmpIds.size()] + 1) % groupj.nodes.size();
				tmpPath.nodes.push_back(groupj.nodes[postPos]);
				groupj = tmpPath;
			} // groupj
		} // groupi
	}
#pragma endregion
	void SvgManager::convertSelectedPathToConnectedGroups()
	{
		typedef std::shared_ptr<SvgAbstractObject> ObjPtr;
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;

			// 1. pre-process, split into individual small paths
			splitPath(layer->root, true);
			removeInvalidPaths(layer->root.get());	
			removeSingleNodeAndEmptyNode(layer->root);
			auto rootPtr = (SvgGroup*)layer->root.get();

			// 2. collect all paths, the idx corresponds to paths
			std::vector<ObjPtr> paths; // each path generate two points, a start point and an end point
			std::vector<Point> points, validPoints;
			std::vector<int> pointMapper, validPointIds; // map from points to merged points
			rootPtr->collectObjects(SvgAbstractObject::Path, paths, true);
			rootPtr->collectObjects(SvgAbstractObject::PolyPath, paths, true);
			for (auto path : paths)
			{
				auto ptr = (SvgPath*)path.get();
				points.push_back(Point(ptr->getStartPoint(), points.size()));
				points.push_back(Point(ptr->getEndPoint(), points.size()));
			}

			// 3. find too-close points and merge them
			Tree tree; tree.build(points);
			std::vector<std::vector<int>> mergePaths(points.size());
			std::set<ldp::Int2> pathGraph;
			for (auto p : points)
			{
				std::vector<std::pair<size_t, float>> results;
				tree.pointInSphere(p, PATH_CONTACT_DIST_THRE, results);
				for (auto r : results){
					if (r.first / 2 == p.idx / 2) continue;
					mergePaths[p.idx].push_back(r.first);
				}
			}
			for (int iPoint = 0; iPoint < (int)points.size(); iPoint++)
			{
				bool valid = true;
				for (auto c : mergePaths[iPoint])
				if (c < iPoint){
					valid = false;
					break;
				}
				if (valid){
					validPointIds.push_back(iPoint);
					validPoints.push_back(points[iPoint]);
				}
			}
			pointMapper.resize(points.size(), -1);
			for (int i = 0; i < (int)validPointIds.size(); i++)
			{
				int vid = validPointIds[i];
				pointMapper[vid] = i;
				for (auto c : mergePaths[vid])
					pointMapper[c] = i;
			}

			// debug
			for (int i = 0; i < pointMapper.size(); i++)
			{
				if (pointMapper[i] == -1)
				{
					// the reason for this warning is due to current merge policy:
					// if dist(A,B) < PATH_CONTACT_DIST_THRE, dist(B,C) < PATH_CONTACT_DIST_THRE
					//		dist(A, C) >= PATH_CONTACT_DIST_THRE
					// then this warning may occure
					printf("warning: point %d seems invalid, results may be incorrecrt!\n", i);
				}
			}

			// 3.1 finally we construct the connection graph
			for (int i = 0; i < (int)points.size(); i += 2)
			{
				if (pointMapper[i] >= 0 && pointMapper[i + 1] >= 0)
				{
					pathGraph.insert(ldp::Int2(pointMapper[i], pointMapper[i + 1]));
					pathGraph.insert(ldp::Int2(pointMapper[i + 1], pointMapper[i]));
				}
			}

			// 4. group each polygons and associates
			std::vector<PolyPath> pathGroups;
			graph_to_connected_components(pathGraph, (int)validPointIds.size(), pathGroups);
			graph_convert_inner_loops_to_segs(pathGroups, validPoints);

			// 5. create new path groups
			std::vector<ObjPtr> newGroups;
			for (auto gIds : pathGroups)
			{
				std::shared_ptr<SvgAbstractObject> newPath;
				auto newPathPtr = (SvgPolyPath*)nullptr;
				for (int i = 0; i < (int)gIds.nodes.size(); i++)
				{
					int pointId = validPointIds[gIds.nodes[i]];
					if (i == 0)
					{
						auto oldPathPtr = (SvgPolyPath*)paths[validPointIds[0]].get();
						newPath.reset(new SvgPolyPath);
						newPathPtr = (SvgPolyPath*)newPath.get();
						oldPathPtr->SvgAbstractObject::copyTo(newPathPtr);
						newPathPtr->m_pathStyle = oldPathPtr->m_pathStyle;
						newPathPtr->m_gl_fill_rull = oldPathPtr->m_gl_fill_rull;
						newPathPtr->setParent(rootPtr);
						newPathPtr->m_cmds.push_back(GL_MOVE_TO_NV);
					}
					else
					{
						newPathPtr->m_cmds.push_back(GL_LINE_TO_NV);
					}
					newPathPtr->m_coords.push_back(points[pointId].p[0]);
					newPathPtr->m_coords.push_back(points[pointId].p[1]);
				}
				if (gIds.closed)
				{
					newPathPtr->m_cmds.push_back(GL_LINE_TO_NV);
					int pid = validPointIds[gIds.nodes[0]];
					newPathPtr->m_coords.push_back(points[pid].p[0]);
					newPathPtr->m_coords.push_back(points[pid].p[1]);
				}
				if (newPathPtr)
				{
					newPathPtr->findCorners();
					newGroups.push_back(newPath);
				}
			} // end for gIds

			// 6. remove old and insert new
			removeSelected(rootPtr);
			rootPtr->m_children.insert(rootPtr->m_children.end(), newGroups.begin(), newGroups.end());
		} // end for all layers
		removeSingleNodeAndEmptyNode();
	}
	//////////////////////////////////////////////////////////////////
	/// layer related
	SvgManager::Layer* SvgManager::addLayer(std::string name)
	{
		// get a default layer name
		if (name == "")
		{
			for (auto i = 0; i < 999999; i++)
			{
				std::string name_loc = "layer_" + std::to_string(i);
				if (!getLayer(name_loc))
				{
					name = name_loc;
					break;
				}
			}
		}

		auto ly = getLayer(name);
		if (ly)
			throw std::exception(("addlayer, given name already existed: " + name).c_str());
		std::shared_ptr<Layer> layer(new Layer);
		layer->name = name;
		layer->selected = false;
		layer->root = std::shared_ptr<SvgGroup>(new SvgGroup);
		m_layers.insert(std::make_pair(name, layer));
		return layer.get();
	}

	SvgManager::Layer* SvgManager::getLayer(std::string name)
	{
		auto iter = m_layers.find(name);
		if (iter != m_layers.end())
			return iter->second.get();
		return nullptr;
	}

	const SvgManager::Layer* SvgManager::getLayer(std::string name)const
	{
		auto iter = m_layers.find(name);
		if (iter != m_layers.end())
			return iter->second.get();
		return nullptr;
	}

	SvgManager::Layer* SvgManager::getCurrentLayer()
	{
		return getLayer(m_currentLayerName);
	}

	const SvgManager::Layer* SvgManager::getCurrentLayer()const
	{
		return getLayer(m_currentLayerName);
	}

	void SvgManager::setCurrentLayer(std::string name)
	{
		m_currentLayerName = name;
	}

	void SvgManager::removeLayer(std::string name)
	{
		auto iter = m_layers.find(name);
		if (iter != m_layers.end())
			m_layers.erase(iter);
		else
			throw std::exception(("remove layer, not found: " + name).c_str());
		updateIndex();
		updateBound();
	}

	void SvgManager::mergeSelectedLayers()
	{
		auto tmpLayers = m_layers;
		m_layers.clear();
		auto selectedLayers = m_layers;
		for (auto iter : tmpLayers)
		{
			if (iter.second->selected)
				selectedLayers.insert(iter);
			else
				m_layers.insert(iter);
		}

		if (selectedLayers.size() == 0)
			return;

		auto iter0 = selectedLayers.begin();
		for (auto iter : selectedLayers)
		{
			if (iter0->first == iter.first) continue;
			auto g0 = (SvgGroup*)iter0->second->root.get();
			auto g = (SvgGroup*)iter.second->root.get();
			for (auto c : g->m_children)
			{
				c->setParent(g0);
				g0->m_children.push_back(c);
			}
		}
		m_layers.insert(*iter0);

		updateIndex();
		updateBound();
	}

	void SvgManager::renameLayer(std::string oldname, std::string newname)
	{
		if (oldname == newname)
			return;
		auto iter = m_layers.find(oldname);
		if (iter == m_layers.end())
			throw std::exception(("rename layer, not found: " + oldname).c_str());

		auto newLayer = addLayer(newname);
		newLayer->selected = iter->second->selected;
		newLayer->root = iter->second->root->clone();

		removeLayer(oldname);
		updateIndex();
		updateBound();
	}

	SvgManager::Layer* SvgManager::selectedToNewLayer()
	{
		auto newRoot = std::shared_ptr<SvgAbstractObject>(new SvgGroup);
		auto newRootPtr = (SvgGroup*)newRoot.get();
		for (auto layer_iter : m_layers)
		{
			auto layer = layer_iter.second;
			if (!layer->selected) continue;
			if (layer->root->hasSelectedChildren() || layer->root->isSelected())
			{
				auto g = layer->root->clone(true);
				auto gptr = (SvgGroup*)g.get();
				for (auto c : gptr->m_children)
				{
					c->setParent(newRootPtr);
					newRootPtr->m_children.push_back(c);
				}
			}
		}

		auto layer = addLayer();
		layer->root = newRoot;
		if (newRootPtr->m_children.size())
		{
			updateIndex();
			updateBound();
		}
		return layer;
	}

	////////////////////////////////////////////////////////////////
	///// pair related
	void SvgManager::makeSelectedToPair()
	{

	}

	void SvgManager::removeSelectedPairs()
	{

	}
} // namespace svg