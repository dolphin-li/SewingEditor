#include <QEvent>
#include <GL\glew.h>
#include "BaseMeshViewer.h"

#include "designer2d_cloth.h"
#include "analysis2d_cloth_static.h"

#include "AbstractMeshEventHandle.h"
#include "ClothSelectEventHandle.h"
#include "ClothTranslateEventHandle.h"
#include "ClothRotateEventHandle.h"

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

void AbstractMeshEventHandle::mousePressEvent(QMouseEvent *ev)
{
	m_mouse_press_pt = ev->pos();

	// arcball drag
	if (ev->buttons() == Qt::LeftButton)
		m_viewer->camera().arcballClick(ldp::Float2(ev->x(), ev->y()));

	// pick a point on the mesh
	if (m_viewer->buttons() == Qt::LeftButton && m_viewer->pAnalysis())
	{
		const ldp::Camera& cam = m_viewer->camera();
		ldp::UInt3 tri_id;
		ldp::Double3 tri_coord;
		unsigned int id_l;
		bool res = m_viewer->pAnalysis()->Pick(ldp::Float2(ev->x(), m_viewer->height() - 1 - ev->y()),
			cam, tri_id, tri_coord, id_l, m_picked_screenDepth);
		if (!res){
			m_viewer->pListener()->HilightCadTypeID(Cad::NOT_SET, 0);
			m_viewer->pListener()->Cad_SetPicked(Cad::NOT_SET, 0, 0, 0);
		}
		else{
			m_viewer->pListener()->HilightCadTypeID(Cad::LOOP, id_l);
			m_viewer->pListener()->Cad_SetPicked(Cad::LOOP, id_l, 0, 0);
		}
	} // end if left button
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