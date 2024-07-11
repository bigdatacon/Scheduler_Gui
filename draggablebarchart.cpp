#include "draggablebarchart.h"

DraggableBarChart::DraggableBarChart(QChart *chart, QWidget *parent)
    : QChartView(chart, parent), m_barSet(nullptr) {
    setRenderHint(QPainter::Antialiasing);
}

void DraggableBarChart::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPointF clickPos = chart()->mapToValue(event->pos());
        m_barSet = findBarSet(clickPos);
        if (m_barSet) {
            m_startPos = clickPos;
        }
    }
    QChartView::mousePressEvent(event);
}

void DraggableBarChart::mouseMoveEvent(QMouseEvent *event) {
    if (m_barSet) {
        QPointF movePos = chart()->mapToValue(event->pos());
        qreal deltaX = movePos.x() - m_startPos.x();
        for (int i = 0; i < m_barSet->count(); ++i) {
            m_barSet->replace(i, m_barSet->at(i) + deltaX);
        }
        m_startPos = movePos;
    }
    QChartView::mouseMoveEvent(event);
}

void DraggableBarChart::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_barSet = nullptr;
    }
    QChartView::mouseReleaseEvent(event);
}

QBarSet* DraggableBarChart::findBarSet(const QPointF &point) {
    foreach (QAbstractSeries *series, chart()->series()) {
        QBarSeries *barSeries = qobject_cast<QBarSeries *>(series);
        if (barSeries) {
            foreach (QBarSet *barSet, barSeries->barSets()) {
                for (int i = 0; i < barSet->count(); ++i) {
                    QPointF barPos = chart()->mapToPosition(QPointF(barSet->at(i), i));
                    QRectF barRect(barPos.x() - 5, barPos.y() - 5, 10, 10); // Установите ширину и высоту бара
                    if (barRect.contains(point)) {
                        return barSet;
                    }
                }
            }
        }
    }
    return nullptr;
}
