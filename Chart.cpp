#include "Chart.h"
#include <exception>
#include <QtCharts>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QtWidgets>
#include <QVector>

const int COLUMN = 5;
const int HEIGHT = 250;

Chart::Chart(int _elevator_num, QWidget *parent) :
    elevator_num(_elevator_num),
    pie_series_vector(elevator_num),
    bar_set(new QBarSet("")),
    axisY(new QValueAxis()),
    QWidget(parent) {

    // Set widgets
    auto elevator_panel = new QWidget(this);

    // Set layout
    auto grid_layout = new QGridLayout(elevator_panel);

    QVector<QChartView *> chart_view_vector(elevator_num);

    // Set elevator panel
    for (int i = 0; i < elevator_num; ++i) {
        chart_view_vector[i] = new QChartView(elevator_panel);
        pie_series_vector[i] = new QPieSeries(chart_view_vector[i]);

        chart_view_vector[i]->setMinimumHeight(HEIGHT);
        chart_view_vector[i]->setMinimumWidth(HEIGHT);

        pie_series_vector[i]->setHoleSize(0.35);

        chart_view_vector[i]->setRenderHint(QPainter::Antialiasing);
        chart_view_vector[i]->chart()->setTitle("E" + QString::number(i + 1));
        chart_view_vector[i]->chart()->addSeries(pie_series_vector[i]);
        chart_view_vector[i]->chart()->legend()->setAlignment(Qt::AlignBottom);
        chart_view_vector[i]->chart()->setTheme(QChart::ChartThemeBlueCerulean);
        chart_view_vector[i]->chart()->legend()->setFont(QFont("Arial", 10));
//        chart_view_vector->chart()->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

        grid_layout->addWidget(chart_view_vector[i], i / COLUMN, i % COLUMN);
    }
    elevator_panel->setLayout(grid_layout);

    // Set passenger panel
    auto series = new QBarSeries();
    series->append(bar_set);

    auto chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Passenger Waiting Time");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "Mean" << "Maximum" << "Mode" << "Median";

    auto axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY->setLabelFormat("%d s");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    auto chart_view = new QChartView(chart);
    chart_view->setRenderHint(QPainter::Antialiasing);
    chart_view->setMinimumHeight(HEIGHT);

    grid_layout->addWidget(chart_view, elevator_num / COLUMN, elevator_num % COLUMN, 1, 2);

    setLayout(grid_layout);

    // Initialize
    // Elevator
    for (int i = 0; i < elevator_num; ++i) {
        pie_series_vector[i]->append("Idle: 0 s", 1);
        pie_series_vector[i]->append("Running: 0 s", 1);
    }
    // Passenger
    *bar_set << 0 << 0 << 0 << 0;
    axisY->setRange(0, 1);

    // Connect signals and slots
    connect(this, &Chart::elevator_statistics_signal, this, &Chart::elevator_statistics_slot);
    connect(this, &Chart::passenger_statistics_signal, this, &Chart::passenger_statistics_slot);
}

// Emitters
void Chart::set_elevator_statistics(int elevator, QVector<long long> elevator_statistics) {
    emit elevator_statistics_signal(elevator, elevator_statistics);
}

void Chart::set_passenger_statistics(QVector<long long> passenger_statistics) {
    emit passenger_statistics_signal(passenger_statistics);
}

// Slots
void Chart::elevator_statistics_slot(int elevator, QVector<long long> elevator_statistics) {
    if ((long long) pie_series_vector[elevator]->slices()[0]->value() != elevator_statistics[0]) {  // got a new value
        pie_series_vector[elevator]->slices()[0]->setValue(qreal(elevator_statistics[0]));
        auto idle_time_str = "Idle: " + QString::number(elevator_statistics[0] / 1000) + " s";
        if (pie_series_vector[elevator]->slices()[0]->label() != idle_time_str) {  // need to update label
            pie_series_vector[elevator]->slices()[0]->setLabel(idle_time_str);
        }
    }
    if ((long long) pie_series_vector[elevator]->slices()[1]->value() != elevator_statistics[1]) {  // got a new value
        pie_series_vector[elevator]->slices()[1]->setValue(qreal(elevator_statistics[1]));
        auto running_time_str = "Running: " + QString::number(elevator_statistics[1] / 1000) + " s";
        if (pie_series_vector[elevator]->slices()[1]->label() != running_time_str) {  // need to update label
            pie_series_vector[elevator]->slices()[1]->setLabel(running_time_str);
        }
    }
}

void Chart::passenger_statistics_slot(QVector<long long> passenger_statistics) {
    if (passenger_statistics.size() != 4) {
        throw std::invalid_argument("QVector size must be 4");
    }
    for (int i = 0; i < 4; ++i) {
        auto new_value = qreal(passenger_statistics[i]) / 1000.0;
        if (new_value != bar_set->at(i)) {
            bar_set->replace(i, new_value);
            if (i == 1) {  // maximum value changed
                axisY->setRange(0, new_value * 1.1);
            }
        }
    }
}
