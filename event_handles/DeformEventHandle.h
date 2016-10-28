#pragma once

#include "AbstractEventHandle.h"
class DeformEventHandle :public AbstractEventHandle
{
public:
	DeformEventHandle(SvgViewer* v);
	~DeformEventHandle();
	virtual ProcessorType type() { return ProcessorTypeDeform; }

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
};