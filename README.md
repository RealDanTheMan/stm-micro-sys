STM-MICRO-SYS - Example embedded app for STM32 based microcontroller
-------------------------------------------------------------------------------

The purpose of this application is to boot up the microcontroller device to full power and periodically sample temperature and humidity while displaying results on a connected LCD display.

* App high level logic as well as scripts to build and run the project can be found in `./app/`
* Custom hardware logic for most of the device crucial systems is abstracted in `./corelib/src/` its where most of the implementation lives.
* Low level headers for the device provided by STM32 can be found in `./corelib/stm32/`

<br>
<div style="display: flex; justify-content: space-between;">
  <img src="images/image_01.png" alt="Example Image" width="320" height="320">
  <img src="images/image_02.png" alt="Example Image" width="320" height="320">
</div>
