# DuckyCustomRGB
Software control for **Ducky RGB Keyboard** to create custom lighting effects. Simple effect such as reactive key press over rainbow background can be created. The code is only tested for my **Ducky One 2 RGB TKL** (ANSI layout).

### Light Effect Example
**Startup/Idle Effect**
[![Start up/ idle](https://raw.githubusercontent.com/simon-yeunglm/DuckyCustomRGB/main/images/ducky_keyboard_start_idle.png)](https://youtu.be/zOWBc_wqLH8)

**Reactive Key Press**
[![Start up/ idle](https://raw.githubusercontent.com/simon-yeunglm/DuckyCustomRGB/main/images/ducky_keyboard_reactive_key.png)](https://youtu.be/ribHVTtqROA)

**Blinking Caps Lock and Scroll lock**
[![Start up/ idle](https://raw.githubusercontent.com/simon-yeunglm/DuckyCustomRGB/main/images/ducky_keyboard_blink_lock.png)](https://youtu.be/XlX9y1fH7to)

**Changing Background depends on Holding Key**
[![Start up/ idle](https://raw.githubusercontent.com/simon-yeunglm/DuckyCustomRGB/main/images/ducky_keyboard_hold_key.png)](https://youtu.be/mUIbwAXd1ow)

**System Volume change notification**
[![Start up/ idle](https://raw.githubusercontent.com/simon-yeunglm/DuckyCustomRGB/main/images/ducky_keyboard_volume_notification.png)](https://youtu.be/m7wfYQFCFxE)

### How it works
This software emulates the **Ducky** official **DuckyRGBSeries** software to control the keyboard RGB LED. The process of the emulation is described in the [DuckyAPI](https://github.com/Latedi/DuckyAPI). Note that, we need to disable the **Ducky RGB Series HID** start up program in order to get the same Wireshark capture. Also, some of the returned USB packets (around 8 packets) from the keyboard may not be the same, but we can still connect to the keyboad to control the LED.

This software consist of 2 logical layers:
1. **Keyboard layer** to control each individual LED (under directory `source/keyboard/`). It is responsible for creating the USB packets for changing LED color.
2. **Effect layer** to control different lighting effects (under directory `source/render/`). A rectangular **render target** is allocated to represent the color of the whole keyboard. Each key has a size of 4x4 pixels. And then we create a render graph to defines how the lighting effects are blended together.

This software will create a hidden window to listen to key press event using [Raw Input](https://docs.microsoft.com/en-us/windows/win32/inputdev/raw-input) instead of [Hooks](https://docs.microsoft.com/en-us/windows/win32/winmsg/about-hooks). Note that, the **Fn** key will have no response as this key is not a key regonized by Windows. Although from the Wireshark capture, we can receive the **Fn** key USB packet, when holding the **Fn** key, no other key press event is sent to the PC. So I just don't bother to capture that **Fn** key USB packet to keep thing simple.

### Macro
When running the official **DuckyRGBSeries** software, keyboard macro function will be disabled. So our software suffer from this too. To work around this, our program support software macro for some simple function like, emulate key press input, open application and changing system sound volume.
