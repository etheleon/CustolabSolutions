#!/usr/bin/env perl

use v5.18;
use strict;
use warnings;

use Device::SerialPort::Arduino;

die "Usage $0: <port>\n
run ls /dev/tty* before and after plugging in arduino to see which is your usb port\n" unless $#ARGV == 0;

my $port = shift;

my $Arduino = Device::SerialPort::Arduino->new(
    port     => $port,
    baudrate => 9600,
    databits => 8,
    parity   => 'none',
);

while(1) {
    my $line = $Arduino->receive(1);
    $line =~ s/[^[:ascii:]]//g;
    say $line;
    sleep(1)
}
