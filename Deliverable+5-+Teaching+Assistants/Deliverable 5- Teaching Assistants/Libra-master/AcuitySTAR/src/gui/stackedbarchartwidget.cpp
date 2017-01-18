#include "stackedbarchartwidget.h"

StackedBarChartWidget::StackedBarChartWidget(QWidget *parent) : QWidget(parent)
{

}

void StackedBarChartWidget::setData(std::vector<std::pair<std::string, double>> recordList, QVector<QColor> colors) {
    vRecordList=recordList;
    qvColor = colors;
    repaint();
    update();
}

void StackedBarChartWidget::paintEvent(QPaintEvent *) {

}
