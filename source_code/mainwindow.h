#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include <atomic>
#include <vector>

#include "AntMover.h"
#include "AntRunner.h"


QT_BEGIN_NAMESPACE
namespace Ui {
   class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void gameover();

private slots:
    void on_button_start_clicked();
    void on_gameover();
    void on_fieldSize_currentIndexChanged(const QString &size);
    void on_direction_currentIndexChanged(int index);
    void on_ms_currentIndexChanged(const QString &arg1);

    void deleteAnt(int posX, int posY);
    void drawAnt(int posX, int posY);

private:
    void clearField();
    Field generateField(int sumCells, int sizeCell);

    int getAntSpeedStepMs();
    Route getAntRouteDirection();
    int getSumCellsInField();
    QIcon getIconByDirecton(Route direction);
    void hideSettingFieldWidgets();
    void showSettingFieldWidgets();

    Ui::MainWindow *ui;

    std::mutex mutex;

    AntRunner *antRunner;

    int widthSideField;

    Field field;

    std::atomic<bool> isAntBreaked;
};
#endif // MAINWINDOW_H
