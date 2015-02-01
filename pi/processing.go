package main

import (
	"bytes"
	"flag"
	"fmt"
	"github.com/tarm/goserial"
	"log"
	"regexp"
	"strconv"
	"strings"
	"time"
)

func check(e error) {
	if e != nil {
		log.Fatal(e)
	}
}
func printInfo(l1, l2, l3, l4 *string) {
	for {
		fmt.Print(*l1, *l2, *l3, *l4, "\n")
		time.Sleep(1 * time.Second)
	}
}

func main() {
	//removes address byte for assigning row
	reg, err := regexp.Compile(`[^[A-Za-z0-9: ]+`)
	//captures character l which is appended every new line
	regNewline, err := regexp.Compile(`l`)

	lineRE, err := regexp.Compile(`l(\d)`) // want to know what is in front of 'at'

	port := flag.String("usbport", "/dev/tty.usbmodem1421", "the usb port use ls /dev/tty*")
	flag.Parse()

	c := &serial.Config{Name: *port, Baud: 9600}
	s, err := serial.OpenPort(c)
	check(err)

	var l1, l2, l3, l4 string
	var tempBuf bytes.Buffer

	go printInfo(&l1, &l2, &l3, &l4)

	buf := make([]byte, 256)
	for {
		n, err := s.Read(buf)
		check(err)
		news := string(buf[:n])
		serialString := reg.ReplaceAllString(news, "")

		if regNewline.MatchString(serialString) {
			splitted := strings.Split(serialString, "l")
			tempBuf.WriteString(splitted[0])
			lineNum := lineRE.FindStringSubmatch(tempBuf.String())
			if len(lineNum) == 2 {
				//fmt.Println(lineNum[1])
				//check(err)
				line, _ := strconv.Atoi(lineNum[1])
				switch {
				case line == 1:
					l1 = tempBuf.String()[2:]
				case line == 2:
					l2 = tempBuf.String()[2:]
				case line == 3:
					l3 = tempBuf.String()[2:]
				case line == 4:
					l4 = tempBuf.String()[2:]
				}
			}
			tempBuf.Reset()
			tempBuf.WriteString("l")
			tempBuf.WriteString(splitted[1])
		} else {
			tempBuf.WriteString(serialString)
		}
	}
}
