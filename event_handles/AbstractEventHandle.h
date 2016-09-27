#pragma once

class SvgViewer;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

class AbstractEventHandle
{
public:
	enum ProcessorType{
		ProcessorTypeGeneral = 0,
		ProcessorTypeShape,
		ProcessorTypeGroup,
		ProcessorTypeZoom,

		ProcessorTypeEnd, // the end, no processor for this
	};
public:
	AbstractEventHandle(SvgViewer* v);
	~AbstractEventHandle();
	virtual ProcessorType type() { return ProcessorTypeGeneral; }
	static AbstractEventHandle* create(ProcessorType type, SvgViewer* v);

	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void wheelEvent(QWheelEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void keyReleaseEvent(QKeyEvent*);
protected:
	SvgViewer* m_viewer;
	int m_lastHighlightShapeId;
};