#!/usr/bin/perl
use strict;
use warnings;

# Usage : perl create_files.pl

my $block_size = 64;

# 64b
my $count = 1;
my $filename = "file64B.txt";
my $cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 128 b
$count = (128) / $block_size;
$filename = "file128B.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 256 b
$count = (256)  / $block_size;
$filename = "file256B.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 512b
$count = (512)  / $block_size;
$filename = "file512B.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 1k
$count = (1024)  / $block_size;
$filename = "file1k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 4k
$count = (4 * 1024) / $block_size;
$filename = "file4k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 8k
$count = (8 * 1024) / $block_size;
$filename = "file8k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 12k
$count = (12 * 1024) / $block_size;
$filename = "file12k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 16k
$count = (16 * 1024) / $block_size;
$filename = "file16k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 20k
$count = (20 * 1024) / $block_size;
$filename = "file20k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 24k
$count = (24 * 1024) / $block_size;
$filename = "file24k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 28k
$count = (28 * 1024) / $block_size;
$filename = "file28k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 32k
$count = (32 * 1024) / $block_size;
$filename = "file32k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 
