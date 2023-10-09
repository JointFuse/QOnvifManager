﻿#ifndef MEDIAMANAGEMENT_H
#define MEDIAMANAGEMENT_H
#include "media_management/audioencoderconfiguration.h"
#include "media_management/audioencoderconfigurationoptions.h"
#include "media_management/audioencoderconfigurations.h"
#include "media_management/audiosourceconfigurations.h"
#include "media_management/imagesetting.h"
#include "media_management/imagesettingoptions.h"
#include "media_management/profile.h"
#include "media_management/profiles.h"
#include "media_management/streamuri.h"
#include "media_management/videoencoderconfiguration.h"
#include "media_management/videoencoderconfigurationoptions.h"
#include "media_management/videoencoderconfigurations.h"
#include "media_management/videosourceconfiguration.h"
#include "media_management/videosourceconfigurations.h"
#include "service.h"
#include <QDateTime>

namespace ONVIF {
class MediaManagement : public Service
{
    Q_OBJECT
public:
    explicit MediaManagement(
        const QString& wsdlUrl,
        const QString& username,
        const QString& password);
    VideoSourceConfigurations*        getVideoSourceConfigurations();
    VideoEncoderConfigurations*       getVideoEncoderConfigurations();
    Profiles*                         getProfiles();
    Profile*                          getProfile720P();
    Profile*                          getProfileD1();
    AudioSourceConfigurations*        getAudioSourceConfigurations();
    AudioEncoderConfigurations*       getAudioEncoderConfigurations();
    VideoSourceConfiguration*         getVideoSourceConfiguration(QString name);
    VideoEncoderConfiguration*        getVideoEncoderConfiguration();
    AudioEncoderConfiguration*        getAudioEncoderConfiguration();
    AudioEncoderConfigurationOptions* getAudioEncoderConfigurationOptions();
    VideoEncoderConfigurationOptions* getVideoEncoderConfigurationOptions(
        QString _configToken, QString _profileToken);

    void
    setVideoEncoderConfiguration(VideoEncoderConfiguration* videoConfiguration);
    StreamUri* getStreamUri(const QString& token);
    ImageSetting* getImageSetting(const QString& token);
    ImageSettingOptions* getImageSettingOptions(const QString& token);

    void
    setImageSettings(ImageSetting* imageSettings);

protected:
    Message* newMessage();
    QHash<QString, QString> namespaces(const QString& key);
};
}
#endif // MEDIAMANAGEMENT_H
