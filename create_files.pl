#!/usr/bin/perl
use strict;
use warnings;

# Usage : perl create_files.pl

my $block_size = 4096;

my $count = 1;
my $filename = "file4k.txt";

my $cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";

# 4k
print "$cmd\n";
system($cmd); 

# 32k
$count = (32 * 1024) / $block_size;
$filename = "file32k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 64k
$count = (64 * 1024) / $block_size;
$filename = "file64k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd); 

# 128k

$count = (128 * 1024 ) / $block_size;
$filename = "file128k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd); 

# 256k

$count = (256 * 1024 ) / $block_size;
$filename = "file256k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd);

# 512k

$count = (512 * 1024 ) / $block_size;
$filename = "file512k.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd);

# 1M

$count = ( 1 * 1024 * 1024 ) / $block_size;
$filename = "file1M.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd);


# 4M
$count = ( 4 * 1024 * 1024 ) / $block_size;
$filename = "file4M.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd);

# 8M
$count = ( 8 * 1024 * 1024 ) / $block_size;
$filename = "file8M.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd);

# 256M

$count = (256 * 1024 * 1024 ) / $block_size;
$filename = "file16M.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";
system($cmd);

# 512M

$count = (512 * 1024 * 1024 ) / $block_size;
$filename = "file512M.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd);


# 1G
$block_size = 32768;
$count = ( 1 * 1024 * 1024 * 1024 ) / $block_size;
$filename = "file1G.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd);


# 4G
$count = ( 4 * 1024 * 1024 * 1024 ) / $block_size;
$filename = "file4G.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd);

=begin

# 5G
$count = (5 * 1024 * 1024 * 1024) / $block_size;
$filename = "file5G.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";       
print "$cmd\n";
system($cmd);

# 6G
$count = ( 6 * 1024 * 1024 * 1024 ) / $block_size;
$filename = "file6G.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd);

# 7G
$count = (7 * 1024 * 1024 * 1024) / $block_size;
$filename = "file7G.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";       
print "$cmd\n";

system($cmd);

# 8G
$count = ( 8 * 1024 * 1024 * 1024 ) / $block_size;
$filename = "file8G.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd);

# 10G
$count = (10 * 1024 * 1024 * 1024 ) / $block_size;
$filename = "file10G.txt";
$cmd = "dd if=/dev/zero of=$filename bs=$block_size seek=0 count=$count";
print "$cmd\n";

system($cmd);

