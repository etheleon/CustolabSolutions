#!/usr/bin/env perl

use v5.10;
use strict;
use warnings;

use feature "switch";
use autodie;
use Device::SerialPort::Arduino;
use IO::Handle;

#die "Usage $0: <port>\n" unless $#ARGV == 0;
#$port = shift;

#my $port = '/dev/tty.usbserial-AJ02XSEW';  #on MAC
my $port = '/dev/ttyUSB0';                  #on raspberrypi

my $Arduino = Device::SerialPort::Arduino->new(
    port     => $port,
    baudrate => 115200,
    databits => 8,
    parity   => 'none',
);

open my $output, ">", "/mnt/sensor/output.txt" or die $!;
$output->autoflush;

while(1) {
    my $line = $Arduino->receive(2);
    say $output processing($line);
    #say $output $line;
    sleep(1);
}
sub processing
{
    my ($line) = @_;
    my $value;
    for($line)
    {
       when(m/Temperature: (\d+\.\d+)/)     { $value = 'Temperature: '.eval{$1-27}}
        when(m/CO: (\d+\.\d+)/)             { $value = 'CO: '.eval{10**(-1*(log10($1/13.3)-2))}.'ppm'}
        when(m/NO2: (\d+\.\d+)/)            { $value = 'NO2: '.eval{10**((log10($1/0.8)-6.171579)/1.707511)}.'ppm'}
        #when(m/O3: (\d+\.\d+)/)             { $value = 'O3: '.eval{10**((log10($1/0.8)-6.171579)/1.707511)}.'ppb'} #havent figured this one out
        default {$value=$line}
    }
    return $value;
}

sub log10
{
    my $n = shift;
    return log($n)/log(10);
}
