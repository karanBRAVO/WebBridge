# WebBridge
A lightweight and efficient WebSocket/HTTP server written in C++, paired with a modern HTML, CSS, and JavaScript client for seamless real-time communication.

## Features
- **WebSocket Server**: Real-time, bidirectional communication using WebSocket protocol.
- **HTTP Server**: Serve static files or handle HTTP requests efficiently.
- **HTML/CSS/JavaScript Client**: A responsive and interactive front-end with built-in WebSocket support.
- **Cross-Platform**: Works on Linux, Windows, and macOS.
- **Lightweight**: Minimal dependencies for fast setup and execution.

## Tech Stack
- **Server**: C++ (WebSocket/HTTP implementation using `Boost.Asio` and `Boost.Beast`)
- **Client**: HTML, CSS (for styling), and JavaScript (for WebSocket communication)

## Getting Started

### Prerequisites
- A C++ compiler supporting C++11 or later
- Basic knowledge of WebSockets and HTTP
- A browser to test the client-side application

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
