# Precharge Firmware in C

Hardware: [STM32F103T8U6](https://octopart.com/part/stmicroelectronics/STM32F103T8U6)

## Compile

Ensure `BASE_CONFIG.ready` is `true` once ready for testing.

You can change the optimization option in the Makefile.

```bash
$ make clean # To clear build files
$ make
```

## Code Structure

As visual aid, the image below shows what functions call each other.

![](./images/function-diagram.jpg)