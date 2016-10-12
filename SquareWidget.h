#pragma once

#include <QWidget>
#include <QBoxLayout>

class SquareWidget : public QWidget
{
public:
	SquareWidget(QWidget *parent = 0);
	void setChildWidget(QWidget* widget, qreal aspect=1);
	void resizeEvent(QResizeEvent *event);
	void setAspect(qreal aspect);

	void adjustChildWidget();
private:
	QBoxLayout *m_layout;
	qreal m_aspect;
};