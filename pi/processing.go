package main

import (
	"flag"
	"fmt"
	"github.com/tarm/goserial"
	"log"
	"regexp"
)

func check(e error) {
	if e != nil {
		log.Fatal(e)
	}
}

func main() {
	reg, err := regexp.Compile(`[^[A-Za-z0-9:\n\s]+`)
	check(err)
	regAddNewline, err := regexp.Compile(`l`)
	check(err)

	port := flag.String("usbport", "/dev/tty.usbmodem1421", "the usb port use ls /dev/tty*")
	flag.Parse()

	c := &serial.Config{Name: *port, Baud: 9600}
	s, err := serial.OpenPort(c)
	check(err)
	fmt.Println("Hello World!")

	buf := make([]byte, 256)
	for {
		n, err := s.Read(buf)
		check(err)
		news := string(buf[:n])
		safe := reg.ReplaceAllString(news, "")
		safe = regAddNewline.ReplaceAllString(safe, "\nl")
		fmt.Print(safe)
	}
}
