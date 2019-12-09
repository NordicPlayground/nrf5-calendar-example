nrf-calendar-example
====================
This is a small example of how to keep track of time using the RTC timer and the time.h library. 
By default the example uses the external 32kHz crystal for maximum accuracy. 

Requirements
------------
- nRF5 SDK version 16.0.0
- nRF52 DK (PCA10040) or nRF52840 DK (PCA10056)
- Segger Embedded Studio or Keil uVision v5

TODO
----
- Add SoftDevice support 
- Add an option to use the app_timer module instead of the RTC

Note
----

The project may need modifications to work with other versions or other boards. 

To compile it, clone the repository in the *[SDK]/examples/peripheral/* folder.

About this project
------------------
This application is one of several applications that has been built by the support team at Nordic Semiconductor, as a demo of some particular feature or use case. It has not necessarily been thoroughly tested, so there might be unknown issues. It is hence provided as-is, without any warranty. 

However, in the hope that it still may be useful also for others than the ones we initially wrote it for, we've chosen to distribute it here on GitHub. 

The application is built to be used with the official nRF5 SDK, that can be downloaded from developer.nordicsemi.com

Please post any questions about this project on: 
[https://devzone.nordicsemi.com](https://devzone.nordicsemi.com)