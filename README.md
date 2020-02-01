Building DCore-Python
---------------------

| CI | Status |
| -- | ------ |
| Travis | [![Build Status](https://travis-ci.com/decent-dcore/DCore-Python.svg?branch=master)](https://travis-ci.com/decent-dcore/DCore-Python) |
| Appveyor | [![Build Status](https://ci.appveyor.com/api/projects/status/github/decent-dcore/DCore-Python?svg=true&branch=master)](https://ci.appveyor.com/project/decent-dcore/DCore-Python) |

### Installing prerequisites in Linux

For Debian 10/Ubuntu 18.04 LTS or later, execute in console:

    sudo apt-get install apt-transport-https curl gnupg lsb-release software-properties-common
    curl https://bintray.com/user/downloadSubjectPublicKey?username=decentfoundation | sudo apt-key add -
    sudo add-apt-repository "deb [arch=amd64] https://dl.bintray.com/decentfoundation/$(lsb_release -is | tr "[:upper:]" "[:lower:]") $(lsb_release -cs) libpbc"
    sudo apt-get install build-essential make cmake g++ git libreadline-dev libcrypto++-dev libgmp-dev libpbc-dev libssl-dev libcurl4-openssl-dev libboost-all-dev zlib1g-dev
    mkdir ~/dev

    # Download and build JSON 3.7.3
    curl -LO https://github.com/nlohmann/json/archive/v3.7.3.tar.gz
    tar xf v3.7.3.tar.gz
    cd json-3.7.3
    cmake .
    sudo make -j$(nproc) install
    cd ..
    rm -rf json-3.7.3 v3.7.3.tar.gz

For Fedora 30 or later, execute in console:

    sudo dnf install curl
    sudo curl https://bintray.com/user/downloadSubjectPublicKey?username=decentfoundation -o /etc/pki/rpm-gpg/RPM-GPG-KEY-decentfoundation
    sudo curl https://docs.decent.ch/assets/bintray-decentfoundation-fedora.repo -o /etc/yum.repos.d/bintray-decentfoundation.repo
    sudo dnf install make cmake gcc-c++ git readline-devel cryptopp-devel openssl-devel gmp-devel libpbc-devel libcurl-devel json-devel zlib-devel boost-devel boost-python3-devel python3-devel
    mkdir ~/dev

For CentOS 8, execute in console:

    sudo dnf install curl
    sudo curl https://bintray.com/user/downloadSubjectPublicKey?username=decentfoundation -o /etc/pki/rpm-gpg/RPM-GPG-KEY-decentfoundation
    sudo curl https://docs.decent.ch/assets/bintray-decentfoundation-centos.repo -o /etc/yum.repos.d/bintray-decentfoundation.repo
    sudo dnf install https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
    sudo dnf install --enablerepo PowerTools make cmake gcc-c++ git readline-devel cryptopp-devel openssl-devel gmp-devel libpbc-devel libcurl-devel json-devel zlib-devel boost-devel boost-python3-devel python3-devel
    mkdir ~/dev

### Installing prerequisites in MacOS

* Install Xcode and Command Line Tools as described in http://railsapps.github.io/xcode-command-line-tools.html
* Install Homebrew, see http://brew.sh

Then, execute in console:

    $ brew install cmake boost-python3 cryptopp openssl@1.1 pbc nlohmann-json readline git
    $ brew link --force readline
    $ mkdir ~/dev

### Installing prerequisites in Windows

* Install Git for Windows (https://gitforwindows.org)
* Install CMake tools (https://cmake.org/download)
* Install Python 3.7 (https://www.python.org/downloads/windows)
* Install Visual Studio 2019 Community (https://visualstudio.microsoft.com/downloads)

Then, start _Visual Studio 2019 x64 Native Tools Command Prompt_ and execute:

    mkdir \Projects
    cd \Projects
    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    bootstrap-vcpkg.bat
    vcpkg --triplet x64-windows-static install boost cryptopp curl openssl pbc nlohmann-json

### Obtaining the sources

After all the prerequisites are installed, execute in console (change current path to `~/dev` in Linux/MacOS or to `\Projects` in Windows):

    git clone https://github.com/decent-dcore/DCore-Python.git
    cd DCore-Python
    git submodule update --init --recursive

### Building and installing DCore-Python in Linux or MacOS

In order to build and install DCore-Python, execute in console:

    mkdir -p ~/dev/DCore-Python-build
    cd ~/dev/DCore-Python-build
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DPYTHON_VERSION=37 ~/dev/DCore-Python
    cmake --build . --target all -- -j -l 3.0
    cmake --build . --target install

> Note for Ubuntu 18.04 LTS/CentOS 8, you have to change value of `-DPYTHON_VERSION=` to `36` during the initial configuration.

### Building and installing DCore-Python in Windows

In order to build and install DCore-Python follow the steps:
* start Visual Studio 2019 and open a local folder `C:\Projects\DCore-Python`
* navigate to _Project > CMake Settings_ and adjust installation prefix and paths to vcpkg (if needed)
* build and install artifacts using _Build > Install DCore-Python_

You can use CMake generator to create a Visual Studio 2019 project files and perform _Build > Build All_ action from there, just start the _Visual Studio 2019 x64 Native Tools Command Prompt_ and execute:

    cd \Projects\DCore-Python
    cmake -DCMAKE_TOOLCHAIN_FILE=C:\Projects\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_BUILD_TYPE=Release -DPYTHON_VERSION=37 -G "Visual Studio 16 2019" -A x64 .
