# Arduino Local Web - Standalone Hotspot

Arduino UNO WiFi R4 sketch that creates its own WiFi hotspot and serves a local web page without external network.

## Features
- **Standalone hotspot**: Creates WiFi network `ArduinoTempLocal` (password: `password123`)
- **Local web server**: Responds at `http://192.168.4.1`
- **Real-time temperature**: Reads from pin A0 (simulated or real sensor)
- **Modern design**: Responsive HTML page with inline CSS/JS
- **Auto-refresh**: Updates every 3 seconds via JavaScript fetch
- **No Internet required**: Works completely offline

## Quick Start
1. Upload `local-web.ino` to Arduino UNO WiFi R4
2. Arduino creates hotspot `ArduinoTempLocal`
3. Connect to WiFi (password: `password123`)
4. Open browser to `http://192.168.4.1`
5. See real-time temperature updates

## Files
- `local-web.ino` - Main Arduino sketch
- `upload-simple.sh` - Script to upload to simple file hosts
- `upload-to-drive.py` - Google Drive upload script (requires OAuth)
- `UPLOAD-README.md` - Upload instructions

## How It Works
1. Arduino creates WiFi Access Point
2. Web server listens on port 80
3. HTML page served with temperature display
4. JavaScript fetches `/data` endpoint every 3 seconds
5. Temperature read from analog pin A0 (simulated)

## Customization
- **Change hotspot password**: Modify `apPass` in sketch
- **Change SSID**: Modify `apSSID`
- **Real sensor**: Replace `readTemperature()` with calibration formula
- **Update interval**: Change `UPDATE_INTERVAL`

## License
MIT

## Links
- GitHub: https://github.com/Lisa-286/arduino-local-web
- OpenClaw: https://openclaw.ai
