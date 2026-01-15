# CH32V305-UAC2
ch32v305fbp6 + es9018k2m + opa1678 usb dac  

Due to using inexpensive components, the clock has approximately a 200 Hz error. Additionally, the MCLK GPIO speed exceeds the maximum 50 MHz limit at 192 kHz.  
If desired, you can use an external crystal for the I2S MCLK clock.  
This dac only support 48k,96k,192k 32bits, dsd and other formats are not considered.  
This DAC only supports high-speed connections.  
You might hear some popping sounds during sudden high-load tasks. I have added mute detection code to eliminate some of these occurrences. I hope it helps you. You can adjust your parameters in config.h.  

## setup
Just flash HSDAC.elf to hardware. Don't use HID-Bootloader and dfu, they will be redoing in future.  

## help wanted
Can someone redesign the power amplifier circuit?  

# IMAGE
![DAC Image](resource/dac.jpg)


