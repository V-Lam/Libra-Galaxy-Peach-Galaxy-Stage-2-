#ifndef STACKEDBARCHARTWIDGET_H
#define STACKEDBARCHARTWIDGET_H

#include <QWidget>
#include <vector>

typedef std::vector<std::string> BasicRecord;
class StackedBarChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StackedBarChartWidget(QWidget *parent = 0);
    void setData(std::vector<std::pair<std::string, double>> recordList, QVector<QColor> colors);
protected:
    void paintEvent(QPaintEvent *);
private:

    std::vector<std::pair<std::string, double>> vRecordList;
    QVector<QColor> qvColor;

signals:

public slots:
};

#endif // STACKEDBARCHARTWIDGET_H
