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
		time.Sleep(500 * time.Millisecond)
		//time.Sleep(1 * time.Second)
	}
}

var port string
var defaultPort string = "/dev/tty.usb"

func init() {
	flag.StringVar(&port, "usbport", defaultPort, "the usb port use ls /dev/tty*")
	flag.StringVar(&port, "u", defaultPort, "the usb port use ls /dev/tty* (shorthand)")
}

func main() {
	lcd := lcdScreen{"Initialising", "\tPhase Duration: 10s", "", "", 0xFE, 128, 192, 148, 212}

	flag.Parse()
	if flag.NFlag() == 0 {
		fmt.Println("Guessing usb port..... \n\tUsing ", defaultPort[0])
	}
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
