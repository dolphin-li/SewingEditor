#pragma once

#include <QPoint>
#include <QCursor>
#include <QString>

class SimViewer;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class AbstractSimEventHandle
{
public:
	enum ProcessorType{
		ProcessorTypeGeneral = 0,
		ProcessorTypeEnd, // the end, no processor for this
	};
public:
	AbstractSimEventHandle(SimViewer* v);
	~AbstractSimEventHandle();
	virtual ProcessorType type() { return ProcessorTypeGeneral; }
	static AbstractSimEventHandle* create(ProcessorType type, SimViewer* v);
	QCursor& cursor(){ return m_cursor; }
	const QCursor& cursor()const{ return m_cursor; }
	QString iconFile()const;
	QString inactiveIconFile()const;
	QString toolTips()const;

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
	virtual void handleEnter();
	virtual void handleLeave();

	void pickMesh(QPoint p, bool reset_if_none_picked = false);

	static ldp::Mat3d rotation_from_uv(ldp::Double3 u, ldp::Double3 v);
protected:
	SimViewer* m_viewer;
	int m_lastHighlightShapeId;
	int m_currentSelectedId;
	QPoint m_mouse_press_pt;
	QCursor m_cursor;
	QString m_iconFile;
	QString m_inactiveIconFile;
	QString m_toolTips;
	ldp::Double3 m_picked_screenPos;
};