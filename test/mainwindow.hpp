#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

class QOnvifManager;

namespace Ui {
class MainWindow;
}
namespace device {
class QOnvifDevice;
}

using namespace device;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void movePtz(const float x, const float y, const float z);
    void stopMovePtz();

private slots:
    void onNewDeviceFinded(QOnvifDevice* _device);

    void on_btnRefresh_clicked();
    void on_cmbDevicesComboBox_currentIndexChanged(int index);
    void on_btnRefreshData_clicked();
    void on_actionAbout_triggered();
    void on_btnGetDataAndTime_clicked();
    void on_btsSetDateAndTime_clicked();
    void on_setUsernamePass_clicked();
    void on_btnSetScopes_clicked();
    void on_btngoHome_clicked();
    void on_btnsetHome_clicked();
    void on_btnrefreshPresents_clicked();
    void on_btnRight_pressed();
    void on_btnLeft_pressed();
    void on_actionStopMovement_triggered();
    void on_btnUp_pressed();
    void on_btnDown_pressed();
    void on_btnzoomIn_pressed();
    void on_btnzoomOut_pressed();
    void on_horizontalSlider_zoom_sliderReleased();
    void on_horizontalSlider_zoom_valueChanged(int value);
    void on_actionAbsoluteMoveStepVisibility_triggered();
    void on_pushButton_focusIn_pressed();
    void on_pushButton_focusOut_pressed();
    void on_pushButton_focusOut_released();
    void on_pushButton_focusIn_released();

    void on_pushButton_test_clicked();

private:
    Ui::MainWindow *ui;
    QOnvifManager *ionvifManager;
    QString currentDevice();
};

#endif // MAINWINDOW_HPP
