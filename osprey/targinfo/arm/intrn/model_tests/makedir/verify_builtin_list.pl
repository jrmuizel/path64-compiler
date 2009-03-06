#!/apa/gnu/SunOS5.7/bin/perl
#=======================================================================
# Check Builtin list in buitin models files versus SX COMPILER <builtins> 
#=======================================================================
# HISTORY:
#---------
# Creation by T.TORRET (2005/Apr/26)
#--------------------------------------------------------------------------------
# v3.0 T.TORRET (2005/May/26)
#    translate to new StxP70cc arch 1_4 toolset
#--------------------------------------------------------------------------------

use diagnostics;
use strict;
use DirHandle();

use Cwd; 
use File::Basename;

# Hack to avoid to repeat home directory of this script.
my $base_root=dirname($0);
require "$base_root/DateUs.pm";

my $DirMain=cwd ;

my $Date = DateUs->now();

#-------------------------------------------------------------------------------
# Script identification strings
#-------------------------------------------------------------------------------
my $SName = "verify_builtin_list.pl";
my $SVersion = "3.0";
my $SBuildDate = "Oct '06";
my $ScriptIdentification = "$SName Version $SVersion - $SBuildDate";
my $Platform = "-";


my $SrcDir="src" ;
my $SrcTemplatesDir="templates" ;
my $SrcVobDir="$base_root/../$SrcDir" ;

my $BuiltinListFile="$SrcVobDir/$SrcTemplatesDir/builtin.lst" ;


my $Diff_File="$SrcVobDir/$SrcTemplatesDir/diff_list.lst" ;

my $System = "Unix";
my $Copy   = "cp";
#-------------------------------------------------------------------------------

my $CCPathFile = "" ;
my $CC_Path = "" ;

my @BuiltinCompFiles=("builtin_sx.h","builtin_x3.h","builtin_ax.h") ;


my @ModelList=() ; # List of  Builtin in model    ($BuiltinListFile)
my @CompList=()  ; # List of  Builtin in Compiler 

#=============================================================================
# How to use this script
#
my $NbArgMin=1	;
my $NbArgMax=$NbArgMin+0	;

sub Usage {
	print "Usage: $0  -CCPathFile=CCPATHFILE \n";
	print "		CCPATHFILE	: File containing Compiler include directory\n";
} # end of Usage




#=============================================================================
# Command line analyzer
#
sub AnalyzeCmdLine 
{
	my $ref_ARGV = shift;

	while (scalar(@$ref_ARGV)) {
		my $OneOption = shift @$ref_ARGV;
		if ($OneOption =~ s/^-CCPathFile=//) 
		{
			$CCPathFile = $OneOption;
		} else 
		{
			print "Unknown command line switch: [$OneOption]\n";
			exit(1);
		}
	}
}  # END of	 AnalyzeCmdLine

#-------------------------------------------------------------------------------
# CheckInModel: returns 1 if builtin is also in Model.
#-------------------------------------------------------------------------------
# Parameter:
#   Entry= builtin name to check.
#-------------------------------------------------------------------------------
# Result:
#   1 if builtin is in model 
#	0 else.
#-------------------------------------------------------------------------------
# Prerequisites:
#   None.
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------
sub CheckInModel 
{
	my $Entry = shift;
	my $Index;

	for ($Index=0; $Index<scalar(@ModelList); $Index++) 
	{
		if ($Entry eq $ModelList[$Index]) 
		{
			return 1;	# check success
		}
	}
	return 0;	# check fail
}


#-------------------------------------------------------------------------------
# Parameter:
#   Entry= builtin name to check.
#-------------------------------------------------------------------------------
# Result:
#   1 if builtin is in Compiler 
#	0 else.
#-------------------------------------------------------------------------------
# Prerequisites:
#   None.
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------
sub CheckInComp 
{
	my $Entry = shift;
	my $Index;

	for ($Index=0; $Index<scalar(@CompList); $Index++) 
	{
		if ($Entry eq $CompList[$Index]) 
		{
			return 1;	# check success
		}
	}
	return 0;	# check fail
}

#-------------------------------------------------------------------------------
# Main program
#-------------------------------------------------------------------------------
# Result:
#   All builtin C files generated in $SrcGenerateDir 
#   directory.
#-------------------------------------------------------------------------------
# Prerequisites:
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------

# Set umask for accessibility of created files by group
umask 007;


print "$ScriptIdentification\n" ;

if ( (scalar(@ARGV) < $NbArgMin) or 	(scalar(@ARGV) > $NbArgMax)  )
{
	Usage();
	exit(1);
}



# Analyze Command Line
AnalyzeCmdLine(\@ARGV);


my $CompilerIncDir =""; 
my $OneLine;
my $BuiltinName  = "";


open(CCPATHFILE,"<$CCPathFile") || die "ERROR $CCPathFile File couldn't be open " ;
my $CompilerInc 	=	"DEFSTDINC"	;
while ($OneLine=<CCPATHFILE>)
{
	if ($OneLine =~ s/$CompilerInc\s+=(\S+)//) 
	{
		$CompilerIncDir= $1 ;
		$CompilerIncDir =~ s/\\/\//g	   ;
	}
}
close(CCPATHFILE);




# -------- Read  MODEL Builtin list File -------------------

my $MyBuiltinListFile="$DirMain/$SrcDir/My_builtin.lst" ;

if (-f $MyBuiltinListFile) 
{
 	$BuiltinListFile =	$MyBuiltinListFile ;
}

open(LISTFILE,"<$BuiltinListFile") || die "ERROR $BuiltinListFile model list File couldn't be open " ;
my $OnWork = 0 ;

while ($OneLine=<LISTFILE>)
{
	chomp($OneLine);
	if ($OneLine =~ /^#/) 
	{
		# Comments in this file begin with a #
		next;
	}
	elsif ($OneLine =~ /^(__builtin\w+)/)
	{
		if ($OnWork == 1 )
		{
			print "ERROR end missing for $BuiltinName in $BuiltinListFile  \n";
			exit (1) ;
		}
		$BuiltinName = $1 ;
		$OnWork = 1	   ;
	}
	elsif ($OnWork == 1 )
	{
	 	if ($OneLine =~ /^end/)
		{
			push	@ModelList ,$BuiltinName ; 
		 	$OnWork = 0 ;
		}
	}
}
close(LISTFILE);


# -------- Read STxP70 COMPILER Builtin Files -------------------

my $CompFile_nb= scalar(@BuiltinCompFiles) ;

for (my $i=0; $i<$CompFile_nb  ; $i++)
{
	my $CompFile = $CompilerIncDir."/".$BuiltinCompFiles[$i] ;

	open(LISTFILE,"<$CompFile") || die "ERROR $CompFile builtin compiler File couldn't be open " ;
	my $OnWork = 0 ;

	while ($OneLine=<LISTFILE>)
	{
		chomp($OneLine);

		if ($OneLine =~ /pragma/)
		{
			next ;
		}
		# elsif ($OneLine =~ /(__builtin_\w+)\s+(__builtin_\w+)/)
		# {
		# 	$BuiltinName = $1 ;
		# 	my $OldName  = $2 ;
		# 	if ($OldName =~ /__builtin_/)
		# 	{
		# 	  pop	@CompList  ; 
		# 	}
		# 
		# 	my $Result=CheckInComp($BuiltinName) ;
		# 	if 	($Result == 0)
		# 	{ # We add only when builtin is not already in list
		# 	  # because multiply definition in compiler file
		# 		push	@CompList ,$BuiltinName ; 
		# 	}
		# }
		elsif ($OneLine =~ /extern.*(__builtin_\w+)/)
		{
			$BuiltinName = $1 ;
			my $Result=CheckInComp($BuiltinName) ;
			if 	($Result == 0)
			{ # We add only when builtin is not already in list
			  # because multiply definition in compiler file
				push	@CompList ,$BuiltinName ; 
			}
		}
	}
	close(LISTFILE);

} # end of compiler builtin files investigation



# -------- CHECK Builtin List -------------------

@CompList = sort  {lc($a) cmp lc($b) } @CompList	;
@ModelList= sort  {lc($a) cmp lc($b) } @ModelList	;



 print 	" ------------------------\n " ;
 print 	" --------COMP----------\n" ;
 print 	" ------------------------\n" ;
 print 	"@CompList" ;
 print 	"\n------------------------\n" ;
 print 	" --------MODEL------------\n" ;
 print 	" ------------------------\n" ;
 print 	"@ModelList" ;
 print 	"\n------------------------\n" ;


my $Comp_nb = scalar(@CompList)  ;
my $Model_nb= scalar(@ModelList) ;

if ($Model_nb != $Comp_nb)
{
 	print 	"\n---------------------------------------\n" ;
	print "WARNING              The number of builtins in Compiler ($Comp_nb) \n" ;
	print "is not the same that the number of builtins in model    ($Model_nb) \n" ;
	print "											  delta is  (".($Comp_nb-$Model_nb).") \n" ;
 	print 	"\n---------------------------------------\n" ;

}
print " Missing in MODEL: \n" ;
for (my $i=0; $i<$Comp_nb  ; $i++)
{
 	$BuiltinName = $CompList[$i] ;
	my $Result=CheckInModel($BuiltinName) ;
	if 	($Result == 0)
	{
		print "\t$BuiltinName\n" ;
	}
}
print 	"\n---------------------------------------\n" ;
print " Missing in COMPILER: \n" ;

for (my $i=0; $i<$Model_nb  ; $i++)
{
 	$BuiltinName = $ModelList[$i] ;
	my $Result=CheckInComp($BuiltinName) ;
	if 	($Result == 0)
	{
		print "\t$BuiltinName\n" ;
	}
}
print 	"\n---------------------------------------\n" ;
