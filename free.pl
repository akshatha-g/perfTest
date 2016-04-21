#!/usr/bin/perl
use strict;
use warnings;

my $cmd = "free -m";
while(1) {
    system($cmd);
    sleep(1);
}
