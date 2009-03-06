#!/apa/gnu/SunOS5.7/bin/perl
#=======================================================================
# Generate  Data input files for builtin test
#=======================================================================
# HISTORY:
#---------
# Creation by T.TORRET (2005/Apr/08)
#-----------------------------------
# Modif.   by T.TORRET (2005/Apr/18)
#  		add 8 bits file generation
#-----------------------------------
# Modif.   by T.TORRET (2005/Apr/22)
#  		add 64 bits file generation
#  		add 0x08000	end
#--------------------------------------------------------------------------------

use diagnostics;
use strict;
use DirHandle();

use Cwd; 
use File::Basename;

# Hack to avoid to repeat home directory of this script.
my $base_root=dirname($0);

my $DirMain=cwd ;

#-------------------------------------------------------------------------------
# Script identification strings
#-------------------------------------------------------------------------------
my $SName = "generate_builtin_datavector.pl";
my $SVersion = "1.2";
my $SBuildDate = "Apr '05";
my $ScriptIdentification = "$SName Version $SVersion - $SBuildDate";
my $Platform = "-";

my $System = "Unix";
my $Copy   = "cp";
my $CopyRecursive   = "cp -r";
my $Makedir  = "mkdir";
my $Delete   = "rm -f";
my $RecursiveChMod_ToRead = "chmod -R a+w";
my $Command  = "xxx";

# ----------------------------------------
my $DataNb=0;			;
my $DataDir="$DirMain/data_in" ;
my $DataFileExt=".in"		  ;

my @data8_basic_1 = ( "0x7F", "0x7F", "0x7F", "0x7F", "0x7F", "0x7F", "0x7F", "0x80", "0x80", "0x80", "0x80", "0x80", "0x80", "0xFF", "0xFF", "0xFF", "0xFF", "0xFF", "0x00", "0x00", "0x00", "0x00" , "0x01"   , "0x80"   , "0x81"   , "0x7F"  , "0x00"   , "0xFE"   , "0xFD") ;   
my @data8_basic_2 = ( "0x00", "0xFF", "0x80", "0x01", "0x81", "0x7E", "0x7F", "0x00", "0x7F", "0x01", "0x80", "0x81", "0xFF", "0x80", "0xFE", "0x7F", "0x01", "0x00", "0xFF", "0x01", "0x7F", "0x80" , "0x80"   , "0x80"   , "0xFE"   , "0x80"  , "0x81"   , "0x80"   , "0x82") ;   
my @data8_basic_3 = ( "0x7F", "0x80", "0x00", "0x7F", "0x7F", "0xFF", "0x7F", "0x7F", "0x80", "0x7F", "0x80", "0x00", "0x80", "0x80", "0x80", "0xFF", "0xFF", "0xFF", "0x7F", "0xFF", "0x00", "0x7F" , "0x7F"   , "0xFE"   , "0x7F"   , "0x7F"  , "0x00"   , "0x7E"   , "0x80") ;   
my @data8_basic_4 = ( "0xFF", "0xFF", "0xFF", "0x80", "0x00", "0x7F", "0x7F", "0xFF", "0x7F", "0x7F", "0x80", "0x7F", "0x80", "0x00", "0x80", "0x7F", "0x80", "0x7F", "0xFF", "0x00", "0x00", "0xFF" , "0x0E"   , "0x0F"   , "0x01"   , "0x7F"  , "0xFE"   , "0x00"   , "0xFE") ;   

my @data16_basic_1 = ( "0x7FFF", "0x7FFF", "0x7FFF", "0x7FFF", "0x7FFF", "0x7FFF", "0x7FFF", "0x8000", "0x8000", "0x8000", "0x8000", "0x8000", "0x8000", "0xFFFF", "0xFFFF", "0xFFFF", "0xFFFF", "0xFFFF", "0x0000", "0x0000", "0x0000", "0x0000" , "0x8000"  , "0x8000"  , "0x7FFF"  , "0x8000", "0xFF80"  , "0xFF80"  , "0x8080") ; 
my @data16_basic_2 = ( "0x0000", "0xFFFF", "0x8000", "0x0001", "0x8001", "0x7FFE", "0x7FFF", "0x0000", "0x7FFF", "0x0001", "0x8000", "0x8001", "0xFFFF", "0x8000", "0xFFFE", "0x7FFF", "0x0001", "0x0000", "0xFFFF", "0x0001", "0x7FFF", "0x8000" , "0x8000"  , "0x8001"  , "0x8001"  , "0x0001", "0x8080"  , "0x8081"  , "0x8F7F") ; 
my @data16_basic_3 = ( "0x7FFF", "0x8000", "0x0000", "0x7FFF", "0x7FFF", "0xFFFF", "0x7FFF", "0x7FFF", "0x8000", "0x7FFF", "0x8000", "0x0000", "0x8000", "0x8000", "0x8000", "0xFFFF", "0xFFFF", "0xFFFF", "0x7FFF", "0xFFFF", "0x0000", "0x7FFF" , "0x7FFF"  , "0xFFFF"  , "0x0001"  , "0xFFFF", "0x8000"  , "0x8001"  , "0x7FFF") ;  
my @data16_basic_4 = ( "0xFFFF", "0xFFFF", "0xFFFF", "0x8000", "0x0000", "0x7FFF", "0x7FFF", "0xFFFF", "0x7FFF", "0x7FFF", "0x8000", "0x7FFF", "0x8000", "0x0000", "0x8000", "0x7FFF", "0x8000", "0x7FFF", "0xFFFF", "0x0000", "0x0000", "0xFFFF" , "0xFFFF"  , "0x0001"  , "0xFFFF"  , "0x8000", "0x7FFF"  , "0x0001"  , "0xFFFF") ; 

my @data32_basic_1 = ( "0x7FFFFFFF", "0x7FFFFFFF", "0x7FFFFFFF", "0x7FFFFFFF", "0x7FFFFFFF", "0x7FFFFFFF", "0x7FFFFFFF", "0x80000000", "0x80000000", "0x80000000", "0x80000000", "0x80000000", "0x80000000", "0xFFFFFFFF", "0xFFFFFFFF", "0xFFFFFFFF", "0xFFFFFFFF", "0xFFFFFFFF", "0x00000000", "0x00000000", "0x00000000", "0x00000000" , "0x00008000"  , "0x00018000"  , "0xFFFF8000"  , "0x7FFF8000"  , "0x80018000"  , "0x80008000"  , "0x80028000") ;  
my @data32_basic_2 = ( "0x00000000", "0xFFFFFFFF", "0x80000000", "0x00000001", "0x80000001", "0x7FFFFFFE", "0x7FFFFFFF", "0x00000000", "0x7FFFFFFF", "0x00000001", "0x80000000", "0x80000001", "0xFFFFFFFF", "0x80000000", "0xFFFFFFFE", "0x7FFFFFFF", "0x00000001", "0x00000000", "0xFFFFFFFF", "0x00000001", "0x7FFFFFFF", "0x80000000" , "0x00010000"  , "0x00008000"  , "0xFFFE8000"  , "0x00010000"  , "0x7FFF8000"  , "0x00008000"  , "0x00018000") ;  
my @data32_basic_3 = ( "0x7FFFFFFF", "0x80000000", "0x00000000", "0x7FFFFFFF", "0x7FFFFFFF", "0xFFFFFFFF", "0x7FFFFFFF", "0x7FFFFFFF", "0x80000000", "0x7FFFFFFF", "0x80000000", "0x00000000", "0x80000000", "0x80000000", "0x80000000", "0xFFFFFFFF", "0xFFFFFFFF", "0xFFFFFFFF", "0x7FFFFFFF", "0xFFFFFFFF", "0x00000000", "0x7FFFFFFF" , "0xFFFFFFFF"  , "0xFFFFFFFF"  , "0x0000FFFF"  , "0x10000000"  , "0x8000FFFF"  , "0xFFFF0000"  , "0xFFFF0000") ;  
my @data32_basic_4 = ( "0xFFFFFFFF", "0xFFFFFFFF", "0xFFFFFFFF", "0x80000000", "0x00000000", "0x7FFFFFFF", "0x7FFFFFFF", "0xFFFFFFFF", "0x7FFFFFFF", "0x7FFFFFFF", "0x80000000", "0x7FFFFFFF", "0x80000000", "0x00000000", "0x80000000", "0x7FFFFFFF", "0x80000000", "0x7FFFFFFF", "0xFFFFFFFF", "0x00000000", "0x00000000", "0xFFFFFFFF" , "0x7FFFFFFF"  , "0x7FFFFFFF"  , "0x00008000"  , "0x10000000"  , "0x1FFFFFFF"  , "0x80008000"  , "0x80008000") ;  

my @data64_basic_1 = ( 	"0x7FFFFFFF00000000LL", 
						"0x7FFFFFFFFFFFFFFFLL", 
						"0x7FFFFFFF7FFFFFFFLL", 
						"0x7FFFFFFFFFFFFFFELL", 
						"0x7FFFFFFFFFFFFFFFLL", 
						"0x7FFFFFFF00000001LL", 
						"0x7FFFFFFF80000000LL", 
						"0x8000000000000000LL", 
						"0x800000007FFFFFFFLL", 
						"0x80000000FFFFFFFFLL", 
						"0x8000000000000001LL", 
						"0x8000000000000002LL", 
						"0x8000000080000000LL", 
						"0xFFFFFFFF00000000LL", 
						"0xFFFFFFFFFFFFFFFFLL", 
						"0xFFFFFFFFFFFFFFFELL", 
						"0xFFFFFFFF80000000LL", 
						"0xFFFFFFFF7FFFFFFFLL", 
						"0x00000000FFFFFFFFLL", 
						"0x000000007FFFFFFFLL", 
						"0x0000000000000000LL", 
						"0x0000000180000000LL",
						"0x0000000000000000LL", 
						"0x0000000000008000LL", 
						"0x0000000000018000LL", 
						"0x0000000080008000LL", 
						"0x7FFFFFFF00008000LL", 
						"0xFFFFFFFFFFFF8000LL", 
						"0x0000000080000000LL" ) ;

my @data64_basic_2 = ( 	
						"0x0000000180000000LL",
						"0x0000000000000000LL", 
						"0x0000000000008000LL", 
						"0x0000000000018000LL", 
						"0x0000000080008000LL", 
						"0x7FFFFFFF00008000LL", 
						"0xFFFFFFFFFFFF8000LL", 
						"0xFFFFFFFFFFFFFFFFLL", 
						"0x0000000000000001LL",
						"0x7FFFFFFFFFFFFFFFLL", 
						"0x0000000000000000LL", 
						"0x7FFFFFFF80000000LL", 
						"0x8000000000000000LL", 
						"0x800000007FFFFFFFLL", 
						"0x00000000FFFFFFFFLL", 
						"0x80000000FFFFFFFFLL", 
						"0x000000007FFFFFFFLL", 
						"0x7FFFFFFFFFFFFFFELL", 
						"0x8000000080000000LL", 
						"0x7FFFFFFF00000001LL", 
						"0xFFFFFFFF00000000LL", 
						"0x7FFFFFFF00000000LL", 
						"0xFFFFFFFF80000000LL", 
						"0xFFFFFFFF7FFFFFFFLL", 
						"0x7FFFFFFF7FFFFFFFLL", 
						"0xFFFFFFFFFFFFFFFELL", 
						"0x8000000000000002LL", 
						"0x7FFFFFFFFFFFFFFFLL",
						"0x8000000000000001LL" 
						) ; 

#-------------------------------------------------------------------------------
# Usage: describes PERL script parameters.
#-------------------------------------------------------------------------------
# Parameter:
#   None.
#-------------------------------------------------------------------------------
# Result:
#   None.
#-------------------------------------------------------------------------------
# Prerequisites:
#   None.
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------
sub Usage {
	print "\nUsage: $0 -DataNb=number  \n";
	print "\n   -DataNb 	 ( Mandatory )  : Number of Data to generate for each files \n";
	print "\n";   
}
my $NbArgMin=1	;
my $NbArgMax=$NbArgMin+0	;

#-------------------------------------------------------------------------------
# AnalyzeCmdLine: analyzes command line to retrieve flags.
#-------------------------------------------------------------------------------
# Parameter:
#   reg_ARGV: reference to ARGV global parameter.
#-------------------------------------------------------------------------------
# Result:
#-------------------------------------------------------------------------------
# Prerequisites:
#   None.
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------
sub AnalyzeCmdLine {
	my $ref_ARGV = shift;
	my @OptionList=();
	
	#read all command line option and put it in a list
	while (scalar(@$ref_ARGV)) 
	{
		 my $OneLineOption = shift @$ref_ARGV;
		 push @OptionList,$OneLineOption ;
	} # end while argv

	print "Option list: @OptionList	\n";
	print "Nb Option=".scalar(@OptionList)."\n";


	# process all command  option from list
	while (scalar(@OptionList)) 
	{ 
		my $OneOption = shift @OptionList;
		if ($OneOption =~ /^-DataNb=(\d+)/ )  
		{
			$DataNb = $1;
		} else 
		{
			Usage();
			print "Unknown command line switch: [$OneOption]\n";
			exit(1);
		}
   
	}

	if ($DataNb ==0 )
	{
		Usage();
		print "DataNb  must be different from 0: [$DataNb]\n";
		exit(1);
	}

} # End of AnalyzeCmdLine

#-------------------------------------------------------------------------------
# Generate_8bits_Data
#-------------------------------------------------------------------------------
# Parameter:
#-------------------------------------------------------------------------------
# Result:
#		- Number Data in string at format hexa 8bits separate by ',\n'
#-------------------------------------------------------------------------------
# Prerequisites:
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------

sub Generate_8bits_Data
{
 my $basic_data_choice = shift ;

 my $NumberLines = "" ;
 my $BasicLines  = "" ;

 # ---------------- fixed part--------------------------------------------

 my $basic_nb= scalar(@data8_basic_1) ;

	 if ($basic_data_choice == 2 )
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data8_basic_2[$i]." ,\n"	;
		}
	 }
	 elsif ($basic_data_choice == 3 )
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data8_basic_3[$i]." ,\n"	;
		}
	 }
	 elsif ($basic_data_choice == 4 )
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data8_basic_4[$i]." ,\n"	;
		}
	 }
	 else
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data8_basic_1[$i]." ,\n"	;
		}
	 }

	$NumberLines = $BasicLines  ;

 # ---------------- aleatory part--------------------------------------------

	for (my $i=0; $i< $DataNb ; $i++)
	{
		my $Number = rand(0xFF) + 1	  ;
		my $NumberHexStr = sprintf("%#.2x",$Number)	;
		$NumberLines.= $NumberHexStr ;
		if ( $i != ($DataNb-1) ) 
		{
			$NumberLines.=" ,"  ;
		}
		$NumberLines.="\n"	   ;
	} 
	return $NumberLines ;
} # End Generate_8bits_Data



#-------------------------------------------------------------------------------
# Generate_16bits_Data
#-------------------------------------------------------------------------------
# Parameter:
#-------------------------------------------------------------------------------
# Result:
#		- Number Data in string at format hexa 16bits separate by ',\n'
#-------------------------------------------------------------------------------
# Prerequisites:
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------

sub Generate_16bits_Data
{
 my $basic_data_choice = shift ;

 my $NumberLines = "" ;
 my $BasicLines  = "" ;

 # ---------------- fixed part--------------------------------------------

 my $basic_nb= scalar(@data16_basic_1) ;

	 if ($basic_data_choice == 2 )
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data16_basic_2[$i]." ,\n"	;
		}
	 }
	 elsif ($basic_data_choice == 3 )
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data16_basic_3[$i]." ,\n"	;
		}
	 }
	 elsif ($basic_data_choice == 4 )
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data16_basic_4[$i]." ,\n"	;
		}
	 }
	 else
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data16_basic_1[$i]." ,\n"	;
		}
	 }

	$NumberLines = $BasicLines  ;

 # ---------------- aleatory part--------------------------------------------

	for (my $i=0; $i< $DataNb ; $i++)
	{
		my $Number = rand(0xFFFF) + 1	  ;
		my $NumberHexStr = sprintf("%#.4x",$Number)	;
		$NumberLines.= $NumberHexStr ;
		if ( $i != ($DataNb-1) ) 
		{
			$NumberLines.=" ,"  ;
		}
		$NumberLines.="\n"	   ;
	} 
	return $NumberLines ;
} # End Generate_16bits_Data


#-------------------------------------------------------------------------------
# Generate_32bits_Data
#-------------------------------------------------------------------------------
# Parameter:
#-------------------------------------------------------------------------------
# Result:
#		- Number Data in string at format hexa 32bits separate by ',\n'
#------------------------------------------------------------------------------
# Prerequisites:
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------

sub Generate_32bits_Data
{
 my $basic_data_choice = shift ;

 my $NumberLines = "" ;
 my $BasicLines  = "" ;

 # ---------------- fixed part--------------------------------------------

 my $basic_nb= scalar(@data32_basic_1) ;

	 if ($basic_data_choice == 2 )
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data32_basic_2[$i]." ,\n"	;
		}
	 }
	 elsif ($basic_data_choice == 3 )
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data32_basic_3[$i]." ,\n"	;
		}
	 }
	 elsif ($basic_data_choice == 4 )
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data32_basic_4[$i]." ,\n"	;
		}
	 }
	 else
	 {
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data32_basic_1[$i]." ,\n"	;
		}
	 }

	$NumberLines = $BasicLines  ;

 # ---------------- aleatory part--------------------------------------------

	for (my $i=0; $i< $DataNb ; $i++)
	{
		my $Number = rand(0xFFFFFFFF) + 1	  ;
		my $NumberHexStr = sprintf("%#.8x",$Number)	;
		$NumberLines.= $NumberHexStr ;
		if ( $i != ($DataNb-1) ) 
		{
			$NumberLines.=" ,"  ;
		}
		$NumberLines.="\n"	   ;
	} 
	return $NumberLines ;
} # End Generate_32bits_Data


#-------------------------------------------------------------------------------
# Generate_64bits_Data
#-------------------------------------------------------------------------------
# Parameter:
#-------------------------------------------------------------------------------
# Result:
#		- Number Data in string at format hexa 32bits separate by ',\n'
#------------------------------------------------------------------------------
# Prerequisites:
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------

sub Generate_64bits_Data
{
 my $basic_data_choice = shift ;

 my $NumberLines = "" ;
 my $BasicLines  = "" ;

 # ---------------- fixed part--------------------------------------------

 my $basic_nb= scalar(@data32_basic_1) ;

	if ($basic_data_choice == 2 )
	{
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data64_basic_2[$i]." ,\n"	;
		}
	}
	else
	{
		for (my $i=0; $i<$basic_nb  ; $i++)
		{
		 	$BasicLines .= $data64_basic_1[$i]." ,\n"	;
		}
	}

	$NumberLines = $BasicLines  ;

 # ---------------- aleatory part--------------------------------------------

	for (my $i=0; $i< $DataNb ; $i++)
	{
		my $NumberH = rand(0xFFFFFFFF) + 1 	  ;
		my $NumberL = rand(0xFFFFFFFF) + 1 	  ;
		my $NumberHexStr = sprintf("%#.8x_%#.8xLL",$NumberH,$NumberL)	;
		$NumberHexStr =~s/_0x// ;
		$NumberLines.= $NumberHexStr ;
		if ( $i != ($DataNb-1) ) 
		{
			$NumberLines.=" ,"  ;
		}
		$NumberLines.="\n"	   ;
	} 
	return $NumberLines ;
} # End Generate_64bits_Data








#-------------------------------------------------------------------------------
# Main program
#-------------------------------------------------------------------------------
# Parameter:
#-------------------------------------------------------------------------------
# Result:
#-------------------------------------------------------------------------------
# Prerequisites:
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------

# Set umask for accessibility of created files by group
umask 0007;

# Ensure correct parameter number.
if ( (scalar(@ARGV) < $NbArgMin) or 	(scalar(@ARGV) > $NbArgMax)  )
{
	Usage();
	exit(1);
}

# Other command line parameters?
if (scalar(@ARGV) >= 1) 
{
	AnalyzeCmdLine(\@ARGV);
}

if (!-d "$DataDir") 
{
	$Command = "$Makedir $DataDir";
	if ($System eq "Win32") 
	{
		$Command =~ s/\//\\/g;
	}
	if (system("$Command") != 0) 
	{
		print "$Command\n";
		print "failed\n";
		exit(1);
	}
}

if ( 	( scalar(@data8_basic_1) != scalar(@data8_basic_2) )	 
	or  ( scalar(@data8_basic_1) != scalar(@data8_basic_3) )
	or  ( scalar(@data8_basic_1) != scalar(@data8_basic_4) )	)
	{
		print "problem of data8_basic : tab shall have the same size\n";
		exit(1);
	}

if ( 	( scalar(@data16_basic_1) != scalar(@data16_basic_2) )	 
	or  ( scalar(@data16_basic_1) != scalar(@data16_basic_3) )
	or  ( scalar(@data16_basic_1) != scalar(@data16_basic_4) )	)
	{
		print "problem of data16_basic : tab shall have the same size\n";
		exit(1);
	}

if ( 	( scalar(@data32_basic_1) != scalar(@data32_basic_2) )	 
	or  ( scalar(@data32_basic_1) != scalar(@data32_basic_3) )
	or  ( scalar(@data32_basic_1) != scalar(@data32_basic_4) )  )
	{
		print "problem of data32_basic : tab shall have the same size\n";
		exit(1);
	}

if ( 	( scalar(@data32_basic_1) != scalar(@data16_basic_1)  )	 
	or  ( scalar(@data16_basic_1) != scalar( @data8_basic_1 ) )	  
	or  ( scalar(@data16_basic_1) != scalar(@data64_basic_2 ) )	  
	or  ( scalar(@data16_basic_1) != scalar(@data64_basic_1)  )	  )
	{
		print "problem of dataX_basic : tab 8/16/32/64 shall have the same size\n";
		exit(1);
	}

	my $DataFile="";
	#**********************************************************************
	my	$Lines="";
	$DataFile=$DataDir."/builtin_8b_1".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_8bits_Data(1) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------
	$Lines="";
	$DataFile=$DataDir."/builtin_8b_2".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_8bits_Data(2) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------
	$Lines="";
	$DataFile=$DataDir."/builtin_8b_3".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_8bits_Data(3) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------
	$Lines="";
	$DataFile=$DataDir."/builtin_8b_4".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_8bits_Data(4) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#**********************************************************************

	$Lines="";
	$DataFile=$DataDir."/builtin_16b_1".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_16bits_Data(1) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------
	$Lines="";
	$DataFile=$DataDir."/builtin_16b_2".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_16bits_Data(2) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------
	$Lines="";
	$DataFile=$DataDir."/builtin_16b_3".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_16bits_Data(3) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------
	$Lines="";
	$DataFile=$DataDir."/builtin_16b_4".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_16bits_Data(4) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#**********************************************************************

	$Lines="";
	$DataFile=$DataDir."/builtin_32b_1".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_32bits_Data(1) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------

	$Lines="";
	$DataFile=$DataDir."/builtin_32b_2".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_32bits_Data(2) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------

	$Lines="";
	$DataFile=$DataDir."/builtin_32b_3".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_32bits_Data(3) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------

	$Lines="";
	$DataFile=$DataDir."/builtin_32b_4".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_32bits_Data(4) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------
	#**********************************************************************

	$Lines="";
	$DataFile=$DataDir."/builtin_64b_1".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_64bits_Data(1) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------
	$Lines="";
	$DataFile=$DataDir."/builtin_64b_2".$DataFileExt ;
	if( -f "$DataFile")
	{
		print "$DataFile already exists \n" ;
		exit(1);
	}
	open (DATAFILE,">$DataFile")  || die "ERROR WRITE $DataFile option file couldn't be open ";
	$Lines= Generate_64bits_Data(2) ;
	print DATAFILE $Lines ;
	close(DATAFILE);
	print "$DataFile generated \n" ;
	#---------------------------
	
	
	
	
	
	print "END \n" ;
