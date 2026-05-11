# 🇺🇸 About

**AL-OS** is an experimental operating system written in **C** and booted using **GRUB**.
The project is made "just for fun" and out of interest in low-level programming and OS development.

Runs on real machines in **BIOS mode**.
**UEFI is not supported yet.**

The system is in active development and new changes appear over time.

---

## Author

Developed by **Al-Mus**

Forked by **MrLeo0010**

---

## Features

* custom kernel
* GRUB bootloader
* basic text output
* ISO image in Releases

---

## Boot modes

* BIOS - ✔ supported
* UEFI - ✘ not yet

---

## Releases

➡ [![Releases](https://img.shields.io/github/v/release/Al-Mus/al-os?label=Releases\&color=blue)](https://github.com/MrLeo0010/al-os/releases)

---

## Build

Ubuntu/Debian 13:
sudo apt update && sudo apt install -y build-essential gcc-i686-linux-gnu gcc-14-i686-linux-gnu libc6-dev-i386 binutils nasm grub-pc-bin grub-common xorriso && make iso

You can also use the mrleo0010/al-os-build docker container:
docker run --rm -v "$PWD:/build" mrleo0010/al-os-build sh -c "make iso"

---

## Run

A detailed run guide will appear later.
Currently you can use:

* QEMU
* VirtualBox
* real PC (BIOS mode)

---

## Contribute

Forks, ideas and pull requests - welcome.

---
