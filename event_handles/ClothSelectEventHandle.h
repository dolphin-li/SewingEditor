#pragma once

#include <QPoint>
#include <QCursor>
#include <QString>
#include "AbstractMeshEventHandle.h"
class ClothSelectEventHandle : public AbstractMeshEventHandle
{
public:
	ClothSelectEventHandle(BaseMeshViewer* v);
	~ClothSelectEventHandle();
	virtual ProcessorType type() { return ProcessorTypeClothSelect; }
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
	virtual void handleEnter();
	virtual void handleLeave();
};