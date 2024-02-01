#include <QDebug>
#include <QMessageBox>
#include <cmath>
#include <QFile>
#ifdef QT_DEBUG
#include <thread>
#include <chrono>
#include <QThread>
#endif
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
    if (!ui->checkBox_absoluteMove->isChecked())
    {
        ionvifManager->device(currentDevice())->absoluteMove(
            {
                {
                    ONVIF::Axis::Z,
                    ui->horizontalSlider_zoom->value() * 1.0 / ui->horizontalSlider_zoom->maximum()
                }
            }
            );
    }
    else
    {
        ionvifManager->device(currentDevice())->focusAbsoluteMove(
            ui->horizontalSlider_zoom->value() * 1.0 / ui->horizontalSlider_zoom->maximum());
    }
}


void
MainWindow::on_horizontalSlider_zoom_valueChanged(int value) {
//    if (ui->horizontalSlider_zoom->isSliderDown())
//        return;
//    else
//        ionvifManager->device(currentDevice())->absoluteMove(
//            {
//            {
//                ONVIF::Axis::Z,
//                value * 1.0 / ui->horizontalSlider_zoom->maximum()
//            }
//            }
//        );
        ;
}


void
MainWindow::on_actionAbsoluteMoveStepVisibility_triggered() {
    ui->label_moveStep->setEnabled(ui->checkBox_absoluteMove->isChecked());
    ui->doubleSpinBox_moveStep->setEnabled(ui->checkBox_absoluteMove->isChecked());
}


void MainWindow::on_pushButton_focusIn_pressed()
{
    ionvifManager->device(currentDevice())->focusContinuousMove(0.1);
}


void MainWindow::on_pushButton_focusOut_pressed()
{
    ionvifManager->device(currentDevice())->focusContinuousMove(-0.1);
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

void MainWindow::on_pushButton_connectIp_clicked()
{
    ionvifManager->exploreIp(ui->lineEdit_connectIp->text());
}

void MainWindow::on_pushButton_cmTest_clicked()
{

    auto dvc = ionvifManager->device(currentDevice());
    dvc->comboMove(
        {
            { ONVIF::Axis::X, 1}
        }
    );
#ifdef QT_DEBUG
//    auto killer = false;
//    auto workerReady = false;
//    auto statusCounter = 0;
//    auto worker = [&killer, &workerReady, &statusCounter, login = ionvifManager->user(), password = ionvifManager->password()](){
//        auto mngr = new QOnvifManager{ login, password };
//        mngr->refreshDevicesList();
//        while (mngr->devicesMap().empty())
//            qApp->processEvents();
//        auto dvc = mngr->devicesMap().first();
//        dvc->refreshProfiles();
//        dvc->loadDefaultPtzConfiguration();
//        auto prfLst = dvc->data().profiles.toKenPro;
//        if (prfLst.size())
//            dvc->setMediaProfile(prfLst.first());
//        workerReady = true;
//        while (!killer)
//        {
//            dvc->refreshPtzStatus();
//            ++statusCounter;
//        }
//    };
//    auto dvc = ionvifManager->device(currentDevice());
//    auto wrkThread = QThread::create(worker);
//    wrkThread->start();
//    while (!workerReady)
//        /* wait */;
//    dvc->oneSecContinuousMove(0);
//    killer = true;
//    qDebug() << "Requested ptz status for " << statusCounter << " times";
//    wrkThread->exit();
//    wrkThread->wait();
//    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//    auto statistic = QList<float>{};
//    auto dispersions = QList<float>{};
//    auto info = QString{};
//    auto killer = true;
//    auto worker = [&killer, dvc](){
//        while (killer)
//        {
//            dvc->refreshPtzStatus();
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        }
//    };
//    auto thread = std::thread(std::move(worker));
//    for (auto del = 100; del <= 2000; del *= 1.1)
//    {
//        for (auto i = 0; i < 20; ++i)
//        {
//            dvc->absoluteMove({{ONVIF::Axis::X, 0},{ONVIF::Axis::Y, 0},{ONVIF::Axis::Z, 0}});
//            auto moveStatus = true;
//            while (moveStatus)
//            {
//                dvc->refreshPtzStatus();
//                moveStatus = dvc->data().ptz.status.moveStatus.panTiltX ||
//                        dvc->data().ptz.status.moveStatus.zoomX;
//            }
//            dvc->oneSecContinuousMove(del);
//            dvc->refreshPtzStatus();
//            statistic.push_back(dvc->data().ptz.status.position.panTiltX);
//        }
//        info += "Delay before stop: " + QString::number(del) + "; values: (";
//        for (auto val : statistic)
//            info += QString::number(val) + ", ";
//        info.resize(info.size() - 2);
//        info += ")\n";
//        qDebug() << "Delay before stop: " << del << "; values: " << statistic << "\n"
//                 << "=================================================================================================\n";
//        auto avarage = float(0);
//        for (auto val : statistic)
//            avarage += val;
//        avarage /= statistic.size();
//        auto dispersion = float(0);
//        for (auto val : statistic)
//            dispersion += std::pow(val - avarage, 2);
//        dispersion /= statistic.size() - 1;
//        dispersion = std::pow(dispersion, 0.5);
//        dispersions.push_back(dispersion);
//        statistic.clear();
//    }
//    killer = false;
//    thread.join();
//    info += "Dispersions by different delay: (";
//    for (auto val : dispersions)
//        info += QString::number(val) + ", ";
//    info.resize(info.size() - 2);
//    info += ")\n";
//    QFile file{ "test_log.txt" };
//    file.open(QIODevice::WriteOnly);
//    QDataStream out{ &file };
//    out << info.toStdString().c_str();
//    qDebug() << info.toStdString().c_str();
#endif
}


void MainWindow::on_checkBox_absoluteMove_clicked(bool checked)
{
    if (checked)
        ui->label_15->setText("Focus");
    else
        ui->label_15->setText("Zoom");
}

