0751000050703
[‎3/‎29/‎2019 10:47 AM]  Cuong Pham:  
https://www.google.com/maps/place/Tr%C6%B0%E1%BB%9Dng+Thi%E1%BA%BFu+Sinh+Qu%C3%A2n/@11.0695978,106.5204935,17z/data=!3m1!4b1!4m5!3m4!1s0x310b32a57b808ec5:0xd26b9a0d87f54aaf!8m2!3d11.0695925!4d106.5226822 

https://www.google.com/maps/place/Tr%C6%B0%E1%BB%9Dng+Thi%E1%BA%BFu+Sinh+Qu%C3%A2n/@11.0695978,106.5204935,17z/data=!3m1!4b1!4m5!3m4!1s0x310b32a57b808ec5:0xd26b9a0d87f54aaf!8m2!3d11.0695925!4d106.5226822 
 
#! /usr/bin/perl
use strict;

&Find_Function("./DFT_STA_Lecture");

sub Find_Function {
    my $path = $_[0];
	my $var_dir;
	my $sub_entry;
	my @file = ();
	
	opendir($var_dir, $path);
	@file = readdir ($var_dir);
	close ($path);
	for (my $i = 0; $i <= $#file; $i++) {
        if (($file[$i] eq ".") || ($file[$i] eq "..")) {
	        next;
	    }
	    elsif(-d $path."/".$file[$i]) {
	        my $sub_dir = $path."/".$file[$i];
	        &Find_Function($sub_dir);
	    }
	    else {
	        print "$file[$i]\n";
	    }
	    	
	}
	#while ($sub_entry = readdir $path) {
	#    if (($sub_entry eq ".") || ($sub_entry eq "..")) {
	#	    next;
	#	}
	#	elsif(-d $path."/".$sub_entry) {
	#	    my $sub_dir = $path."/".$sub_entry;
	#	    Find_Function($sub_dir);
	#	}
	#	else {
	#	    print "$sub_entry\n";
	#	}
	#}
	#closedir $path;
}
    
  
