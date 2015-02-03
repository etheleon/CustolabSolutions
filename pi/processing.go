package main

import (
	"bytes"
	"flag"
	"fmt"
	"github.com/tarm/goserial"
	"log"
	"time"
)

type lcdScreen struct {
	line1, line2, line3, line4                  string
	address, row1add, row2add, row3add, row4add byte
}

func check(e error) {
	if e != nil {
		log.Fatal(e)
	}
}

func printInfo(lcd *lcdScreen) {
	for {
		fmt.Print(lcd.line1, lcd.line2, lcd.line3, lcd.line4, "\n")
		time.Sleep(1 * time.Second)
	}
}

var port string

func init() {
	const (
		defaultPort = "/dev/tty.usbmodem1421"
		usage       = "the usb port use ls /dev/tty*"
	)
	flag.StringVar(&port, "usbport", defaultPort, usage)
	flag.StringVar(&port, "u", defaultPort, usage+" (shorthand)")
}

func main() {
	lcd := lcdScreen{"", "", "", "", 0xFE, 128, 192, 148, 212}

	flag.Parse()

	c := &serial.Config{Name: port, Baud: 9600}
	s, err := serial.OpenPort(c)
	check(err)

	go printInfo(&lcd)

	var tempBuf bytes.Buffer
	buf := make([]byte, 128)
	for {
		n, err := s.Read(buf)
		check(err)
		for i := range buf[:n] {
			if bytes.Equal([]byte{buf[i]}, []byte{lcd.address}) {
				row := tempBuf.Next(1)
				switch {
				case bytes.Equal(row, []byte{lcd.row1add}):
					lcd.line1 = "\nPhase: "
					lcd.line1 += tempBuf.String()
				case bytes.Equal(row, []byte{lcd.row2add}):
					lcd.line2 = "\n\tPhase Duration: "
					lcd.line2 += tempBuf.String()
				case bytes.Equal(row, []byte{lcd.row3add}):
					lcd.line3 = "\n\tPhase Time left: "
					lcd.line3 += tempBuf.String()
				case bytes.Equal(row, []byte{lcd.row4add}):
					lcd.line4 = "\n\tTotal Time left: "
					lcd.line4 += tempBuf.String()
				}
				tempBuf.Reset()
			} else {
				tempBuf.WriteByte(buf[i])
			}
		}
	}
}
