# Apple Controller

## Introduction

Welcome to this repository who visited for solve the irritation when using Apple device,(especially, iPad)

## How It works?

Basically, it works as bluetooth HID Device. It sends data to host device like a keyboard, mouse. it can send not only just basic action(prev, next, start, stop, forward, backward..) but actual mouse movement if you want it and add   accelerometer(MPU6050)

## Changelog

### Alpha 0.1 (Mar 31th, 2021)

 It works but literally, it just works. i used ESP32 BLE Mouse (Tv-K) library.(i used it for research and later, it will be replaced with native code after problems i'm facing with be solved.) I tested it with Android, iPadOS, Mac(Big Sur). Unlike my expectation, mouse cursor appears and works well on Apple Device.

I'm facing with problems following:

1. I couldn't get angle data from MPU6050. Working but... it really sucks. Apple Controller sends not that good data to host device. I'm gonna trying to solve this problem using DMP. Anyway, that's really problematic since it ruins whole development process. 

