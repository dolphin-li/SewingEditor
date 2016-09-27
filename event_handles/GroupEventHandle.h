#pragma once

#include "AbstractEventHandle.h"

class GroupEventHandle :public AbstractEventHandle
{
public:
	GroupEventHandle(SvgViewer* v);
	~GroupEventHandle();
	virtual ProcessorType type() { return ProcessorTypeGroup; }

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
};