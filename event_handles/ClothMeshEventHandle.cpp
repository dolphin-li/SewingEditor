#include <QEvent>
#include <GL\glew.h>
#include "BaseMeshViewer.h"

#include "ClothMeshEventHandle.h"
#include "designer2d_cloth.h"
#include "analysis2d_cloth_static.h"

ClothMeshEventHandle::ClothMeshEventHandle(BaseMeshViewer* v) : AbstractMeshEventHandle(v)
{
	m_cursor = QCursor(Qt::CursorShape::ArrowCursor);
	m_iconFile = "";
	m_toolTips = "cloth handle";
}

ClothMeshEventHandle::~ClothMeshEventHandle()
{

}

void ClothMeshEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mousePressEvent(ev);
}

void ClothMeshEventHandle::mouseReleaseEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseReleaseEvent(ev);
}

void ClothMeshEventHandle::mouseDoubleClickEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseDoubleClickEvent(ev);
}

void ClothMeshEventHandle::mouseMoveEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mouseMoveEvent(ev);
}

void ClothMeshEventHandle::wheelEvent(QWheelEvent *ev)
{
	AbstractMeshEventHandle::wheelEvent(ev);
}

void ClothMeshEventHandle::keyPressEvent(QKeyEvent *ev)
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

void ClothMeshEventHandle::keyReleaseEvent(QKeyEvent *ev)
{
	AbstractMeshEventHandle::keyReleaseEvent(ev);
}
