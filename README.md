# Multithreaded Proxy Server

A simple multithreaded web proxy server to deliver web content on behalf of a remote web server.

## How it works

### Proxy server
1. The proxy server receives a request from the web browser.

2. The proxy server logs the request message header to a log file, `Proxy.log`

3. The HTTP request message is parsed to determine it's request type.

4. If the request is anything other than a GET request, state the type and exit the handling thread. Otherwise, the request header is parsed to determine the origin server to which the request is directed.

5. The proxy server then makes a connection to the origin server and makes a new request for the item. It sends this request and awaits a reply from the origin server.

6. The proxy server then sends the response back to the client web browser and exits the thread.

## Compiling
The user must first edit the following define statements:

* `SERVER_IP`    
* `PORT`

The two programs can then be compiled using the following `Makefile` configuration.

```
# Usage:
# make			# compiles source files
# make clean	# removes compiled files

all:
	@echo "Compiling server code..."
	$(CC) proxy.c -o proxy -lpthread
	@echo "Done! Copy the compiled file(s) to their respective machines."

clean:
	@echo "Cleaning up..."
	rm -f proxy
```

## Usage
The user must start the proxy server with the following command in a terminal:
* `./proxy`

The user must also configure their web browser to use a proxy.
