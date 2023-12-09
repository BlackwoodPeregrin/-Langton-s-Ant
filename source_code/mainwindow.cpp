#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDialog>
#include <QDebug>
#include <QProgressBar>
#include <QMessageBox>

#include "AntRunner.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isAntBreaked(false)
{
    ui->setupUi(this);

    QSize sizeFreeField = this->size() - ui->centralwidget->size();

    widthSideField = std::min(sizeFreeField.width(), sizeFreeField.height());

    int sumCells = getSumCellsInField();
    field = generateField(sumCells, widthSideField / sumCells);

    antRunner = new AntRunner(field, getAntSpeedStepMs(), getAntRouteDirection(), mutex);

    drawAnt(antRunner->getPosX(), antRunner->getPosY());

    connect(antRunner, SIGNAL(prevPosAnt(int, int)), this, SLOT(deleteAnt(int, int)));
    connect(antRunner, SIGNAL(nextPosAnt(int, int)), this, SLOT(drawAnt(int, int)));
    connect(antRunner, SIGNAL(antBreaked()), this, SLOT(on_gameover()));
}

MainWindow::~MainWindow() {
    delete ui;
    if (antRunner->isRunning()) {
        antRunner->stop();
    }
    delete antRunner;
}

void MainWindow::on_button_start_clicked() {
    if (isAntBreaked) {
        clearField();

        int sumCells = getSumCellsInField();
        field = generateField(sumCells, widthSideField / sumCells);

        antRunner->changeField(field, getAntRouteDirection());

        isAntBreaked = false;
    }

    if (antRunner->isRunning() == false) {
        ui->button_start->setText("STOP");
        hideSettingFieldWidgets();
        antRunner->start();
    } else {
        antRunner->stop();
        ui->button_start->setText("START");
        showSettingFieldWidgets();
    }
}

void MainWindow::on_gameover() {
    std::lock_guard<std::mutex> l(mutex);

    isAntBreaked = true;
    antRunner->stop();

    ui->button_start->setText("START");
    showSettingFieldWidgets();

    QMessageBox msgBox;
    msgBox.setText("Ant is dead.");
    msgBox.exec();
}

void MainWindow::on_fieldSize_currentIndexChanged(const QString &size) {
    clearField();

    int sumCells = size.toInt();
    field = generateField(sumCells, widthSideField / sumCells);

    antRunner->changeField(field, getAntRouteDirection());

    isAntBreaked = false;
}

void MainWindow::on_direction_currentIndexChanged(int index) {
    int x = antRunner->getPosX();
    int y = antRunner->getPosY();
    field[x][y]->setIcon(getIconByDirecton(getAntRouteDirection()));
}

void MainWindow::on_ms_currentIndexChanged(const QString &arg1) {
    antRunner->setAntSpeedStep(arg1.toInt());
}

void MainWindow::clearField() {
    while (QLayoutItem *item = ui->field->takeAt(0)) {
        if (QWidget *w = item->widget()) {
            delete w;
            w = nullptr;
        }
        delete item;
        item = nullptr;
    }

    field.clear();
}

Field MainWindow::generateField(int sumCells, int sizeCell) {
    Field field;
    for (int row = 0; row < sumCells; ++row) {
        field.push_back(std::vector<QPushButton*>());

        for (int column = 0; column < sumCells; ++column) {
            QPushButton* button = new QPushButton();
            button->setMinimumHeight(sizeCell);
            button->setMinimumWidth(sizeCell);
            button->setStyleSheet(whiteCellStyle);
            button->setEnabled(false);
            button->setIconSize(QSize(sizeCell, sizeCell));
            ui->field->addWidget(button, row, column);

            field.back().push_back(button);
        }
    }
    return field;
}

void MainWindow::deleteAnt(int posX, int posY) {
    std::lock_guard<std::mutex> lock(mutex);

    field[posX][posY]->setIcon(QIcon());
}

void MainWindow::drawAnt(int posX, int posY) {
    std::lock_guard<std::mutex> lock(mutex);

    field[posX][posY]->setIcon(getIconByDirecton(antRunner->getDirection()));
}

int MainWindow::getAntSpeedStepMs() {
    return ui->ms->currentText().toInt();
}

Route MainWindow::getAntRouteDirection() {
    int idx = ui->direction->currentIndex();
    switch(idx) {
        case 0:
            return Route::kRight;
        case 1:
            return Route::kDown;
        case 2:
            return Route::kLeft;
        case 3:
            return Route::kUp;
        default:
            throw std::invalid_argument("Not supported direction index: '" + std::to_string(idx));
    }
}

int MainWindow::getSumCellsInField() {
    return ui->fieldSize->currentText().toInt();
}

QIcon MainWindow::getIconByDirecton(Route direction) {
    switch (direction) {
        case kRight:
            return QIcon(":/ant_direction_right.png");
        case kDown:
            return QIcon(":/ant_direction_down.png");
        case kLeft:
            return QIcon(":/ant_direction_left.png");
        case kUp:
            return QIcon(":/ant_direction_up.png");
    }
}

void MainWindow::hideSettingFieldWidgets() {
    ui->fieldSize->hide();
    ui->ms->hide();
    ui->label->hide();
    ui->label_2->hide();
    ui->direction->hide();
}

void MainWindow::showSettingFieldWidgets() {
    ui->fieldSize->show();
    ui->ms->show();
    ui->label->show();
    ui->label_2->show();
    ui->direction->show();
}
