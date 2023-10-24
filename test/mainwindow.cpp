#include <QDebug>
#include <QMessageBox>
#include <cmath>

#include "mainwindow.hpp"
#include "qonvifdevice.hpp"
#include "qonvifmanager.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ionvifManager = new QOnvifManager("admin", "", this);
    connect(
        ionvifManager,
        &QOnvifManager::newDeviceFinded,
        this,
        &MainWindow::onNewDeviceFinded);
    on_btnRefresh_clicked();
}

MainWindow::~MainWindow() {
    delete ionvifManager;
    delete ui;
}

void
MainWindow::movePtz(const float x, const float y, const float z) {
    if (ui->checkBox_absoluteMove->isChecked())
    {
//        ionvifManager->device(currentDevice())->refreshPtzStatus();
//        auto inf = ionvifManager->device(currentDevice())->data();
//        auto step = ui->doubleSpinBox_moveStep->value();
//        std::map<ONVIF::Axis, float> moves;
//        if (x != 0 || y != 0)
//        {
//            moves[ONVIF::Axis::X] = inf.ptz.status.position.panTiltX + (x == 0 ? 0 : std::copysign(step, x));
//            moves[ONVIF::Axis::Y] = inf.ptz.status.position.panTiltY + (y == 0 ? 0 : std::copysign(step, y));
//        }
//        if (z != 0) moves[ONVIF::Axis::Z] = inf.ptz.status.position.zoomX + std::copysign(step, z);
//        ionvifManager->device(currentDevice())->absoluteMove(moves);
        auto step = ui->doubleSpinBox_moveStep->value();
        std::map<ONVIF::Axis, float> moves;
        if (x != 0 || y != 0)
        {
            moves[ONVIF::Axis::X] = (x == 0 ? 0 : std::copysign(step, x));
            moves[ONVIF::Axis::Y] = (y == 0 ? 0 : std::copysign(step, y));
        }
        if (z != 0) moves[ONVIF::Axis::Z] = std::copysign(step, z);
        ionvifManager->device(currentDevice())->relativeMove(moves);
    }
    else
        ionvifManager->device(currentDevice())->continuousMove(x, y, z);
}

void
MainWindow::stopMovePtz() {
    if (ui->checkBox_absoluteMove->isChecked())
        return;
    else
        ionvifManager->device(currentDevice())->stopMovement();
}

void
MainWindow::onNewDeviceFinded(QOnvifDevice* _device) {
    ui->cmbDevicesComboBox->addItem(
        _device->data()
            .probeData.deviceServiceAddress.replace("http://", "")
            .replace("/onvif/device_service", ""),
        _device->data().probeData.endPointAddress);

    _device->refreshProfiles();
    _device->loadDefaultPtzConfiguration();
    auto prfLst = _device->data().profiles.toKenPro;
    if (prfLst.size())
        _device->setMediaProfile(prfLst.first());
}

void
MainWindow::on_btnRefresh_clicked() {
    ui->btnRefresh->setEnabled(false);
    ui->cmbDevicesComboBox->clear();
    connect(
        ionvifManager,
        &QOnvifManager::deviceSearchingEnded,
        this,
        [this]() { ui->btnRefresh->setEnabled(true); },
        Qt::UniqueConnection);
    ionvifManager->refreshDevicesList();
}

void
MainWindow::on_cmbDevicesComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index)
    //    on_btnRefreshData_clicked();
}

void
MainWindow::on_btnRefreshData_clicked() {
    ui->txtLocation->setText("");
    ui->txtName->setText("");
    ionvifManager->refreshDeviceCapabilities(currentDevice());
    ionvifManager->refreshDeviceInformations(currentDevice());
    ionvifManager->refreshDeviceProfiles(currentDevice());
    ionvifManager->refreshDeviceImageSetting(currentDevice());
    ionvifManager->refreshDeviceImageSettingOptions(currentDevice());
    ionvifManager->refreshDeviceVideoConfigs(currentDevice());
    ionvifManager->refreshDeviceVideoConfigsOptions(currentDevice());
    ionvifManager->refreshDeviceStreamUris(currentDevice());

    ionvifManager->refreshDeviceUsers(currentDevice());
    ionvifManager->refreshDeviceScopes(currentDevice());
    ionvifManager->refreshDeviceInterfaces(currentDevice());
    ionvifManager->refreshDeviceDNS(currentDevice());
    ionvifManager->refreshDeviceDefaultGateway(currentDevice());
    ionvifManager->refreshDeviceDiscoveryMode(currentDevice());
    ionvifManager->refreshDeviceHostname(currentDevice());
    ionvifManager->refreshDeviceNTP(currentDevice());
    ionvifManager->refreshDevicePtzConfigs(currentDevice());

    // setScopes
    ui->txtLocation->setText(
        ionvifManager->device(currentDevice())->data().scopes.location);
    ui->txtName->setText(
        ionvifManager->device(currentDevice())->data().scopes.name);
    ui->txtHardware->setText(
        ionvifManager->device(currentDevice())->data().scopes.hardware);

    on_btnGetDataAndTime_clicked();

    auto device = ionvifManager->device(currentDevice());
    device->loadDefaultPtzConfiguration();
    device->refreshPtzStatus();
    device->refreshImageStatus();
    device->refreshFocusMoveOptions();
    Q_UNUSED(device)

    // <- add a breake point here to see device informations in debuger.
}

void
MainWindow::on_actionAbout_triggered() {
    QMessageBox aboutMessageBox(this);
    aboutMessageBox.setText(
        "developed by: Mehrdad Shobeyri \nemail: mehrdad.shobeyri@yahoo.com");
    aboutMessageBox.exec();
}

void
MainWindow::on_btnGetDataAndTime_clicked() {
    Data::DateTime dateAndTime;
    ionvifManager->deviceDateAndTime(currentDevice(), dateAndTime);
    ui->dateTimeEditLocal->setDateTime(dateAndTime.localTime);
    ui->dateTimeEditUtc->setDateTime(dateAndTime.utcTime);
}

void
MainWindow::on_btsSetDateAndTime_clicked() {
    ionvifManager->setDeviceDateAndTime(
        currentDevice(),
        ui->dateTimeEditSet->dateTime(),
        "GMT-3:30",
        true,
        false);
}

QString
MainWindow::currentDevice() {
    return ui->cmbDevicesComboBox->currentData().toString();
}

void
MainWindow::on_setUsernamePass_clicked() {
    ionvifManager->setDefaulUsernameAndPassword(
        ui->txtUserName->text(), ui->txtPassword->text());
    on_btnRefresh_clicked();
}

void
MainWindow::on_btnSetScopes_clicked() {
    ionvifManager->setDeviceScopes(
        currentDevice(), ui->txtName->text(), ui->txtLocation->text());
}

void
MainWindow::on_btngoHome_clicked() {
    ionvifManager->device(currentDevice())->goHomePosition();
}

void
MainWindow::on_btnsetHome_clicked() {
    ionvifManager->device(currentDevice())->setHomePosition();
}

void
MainWindow::on_btnrefreshPresents_clicked() {
    ionvifManager->device(currentDevice())->refreshPresets();
}

void
MainWindow::on_btnRight_pressed() {
    movePtz(0.5, 0, 0);
}

void
MainWindow::on_btnLeft_pressed() {
    movePtz(-0.5, 0, 0);
}


void
MainWindow::on_actionStopMovement_triggered() {
    stopMovePtz();
}


void
MainWindow::on_btnUp_pressed() {
    movePtz(0, 0.5, 0);
}


void
MainWindow::on_btnDown_pressed() {
    movePtz(0, -0.5, 0);
}


void
MainWindow::on_btnzoomIn_pressed() {
    movePtz(0, 0, 0.5);
}


void
MainWindow::on_btnzoomOut_pressed() {
    movePtz(0, 0, -0.5);
}


void
MainWindow::on_horizontalSlider_zoom_sliderReleased() {
    ionvifManager->device(currentDevice())->absoluteMove(
        {
        {
            ONVIF::Axis::Z,
            ui->horizontalSlider_zoom->value() * 1.0 / ui->horizontalSlider_zoom->maximum()
        }
        }
    );
}


void
MainWindow::on_horizontalSlider_zoom_valueChanged(int value) {
    if (ui->horizontalSlider_zoom->isSliderDown())
        return;
    else
        ionvifManager->device(currentDevice())->absoluteMove(
            {
            {
                ONVIF::Axis::Z,
                value * 1.0 / ui->horizontalSlider_zoom->maximum()
            }
            }
        );
}


void
MainWindow::on_actionAbsoluteMoveStepVisibility_triggered() {
    ui->label_moveStep->setEnabled(ui->checkBox_absoluteMove->isChecked());
    ui->doubleSpinBox_moveStep->setEnabled(ui->checkBox_absoluteMove->isChecked());
}


void MainWindow::on_pushButton_focusIn_pressed()
{
    ionvifManager->device(currentDevice())->focusContinuousMove(1);
}


void MainWindow::on_pushButton_focusOut_pressed()
{
    ionvifManager->device(currentDevice())->focusContinuousMove(-1);
}


void MainWindow::on_pushButton_focusOut_released()
{
    ionvifManager->device(currentDevice())->focusStopMove();
}


void MainWindow::on_pushButton_focusIn_released()
{
    ionvifManager->device(currentDevice())->focusStopMove();
}


void MainWindow::on_pushButton_test_clicked()
{
    auto dvc = ionvifManager->device(currentDevice());
#ifndef QT_DEBUG
    dvc->relativeMove({{ ONVIF::Axis::X, 0.083 }});
    dvc->relativeMove({{ ONVIF::Axis::X, 0.056 }});
    dvc->relativeMove({{ ONVIF::Axis::X, 0.028 }});
    dvc->relativeMove({{ ONVIF::Axis::X, 0.017 }});
#else
    dvc->relativeMoving({{ ONVIF::Axis::X, 0.083 }}, 5, 5);
    dvc->relativeMoving({{ ONVIF::Axis::X, 0.056 }}, 5, 5);
    dvc->relativeMoving({{ ONVIF::Axis::X, 0.028 }}, 5, 5);
    dvc->relativeMoving({{ ONVIF::Axis::X, 0.017 }}, 5, 5);
#endif
}

