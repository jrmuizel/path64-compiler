#!/apa/gnu/SunOS5.7/bin/perl
#=======================================================================
# Check output files of  builtin between SX reference and C models
#=======================================================================
# HISTORY:
#---------
# Creation by T.TORRET (2005/Apr/12)
#--------------------------------------------------------------------------------
# Modif.  by T.TORRET (2005/Apr/15)
#		add CARRY check
#--------------------------------------------------------------------------------

use diagnostics;
use strict;
use DirHandle();

use Cwd; 						 
use File::Basename;

use Text::Diff;
require Text::Diff;

# Hack to avoid to repeat home directory of this script.
my $base_root=dirname($0);


my $DirMain=cwd ;

#-------------------------------------------------------------------------------
# Script identification strings
#-------------------------------------------------------------------------------
my $SName = "check_builtin.pl";
my $SVersion = "0.2";
my $SBuildDate = "Apr '05";
my $ScriptIdentification = "$SName Version $SVersion - $SBuildDate";
my $Platform = "-";



my $Ref_File="" ;
my $Tocheck_File="" ;
my $Result_File="" ;

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
	print "\nUsage: $0 -ref=file1 -tocheck=file2 -out=fileresult\n";
	print "\n   -ref= 	     ( Mandatory )  : reference file with path\n";
	print "\n   -tocheck= 	 ( Mandatory )  : file to check with path\n";
	print "\n   -out= 	    ( Mandatory )   : diff result file name\n";
	print "\n";   
}
my $NbArgMin=3	;
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
		if ($OneOption =~ s/^-ref=// )  
		{
			$Ref_File = $OneOption;
		}  
		elsif ($OneOption =~ s/^-tocheck=// )  
		{
			$Tocheck_File = $OneOption;
		} 
		elsif ($OneOption =~ s/^-out=// )  
		{
			$Result_File = $OneOption;
		} 
		else 
		{
			Usage();
			print "Unknown command line switch: [$OneOption]\n";
			exit(1);
		}
   
	}

if (!-f "$Ref_File") 
	{
		Usage();
		print "Reference file ($Ref_File) doesn't exist\n";
		exit(1);
	}
if (!-f "$Tocheck_File") 
	{
		Usage();
		print "file to check ($Tocheck_File) doesn't exist\n";
		exit(1);
	}

if ( $Result_File eq "" )
	{
		Usage();
		print "You shoud give a result file name\n";
		exit(1);
	}



} # End of AnalyzeCmdLine

#-------------------------------------------------------------------------------
# FilterResult: Format Result File
#-------------------------------------------------------------------------------
# Parameter : Result file    (with access path)
#-------------------------------------------------------------------------------
# Result:  formatted String 
#   None.
#-------------------------------------------------------------------------------
# Prerequisites:
#	$ThisFile exists
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------
sub FilterResult
{
	my $ThisFile  	= shift ;

 	my $FilterStr = ""	 ; 
	my $BuiltinName=""   ;
	my $BuiltinHeader="" ;

	if (open(FILEDATA,"<$ThisFile") )  
	{
		my 	$OneLine ;

			while ($OneLine=<FILEDATA>) 
			{
				if ( $OneLine =~ /------/ ) 
				{
					next ;
				}
				elsif ( $OneLine =~ /CARRY/ ) 
				{
					$FilterStr  .= $OneLine ;
				}
				elsif ( $OneLine =~ /WARNING/ ) 
				{
					next ;
				}
				elsif ( $OneLine =~ /built/ ) 
				{
					$FilterStr  .= $OneLine ;
					chomp($OneLine)  ;
				  	$BuiltinName = $OneLine ;
				}
				elsif ( $OneLine =~ /Parameter/ ) 
				{
					$FilterStr  .= $OneLine ;
				  	$BuiltinHeader .= $OneLine ;
				}
				else
				{
					$FilterStr  .= $OneLine ;
				}
			}
	}
	else
	{
		print "ERROR Cannot OPEN File : $ThisFile\n" ;
		exit (1) ;
	}
	close(FILEDATA);

 	return ($BuiltinName,$BuiltinHeader,$FilterStr) ;
} #end of FilterResult


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

my $BuiltinNameRef		 = "";
my $BuiltinHeaderRef	 = "";
my $BuiltinNameTocheck	 = "";
my $BuiltinHeaderTocheck = "";
my $RefString     = "";
my $ToCheckString = "";

($BuiltinNameRef,    $BuiltinHeaderRef,     $RefString)     = FilterResult($Ref_File);
($BuiltinNameTocheck,$BuiltinHeaderTocheck, $ToCheckString) = FilterResult($Tocheck_File);

my $diff = diff \$RefString, \$ToCheckString ;

open (RESULTFILE,">$Result_File")  || die "ERROR WRITE RESULT $Result_File file couldn't be open ";

print RESULTFILE "--------------------------\n" ;
print RESULTFILE "*** Name $BuiltinNameRef\n" ;
print RESULTFILE "*** Header \n$BuiltinHeaderRef\n" ;

if 	($diff eq "")
{
	print RESULTFILE "*** Status SUCCESS\n" ;
}
else
{
	print RESULTFILE "*** Status FAIL\n" ;
}
print RESULTFILE "--------------------------\n" ;
print RESULTFILE "1st(ref) versus 2nd (model) \n" ;
print RESULTFILE "*** Differences:\n" ;
print RESULTFILE $diff ;
print RESULTFILE "--------------------------\n" ;

close(RESULTFILE);
