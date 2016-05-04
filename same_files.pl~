#!/usr/bin/perl                                                                 
use strict;                                                                     
use warnings;                                                                   
use POSIX ();                                                                   
use POSIX qw(ceil);                                                             
                                                                                
# Usage : perl same_files.pl <size> [dir] [<count>]

my $argc = @ARGV;
if ($argc == 0) {
    print "Usage : perl same_files.pl <size> [dir] [<count>]\n";
    exit;
}

my $size = $ARGV[0];
my $dir  = $ARGV[1] ? $ARGV[1]."/" : "";
my $loopc = $ARGV[2] ? $ARGV[2] : 64;
                                                                               
my $file = "file";
my $num = 1;
my $block_size = $size; 

my $cmd; 
my $count = $size / $block_size; 

for (; $num <= $loopc ; $num++) {
    $cmd = "dd if=/dev/zero of=$dir".'/'."$file"."$num bs=$block_size seek=0 count=$count";
    print "$cmd\n";
    system($cmd);
}                                                                       
