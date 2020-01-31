# plugin-placesmenu

### Simple Places Menu for LXQt-Panel similar to [Places Status Indicator(Gnome Shell)](https://extensions.gnome.org/extension/8/places-status-indicator/)

#### This project has no release versions, only the master branch.

![plugin-placesmenu](http://alexandrecvieira.droppages.com/images/indicator-places/plugin-placesmenu.png)

<img alt="plugin-placesmenu-connect-message" title="plugin-placesmenu Connect Message" src="http://alexandrecvieira.droppages.com/images/indicator-places/plugin-placesmenu-connect-message.png" width="320" /> <img alt="plugin-placesmenu-eject-message" title="plugin-placesmenu Eject Message" src="http://alexandrecvieira.droppages.com/images/indicator-places/plugin-placesmenu-eject-message.png" width="320" />

#### Dependencies
    sudo apt install lxqt-build-tools libqt5x11extras5-dev liblxqt0-dev \
	liblxqt-globalkeys0-dev liblxqt-globalkeys-ui0-dev libkf5windowsystem-dev \
	libfm-qt-dev libglib2.0-dev libnotify-dev

#### Installation
	git clone https://github.com/alexandrecvieira/plugin-placesmenu.git
	cd plugin-placesmenu
	mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_DATAROOTDIR=/usr/share \
	-DCMAKE_INSTALL_LIBDIR=/usr/lib/x86_64-linux-gnu \
	-DCMAKE_MODULE_PATH=/usr/share/cmake/lxqt-build-tools/find-modules/
    make
    sudo make install
