#!/usr/bin/perl

my $input_file = "";
my $output_file = "";
my @keyword = ("void", "double", "int", "char", "unsigned");
my $fun_name = "";
my @variable = "";
my @tmp_line = "";
my $tmp_idx_line = 0;
my $flag_keyword = 0;
my $flag_replace = 0;
my $channel_num = 0;
my $reg_index = 0;
my $adc_name = "";

#***************** MAIN PROGRAM **************

&input_argument();

#********************************************

sub input_argument {
  if ($#ARGV < 0) {
      &help;
  }
  elsif ($#ARGV == 1) {
    print("**** No output directory specified ****\n");
    print("**** Create directory dir_convert  ****\n");
    $input_file = $ARGV[0];
    mkdir dir_convert;
  }
  elsif ($#ARGV == 2) {
    $input_file = $ARGV[0];
    $output_file = $ARGV[1];
  }
  else {
    &help;
    die("\n\n\n****************** Error!! Too many arguments ******************\n");
  }
   
}

sub help {
  print("*****               How to use                 *****\n");
  print("*****   $0 <input_file> [output_directory]     *****\n");
  print("***** input_file: File name want to convert    *****\n");
  print("***** output_directory: File converted will    *****\n");
  print("***** putted in this directory. If not specify *****\n");
  print("*****      file putted in dir_convert          *****\n");
}

sub convert_line {
  my $reset_flag = 0;
  my $in_function = 0; 
  my $first_time = 0;

  unless(open($INPUT, $input_file)) {
    die ("Error happen! Can not open file $input_file\n");
  }

  unless(open($OUT, ">$output_file") {
    die ("Error happne! Cannot create file $output_file\n");
  }

  my @line = <INPUT>;
  for ($i=0; $i < $#line; $i++) {

  #*************** COMMENT MULTI LINE *************
    if ($line[$i] =~ /^\Q\/*\E/) {
      $ignore_multi = 1;
    }

    if ($ignore_multi == 1) {
      if ($line[$i] !~ /\Q*\/\E/) {
        next;
      }
      elsif ($line[$i] =~ /\Q*\/\E$/) {
        $ignore_mutil = 0;
        next;
      }
      else {
        my @ignore = split (/\Q*\//, $line[$i]);
        $line[$i] = $ignore[1];
      }
    }
  #*************************************************

  #************** COMMENT SINGLE LINE **************
    if ($line[$i] =~ /^s*\/\//) {   
       next;
    }
  #**************************************************

  #************** CONVERT CHECK_RESET **************
    if ($line[$i] =~ "void Check_Reset") {
      if ($line[$i] =~ /\{/) {
        $reset_flag = 2;
      }
      else {
        $reset_flag = 1;
      }
    }

    if ($reset_flag == 1) {
      if ($line[$i] eq "\n") {
        next;
      }
      elsif ($line[$i] =~ /\{/) {
        $reset_flag = 2;
        $first_time = 1;
      }
    }
   
    if ($reset_flag == 2) {
      if ($first_time == 1) {
          $first_time = 0;
          next;
      }
      if ($line[$i] =~ /\{/) {
         $in_function++;
      }         
      if (($line[$i] =~ /\}/) {
        $in_function--;
      }
   
     if ($in_function == -1) {
        $reset_flag = 0;
        next;
     }
     else {
        &convert_check_reset($line[$i]); 
    }
  }
  
  #**************************************************

    if (($flag_keyword == 1) && ($line[$i] eq "\n")) { #detect keyword and next is empty line
        next;
    }
    elsif (($flag_keyword == 1) && ($line[$i] =~ /^\;/)) { #variable
    }    
    elsif (($line[$i] =~ /\Q#define\E/) && ($input_file =~ "SARAD_U2A_driver.h")) {
        next;
    }
    elsif (($line[$i] =~ /\Q#define\E/) && ($input_file =~ "CONTROL_PORT_driver.h")) {
        if ($line[$i] =~ /ID_*/) {
            my @id_arr = split(/ /, $line[$i]);
            my $global_line = "global $id_arr[1]"
            my $assign_value = "$id_arr[1] = $id_arr[2]"
            $line[$i] = $global_line . "\n" . "$assign_value"
        }
    } 
    elsif ($line[$i] =~ "#include") {
      my $name_include = "";
      my @arr_include = split(/ /, $line[$i]);
      $arr_include[1] =~ s/\"//g;
      $line[$i] = "from" . " $arr_include[1] " . "import *";
    } 
    elsif {
      for ($idx = 0; $idx < $#keyword; $idx++) {
          if ($line[$i] =~ $keyword[$idx]) {
            $flag_keyword = 1;
            
          }
          if (($flag_keyword == 1) && ($line[$i] =~ /\(/)) {
              my @check_function = split(/\(/,$line[$i]);
              if ($check_function[0] =~ $keyword[$idx]) {
                &get_nam_function($line[$i]);
                 $flag_keyword = 0; #already finish split function
              }
            }
            else {
              $tmp_line[$tmp_idx_line] = $line[$i];
              $tmp_idx_line++;
          }
      }
    }
} 


#=========== SUB GET NAME FUNCTION ==========   
#Input: Line read
#Output: def <name_fucntion> <variable>
#============================================ 
sub get_name_function {
  my $name_raw = @_[0];
  my @function_arr = split (/\(/, $name_raw);
  my @var_arr = split(/\)/, $function_arr[1]);
  my @each_arr = split(/,/, $var_arr[0]);

  for ($f_idx = 0; $f_idx <= $#keyword; $f_idx++) {
    $function_arr[0] =~ s/$keyword[$f_idx]//g;
  }
  
  for ($var_idx=0; $var_idx <= $#each_arr; $var_idx++) {
    for ($idx = 0; $idx < $#keyword; $idx++) {
      $each_arr[$var_idx] =~ s/$keyword[$idx]//g;
    }
  }
  $func_name = "def ". $function_arr[0];
  @variable = @each_arr;
}

  
#========== SUB CONVERT CHECK_RESET() ========   
#Input: Line in function void Check_Reset()
#Output: Function Check_Reset() Python
#============================================ 
sub convert_check_reset {
 my $line_reset = @_[1]; 
 my $adc_index = 0;

 if (($line_reset =~ /^*reg_ptr_base_long*&ADC*/) || ($line_reset =~ /^*reg_ptr_base_long*&SARAD/)) {
    my @arr_line_convert = split(/./,$line_reset);
    my $reg_name = $arr_line_convert[1];
    if ($arr_line_convert[0] =~ "ADC0") {
      $adc_name = "ADC";
      $adc_index = 0;
    }
    elsif ($arr_line_convert[0] =~ "ADC2") {
      $adc_name = "ADC";
      $adc_index = 2;
    }
    elsif ($arr_line_convert[0] =~ "SARAD") {
      $adc_name = "SARAD";
      $adc_index = 10;
    }
    $reg_name =~ s/\[0\]//g;
 }

 elsif ($line_reset =~ /reg_index/) {
  $line_reset =~ s/\s+//g;
  $line_reset =~ s/^*=//g;
  my @line_number = split(/;/, $line_reset);
  $channel_num =  $line_number[0];
  $flag_replace = 1;
 } 
    
 elsif (($flag_replace == 1) && ($line_reset =~ /^\s*for*channel/) && ($adc_name !~ "SARAD")) {
  #$line_reset =~ s/\s+//g;
  #if ($line_reset =~ /\Qfor(unsignedintchannel=0;channel<CHANNEL_NUM;channel++)\E/) {
    $line_reset = "for channel in range (0,channel_num):\n";
    $flag_replace = 2;
 }

 elsif (($flag_replace == 2) && ($line_reset =~ /^\s*for*index/) && ($adc_name != "SARAD")) {
  # $line_reset =~ s/\s+//g;
  # if ($line_reset =~ /\Qfor(unsignedintindex=0;index<reg_index;index++)\E/) 
   $line_reset = "\tfor index in range (0,scan_group):\n";
   $flag_replace = 3;
 }

 elsif ((($flag_replace == 3) && ($line_reset =~ /^\s*if\s*\(\s*\*reg_ptr_long/)) || ($adc_index == 10)) {
    $line_reset =~ s/^*!=//g;
    my @init_value = split(/\)/, $line_reset);
    $init_value[0] =~ s/\s+//g;
    if ($adc_index == 0) {
      $line_reset = "reg_acc \( \"RD\", \"ADC\[\" + str\(channel\) + \"\]."."$reg_name"."\[\" + str \(index\) + \"\]\", 1, " . "$init_value[0]";
    }
    elsif ($adc_index == 2) {
      $line_reset = "reg_acc \( \"RD\", \"ADC\[\" + str\(2\) + \"\]."."$reg_name"."\[\" + str \(index\) + \"\]\", 1, " . "$init_value[0]";
   }
   elsif ($adc_name =~ "SARAD") {
      $line_reset = "reg_acc \( \"RD\", \"SARAD\[\" + str\(0\) + \"\]."."$reg_name"."\[\" + str \(index\) + \"\]\", 1, " . "$init_value[0]";
   }

}


}
print OUT $line[$i]; 
    
    
    
  
