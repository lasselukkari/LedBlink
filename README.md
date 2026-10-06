# Arduino Full Stack Tutorial

Control an ESP32 LED from a React interface built with Vite and TypeScript.
The board runs aWOT 4 and serves both the HTTP API and the compressed frontend.
During development, Vite serves the interface on your computer and proxies
requests to the board.

The [full-stack tutorial](https://awot.net/en/guide/tutorial.html) walks through
the project. This repository contains the complete application.

## Requirements

- Node.js 22.12 or newer and npm.
- Arduino IDE with the [ESP32 board package](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).
- An ESP32 board with a regular GPIO LED, a USB cable, and a shared Wi-Fi network
  for the board and your computer.
- aWOT **4.0.0**, installed as an Arduino library. If Library Manager offers
  this version, install it there. To install the development version, open the
  `libraries` directory in your Arduino sketchbook and run:

  ```sh
  git clone --branch 4 https://github.com/lasselukkari/aWOT.git aWOT
  ```

Find the sketchbook location in the Arduino IDE preferences. Keep the complete
library directory, including `src/`, and restart the IDE after installation.

## Build and upload

From the project directory, install the locked dependencies and generate the
frontend assets:

```sh
npm ci
npm run build:firmware
```

This checks TypeScript, builds the frontend into `dist/`, and generates
`BlinkServer/StaticFiles.h`. Generate this header before compiling the sketch;
it is excluded from version control and is rebuilt from the frontend sources.

Open `BlinkServer/BlinkServer.ino` in Arduino IDE. Set `WIFI_SSID` and
`WIFI_PASSWORD`, then set `LED_PIN` and `LED_ACTIVE_LOW` for your board. The
defaults are GPIO 2 and an active-high LED. Boards with addressable RGB LEDs
need their own LED driver.

Select your board and serial port, then compile and upload. Open Serial Monitor
at **115200 baud** and open the printed address in your browser, for example
`http://192.168.1.227/`. The ESP32 now serves the interface and API.

The generated asset router is mounted once after the LED routes. The firmware
uses `App` and explicitly closes each client after processing its request.

## Develop the frontend

Keep the flashed board running. Set the `/led` proxy target in `vite.config.ts`
to the IP address printed in Serial Monitor, then run:

```sh
npm run dev
```

Open the local URL printed by Vite, usually `http://localhost:5173`. Frontend
edits update in the browser without uploading the firmware. Relative requests
to `/led` are forwarded to the board during development and go directly to it
when the built interface is served by the ESP32. Restart Vite after changing
the proxy configuration.

The interface reads the initial LED state, disables the button during an update,
and displays the state confirmed by the board. Connection failures and invalid
responses appear on the page; check the board and reload to retry.

## API

| Request | Result |
| --- | --- |
| `GET /led` | Return `0` for off or `1` for on. |
| `PUT /led` with exactly one byte, `0` or `1` | Set the LED and return its new state. |
| `PUT /led` with any other body | Return HTTP 400 and keep the current LED state. |

Replace the address below with your board's address:

```sh
curl --fail http://192.168.1.227/led
curl --fail -X PUT -H 'Content-Type: text/plain' --data-binary '1' http://192.168.1.227/led
curl --fail -X PUT -H 'Content-Type: text/plain' --data-binary '0' http://192.168.1.227/led
curl -i -X PUT --data-binary 'invalid' http://192.168.1.227/led
```

## Scripts

| Command | Purpose |
| --- | --- |
| `npm run dev` | Start Vite with the board's API proxy. |
| `npm run build` | Check TypeScript and produce the frontend in `dist/`. |
| `npm run build:firmware` | Build the frontend and generate `BlinkServer/StaticFiles.h`. |
| `npm run preview` | Serve the production frontend locally for inspection. |

The asset converter is the published `awot-static` command from
`awot-scripts@3.1.2`, verified with aWOT 4. Its configuration in `package.json`
uses `dist/` as the source and `BlinkServer/` as the sketch directory. Generated
handlers serve gzip-compressed files with their content type and length.

After changing the frontend, run `npm run build:firmware` and upload the sketch
again. After changing only firmware code, compile and upload. Check the Arduino
compiler's size report against your board's application partition; the embedded
web assets consume program flash alongside the sketch and library.
