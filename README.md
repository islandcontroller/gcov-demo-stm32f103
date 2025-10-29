# template-stm32f1xx

A bare-minimum template for your next project using a STMicroelectronics STM32F1-series MCU!

<p align="center"><img src="scr.png" /></p>

## Requirements

* Hardware
  * STM32F1-series microcontroller
  * *SEGGER J-Link* or *ST-Link* debug probe (see also: [islandcontroller/jtag-wire-adapter](https://github.com/islandcontroller/jtag-wire-adapter))
* Software
  * Linux OS or WSL installation
  * [Docker Engine](https://docs.docker.com/engine/install/debian/) (running within WSL if applicable)
  * VSCode [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension
  * (WSL only) [usbipd-win](https://learn.microsoft.com/en-us/windows/wsl/connect-usb)

## Usage

* Clone this repository using the following command. Note the use of the `--recursive` tag.
  ```
  git clone --recursive https://github.com/islandcontroller/template-stm32f1xx
  ```
* Open the folder in VSCode
* Connect debug probe
  * (WSL only) attach to WSL using `usbipd attach --wsl --busid <...>`. **This needs to be completed before starting the Dev Container.**
* Run the command "**Dev Containers: Reopen in Container**"
  * Check if the debug probe is recognised as a connected USB device by running `lsusb`.
  * On first launch, you may need to install some udev rules on your host machine. Copy the files to your workspace by running `setup-devcontainer` inside the container.
  * Re-open the workspace on your host and run the `install-rules` script inside the `.vscode/setup` folder.

        cd .vscode/setup
        sudo ./install-rules

  * Afterwards, restart the devcontainer.
* If prompted, select the "`Arm GNU Toolchain x.x`" CMake Kit. 

## Configuration

If you want to change the project name (and hence binary output name), do so in the following files:

* `CMakeLists.txt`
* `.vscode/launch.json`

The project is configured for a `STM32F108C8` MCU by default. If your project uses a different chip, adjust the MCU type in the following files:

* `CMakeLists.txt`
* `Controller/stm32f1xx_hal_conf.h`
* `.vscode/launch.json`