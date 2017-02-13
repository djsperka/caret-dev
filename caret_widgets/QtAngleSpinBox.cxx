#include "QtAngleSpinBox.h"
#include <cstdlib>

QtAngleSpinBox::QtAngleSpinBox(int minValue, int maxValue, int step, QWidget * parent, const char * name)
: QSpinBox(parent)
{
	setMinimum(minValue);
	setMaximum(maxValue);
	setSingleStep(step);
}


QString QtAngleSpinBox::mapValueToText( int value )
{
	QString s;
	return s.sprintf("%d.%03d", value/1000, std::abs(value)%1000);
}

int QtAngleSpinBox::mapTextToValue( bool *ok )
{
    return (int) ( 1000 * text().toFloat() );
}
