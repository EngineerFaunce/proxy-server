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