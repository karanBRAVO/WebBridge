# WebBridge
A lightweight and efficient WebSocket/HTTP server written in C++, paired with a modern HTML, CSS, and JavaScript client for seamless real-time communication.

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![License](https://img.shields.io/badge/license-MIT-blue)

## Table of Contents
- [Features](#features)
- [Tech Stack](#tech-stack)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Contributing](#contributing)
- [License](#license)

## Features
- **WebSocket Server**: Real-time, bidirectional communication using WebSocket protocol.
- **HTTP Server**: Serve static files or handle HTTP requests efficiently.
- **HTML/CSS/JavaScript Client**: A responsive and interactive front-end with built-in WebSocket support.
- **Cross-Platform**: Works on Linux, Windows, and macOS.
- **Lightweight**: Minimal dependencies for fast setup and execution.

## Tech Stack
- **Server**: C++ (WebSocket/HTTP implementation using `Boost.Asio` and `Boost.Beast`)
- **Client**:
	- GUI: HTML, CSS (for styling), and JavaScript (for WebSocket communication)
 	- CLI: C++ (using `Boost.Asio` and `Boost.Beast`)

## Getting Started

### Prerequisites
- A C++ compiler supporting C++11 or later
- Basic knowledge of WebSockets and HTTP
- A browser to test the client-side application
- Boost Library: [download Boost](https://www.boost.org/releases/latest/)

### Installation

1. Clone the repository:
```bash
git clone https://github.com/your-username/WebBridge.git
cd WebBridge
```

2. Open up the `solution` file in Visual Studio

3. Add the `Boost` directory to the project properties:
	- Right-click on the project in Solution Explorer
	- Select `Properties`
	- Under `C/C++`, select `General`
	- Add the path to the `include` directory in `Additional Include Directories`

4. Build the server
	- Right-click on the project in Solution Explorer
	- Select `Build`
	- Ensure there are no errors

5. Run the server
	- Replace `your.exe` with the name of your compiled server executable
	- The server will start listening on port 8080

	```bash
	./your.exe 0.0.0.0 8080 .
	```

6. Open your web browser and navigate to `http://localhost:8080` to see the client interface.

`note`: If you have have build the project in Visual Studio then you can directly run
```bash
.\x64\Debug\ClipTrans.exe 192.168.31.189 8080 .\client\
# replace "192.168.31.189" with your ip
```

Get your IP Address using below command
```bash
ipconfig
```

## GUI Preview
Visit: http://192.168.31.189:8080/

![Screenshot 2025-05-14 213600](https://github.com/user-attachments/assets/f610f398-afaa-47b8-b7b9-556d694c886a)

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request. For major changes, please open an issue first to discuss what you would like to change.

## License
This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.
