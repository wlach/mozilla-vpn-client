#!/bin/bash

. $(dirname $0)/commons.sh

print N "This script compiles Qt5 statically"
print N ""

if [ "$1" = "" ] || [ "$2" = "" ] || [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
  print G "Usage:"
  print N "\t$0 <QT_source_folder> <destination_folder>"
  print N ""
  exit 0
fi

[ -d "$1" ] || die "Unable to find the QT source folder."

cd "$1" || die "Unable to enter into the QT source folder"

printn Y "Cleaning the folder... "
make distclean &>/dev/null;
print G "done."

LINUX="
  -platform linux-clang \
  -no-linuxfb \
  -xcb \
"

MACOS="
  -appstore-compliant \
  -no-dbus
"

if [ "$OSTYPE" == "linux-gnu" ]; then
  print G "Configure for linux"
  PLATFORM=$LINUX
elif [ "$OSTYPE" == "darwin" ]; then
  print G "Configure for darwin"
  PLATFORM=$MACOS
else
  die "Unsupported platform (yet?)"
fi

print G "Wait..."
./configure \
  --prefix=$2 \
  --recheck-all \
  -opensource \
  -confirm-license \
  -release \
  -static \
  -strip \
  -silent \
  -no-compile-examples \
  -nomake tests \
  -nomake tools \
  -no-sql-psql \
  -no-sql-sqlite \
  -skip qt3d \
  -skip webengine \
  -skip qtwayland \
  -skip qtmultimedia \
  -skip qtserialport \
  -skip qtsensors \
  -skip qtwebsockets \
  -skip qtxmlpatterns \
  -skip qtcharts \
  -skip qtgamepad \
  -skip qtwebchannel \
  -skip qtandroidextras \
  -feature-imageformat_png \
  -qt-libpng \
  -no-feature-cups \
  -no-feature-d3d12 \
  -no-feature-assistant \
  -no-feature-sqlmodel \
  -no-feature-socks5 \
  -no-feature-wizard \
  -no-feature-whatsthis \
  -no-feature-valgrind \
  -no-feature-testlib_selfcover \
  -no-feature-imageformat_jpeg \
  -no-feature-location-labs-plugin \
  -no-feature-geoservices_esri \
  -no-feature-geoservices_here \
  -no-feature-geoservices_itemsoverlay \
  -no-feature-geoservices_mapbox \
  -no-feature-geoservices_mapboxgl \
  -no-feature-geoservices_osm \
  $PLATFORM || die "Configuration error."

print G "Compiling..."
make -j8 || die "Make failed"

print G "Installing..."
make -j8 install || die "Make install failed"

print G "All done!"
