# MahPicoBoi

![MahPicoBoy Logo Text Alt3 big](https://github.com/AP-AAiS/MahPicoBoi/assets/68875171/7395376f-ddb4-439d-9d3f-3c267d7ef070)

A Raspberry Pi Pico based MIDI interface for the Nintendo Game Boy

# This sound is what all true warriors strive for!
Hey there! Welcome to the repository of MahPicoBoi, a Raspberry Pi Pico based MIDI interface for your Game Boy. This project was designed from the ground up to be cheap, reliable, simple and easy to build, meaning that you can build your own MahPicoBoi with nothing more than a soldering iron, a pair of scissors, a few select components and a bit of time on your hands.

![Schritt 6](https://github.com/AP-AAiS/MahPicoBoi/assets/68875171/24ec21bd-c762-4de8-b614-6a071cfe130e)

# Features
- Can send MIDI signals to your Game Boy
- Can send out MIDI signals received from your Game Boy  
- Can act as a MIDI-THRU device
- Works with both LSDJ and mGB
- Can filter out Continuous Controller messages with the press of a button, allowing you to play full MIDI files on your Game Boy without the CCs messing up the mGB config
- Works with all versions of the Game Boy, aside from the Game Boy Micro 
- Based on the Raspberry Pi Pico, one of the cheapest RP2040 Microcontrollers on the market right now
- Quick and easy to build thanks to the step-by-step tutorial linked below

# Video Demonstration
![MahPicoBoi V1](https://github.com/AP-AAiS/MahPicoBoi/assets/68875171/a8ee51e2-02f3-44e1-a3b9-d9f6cccd48a4)

You can find a video that goes over all the technical details of MahPicoBoi and shows it in action over on my YouTube channel:
https://youtu.be/hq1gwZe8EhA

# Getting your own MahPicoBoi
![image](https://github.com/AP-AAiS/MahPicoBoi/assets/68875171/159e9deb-1e86-4603-a3c3-a50a9f353de3)

For those of you interested in getting your own MahPicoBoi in a simple, pre-built package, there are now several different kits of the official MahPicoBoi V2 PCB for sale on my e-bay page. You can check them out here:
[https://www.ebay.de/itm/315250373588](https://www.ebay.de/itm/315250373588)
# Build your own MahPicoBoi
Building your own MahPicoBoi is as easy as it is fast! All you need to do is get your hands on the following list of components, then you can simply follow the step-by-step guide from my website.

# Required Components
- 1x 	        Game Boy variant other than the Game Boy Micro*
- 1x 	        MIDI-Source
- 1x 	        Game Boy (NOT GBA!) Flash-Cart with mGB or LSDJ**
- 1x 	        Raspberry Pi Pico Microcontroller
- 1x 	        Micro USB Cable
- 1x 	        H11L1 or 6n 138 Optocoupler
- 1x 	        220 Ω Resistor
- 1x 	        470 Ω Resistor
- 1x 	        1n914 or 1n4148 Diode
- 1x 	        MIDI Cable / USB MIDI Cable
- 1x – 2x 	  5 pin MIDI Connector***
- 1x 	        Set of Jumper Wires
- 1x 	        Game Boy or Game Boy Pocket Game Link Cable
- 3 – 4x 	    Male Dupont Connectors or Luster Terminals or Similar
- 1x 	        Breadboard
- 1x 	        Set of Raspberry Pi Pico Pin Headers
- 0x – 1x 	  10 Ω Resistor***
- 0x – 1x 	  33 Ω Resistor***

*GB Micro is not supported, since neither mGB nor LSDJ work with it.

**While you can run mGB and LSDJ on a GBA flashcart, you'll need an original GB / GBC Flash Cart to be able to access the GBA's GBP Link Port.

***If you don't care about the MIDI-OUT capabilities of your MahPicoBoi, you can get away with only using one MIDI port. In that case, you also won't need the 10 Ω and 33 Ω resistors. 

# Build Guide
![MIDI GB Complete_Steckplatine ENGLISH](https://github.com/AP-AAiS/MahPicoBoi/assets/68875171/6e413c80-334f-43bc-8100-0ea7eda8ebf1)


Detailed build instructions can be found over at my website. Simply head on over to:
https://rykani.com/mahpicoboi

# Usage Guide
- Connect your MIDI Source to the MIDI-IN and MIDI-OUT ports of your MahPicoBoi
- Connect your Raspberry Pi Pico to a PC via a Micro USB cable
- Drag and drop the MahPicoBoiDX.uf2 file onto your Pico
- Wait for the LED to turn on, keep the Pico connected to the USB port for power
- Start mGB / LSDJ on your Game Boy
- Connect your modified Game Link Cable to your Game Boy
- Done! MIDI messages received on the MIDI-IN port will now be sent out to your Game Boy
- If you want to block out CC messages, press the bootsel button on your Pico. The LED should now start blinking and CC messages will be filtered out

# Compatible Software
- Trash80's mGB: https://github.com/trash80/mGB
- TonyTwoStep's fork of mGB : https://github.com/TonyTwoStep/mGB
- LSDJ: https://www.littlesounddj.com/lsd/index.php
- Probably everything else that uses a standard MIDI implementation. I haven't tried anything else, though, so no guarantees!

# Future Development Plans
- Have the Pico read data from the serial-out wire and send it out via the TX pin. This would save us having to switch out the MIDI-OUT jumper wire when switching between Game Boy MIDI-OUT and MIDI-THRU mode.
- TBD

# Suggestions
Do you have your own suggestions on how to develop and improve MahPicoBoi in the future? Send them to me via E-Mail at: complaints@rykani.com

Thanks for giving this little project your time!

# Thanks
The MahPicoBoi wouldn't exist without the brilliant work of a large group of amazing people. In no particular order, I'd like to thank:
- Timothy Lamb, aka "Trash80", for his trailblazing work on the ArduinoBoy and for creating mGB
- "TonyTwoStep" for his development of an updated version of mGB
- Ehren Julien-Neitzert, aka "ehrenjn", for creating the incredibly useful Gameboy-Serial-Link-Debugger
- The entire PanDoc group for their impressive work documenting the Game Boy hardware
- The GBATEK group for their similarly incredible work documenting the GBA, DS and DSi
- All the people that have worked and are working on GBDK and GBDK-2020
- The team from gbdev.io that gave us all a great starting off point for diving into the complex world of Game Boy software development

MahPicoBoi wouldn't exist without you guys. From the bottom of my heart: Thank you for being awesome!

# Support the Dev
<img width="620" alt="TextLogo_white_stroke@2x" src="https://github.com/AP-AAiS/MahPicoBoi/assets/68875171/2cefabce-ec8c-4a41-8927-d82beb590c35">

https://ko-fi.com/apaais

If you want to help me pay off some of the development costs of MahPicoBoi, you can send me a buck or two over at Ko-fi. Don't worry, there's no paywalled content nor anything else on there. My Kofi account just exists for people who have the means and will to support me. If you lack either, don't worry, I'll make sure you're not missing out on anything!
