# AL-OS

![OS Dev](https://img.shields.io/badge/OS-Development-blue)
![Language C](https://img.shields.io/badge/Language-C-green)
![Bootloader GRUB](https://img.shields.io/badge/Bootloader-GRUB-orange)
![Status Experimental](https://img.shields.io/badge/Status-Experimental-yellow)

---

## 🇷🇺 О проекте

**AL-OS** - экспериментальная операционная система на языке **C** с загрузчиком **GRUB**.
Проект создаётся "ради прикола" и интереса к системному программированию.

Работает на обычных компьютерах **в BIOS-режиме**.
**UEFI пока не поддерживается.**

Система находится в активной разработке, обновления выходят постепенно.

---

## Автор

Создатель проекта - **Al-Mus**

Создатель форка - **MrLeo0010**

---

## Возможности

* собственное ядро
* GRUB загрузка
* базовый вывод
* ISO-образ в Releases

---

## Поддерживаемые режимы загрузки

* BIOS - ✔ поддерживается
* UEFI - ✘ пока нет

---

## Releases

➡ [![Releases](https://img.shields.io/github/v/release/MrLeo0010/al-os?label=Releases\&color=blue)](https://github.com/MrLeo0010/al-os/releases)

---

## Build

Ubuntu/Debian:
sudo apt update && sudo apt install -y build-essential gcc-i686-linux-gnu gcc-14-i686-linux-gnu libc6-dev-i386 binutils nasm grub-pc-bin grub-common xorriso && make iso

Также вы можете использовать docker контейнер mrleo0010/al-os-build:
docker run --rm -v "$PWD:/build" mrleo0010/al-os-build sh -c "make iso"

---

## Запуск

Инструкция по запуску появится позже.
На данный момент можно использовать:

* QEMU
* VirtualBox
* реальный ПК (BIOS mode)

---

## Участие

Форк, предложения, Pull Requests - приветствуются.
Если есть идеи - создавайте Issue.

---

## Что хочу добавить

1. Добавить поддержку интернета и команды ping
2. Добавить поддержку исполняемых файлов линукс elf 
3. Пока не придумал, буду дополнять 

Порядок идей выставлен по приоритету. 1 - это то что нужно и хочу обязательно, 2 - желательно, остальное с низким приоритетом

---
