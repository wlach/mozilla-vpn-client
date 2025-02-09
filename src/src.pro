# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include($$PWD/../version.pri)
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += BUILD_ID=\\\"$$BUILD_ID\\\"

!isEmpty(MVPN_EXTRA_USERAGENT) {
    DEFINES += MVPN_EXTRA_USERAGENT=\\\"$$MVPN_EXTRA_USERAGENT\\\"
}

QT += network
QT += quick
QT += widgets
QT += charts

TEMPLATE  = app

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += \
            hacl-star \
            hacl-star/kremlin \
            hacl-star/kremlin/minimal \
            ../glean/generated

DEPENDPATH  += $${INCLUDEPATH}

OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
UI_DIR = .ui

SOURCES += \
        apppermission.cpp \
        authenticationlistener.cpp \
        captiveportal/captiveportal.cpp \
        captiveportal/captiveportaldetection.cpp \
        captiveportal/captiveportaldetectionimpl.cpp \
        captiveportal/captiveportalmonitor.cpp \
        captiveportal/captiveportalnotifier.cpp \
        captiveportal/captiveportalrequest.cpp \
        captiveportal/captiveportalmultirequest.cpp \
        closeeventhandler.cpp \
        command.cpp \
        commandlineparser.cpp \
        commands/commandactivate.cpp \
        commands/commanddeactivate.cpp \
        commands/commanddevice.cpp \
        commands/commandlogin.cpp \
        commands/commandlogout.cpp \
        commands/commandselect.cpp \
        commands/commandservers.cpp \
        commands/commandstatus.cpp \
        commands/commandui.cpp \
        connectioncheck.cpp \
        connectiondataholder.cpp \
        connectionhealth.cpp \
        controller.cpp \
        cryptosettings.cpp \
        curve25519.cpp \
        errorhandler.cpp \
        featurelist.cpp \
        fontloader.cpp \
        hacl-star/Hacl_Chacha20.c \
        hacl-star/Hacl_Chacha20Poly1305_32.c \
        hacl-star/Hacl_Curve25519_51.c \
        hacl-star/Hacl_Poly1305_32.c \
        ipaddress.cpp \
        ipaddressrange.cpp \
        ipfinder.cpp \
        leakdetector.cpp \
        localizer.cpp \
        logger.cpp \
        loghandler.cpp \
        logoutobserver.cpp \
        main.cpp \
        models/device.cpp \
        models/devicemodel.cpp \
        models/helpmodel.cpp \
        models/keys.cpp \
        models/server.cpp \
        models/servercity.cpp \
        models/servercountry.cpp \
        models/servercountrymodel.cpp \
        models/serverdata.cpp \
        models/survey.cpp \
        models/surveymodel.cpp \
        models/user.cpp \
        mozillavpn.cpp \
        networkmanager.cpp \
        networkrequest.cpp \
        networkwatcher.cpp \
        notificationhandler.cpp \
        pinghelper.cpp \
        pingsender.cpp \
        platforms/dummy/dummyapplistprovider.cpp \
        platforms/dummy/dummynetworkwatcher.cpp \
        qmlengineholder.cpp \
        releasemonitor.cpp \
        rfc1918.cpp \
        rfc4193.cpp \
        serveri18n.cpp \
        settingsholder.cpp \
        simplenetworkmanager.cpp \
        statusicon.cpp \
        systemtrayhandler.cpp \
        tasks/accountandservers/taskaccountandservers.cpp \
        tasks/adddevice/taskadddevice.cpp \
        tasks/authenticate/taskauthenticate.cpp \
        tasks/captiveportallookup/taskcaptiveportallookup.cpp \
        tasks/controlleraction/taskcontrolleraction.cpp \
        tasks/function/taskfunction.cpp \
        tasks/heartbeat/taskheartbeat.cpp \
        tasks/removedevice/taskremovedevice.cpp \
        tasks/surveydata/tasksurveydata.cpp \
        timercontroller.cpp \
        timersingleshot.cpp \
        update/updater.cpp \
        update/versionapi.cpp \
        urlopener.cpp

HEADERS += \
        apppermission.h \
        applistprovider.h \
        authenticationlistener.h \
        captiveportal/captiveportal.h \
        captiveportal/captiveportaldetection.h \
        captiveportal/captiveportaldetectionimpl.h \
        captiveportal/captiveportalmonitor.h \
        captiveportal/captiveportalnotifier.h \
        captiveportal/captiveportalrequest.h \
        captiveportal/captiveportalmultirequest.h \
        captiveportal/captiveportalresult.h \
        closeeventhandler.h \
        command.h \
        commandlineparser.h \
        commands/commandactivate.h \
        commands/commanddeactivate.h \
        commands/commanddevice.h \
        commands/commandlogin.h \
        commands/commandlogout.h \
        commands/commandselect.h \
        commands/commandservers.h \
        commands/commandstatus.h \
        commands/commandui.h \
        connectioncheck.h \
        connectiondataholder.h \
        connectionhealth.h \
        constants.h \
        controller.h \
        controllerimpl.h \
        cryptosettings.h \
        curve25519.h \
        errorhandler.h \
        featurelist.h \
        fontloader.h \
        ipaddress.h \
        ipaddressrange.h \
        ipfinder.h \
        leakdetector.h \
        localizer.h \
        logger.h \
        loghandler.h \
        logoutobserver.h \
        models/device.h \
        models/devicemodel.h \
        models/helpmodel.h \
        models/keys.h \
        models/server.h \
        models/servercity.h \
        models/servercountry.h \
        models/servercountrymodel.h \
        models/serverdata.h \
        models/survey.h \
        models/surveymodel.h \
        models/user.h \
        mozillavpn.h \
        networkmanager.h \
        networkrequest.h \
        networkwatcher.h \
        networkwatcherimpl.h \
        notificationhandler.h \
        pinghelper.h \
        pingsender.h \
        pingsendworker.h \
        platforms/dummy/dummyapplistprovider.h \
        platforms/dummy/dummynetworkwatcher.h \
        qmlengineholder.h \
        releasemonitor.h \
        rfc1918.h \
        rfc4193.h \
        serveri18n.h \
        settingsholder.h \
        simplenetworkmanager.h \
        statusicon.h \
        systemtrayhandler.h \
        task.h \
        tasks/accountandservers/taskaccountandservers.h \
        tasks/adddevice/taskadddevice.h \
        tasks/authenticate/taskauthenticate.h \
        tasks/captiveportallookup/taskcaptiveportallookup.h \
        tasks/controlleraction/taskcontrolleraction.h \
        tasks/function/taskfunction.h \
        tasks/heartbeat/taskheartbeat.h \
        tasks/removedevice/taskremovedevice.h \
        tasks/surveydata/tasksurveydata.h \
        timercontroller.h \
        timersingleshot.h \
        update/updater.h \
        update/versionapi.h \
        urlopener.h

webextension {
    message(Enabling the webextension support)

    DEFINES += MVPN_WEBEXTENSION

    SOURCES += \
            server/serverconnection.cpp \
            server/serverhandler.cpp
    HEADERS += \
            server/serverconnection.h \
            server/serverhandler.h
}

inspector {
    message(Enabling the inspector)

    QT+= websockets
    QT+= testlib
    QT.testlib.CONFIG -= console
    CONFIG += no_testcase_installs

    RESOURCES += inspector/inspector.qrc

    DEFINES += MVPN_INSPECTOR

    SOURCES += \
            inspector/inspectorhttpconnection.cpp \
            inspector/inspectorhttpserver.cpp \
            inspector/inspectorwebsocketconnection.cpp \
            inspector/inspectorwebsocketserver.cpp

    HEADERS += \
            inspector/inspectorhttpconnection.h \
            inspector/inspectorhttpserver.h \
            inspector/inspectorwebsocketconnection.h \
            inspector/inspectorwebsocketserver.h
}

# Signal handling for unix platforms
unix {
    SOURCES += signalhandler.cpp
    HEADERS += signalhandler.h
}

RESOURCES += qml.qrc
RESOURCES += ../glean/glean.qrc

QML_IMPORT_PATH =
QML_DESIGNER_IMPORT_PATH =

production {
    message(Production build)
    DEFINES += MVPN_PRODUCTION_MODE
    RESOURCES += logo_prod.qrc
} else {
    message(Staging build)
    RESOURCES += logo_beta.qrc
}

balrog {
    message(Balrog enabled)
    DEFINES += MVPN_BALROG

    SOURCES += update/balrog.cpp
    HEADERS += update/balrog.h
}

DUMMY {
    message(Dummy build)

    CONFIG += c++1z

    win* {
      CONFIG += embed_manifest_exe
      QT += svg
    } else {
      versionAtLeast(QT_VERSION, 5.15.1) {
        QMAKE_CXXFLAGS *= -Werror
      }
    }

    macos {
      TARGET = MozillaVPN
    } else {
      TARGET = mozillavpn
    }

    QT += networkauth

    DEFINES += MVPN_DUMMY

    SOURCES += \
            platforms/dummy/dummycontroller.cpp \
            platforms/dummy/dummycryptosettings.cpp \
            platforms/dummy/dummypingsendworker.cpp \
            systemtraynotificationhandler.cpp \
            tasks/authenticate/desktopauthenticationlistener.cpp

    HEADERS += \
            platforms/dummy/dummycontroller.h \
            platforms/dummy/dummypingsendworker.h \
            systemtraynotificationhandler.h \
            tasks/authenticate/desktopauthenticationlistener.h
}

# Platform-specific: Linux
else:linux:!android {
    message(Linux build)
    include($$PWD/golang.pri)

    TARGET = mozillavpn
    QT += networkauth
    QT += dbus

    CONFIG += c++14

    DEFINES += MVPN_LINUX
    DEFINES += PROTOCOL_VERSION=\\\"$$DBUS_PROTOCOL_VERSION\\\"

    SOURCES += \
            eventlistener.cpp \
            platforms/linux/backendlogsobserver.cpp \
            platforms/linux/dbusclient.cpp \
            platforms/linux/linuxcontroller.cpp \
            platforms/linux/linuxcryptosettings.cpp \
            platforms/linux/linuxdependencies.cpp \
            platforms/linux/linuxnetworkwatcher.cpp \
            platforms/linux/linuxnetworkwatcherworker.cpp \
            platforms/linux/linuxpingsendworker.cpp \
            platforms/linux/linuxsystemtrayhandler.cpp \
            systemtraynotificationhandler.cpp \
            tasks/authenticate/desktopauthenticationlistener.cpp

    HEADERS += \
            eventlistener.h \
            platforms/linux/backendlogsobserver.h \
            platforms/linux/dbusclient.h \
            platforms/linux/linuxcontroller.h \
            platforms/linux/linuxdependencies.h \
            platforms/linux/linuxnetworkwatcher.h \
            platforms/linux/linuxnetworkwatcherworker.h \
            platforms/linux/linuxpingsendworker.h \
            platforms/linux/linuxsystemtrayhandler.h \
            systemtraynotificationhandler.h \
            tasks/authenticate/desktopauthenticationlistener.h

    # The daemon source code:
    SOURCES += \
            ../3rdparty/wireguard-tools/contrib/embeddable-wg-library/wireguard.c \
            daemon/daemon.cpp \
            platforms/linux/daemon/dbusservice.cpp \
            platforms/linux/daemon/dnsutilslinux.cpp \
            platforms/linux/daemon/iputilslinux.cpp \
            platforms/linux/daemon/linuxdaemon.cpp \
            platforms/linux/daemon/polkithelper.cpp \
            platforms/linux/daemon/wireguardutilslinux.cpp

    HEADERS += \
            ../3rdparty/wireguard-tools/contrib/embeddable-wg-library/wireguard.h \
            daemon/interfaceconfig.h \
            daemon/daemon.h \
            daemon/dnsutils.h \
            daemon/iputils.h \
            daemon/wireguardutils.h \
            platforms/linux/daemon/dbusservice.h \
            platforms/linux/daemon/dbustypeslinux.h \
            platforms/linux/daemon/dnsutilslinux.h \
            platforms/linux/daemon/iputilslinux.h \
            platforms/linux/daemon/polkithelper.h \
            platforms/linux/daemon/wireguardutilslinux.h

    isEmpty(USRPATH) {
        USRPATH=/usr
    }
    isEmpty(ETCPATH) {
        ETCPATH=/etc
    }

    DBUS_ADAPTORS += platforms/linux/daemon/org.mozilla.vpn.dbus.xml
    DBUS_INTERFACES = platforms/linux/daemon/org.mozilla.vpn.dbus.xml

    GO_MODULES = ../linux/netfilter/netfilter.go
    
    target.path = $${USRPATH}/bin
    INSTALLS += target

    desktopFile.path = $${USRPATH}/share/applications
    desktopFile.files = ../linux/extra/MozillaVPN.desktop
    INSTALLS += desktopFile

    autostartFile.path = $${ETCPATH}/xdg/autostart
    autostartFile.files = ../linux/extra/MozillaVPN-startup.desktop
    INSTALLS += autostartFile

    icon16x16.path = $${USRPATH}/share/icons/hicolor/16x16/apps
    icon16x16.files = ../linux/extra/icons/16x16/mozillavpn.png
    INSTALLS += icon16x16

    icon32x32.path = $${USRPATH}/share/icons/hicolor/32x32/apps
    icon32x32.files = ../linux/extra/icons/32x32/mozillavpn.png
    INSTALLS += icon32x32

    icon48x48.path = $${USRPATH}/share/icons/hicolor/48x48/apps
    icon48x48.files = ../linux/extra/icons/48x48/mozillavpn.png
    INSTALLS += icon48x48

    DEFINES += MVPN_ICON_PATH=\\\"$${USRPATH}/share/icons/hicolor/64x64/apps/mozillavpn.png\\\"
    icon64x64.path = $${USRPATH}/share/icons/hicolor/64x64/apps
    icon64x64.files = ../linux/extra/icons/64x64/mozillavpn.png
    INSTALLS += icon64x64

    icon128x128.path = $${USRPATH}/share/icons/hicolor/128x128/apps
    icon128x128.files = ../linux/extra/icons/128x128/mozillavpn.png
    INSTALLS += icon128x128

    polkit_actions.files = platforms/linux/daemon/org.mozilla.vpn.policy
    polkit_actions.path = $${USRPATH}/share/polkit-1/actions
    INSTALLS += polkit_actions

    dbus_conf.files = platforms/linux/daemon/org.mozilla.vpn.conf
    dbus_conf.path = $${USRPATH}/share/dbus-1/system.d/
    INSTALLS += dbus_conf

    dbus_service.files = platforms/linux/daemon/org.mozilla.vpn.dbus.service
    dbus_service.path = $${USRPATH}/share/dbus-1/system-services
    INSTALLS += dbus_service

    systemd_service.files = ../linux/debian/mozillavpn.service
    systemd_service.path = /usr/lib/systemd/system
    INSTALLS += systemd_service

    CONFIG += link_pkgconfig
    PKGCONFIG += polkit-gobject-1
}

# Platform-specific: android
else:android {
    message(Android build)

    versionAtLeast(QT_VERSION, 5.15.1) {
      QMAKE_CXXFLAGS *= -Werror
    }

    # Android Deploy-to-Qt strips the info anyway
    # but we want to create an extra bundle with the info :)
    CONFIG += force_debug_info
    CONFIG += c++14

    TARGET = mozillavpn
    QT += networkauth
    QT += svg
    QT += androidextras
    QT += qml
    QT += xml
    LIBS += \-ljnigraphics\

    DEFINES += MVPN_ANDROID

    ANDROID_ABIS = x86 x86_64 armeabi-v7a arm64-v8a

    INCLUDEPATH += platforms/android

    SOURCES +=  platforms/android/androidauthenticationlistener.cpp \
                platforms/android/androidcontroller.cpp \
                platforms/android/androidnotificationhandler.cpp \
                platforms/android/androidutils.cpp \
                platforms/android/androidwebview.cpp \
                platforms/android/androidvpnactivity.cpp \
                platforms/android/androiddatamigration.cpp \
                platforms/android/androidappimageprovider.cpp \
                platforms/android/androidapplistprovider.cpp \
                platforms/android/androidsharedprefs.cpp \
                tasks/authenticate/desktopauthenticationlistener.cpp

    HEADERS +=  platforms/android/androidauthenticationlistener.h \
                platforms/android/androidcontroller.h \
                platforms/android/androidnotificationhandler.h \
                platforms/android/androidutils.h \
                platforms/android/androidwebview.h \
                platforms/android/androidvpnactivity.h \
                platforms/android/androiddatamigration.h\
                platforms/android/androidappimageprovider.h \
                platforms/android/androidapplistprovider.h \
                platforms/android/androidsharedprefs.h \
                tasks/authenticate/desktopauthenticationlistener.h

    # Usable Linux Imports
    SOURCES += platforms/linux/linuxpingsendworker.cpp \
               platforms/linux/linuxcryptosettings.cpp

    HEADERS += platforms/linux/linuxpingsendworker.h

    # We need to compile our own openssl :/
    exists(../3rdparty/openSSL/openssl.pri) {
       include(../3rdparty/openSSL/openssl.pri)
    } else{
       message(Have you imported the 3rd-party git submodules? Read the README.md)
       error(Did not found openSSL in 3rdparty/openSSL - Exiting Android Build )
    }

    # For the android build we need to unset those
    # Otherwise the packaging will fail 🙅
    OBJECTS_DIR =
    MOC_DIR =
    RCC_DIR =
    UI_DIR =

    DISTFILES += \
        ../android/AndroidManifest.xml \
        ../android/build.gradle \
        ../android/gradle/wrapper/gradle-wrapper.jar \
        ../android/gradle/wrapper/gradle-wrapper.properties \
        ../android/gradlew \
        ../android/gradlew.bat \
        ../android/res/values/libs.xml

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../android
}

# Platform-specific: MacOS
else:macos {
    message(MacOSX build)

    versionAtLeast(QT_VERSION, 5.15.1) {
      QMAKE_CXXFLAGS *= -Werror
    }

    TARGET = MozillaVPN
    QMAKE_TARGET_BUNDLE_PREFIX = org.mozilla.macos
    QT += networkauth

    CONFIG += c++1z

    # For the loginitem
    LIBS += -framework ServiceManagement
    LIBS += -framework Security
    LIBS += -framework CoreWLAN

    DEFINES += MVPN_MACOS

    SOURCES += \
            platforms/macos/macosmenubar.cpp \
            platforms/macos/macospingsendworker.cpp \
            platforms/macos/macosstartatbootwatcher.cpp \
            systemtraynotificationhandler.cpp \
            tasks/authenticate/desktopauthenticationlistener.cpp

    OBJECTIVE_SOURCES += \
            platforms/macos/macoscryptosettings.mm \
            platforms/macos/macosnetworkwatcher.mm \
            platforms/macos/macosutils.mm

    HEADERS += \
            platforms/macos/macosmenubar.h \
            platforms/macos/macospingsendworker.h \
            platforms/macos/macosstartatbootwatcher.h \
            systemtraynotificationhandler.h \
            tasks/authenticate/desktopauthenticationlistener.h

    OBJECTIVE_HEADERS += \
            platforms/macos/macosnetworkwatcher.h \
            platforms/macos/macosutils.h

    isEmpty(MVPN_MACOS) {
        message(No integration required for this build - let\'s use the dummy controller)

        SOURCES += platforms/dummy/dummycontroller.cpp
        HEADERS += platforms/dummy/dummycontroller.h
    } else:networkextension {
        message(Network extension mode)

        DEFINES += MVPN_MACOS_NETWORKEXTENSION

        INCLUDEPATH += \
                    ../3rdparty/Wireguard-apple/WireGuard/WireGuard/Crypto \
                    ../3rdparty/wireguard-apple/WireGuard/Shared/Model \

        OBJECTIVE_SOURCES += \
                platforms/ios/ioscontroller.mm \
                platforms/ios/iosglue.mm

        OBJECTIVE_HEADERS += \
                platforms/ios/iosscontroller.h
    } else {
        message(Daemon mode)

        DEFINES += MVPN_MACOS_DAEMON

        SOURCES += \
                   daemon/daemon.cpp \
                   daemon/daemonlocalserver.cpp \
                   daemon/daemonlocalserverconnection.cpp \
                   localsocketcontroller.cpp \
                   wgquickprocess.cpp \
                   platforms/macos/daemon/macosdaemon.cpp \
                   platforms/macos/daemon/macosdaemonserver.cpp
        HEADERS += \
                   daemon/interfaceconfig.h \
                   daemon/daemon.h \
                   daemon/daemonlocalserver.h \
                   daemon/daemonlocalserverconnection.h \
                   daemon/dnsutils.h \
                   daemon/iputils.h \
                   daemon/wireguardutils.h \
                   localsocketcontroller.h \
                   wgquickprocess.h \
                   platforms/macos/daemon/macosdaemon.h \
                   platforms/macos/daemon/macosdaemonserver.h
    }

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
    QMAKE_INFO_PLIST=../macos/app/Info.plist
    QMAKE_ASSET_CATALOGS_APP_ICON = "AppIcon"

    production {
        QMAKE_ASSET_CATALOGS = $$PWD/../macos/app/Images.xcassets
    } else {
        QMAKE_ASSET_CATALOGS = $$PWD/../macos/app/Images-beta.xcassets
    }
}

# Platform-specific: IOS
else:ios {
    message(IOS build)

    TARGET = MozillaVPN
    QMAKE_TARGET_BUNDLE_PREFIX = org.mozilla.ios
    QT += svg
    QT += gui-private

    CONFIG += c++1z

    # For the authentication
    LIBS += -framework AuthenticationServices

    # For notifications
    LIBS += -framework UIKit
    LIBS += -framework Foundation
    LIBS += -framework StoreKit
    LIBS += -framework UserNotifications

    DEFINES += MVPN_IOS

    SOURCES += \
            platforms/ios/taskiosproducts.cpp \
            platforms/macos/macospingsendworker.cpp

    OBJECTIVE_SOURCES += \
            platforms/ios/iaphandler.mm \
            platforms/ios/iosauthenticationlistener.mm \
            platforms/ios/ioscontroller.mm \
            platforms/ios/iosdatamigration.mm \
            platforms/ios/iosglue.mm \
            platforms/ios/iosnotificationhandler.mm \
            platforms/ios/iosutils.mm \
            platforms/macos/macoscryptosettings.mm

    HEADERS += \
            platforms/ios/taskiosproducts.h \
            platforms/macos/macospingsendworker.h

    OBJECTIVE_HEADERS += \
            platforms/ios/iaphandler.h \
            platforms/ios/iosauthenticationlistener.h \
            platforms/ios/ioscontroller.h \
            platforms/ios/iosdatamigration.h \
            platforms/ios/iosnotificationhandler.h \
            platforms/ios/iosutils.h

    QMAKE_INFO_PLIST= $$PWD/../ios/app/Info.plist
    QMAKE_ASSET_CATALOGS_APP_ICON = "AppIcon"

    production {
        QMAKE_ASSET_CATALOGS = $$PWD/../ios/app/Images.xcassets
    } else {
        QMAKE_ASSET_CATALOGS = $$PWD/../ios/app/Images-beta.xcassets
    }

    app_launch_screen.files = $$files($$PWD/../ios/app/MozillaVPNLaunchScreen.storyboard)
    QMAKE_BUNDLE_DATA += app_launch_screen

    ios_launch_screen_images.files = $$files($$PWD/../ios/app/launch.png)
    QMAKE_BUNDLE_DATA += ios_launch_screen_images
}

else:win* {
    message(Windows build)

    TARGET = MozillaVPN

    CONFIG += c++1z
    QMAKE_CXXFLAGS += -MP

    QT += networkauth
    QT += svg

    CONFIG += embed_manifest_exe
    DEFINES += MVPN_WINDOWS

    production {
        RC_ICONS = ui/resources/logo.ico
    } else {
        RC_ICONS = ui/resources/logo-beta.ico
    }

    SOURCES += \
        daemon/daemon.cpp \
        daemon/daemonlocalserver.cpp \
        daemon/daemonlocalserverconnection.cpp \
        eventlistener.cpp \
        localsocketcontroller.cpp \
        platforms/windows/daemon/windowsdaemon.cpp \
        platforms/windows/daemon/windowsdaemonserver.cpp \
        platforms/windows/daemon/windowsdaemontunnel.cpp \
        platforms/windows/daemon/windowstunnelmonitor.cpp \
        platforms/windows/windowscommons.cpp \
        platforms/windows/windowscryptosettings.cpp \
        platforms/windows/windowsdatamigration.cpp \
        platforms/windows/windowsnetworkwatcher.cpp \
        platforms/windows/windowspingsendworker.cpp \
        platforms/windows/windowsstartatbootwatcher.cpp \
        tasks/authenticate/desktopauthenticationlistener.cpp \
        systemtraynotificationhandler.cpp \
        wgquickprocess.cpp

    HEADERS += \
        daemon/interfaceconfig.h \
        daemon/daemon.h \
        daemon/daemonlocalserver.h \
        daemon/daemonlocalserverconnection.h \
        daemon/dnsutils.h \
        daemon/iputils.h \
        daemon/wireguardutils.h \
        eventlistener.h \
        localsocketcontroller.h \
        platforms/windows/daemon/windowsdaemon.h \
        platforms/windows/daemon/windowsdaemonserver.h \
        platforms/windows/daemon/windowsdaemontunnel.h \
        platforms/windows/daemon/windowstunnelmonitor.h \
        platforms/windows/windowscommons.h \
        platforms/windows/windowsdatamigration.h \
        platforms/windows/windowsnetworkwatcher.h \
        platforms/windows/windowspingsendworker.h \
        tasks/authenticate/desktopauthenticationlistener.h \
        platforms/windows/windowsstartatbootwatcher.h \
        systemtraynotificationhandler.h \
        wgquickprocess.h
}

else:wasm {
    message(WASM \\o/)
    DEFINES += MVPN_DUMMY
    DEFINES += MVPN_WASM

    versionAtLeast(QT_VERSION, 5.15.1) {
      QMAKE_CXXFLAGS *= -Werror
    }

    TARGET = mozillavpn
    QT += svg

    CONFIG += c++1z

    SOURCES += \
            platforms/dummy/dummycontroller.cpp \
            platforms/dummy/dummycryptosettings.cpp \
            platforms/dummy/dummypingsendworker.cpp \
            platforms/macos/macosmenubar.cpp \
            platforms/wasm/wasmauthenticationlistener.cpp \
            platforms/wasm/wasmnetworkrequest.cpp \
            platforms/wasm/wasmnetworkwatcher.cpp \
            platforms/wasm/wasmwindowcontroller.cpp \
            systemtraynotificationhandler.cpp

    HEADERS += \
            platforms/dummy/dummycontroller.h \
            platforms/dummy/dummypingsendworker.h \
            platforms/macos/macosmenubar.h \
            platforms/wasm/wasmauthenticationlistener.h \
            platforms/wasm/wasmnetworkwatcher.h \
            platforms/wasm/wasmwindowcontroller.h \
            systemtraynotificationhandler.h

    SOURCES -= networkrequest.cpp
    RESOURCES += platforms/wasm/networkrequests.qrc
}

# Anything else
else {
    error(Unsupported platform)
}

RESOURCES += $$PWD/../translations/servers.qrc

exists($$PWD/../translations/translations.pri) {
    include($$PWD/../translations/translations.pri)
} else {
    message(Languages were not imported - using fallback english)
    TRANSLATIONS += \
        ../translations/mozillavpn_en.ts

    ts.commands += lupdate $$PWD -no-obsolete -ts $$PWD/../translations/mozillavpn_en.ts
    ts.CONFIG += no_check_exist
    ts.output = $$PWD/../translations/mozillavpn_en.ts
    ts.input = .
    QMAKE_EXTRA_TARGETS += ts
    PRE_TARGETDEPS += ts
}

QMAKE_LRELEASE_FLAGS += -idbased
CONFIG += lrelease
CONFIG += embed_translations

debug {
    SOURCES += gleantest.cpp
    HEADERS += gleantest.h
}

coverage {
    message(Coverage enabled)
    QMAKE_CXXFLAGS += -fprofile-instr-generate -fcoverage-mapping
    QMAKE_LFLAGS += -fprofile-instr-generate -fcoverage-mapping
}
