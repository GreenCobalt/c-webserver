# C Web Server
A simple (non-complete) HTTP server in C


Compiling:
```
git clone https://github.com/GreenCobalt/simple_c_webserver
cd simple_c_webserver
make
```

Running:
```
./server
```
Will open a socket on all interfaces, port 9000. When entered into a web browser, the server will echo the HTTP version, headers, etc.