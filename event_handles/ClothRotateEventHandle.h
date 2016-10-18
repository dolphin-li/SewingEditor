#pragma once

#include <QPoint>
#include <QCursor>
#include <QString>
#include "AbstractMeshEventHandle.h"

class ClothRotateEventHandle : public AbstractMeshEventHandle
{
public:
	ClothRotateEventHandle(BaseMeshViewer* v);
	~ClothRotateEventHandle();
	virtual ProcessorType type() { return ProcessorTypeClothRotate; }
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
protected:
	ldp::Mat3f m_trackBallMouseClickR;
};