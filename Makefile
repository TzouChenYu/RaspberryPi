resistanceReader: resistanceReader.cpp Linux_UART.cpp
	g++ -O3 resistanceReader.cpp Linux_UART.cpp -lpthread -lwiringPi -o resistanceReader

resistanceReaderController: resistanceReaderController.c
	gcc resistanceReaderController.c -o resistanceReaderController

install:resistanceReader resistanceReaderController
	sudo rm /bin/resistanceReaderController
	sudo cp resistanceReaderController /bin/
	
