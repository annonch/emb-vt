#!/usr/bin/perl

use strict;
use warnings;

my $filename = $ARGV[0]; # input is just algo name

my $cur_file;
my $out_file;
my $time_s=0;
my $time_pns=0;

$cur_file = $filename;
$out_file = $cur_file . "_data";

open(my $fh2, '>', $out_file)
    or die "Could not open out file '$out_file' $!";

open(my $fh, '<:encoding(UTF-8)', $cur_file)
    or die "could not open read file '$cur_file' $!";

while (my $row = <$fh>) {
    chomp $row;
    if ( $row =~ /.* TIME-RISE: (\d+).(\d+) nanoseconds / ) {
                $time_s=$1;
                $time_pns=$2;
                print $fh2 "$time_s.\.$time_pns\n";
    }
    elsif ($row =~ /.* TIME-FALL: (\d+).(\d+) nanoseconds/ ) {
                $time_s=$1;
                $time_pns=$2;
                print $fh2 "$time_s.$time_pns\n";
    }
}
