#pragma once

#include <QPoint>
#include <QCursor>
#include <QString>
#include "AbstractMeshEventHandle.h"
class ClothMeshEventHandle : public AbstractMeshEventHandle
{
public:
	ClothMeshEventHandle(BaseMeshViewer* v);
	~ClothMeshEventHandle();
	virtual ProcessorType type() { return ProcessorTypeCloth; }
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
};