#include "SquareWidget.h"
#include "qevent.h"
SquareWidget::SquareWidget(QWidget *parent) : QWidget(parent)
{
	m_layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
	m_aspect = 1;
}

void SquareWidget::setChildWidget(QWidget* widget, qreal aspect)
{
	m_layout->addItem(new QSpacerItem(0, 0));
	m_layout->addWidget(widget);
	m_layout->addItem(new QSpacerItem(0, 0));
	setAspect(aspect);
}

void  SquareWidget::setAspect(qreal aspect)
{
	m_aspect = aspect;
}

void SquareWidget::resizeEvent(QResizeEvent *event)
{
	adjustChildWidget();
}

void SquareWidget::adjustChildWidget()
{
	float thisAspectRatio = (float)width() / height();
	int widgetStretch, outerStretch;

	if (thisAspectRatio > m_aspect) // too wide
	{
		m_layout->setDirection(QBoxLayout::LeftToRight);
		widgetStretch = height() * m_aspect; // i.e., my width
		outerStretch = (width() - widgetStretch) / 2 + 0.5;
	}
	else // too tall
	{
		m_layout->setDirection(QBoxLayout::TopToBottom);
		widgetStretch = width() / m_aspect; // i.e., my height
		outerStretch = (height() - widgetStretch) / 2 + 0.5;
	}

	m_layout->setStretch(0, outerStretch);
	m_layout->setStretch(1, widgetStretch);
	m_layout->setStretch(2, outerStretch);
}