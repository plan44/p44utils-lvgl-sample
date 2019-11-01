
p44utils-lvgl-sample
====================

Sample for p44utils based app using LittlevGL for UI

Note: **This is Work In Progress**

## Build

### OpenWrt

There's a package `p44utils-lvgl-sample` in the [public plan44 feed](https://github.com/plan44/plan44-feed/tree/master/p44utils-lvgl-sample)

### macOS

Use the XCode project with XCode 11

### linux

The standard procedure _should_ work (but it isn't tested):

    autoreconf -fi
    ./configure
    make
    install


## Usage

    p44lvglsample --uiconfig uiconfig_sample.json
    
There is a very basic _uiconfig_sample.json_ file included in the repository. TBD: extend to show more features

*(c) 2019 by Lukas Zeller / [plan44.ch](http://www.plan44.ch/automation)*







