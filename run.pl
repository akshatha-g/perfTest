#!/usr/bin/perl
use strict;
use warnings;

my $file = $ARGV[0];
my $n = $ARGV[1];

my $i = 0;

my $cmd = "./mp_big /mnt/ramdisk/ $file";

#my $cmd = "./file_rd $file";

#my $cmd = "./ioctl $file";

#my $cmd = "./rd_all $file";

for ($i = 0; $i < $n ; $i++) {
    system($cmd);
}
