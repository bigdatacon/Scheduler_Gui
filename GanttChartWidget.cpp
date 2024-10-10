#include "GanttChartWidget.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <iostream>
#include <QMessageBox>
#include <QDebug> // Подключите для использования qDebug
#include <QLabel>



GanttChartWidget::GanttChartWidget(QWidget *pParent)
    : QWidget(pParent), m_iDraggedJob(-1), m_iDraggedMachine(-1), m_bJsMode(true) {
    m_pGanttChart = new GanttChart();
    Initialize();

    // Создаем тулбар и кнопки
    m_pToolBar = new QToolBar(this);
    //кнопка Запустить солвер из файл
    m_pSolveButton = new QPushButton("Запустить солвер из файла", this);
    // кнопка для отображения времени по рабочим
    m_pShowTimeButton = new QPushButton("Отобразить время по рабочим", this);
    m_pShowMetricsButton = new QPushButton("Отобразить метрики расписания", this);  // Новая кнопка

    // Добавляем кнопки на тулбар
    m_pToolBar->addWidget(m_pSolveButton);
    m_pToolBar->addWidget(m_pShowTimeButton);
    m_pToolBar->addWidget(m_pShowMetricsButton);  // Добавляем кнопку метрик

    // Создаем вертикальный layout и добавляем тулбар и график в него
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);  // Убираем отступы
    layout->addWidget(m_pToolBar, 0, Qt::AlignLeft);  // Тулбар наверху, выравниваем влево
    layout->addStretch(1);  // Пространство для графика

    setLayout(layout);  // Устанавливаем layout для виджета

    // Подключаем сигнал нажатия кнопки к слоту
    connect(m_pSolveButton, &QPushButton::clicked, this, &GanttChartWidget::OnSolveButtonClicked_2);
    connect(m_pShowTimeButton, &QPushButton::clicked, this, &GanttChartWidget::onWorkersTimeButtonClicked);
    connect(m_pShowTimeButton, &QPushButton::clicked, this, &GanttChartWidget::DrawWorkersTimeChart); // Подключаем сигнал к слоту
    connect(m_pShowMetricsButton, &QPushButton::clicked, this, &GanttChartWidget::OnShowScheduleMetricsClicked);  // Подключаем кнопку метрик

}


GanttChartWidget::~GanttChartWidget() {
    delete m_pGanttChart;
}

void GanttChartWidget::Initialize() {
    resize(1424, 968);
    m_oChartImage = QImage(size(), QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);
}

void GanttChartWidget::OnSolveButtonClicked() {
    // Здесь можно поместить логику для запуска солвера из файла
    QString filename = QFileDialog::getOpenFileName(this, "Выберите файл для солвера", "", "JSON Files (*.json);;All Files (*)");

    if (!filename.isEmpty()) {
        // Создаем немодальное окно с информацией о выбранном файле
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setText("Файл выбран");
        msgBox->setInformativeText("Вы выбрали файл: " + filename);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->setModal(false); // Делаем окно немодальным
        msgBox->show();

        // Загружаем данные из файла
        LoadJsonData(filename);
    } else {
        // Создаем немодальное предупреждение о том, что файл не выбран
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setText("Нет файла");
        msgBox->setInformativeText("Ничего не получено");
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->setModal(false); // Делаем окно немодальным
        msgBox->show();
    }
}



void GanttChartWidget::OnSolveButtonClicked_2() {
    QMessageBox::information(this, "Файл не выбран", "перерисовывю на структурах: " );
    const QString filename;
    LoadJsonData_2(filename);
}


void GanttChartWidget::LoadJsonData(const QString &filename) {
    if (m_pGanttChart) {
        std::cout << "This file_name in LoadJsonData: " << filename.toStdString() << std::endl;
        m_pGanttChart->LoadJsonData(filename);
        DrawGanttChart(); // Перерисовываем диаграмму при загрузке данных
        update();
    }
}

void GanttChartWidget::LoadJsonData_2(const QString &filename) {
    if (m_pGanttChart) {
        std::cout << "This file_name in LoadJsonData: " << filename.toStdString() << std::endl;
        m_pGanttChart->LoadJsonData_2(filename);
        DrawGanttChart(); // Перерисовываем диаграмму при загрузке данных
        update();
    }
}

void GanttChartWidget::DrawGanttChart() {
    // Перед изменением изображения и вызовом QPainter убедимся, что изображение чистое
    m_oChartImage = QImage(size(), QImage::Format_ARGB32);  // Обновляем размеры изображения
    m_oChartImage.fill(Qt::white);  // Очищаем изображение

    // Создаем QPainter для работы с m_oChartImage
    QPainter oPainter(&m_oChartImage);

    // Убедимся, что QPainter активен
    if (!oPainter.isActive()) {
        qWarning("QPainter on m_oChartImage is not active");
        return;
    }

    // Передаем размеры виджета в функцию DrawGanttChart
    if (m_pGanttChart) {
        m_pGanttChart->DrawGanttChart(&oPainter, width(), height());
    }

    // Обновляем виджет для отображения
    update();  // Обновляем экран
}

void GanttChartWidget::DrawWorkersTimeChart() {
    QPainter painter(&m_oWorkersImage);
    if (!painter.isActive()) {
        qWarning("QPainter on m_oWorkersImage is not active");
        return;
    }

    if (m_pGanttChart) {
        m_pGanttChart->DrawWorkersTimeChart(&painter, m_oWorkersImage.width(), m_oWorkersImage.height());
    }

    update();
}


//void GanttChartWidget::paintEvent(QPaintEvent *event) {
//    QPainter oPainter(this);

//    // Рисуем сохраненную диаграмму с учетом тулбара
//    int toolbarHeight = m_pToolBar->height();

//    if (m_bDisplayingWorkersTimeChart) {
//        oPainter.drawImage(0, toolbarHeight, m_oWorkersImage);  // Отрисовываем workers chart
//    } else {
//        oPainter.drawImage(0, toolbarHeight, m_oChartImage);  // Отрисовываем Gantt chart
//    }
//}

void GanttChartWidget::paintEvent(QPaintEvent *event) {
    QPainter oPainter(this);

    // Рисуем с учетом высоты тулбара
    int toolbarHeight = m_pToolBar->height();
//    oPainter.translate(0, toolbarHeight); // Смещаем систему координат

    if (m_bDisplayingWorkersTimeChart) {
        // Отрисовываем workers chart
        m_pGanttChart->DrawWorkersTimeChart(&oPainter, width(), height() - toolbarHeight);
    } else {
        // Отрисовываем Gantt chart
        m_pGanttChart->DrawGanttChart(&oPainter, width(), height() - toolbarHeight);
    }
}



void GanttChartWidget::resizeEvent(QResizeEvent *event) {
    int toolbarHeight = m_pToolBar->height();

    // Обновляем размеры изображений для графиков
    m_oChartImage = QImage(event->size().width(), event->size().height() - toolbarHeight, QImage::Format_ARGB32);
    m_oWorkersImage = QImage(event->size().width(), event->size().height() - toolbarHeight, QImage::Format_ARGB32);
    m_oChartImage.fill(Qt::white);
    m_oWorkersImage.fill(Qt::white);

    // Отрисовываем активный график
    if (m_bDisplayingWorkersTimeChart) {
        DrawWorkersTimeChart();
    } else {
        DrawGanttChart();
    }
}




void GanttChartWidget::onWorkersTimeButtonClicked() {
    m_bDisplayingWorkersTimeChart = !m_bDisplayingWorkersTimeChart;
    updateChart();
}

// Update the chart based on the current display flag
void GanttChartWidget::updateChart() {
    resizeEvent(new QResizeEvent(size(), size()));
}


void GanttChartWidget::mousePressEvent(QMouseEvent *event) {
    QPoint clickPos = event->pos();
    bool barClicked = false;

    // Проверяем, что объект GanttChart доступен
    if (!m_pGanttChart) {
        qDebug() << "m_pGanttChart не инициализирован";
        return;
    }

    // Используем геттеры для доступа к операциям
    auto &msOperations = m_pGanttChart->getMsOperations();
    auto &jsOperations = m_pGanttChart->getJsOperations();

    // Логируем координаты клика
    qDebug() << "Координаты клика: " << clickPos;

    // --- Логика для кликов по нижнему графику (jsOperations) ---
    if (event->button() == Qt::LeftButton) {
        for (auto &sOp : jsOperations) {
            qDebug() << "Проверяем бар: Д" << sOp.iJob << ", QRect:" << sOp.rect;

            if (sOp.rect.contains(clickPos)) {
                qDebug() << "Клик по бару: Д" << sOp.iJob;

                // Снимаем выделение со всех баров перед установкой выделения на нужный
                for (auto &jobOp : jsOperations) {
                    jobOp.bHighlighted = false;
                }
                for (auto &machineOp : msOperations) {
                    machineOp.bHighlighted = false;
                }

                // Бар найден — выделяем его
                sOp.bHighlighted = true;

                // Также находим все связанные бары на верхнем графике и выделяем их
                for (auto &machineIndex : sOp.vMachinesIndexes) {
                    for (auto &mOp : msOperations) {
                        if (mOp.iMachine == machineIndex && mOp.iJob == sOp.iJob &&  sOp.iStart == mOp.iStart && sOp.iFinish == mOp.iFinish) {
                            mOp.bHighlighted = true;
                            qDebug() << "Выделен бар на верхнем графике: Машина" << mOp.iMachine;
                        }
                    }
                }

                barClicked = true;
                break; // Прекращаем поиск, так как нашли нужный бар
            }
        }
    }

    // --- Логика для кликов по верхнему графику (msOperations) ---
    if (!barClicked && event->button() == Qt::LeftButton) { // Если не кликнули по нижнему графику, проверяем верхний
        for (auto &mOp : msOperations) {
            qDebug() << "Проверяем бар на верхнем графике: Машина" << mOp.iMachine << ", QRect:" << mOp.rect;

            if (mOp.rect.contains(clickPos)) {
                qDebug() << "Клик по бару на верхнем графике: Машина" << mOp.iMachine;

                // Снимаем выделение со всех баров
                for (auto &jobOp : jsOperations) {
                    jobOp.bHighlighted = false;
                }
                for (auto &machineOp : msOperations) {
                    machineOp.bHighlighted = false;
                }

                // Выделяем кликнутый бар на верхнем графике
                mOp.bHighlighted = true;

                // Теперь находим все связанные бары по совпадению iJob, iStart и iFinish
                for (auto &mRelatedOp : msOperations) {
                    if (mRelatedOp.iJob == mOp.iJob && mRelatedOp.iStart == mOp.iStart && mRelatedOp.iFinish == mOp.iFinish) {
                        mRelatedOp.bHighlighted = true;
                        qDebug() << "Выделен связанный бар: Машина" << mRelatedOp.iMachine
                                 << " для задания Д" << mRelatedOp.iJob
                                 << " с началом " << mRelatedOp.iStart
                                 << " и окончанием " << mRelatedOp.iFinish;
                    }
                }

                // Аналогично, выделяем связанные бары на нижнем графике
                for (auto &sOp : jsOperations) {
                    if (sOp.iJob == mOp.iJob && (std::find(sOp.vMachinesIndexes.begin(), sOp.vMachinesIndexes.end(), mOp.iMachine) != sOp.vMachinesIndexes.end())
                            &&  sOp.iStart == mOp.iStart && sOp.iFinish == mOp.iFinish
                            ) {
                        sOp.bHighlighted = true;
                        qDebug() << "Выделен бар на нижнем графике: Д" << sOp.iJob;
                    }
                }
                barClicked = true;
                break; // Прекращаем поиск
            }
        }
    }


    if (!barClicked) {
        qDebug() << "Клик вне баров, снятие выделения";

        // Снимаем выделение со всех баров, если кликнули вне какого-либо из них
        for (auto &jobOp : jsOperations) {
            jobOp.bHighlighted = false;
        }
        for (auto &machineOp : msOperations) {
            machineOp.bHighlighted = false;
        }
    }

    qDebug() << "Перерисовка диаграммы";
    if (event->button() == Qt::RightButton) {
        // Обработка правого клика, вызываем отдельную функцию
        handleRightClick(clickPos, jsOperations, msOperations);
    }
    update();
}


void GanttChartWidget::handleRightClick(QPoint clickPos, std::vector<SJobOperation> &jsOperations, std::vector<SMachineOperation> &msOperations) {
    // Проверка на клики по нижнему графику
    for (auto &sOp : jsOperations) {
        if (sOp.rect.contains(clickPos)) {
            qDebug() << "Правый клик по бару: Д" << sOp.iJob;

            // Формируем текст для отображения информации о баре
            QString info = QString("Деталь: %1\nНачало: %2\nКонец: %3\nМашины: ")
                            .arg(sOp.iJob)
                            .arg(sOp.iStart)
                            .arg(sOp.iFinish);

            // Добавляем индексы машин
            for (int machineIndex : sOp.vMachinesIndexes) {
                info.append(QString("%1, ").arg(machineIndex));
            }
            if (!sOp.vMachinesIndexes.empty()) {
                info.chop(2);
            }

            // Создаем немодальное окно с информацией
//            QMessageBox *msgBox = new QMessageBox(this);
//            msgBox->setText("Информация о детали");
//            msgBox->setInformativeText(info);
////            msgBox->setStandardButtons(QMessageBox::Ok);
//            msgBox->setModal(false); // Делаем окно немодальным
//            msgBox->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint); // Устанавливаем флаги для закрытия при клике вне окна
//            msgBox->show();

            // Создаем немодальное окно с информацией
            QDialog *dialog = new QDialog(this);
            dialog->setWindowTitle("Информация о детали");
            dialog->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint); // Устанавливаем флаг для закрытия при клике вне окна
            dialog->setAttribute(Qt::WA_DeleteOnClose); // Удаляем виджет при закрытии

            // Создаем QLabel для отображения информации
            QVBoxLayout *layout = new QVBoxLayout();
            QLabel *infoLabel = new QLabel(info, dialog);
            layout->addWidget(infoLabel);
            dialog->setLayout(layout);

            // Показываем диалог
            dialog->show();



            return; // Завершаем выполнение функции после обработки
        }
    }

    // Проверка на клики по верхнему графику
    for (auto &mOp : msOperations) {
        if (mOp.rect.contains(clickPos)) {
            qDebug() << "Правый клик по бару на верхнем графике: Машина" << mOp.iMachine;

            // Формируем текст для отображения информации о баре
            QString info = QString("Машина: %1\nНачало: %2\nКонец: %3\nРабота: %4")
                            .arg(mOp.iMachine)
                            .arg(mOp.iStart)
                            .arg(mOp.iFinish)
                            .arg(mOp.iJob);

            // Создаем немодальное окно с информацией
//            QMessageBox *msgBox = new QMessageBox(this);
//            msgBox->setText("Информация о машине");
//            msgBox->setInformativeText(info);
////            msgBox->setStandardButtons(QMessageBox::Ok);
//            msgBox->setModal(false); // Делаем окно немодальным
//            msgBox->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint); // Устанавливаем флаги для закрытия при клике вне окн
//            msgBox->show();


            // Создаем немодальное окно с информацией
            QDialog *dialog = new QDialog(this);
            dialog->setWindowTitle("Информация о детали");
            dialog->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint); // Устанавливаем флаг для закрытия при клике вне окна
            dialog->setAttribute(Qt::WA_DeleteOnClose); // Удаляем виджет при закрытии

            // Создаем QLabel для отображения информации
            QVBoxLayout *layout = new QVBoxLayout();
            QLabel *infoLabel = new QLabel(info, dialog);
            layout->addWidget(infoLabel);
            dialog->setLayout(layout);

            // Показываем диалог
            dialog->show();


            return; // Завершаем выполнение функции после обработки
        }
    }
}

void GanttChartWidget::OnShowScheduleMetricsClicked() {
    // Создаем новое диалоговое окно для отображения данных
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Метрики расписания");
    dialog->resize(600, 400);  // Устанавливаем размер окна

    // Создаем текстовое поле для отображения данных
    QTextEdit* textEdit = new QTextEdit(dialog);
    textEdit->setReadOnly(true);  // Делаем поле только для чтения

    // Добавляем текст
    QString dataText;
    for (int i = 0; i < 1000; ++i) {
        dataText += QString("Строка %1: Время = %2, Стоимость = %3, Отклонение = %4, Утилизация = %5, Свободные машины = %6\n")
                        .arg(i + 1)
                        .arg(m_ScheduleMetrics.iScheduleTime)
                        .arg(m_ScheduleMetrics.iScheduleCost)
                        .arg(m_ScheduleMetrics.iSumDeviation)
                        .arg(m_ScheduleMetrics.iDeltaUtilization)
                        .arg(m_ScheduleMetrics.iFreeMachinesCount);
    }
    textEdit->setText(dataText);

    // Добавляем вертикальную и горизонтальную полосу прокрутки
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Устанавливаем layout в диалоговом окне
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialog);
    dialogLayout->addWidget(textEdit);
    dialog->setLayout(dialogLayout);

    // Показываем диалог
    dialog->exec();
}



void GanttChartWidget::mouseMoveEvent(QMouseEvent *event) {
    // Логика по перетаскиванию баров
}
