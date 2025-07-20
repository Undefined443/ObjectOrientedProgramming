#ifndef CHART_H
#define CHART_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QString>
#include <QVector>
#include <QWidget>
#include <QtCharts>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class Chart : public QWidget {
   public:
    friend class MainWindow;

    Chart(int _elevator_num, QWidget *parent = nullptr);

    void set_elevator_statistics(int elevator, QVector<long long> elevator_statistics);  // update elevator statistics

    void set_passenger_statistics(QVector<long long> passenger_statistics);  // update passenger statistics

   private:
    Q_OBJECT
    int elevator_num;
    QVector<QPieSeries *> pie_series_vector;
    QBarSet *bar_set;
    QValueAxis *axisY;

   signals:
    void elevator_statistics_signal(int elevator, QVector<long long> elevator_statistics);

    void passenger_statistics_signal(QVector<long long> passenger_statistics);

   private slots:
    void elevator_statistics_slot(int elevator, QVector<long long> elevator_statistics);

    void passenger_statistics_slot(QVector<long long> passenger_statistics);
};

#endif  // CHART_H
