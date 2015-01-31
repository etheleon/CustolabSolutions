package main

import (
	"fmt"
	"github.com/tarm/goserial"
	"log"
)

func main() {
	c := &serial.Config{Name: "/dev/tty.usbmodem1421", Baud: 9600}
	s, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println("Hello World!")

	buf := make([]byte, 256)
	for {

		n, err := s.Read(buf)
		if err != nil {
			log.Fatal(err)
		}
		news := string(buf[:n])
		fmt.Print(news)
	}
}
