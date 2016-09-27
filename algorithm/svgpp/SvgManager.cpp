#include "glew.h"
#include "freeglut.h"

#include "SvgManager.h"

#include "Camera.h"

#include "nvprsvg\svg_loader.hpp"
#include "stc\scene_stc.hpp"

#include "SvgAbstractObject.h"
#include "SvgAttribute.h"
#include "SvgPath.h"
#include "SvgText.h"
#include "SvgClipPath.h"
#include "SvgGroup.h"

namespace svg
{
#pragma region --helper

	struct ConvertPathProcessor : PathSegmentProcessor
	{
		Path *p;

		GLuint &path;
		GLenum &fill_rule;

		vector<GLubyte> cmds;
		vector<GLfloat> coords;
		vector<int> cmdPos;

		ConvertPathProcessor(Path *p_, GLuint &path_, GLenum &fill_rule_)
			: p(p_)
			, path(path_)
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
			if (!path) {
				path = glGenPathsNV(1);
			}
			//glPathCommandsNV(path,
			//	GLsizei(cmds.size()), &cmds[0],
			//	GLsizei(coords.size()), GL_FLOAT, &coords[0]);
		}
	};

	class ConvertVisitor : public ClipSaveVisitor, public enable_shared_from_this<ConvertVisitor>
	{
	public:
		ConvertVisitor(SvgGroup* g)
		{
			m_rootGroup = g;
			m_group = nullptr;
		}
		virtual void visit(ShapePtr shape)
		{
			SvgPath* path = new SvgPath();
			path->m_pathStyle = shape->getPath()->style;
			ConvertPathProcessor processor(shape->getPath().get(), path->m_gl_path_id, 
				path->m_gl_fill_rull);
			shape->getPath()->processSegments(processor);
			path->m_cmds.insert(path->m_cmds.end(), processor.cmds.begin(), processor.cmds.end());
			path->m_segmentPos.insert(path->m_segmentPos.end(), processor.cmdPos.begin(), processor.cmdPos.end());
			path->m_coords.insert(path->m_coords.end(), processor.coords.begin(), processor.coords.end());
			Cg::float4 bd = shape->getPath()->getBounds();
			path->setBound(ldp::Float4(bd.x, bd.z, bd.y, bd.w));
			m_group->m_children.push_back(std::shared_ptr<SvgPath>(path));
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
			if (!t->m_gl_path_id)
				t->m_gl_path_id = glGenPathsNV(1);
			t->updateText();
			m_group->m_children.push_back(std::shared_ptr<SvgText>(t));
		}
		virtual void apply(GroupPtr group) 
		{
			if (m_group == nullptr)
			{
				m_group = m_rootGroup;
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
		SvgGroup* m_rootGroup;
		SvgGroup* m_group;
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

	SvgManager::SvgManager()
	{

	}

	SvgManager::~SvgManager()
	{

	}

	void SvgManager::init(Camera* cam)
	{
		m_renderCam = cam;
	}

	void SvgManager::load(const char* svg_file)
	{
		// load scene using nvpr implementation
		SvgScenePtr svg_scene = svg_loader(svg_file);
		
		// convert to my data structure
		m_rootGroup = std::shared_ptr<SvgGroup>(new SvgGroup);
		ConvertTraversal traversal; 
		svg_scene->traverse(VisitorPtr(new ConvertVisitor(m_rootGroup.get())), traversal);

		updateIndex();
		updateBound();

		if (m_renderCam)
		{
			ldp::Float4 b = m_rootGroup->getBound();
			m_renderCam->setFrustum(b[0], b[1], b[2], b[3], -1, 1);
		}
	}

	int SvgManager::width()const
	{
		if (!m_rootGroup.get())
			return 0;
		return m_rootGroup->width();
	}

	int SvgManager::height()const
	{
		if (!m_rootGroup.get())
			return 0;
		return m_rootGroup->height();
	}

	void SvgManager::updateIndex()
	{
		int idx = SvgAbstractObject::INDEX_BEGIN;
		m_idxMap.clear();
		updateIndex(m_rootGroup.get(), idx);
	}

	void SvgManager::updateIndex(SvgAbstractObject* obj, int& idx)
	{
		obj->setId(idx);
		m_idxMap.insert(std::make_pair(idx, obj));
		idx++;
		if (obj->objectType() == SvgAbstractObject::Group)
		{
			SvgGroup* g = (SvgGroup*)obj;
			for (size_t i = 0; i < g->m_children.size(); i++)
				updateIndex(g->m_children[i].get(), idx);
		}
	}

	void SvgManager::updateBound()
	{
		updateBound(m_rootGroup.get());
	}

	void SvgManager::updateBound(SvgAbstractObject* obj)
	{
		ldp::Float4 box(FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX);

		if (obj->objectType() == SvgAbstractObject::Group)
		{
			SvgGroup* g = (SvgGroup*)obj;
			for (size_t i = 0; i < g->m_children.size(); i++)
			{
				updateBound(g->m_children[i].get());
				box = g->m_children[i]->unionBound(box);
			}
		}

		obj->setBound(obj->unionBound(box));
	}

	void SvgManager::render()
	{
		if (m_rootGroup.get() == nullptr)
			return;

		m_rootGroup->render();
	}

	void SvgManager::renderIndex()
	{
		if (m_rootGroup.get() == nullptr)
			return;

		m_rootGroup->renderId();
	}

	void SvgManager::selectShapeByIndex(int id, SelectOp op)
	{
		selectShapeByIndex(m_rootGroup.get(), id, op);
	}
	void SvgManager::selectShapeByIndex(SvgAbstractObject* obj, int id, SelectOp op)
	{
		switch (op)
		{
		case svg::SvgManager::SelectThis:
			if (id >= SvgAbstractObject::INDEX_BEGIN)
				obj->setSelected(obj->getId() == id);
			break;
		case svg::SvgManager::SelectUnion:
			if (obj->getId() == id)
				obj->setSelected(!obj->isSelected());
			break;
		case svg::SvgManager::SelectAll:
			obj->setSelected(true);
			break;
		case svg::SvgManager::SelectNone:
			obj->setSelected(false);
			break;
		case svg::SvgManager::SelectInverse:
			obj->setSelected(!obj->isSelected());
			break;
		default:
			break;
		}
		if (obj->objectType() == SvgAbstractObject::Group)
		{
			SvgGroup* g = (SvgGroup*)obj;
			for (size_t i = 0; i < g->m_children.size(); i++)
				selectShapeByIndex(g->m_children[i].get(), id, op);
		}
	}

	void SvgManager::highlightShapeByIndex(int lastId, int thisId)
	{
		auto lastIt = m_idxMap.find(lastId);
		if (lastIt != m_idxMap.end())
			lastIt->second->setHighlighted(false);
		auto thisIt = m_idxMap.find(thisId);
		if (thisIt != m_idxMap.end())
			thisIt->second->setHighlighted(true);
	}
}