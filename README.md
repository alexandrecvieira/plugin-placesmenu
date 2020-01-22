# plugin-placesmenu
Simple Places Menu for LXQt-Panel similar to [Places Status Indicator(Gnome Shell)](https://extensions.gnome.org/extension/8/places-status-indicator/)

#### Dependencies
    sudo apt install lxqt-build-tools libqt5x11extras5-dev liblxqt0-dev \
	liblxqt-globalkeys0-dev liblxqt-globalkeys-ui0-dev libkf5windowsystem-dev \
	libfm-qt-dev libglib2.0-dev libnotify-dev

#### Installation
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_DATAROOTDIR=/usr/share \
	-DCMAKE_INSTALL_LIBDIR=/usr/lib/x86_64-linux-gnu \
	-DCMAKE_MODULE_PATH=/usr/share/cmake/lxqt-build-tools/find-modules/
    make
    sudo make install
