#ifndef SLIDERWIDGET_HPP
#define SLIDERWIDGET_HPP

#include "ui_sliderWidget.h"
#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{


class SliderWidget : public QWidget
{
	Q_OBJECT
public:
	enum SliderType { Opacity, UpperThreshold, LowerThreshold };
	SliderWidget( QWidget *parent, QViewerCore *core );

	void setVisible( SliderType, bool );

public Q_SLOTS:
	void opacityChanged( int );
	void lowerThresholdChanged( int );
	void upperThresholdChanged( int );
	void synchronize();

private:
	double norm( const double &min, const double &max, const int &pos );
	Ui::sliderwidget m_Interface;
	QViewerCore *m_ViewerCore;

};

}
}
}


#endif