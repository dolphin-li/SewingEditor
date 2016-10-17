#include <QEvent>
#include <GL\glew.h>
#include "BaseMeshViewer.h"

#include "ClothSelectEventHandle.h"
#include "designer2d_cloth.h"
#include "analysis2d_cloth_static.h"

ClothSelectEventHandle::ClothSelectEventHandle(BaseMeshViewer* v) : AbstractMeshEventHandle(v)
{
	m_cursor = QCursor(Qt::CursorShape::ArrowCursor);
	m_iconFile = "icons/selection.png";
	m_inactiveIconFile = "icons/selection_inactive.png";
	m_toolTips = "cloth handle";
}

ClothSelectEventHandle::~ClothSelectEventHandle()
{

}

void ClothSelectEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mousePressEvent(ev);
}

void ClothSelectEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseReleaseEvent(ev);
}

void ClothSelectEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseDoubleClickEvent(ev);
}

void ClothSelectEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseMoveEvent(ev);
}

void ClothSelectEventHandle::wheelEvent(QWheelEvent *ev)
{
	AbstractMeshEventHandle::wheelEvent(ev);
}

void ClothSelectEventHandle::keyPressEvent(QKeyEvent *ev)
{
	AbstractMeshEventHandle::keyPressEvent(ev);
	switch (ev->key())
	{
	default:
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

void ClothSelectEventHandle::keyReleaseEvent(QKeyEvent *ev)
{
	AbstractMeshEventHandle::keyReleaseEvent(ev);
}
