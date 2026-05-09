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

## Project Website

This project has its own website. There's no information there, but you can download the system or source code. It's not really necessary, but I've included it just in case. This is a demo version of the site, so I'll update it in the future.

https://MrLeo0010.github.io/al-os

[![Website](https://img.shields.io/badge/%F0%9F%8C%90_Website-AL--OS-green?style=flat&logo=github)](https://MrLeo0010.github.io/al-os)

---

## Releases

➡ [![Releases](https://img.shields.io/github/v/release/Al-Mus/al-os?label=Releases\&color=blue)](https://github.com/MrLeo0010/al-os/releases)

---

## Build

Ubuntu/Debian 13:
sudo apt update && sudo apt install -y build-essential gcc-i686-linux-gnu gcc-14-i686-linux-gnu libc6-dev-i386 binutils nasm grub-pc-bin grub-common xorriso && make iso

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

## What I want to add

1. Add internet support and ping commands
2. Add support for Linux elf executables
3. I haven't figured it out yet, but I'll keep adding to it. 

The order of ideas is set by priority. 1 is what I need and want absolutely, 2 is desirable, the rest is with a low priority.

---


