#pragma once

#include "AbstractEventHandle.h"

class ShapeEventHandle :public AbstractEventHandle
{
public:
	ShapeEventHandle(SvgViewer* v);
	~ShapeEventHandle();
	virtual ProcessorType type() { return ProcessorTypeShape; }

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
};