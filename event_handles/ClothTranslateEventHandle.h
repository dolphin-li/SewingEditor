#pragma once

#include <QPoint>
#include <QCursor>
#include <QString>
#include "AbstractMeshEventHandle.h"
class ClothTranslateEventHandle : public AbstractMeshEventHandle
{
public:
	ClothTranslateEventHandle(BaseMeshViewer* v);
	~ClothTranslateEventHandle();
	virtual ProcessorType type() { return ProcessorTypeClothTranslate; }
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
};