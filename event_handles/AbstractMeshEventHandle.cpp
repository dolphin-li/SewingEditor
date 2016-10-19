#include <QEvent>
#include <GL\glew.h>
#include "BaseMeshViewer.h"

#include "designer2d_cloth.h"
#include "analysis2d_cloth_static.h"

#include "AbstractMeshEventHandle.h"
#include "ClothSelectEventHandle.h"
#include "ClothTranslateEventHandle.h"
#include "ClothRotateEventHandle.h"

#include "global_data_holder.h"
#include "svgpp\SvgPolyPath.h"
#include "svgpp\SvgManager.h"

ldp::Mat3d AbstractMeshEventHandle::rotation_from_uv(ldp::Double3 u, ldp::Double3 v)
{
	ldp::Double3 x = u.normalize();
	ldp::Double3 z = x.cross(v).normalize();
	ldp::Double3 y = z.cross(x);
	ldp::Mat3d R;
	R(0, 0) = x[0]; R(0, 1) = y[0]; R(0, 2) = z[0];
	R(1, 0) = x[1]; R(1, 1) = y[1]; R(1, 2) = z[1];
	R(2, 0) = x[2]; R(2, 1) = y[2]; R(2, 2) = z[2];
	return R;
}

AbstractMeshEventHandle::AbstractMeshEventHandle(BaseMeshViewer* v)
{
	m_viewer = v;
	m_lastHighlightShapeId = -1;
	m_currentSelectedId = -1;
	m_cursor = QCursor(Qt::CursorShape::ArrowCursor);
	m_iconFile = "";
	m_inactiveIconFile = "";
	m_toolTips = "general handle";
}

AbstractMeshEventHandle::~AbstractMeshEventHandle()
{

}

QString AbstractMeshEventHandle::iconFile()const
{
	return m_iconFile;
}

QString AbstractMeshEventHandle::inactiveIconFile()const
{
	return m_inactiveIconFile;
}

void AbstractMeshEventHandle::handleEnter()
{
	m_viewer->setFocus();
}
void AbstractMeshEventHandle::handleLeave()
{
	m_viewer->clearFocus();
}

QString AbstractMeshEventHandle::toolTips()const
{
	return m_toolTips;
}

AbstractMeshEventHandle* AbstractMeshEventHandle::create(ProcessorType type, BaseMeshViewer* v)
{
	switch (type)
	{
	case AbstractMeshEventHandle::ProcessorTypeGeneral:
		return new AbstractMeshEventHandle(v);
	case AbstractMeshEventHandle::ProcessorTypeClothSelect:
		return new ClothSelectEventHandle(v);
	case AbstractMeshEventHandle::ProcessorTypeClothTranslate:
		return new ClothTranslateEventHandle(v);
	case AbstractMeshEventHandle::ProcessorTypeClothRotate:
		return new ClothRotateEventHandle(v);
	case AbstractMeshEventHandle::ProcessorTypeEnd:
	default:
		return nullptr;
	}
}

void AbstractMeshEventHandle::pickMesh(QPoint p, bool reset_if_none_picked)
{
	const ldp::Camera& cam = m_viewer->camera();
	ldp::UInt3 tri_id;
	ldp::Double3 tri_coord;
	unsigned int id_l;
	bool res = m_viewer->pAnalysis()->Pick(ldp::Float2(p.x(), m_viewer->height() - 1 - p.y()),
		cam, tri_id, tri_coord, id_l, m_picked_screenPos);
	if (!res){
		if (reset_if_none_picked){
			m_viewer->pListener()->HilightCadTypeID(Cad::NOT_SET, 0);
			m_viewer->pListener()->Cad_SetPicked(Cad::NOT_SET, 0, 0, 0);
		}
	}
	else{
		m_viewer->pListener()->HilightCadTypeID(Cad::LOOP, id_l);
		m_viewer->pListener()->Cad_SetPicked(Cad::LOOP, id_l, 0, 0);
	}
}

bool AbstractMeshEventHandle::getPickedMeshFrameInfo(ldp::Double3& o, ldp::Double3& u, ldp::Double3& v, int& id_l)const
{
	Cad::CAD_ELEM_TYPE itype_cad_part;
	unsigned int id_cad_part;
	double tmp_x, tmp_y;
	m_viewer->pListener()->Cad_GetPicked(itype_cad_part, id_cad_part, tmp_x, tmp_y);
	if (itype_cad_part == Cad::LOOP && m_viewer->pListener()->GetCad().IsElemID(itype_cad_part, id_cad_part))
	{
		const auto& handle = m_viewer->pAnalysis()->getClothHandle();
		const auto& world = m_viewer->pAnalysis()->getWorld();
		const auto& conv = world.GetIDConverter(m_viewer->pAnalysis()->get_id_field_base());
		auto idEa = conv.GetIdEA_fromCad(id_cad_part, Cad::LOOP);
		ldp::Double3 p, n, h;
		handle.GetAnchor_3D(o.ptr(), u.ptr(), v.ptr(), idEa);
		id_l = id_cad_part;
		return true;
	}
	return false;
}

void AbstractMeshEventHandle::updateSvg3dInfo()
{
	ldp::Double3 o, u, v;
	int id_l;
	if (!getPickedMeshFrameInfo(o, u, v, id_l))
		return;
	auto iter = g_dataholder.m_clothLoopId2svgIdMap.find(id_l);
	if (iter != g_dataholder.m_clothLoopId2svgIdMap.end())
	{
		auto obj = g_dataholder.m_svgManager->getObjectById(iter->second);
		if (obj == nullptr)
			throw std::exception("error: loop_to_svg index not valid!");
		if (obj->objectType() != svg::SvgAbstractObject::PolyPath)
			throw std::exception("error: loop_to_svg index not valid!");
		auto poly = (svg::SvgPolyPath*)obj;
		float scale = g_dataholder.m_svgManager->getPixelToMeters();
		poly->set3dCenter(o / scale);
		poly->set3dRot(ldp::QuaternionF().fromRotationMatrix(rotation_from_uv(u, v)));
	} // end if iter
}

void AbstractMeshEventHandle::mousePressEvent(QMouseEvent *ev)
{
	m_mouse_press_pt = ev->pos();

	// arcball drag
	if (ev->buttons() == Qt::LeftButton)
		m_viewer->camera().arcballClick(ldp::Float2(ev->x(), ev->y()));
}

void AbstractMeshEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{

}

void AbstractMeshEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	if (ev->button() == Qt::MouseButton::MiddleButton)
		m_viewer->resetCamera();
}

void AbstractMeshEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	if (ev->buttons() == Qt::LeftButton)
		m_viewer->camera().arcballDrag(ldp::Float2(ev->x(), ev->y()));
	if (ev->buttons() == Qt::MidButton)
	{
		QPoint dif = ev->pos() - m_viewer->lastMousePos();
		ldp::Float3 bmin, bmax;
		m_viewer->getModelBound(bmin, bmax);
		float len = (bmax - bmin).length() / sqrt(3.f);
		ldp::Float3 t(-(float)dif.x() / m_viewer->width(), (float)dif.y() / m_viewer->height(), 0);
		m_viewer->camera().translate(t * len);
		m_viewer->camera().arcballSetCenter((bmin + bmax) / 2.f + t * len);
	}
}

void AbstractMeshEventHandle::wheelEvent(QWheelEvent *ev)
{
	float s = 1.1;
	if (ev->delta() < 0)
		s = 1.f / s;

	float fov = std::max(1e-3f, std::min(160.f, m_viewer->camera().getFov()*s));
	m_viewer->camera().setPerspective(fov, m_viewer->camera().getAspect(),
		m_viewer->camera().getFrustumNear(), m_viewer->camera().getFrustumFar());
}

void AbstractMeshEventHandle::keyPressEvent(QKeyEvent *ev)
{
	switch (ev->key())
	{
	default:
		break;
	case Qt::Key_E:
		if (ev->modifiers() == Qt::NoModifier)
			m_viewer->setEdgeMode(!m_viewer->isEdgeMode());
		break;
	case Qt::Key_B:
		if (ev->modifiers() == Qt::NoModifier && m_viewer->pAnalysis() && m_viewer->pListener())
		{
			if (m_viewer->pAnalysis()->GetMode() == CLOTH_INITIAL_LOCATION){
				m_viewer->pListener()->Solve_fromCad_InitValue();
				m_viewer->pAnalysis()->PerformStaticSolver();
			}
			else
				m_viewer->pAnalysis()->SetClothPiecePlacingMode();
		}
		break;
	}
}

void AbstractMeshEventHandle::keyReleaseEvent(QKeyEvent *ev)
{

}
