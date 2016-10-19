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

void ClothSelectEventHandle::handleEnter()
{
	AbstractMeshEventHandle::handleEnter();
}

void ClothSelectEventHandle::handleLeave()
{
	AbstractMeshEventHandle::handleLeave();
}

void ClothSelectEventHandle::mousePressEvent(QMouseEvent *ev)
{
	AbstractMeshEventHandle::mousePressEvent(ev);

	// pick a point on the mesh
	if (m_viewer->buttons() == Qt::LeftButton && m_viewer->pAnalysis())
	{
		pickMesh(ev->pos());
	} // end if left button
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
}

void ClothSelectEventHandle::keyReleaseEvent(QKeyEvent *ev)
{
	AbstractMeshEventHandle::keyReleaseEvent(ev);
}
