#pragma once

#include "AbstractEventHandle.h"
#include <QPoint>
class ZoomEventHandle :public AbstractEventHandle
{
public:
	ZoomEventHandle(SvgViewer* v);
	~ZoomEventHandle();
	virtual ProcessorType type() { return ProcessorTypeZoom; }

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
protected:
	QPoint m_mouse_press_pt;
};