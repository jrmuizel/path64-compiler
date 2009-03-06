#!/apa/gnu/SunOS5.7/bin/perl
#=======================================================================
# Generate  Src files for builtin test
#=======================================================================
# HISTORY:
#---------
# Creation by T.TORRET (2005/Apr/08)
#--------------------------------------------------------------------------------
# Modif T.TORRET (2005/Apr/18)
#	add immediate value in parameter
#	add mask value in parameter
#--------------------------------------------------------------------------------
# Modif T.TORRET (2005/Apr/19)
#	add immediate value at parameter 1 => pb on datanb
#--------------------------------------------------------------------------------
# Modif T.TORRET (2005/Apr/20)
#	The C librarie doesn't support printf 64 bits
#	=> all printf shall be re-written
#--------------------------------------------------------------------------------
# Modif T.TORRET (2005/May/13)
#	Fix pb when no input parameter (void)
#--------------------------------------------------------------------------------
# Modif T.TORRET (2006/Oct/17)
#	translate for STXP70CC
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
my $SName = "generate_builtin_src_test.pl";
my $SVersion = "2.0";
my $SBuildDate = "Oct '06";
my $ScriptIdentification = "$SName Version $SVersion - $SBuildDate";
my $Platform = "-";

my $SrcDir="src" ;
my $SrcTemplatesDir="templates" ;
my $SrcVobDir="$base_root/../$SrcDir" ;

my $SrcGenerateDir  = "$DirMain/$SrcDir"  ;
my $SrcTemplate     = "$SrcVobDir/$SrcTemplatesDir/builtin_test_main_template.c"	;
my $SrcBuiltinTemplate  = "$SrcVobDir/$SrcTemplatesDir/basic_builtin.c"	;
my $SrcBuiltinFile  = "$DirMain/$SrcDir/main###BUILTIN_NAME###.c" ;

my $DataDir="$DirMain/data_in" ;
my $DataFileExt=".in"		   ;

my $BuiltinListFile="$SrcVobDir/$SrcTemplatesDir/builtin.lst" ;


my $NB_MAX_IN	= 4 ;	# number max of input  parameter for builtin
my $NB_MAX_OUT	= 1 ;	# number max of output parameter for builtin

my $System = "Unix";
my $Copy   = "cp";
#-------------------------------------------------------------------------------

my $VobRoot="/vob";



#-------------------------------------------------------------------------------
# Global patterns used in C templates
#-------------------------------------------------------------------------------

my $BUILTIN_NAME			= "###BUILTIN_NAME###"			;
my $GENERATION_DATE 		= "###GENERATION_DATE###"		;
my $INPUT_DATA_TAB_SIZE    	= "###INPUT_DATA_TAB_SIZE###" 	;
my $INPUT_PARAM_NB 			= "###INPUT_PARAM_NB###"		;
my $OUTPUT_PARAM_NB 		= "###OUTPUT_PARAM_NB###"		;
 
my $INPUT_DATA  	= 	"###INPUT_DATA_"   	;	
my $OUTPUT_DATA 	= 	"###OUTPUT_DATA_"   ;	
my $DATATYPE 		= 	"_TYPE###"			;
my $DATALIST 		= 	"_LIST###"  		;
my $DATAPRINTF		=	"_PRINTF###"		;
my $IS_NOT_IMMEDIATE=	"_IS_NOT_IMMEDIATE###" 	; 
my $IS_IMMEDIATE	=	"_IS_IMMEDIATE###" 		; 
my $TAB_BUILTIN     =   "###TAB_BUILTIN###"  	;

 
my $VOID = "void"	;
 
my $INT_HEX_PRINTF_FORMAT		= "0x"		;
my $LONG_HEX_PRINTF_FORMAT		= "0lx"		;
my $LONGLONG_HEX_PRINTF_FORMAT	= "0llx"		;  # C librarie currently doesn't support printf 64 bits


my @InParamList  	= () ;
my @OutParamList 	= () ;
my @InDataFile	 	= () ;
my @ImmedParamNb  	= () ;	# list of parameter that are immediate
my @InParamMaskList	= () ;  # list of mask (one for each input parameter)

#=============================================================================
# How to use this script
#
sub Usage {
	print "Usage: $0  \n";
} # end of Usage

#=============================================================================
# Command line analyzer
#
sub AnalyzeCmdLine 
{


}  # END of	 AnalyzeCmdLine

#=============================================================================
# IsAnImmediate 
#-------------------------------------------------------------------------------
# Parameter :
#  parameter number to check
#-------------------------------------------------------------------------------
# Result:
#   Is an Immediate parameter (1) or not (0).
#-------------------------------------------------------------------------------
# Prerequisites:
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------
sub IsAnImmediate 
{
	my $Entry = shift;

	my $IsImmediate = 0 ;
	my $Index = scalar(@ImmedParamNb) ;

	while ( ($IsImmediate != 1  ) and ( $Index != 0 )    )
	{
	 	if ( $Entry == $ImmedParamNb[$Index-1])
	 	{
			$IsImmediate = 1 ;
	 	}
		$Index -- ;
	}

   return $IsImmediate	;

}  # END of	 IsAnImmediate


#-------------------------------------------------------------------------------
# Translate_Type_To_Printf_Format: Translate variable type to Printf Format .
#-------------------------------------------------------------------------------
# Parameter :
#-------------------------------------------------------------------------------
# Result:
#   None.
#-------------------------------------------------------------------------------
# Prerequisites:
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------
sub Translate_Type_To_Printf_Format
{
	 my $C_Type = shift  ;

	 my $Format = $INT_HEX_PRINTF_FORMAT ;

	 if   ( $C_Type =~ /void/ )
	 {
	 	$Format = "empty" ;
	 } 
	 elsif   ( $C_Type =~ /long.*long/ )
	 {
		  $Format = $LONGLONG_HEX_PRINTF_FORMAT ;	
	 } 
	 elsif ( $C_Type =~ /long/ )
	 {
		  $Format = $LONG_HEX_PRINTF_FORMAT ;	
	 }

	 return $Format	  ;

} # end  Translate_Type_To_Printf_Format




#-------------------------------------------------------------------------------
# GetDataStr: Read One Data File
#-------------------------------------------------------------------------------
# Parameter : Data file    (without access path)
#-------------------------------------------------------------------------------
# Result: Data Str
#   	  Number of Data In file
#-------------------------------------------------------------------------------
# Prerequisites:
#	$DataDir  exists
#	$DataFile exists
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------
sub GetDataStr
{
	my $DataFile  	= shift ;

	my $DataFileLoc  	= "$DataDir/$DataFile$DataFileExt" ;

	my $NbData = 0 ;
 	my $DataStr = ""	; 

	if (open(FILEDATA,"<$DataFileLoc") )  
	{
		my 	$OneLine ;

			while ($OneLine=<FILEDATA>) 
			{
				$DataStr .=  $OneLine ;
				$NbData ++ ;
			}
	}
	else
	{
		print "ERROR Cannot OPEN Data File : $DataFileLoc\n" ;
		exit (1) ;
	}
	close(FILEDATA);

 	return ($NbData,$DataStr) ;
} #end of GetDataStr


#-------------------------------------------------------------------------------
# GetOneData: Read One Data in One Data File
#-------------------------------------------------------------------------------
# Parameter : Data file    (without access path)
#-------------------------------------------------------------------------------
# Result: One Data Str	 (string is null when file is all done)
#   	  Number of Data In file
#-------------------------------------------------------------------------------
# Prerequisites:
#	$DataDir  exists
#	$DataFile exists
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------
sub GetOneData
{
	my $DataFile  	= shift ;
	my $NbToFind 	= shift ;

	my $DataFileLoc  	= "$DataDir/$DataFile$DataFileExt" ;

	my $NbData = 0 ;
 	my $DataStr = ""	; 

	if (open(FILEDATA,"<$DataFileLoc") )  
	{
		my 	$OneLine ;

			while ($OneLine=<FILEDATA>) 
			{
				chomp  $OneLine ;
				if ($NbToFind == $NbData )
				{
					$OneLine =~ s/\,// ;
					$DataStr =  $OneLine ;
				}
				$NbData ++ ;
			}
	}
	else
	{
		print "ERROR Cannot OPEN Data File : $DataFileLoc\n" ;
		exit (1) ;
	}
	close(FILEDATA);

 	return ($NbData,$DataStr) ;
} #end of GetOneData




#-------------------------------------------------------------------------------
# GenerateSrc: generates builtin Src file .
#-------------------------------------------------------------------------------
# Parameter :
#	  BuiltinName  	
#	  BuiltImmediate : is there any builtin parameter immediate
#-------------------------------------------------------------------------------
# Result:
#   None.
#-------------------------------------------------------------------------------
# Prerequisites:
#   $SrcTemplatesDir shall exist.
#   $SrcGenerateDir  shall exist.
#   $SrcTemplate     shall exist.
#   $SrcBuiltinTemplate     shall exist.
#-------------------------------------------------------------------------------
# Comments:
#-------------------------------------------------------------------------------
sub GenerateSrc 
{
	my $BuiltinName  	= shift ;
	my $BuiltImmediate  = shift ;
	
	my @DataSize  		= () ;

	my $OneSrcBuiltinFile = $SrcBuiltinFile ;
	$OneSrcBuiltinFile =~  s/$BUILTIN_NAME/$BuiltinName/  ;

	my $GenDate = $Date->formatLong();

	my $NbInParam  = scalar(@InParamList )  ;
	my $NbOutParam = scalar(@OutParamList ) ;
	my $NbInDataFile  = scalar(@InDataFile )  ;
	my $StrBuiltin    = "" ;

	if ( $NbInParam > $NB_MAX_IN  )
	{
		print $BuiltinName." Too much Input Parameters:$NbInParam , MAX is $NB_MAX_IN \n" ;
		exit (1) ;
	}
	if ( $NbOutParam > $NB_MAX_OUT  )
	{
		print $BuiltinName." Too much Output Parameters:$NbOutParam ,  MAX is $NB_MAX_OUT \n" ;
		exit (1) ;
	}
	if ( $NbInDataFile != $NbInParam  )
	{
		print $BuiltinName." Number of Input data file ($NbInDataFile) shall be equal to Number of Input Parameters($NbInParam) \n" ;
		exit (1) ;								  
	}

	if ( ( $NbOutParam ==  1 ) && ($OutParamList[0] =~ /void/) )
   {
      $NbOutParam = 0;
   }
   
	my @InPrintfFormatList  = () ;
	my @OutPrintfFormatList = () ;
	 
	for (my $Index=0; $Index<$NbInParam; $Index++) 
	{
		my $OneFormat = Translate_Type_To_Printf_Format($InParamList[$Index]) ;
		push @InPrintfFormatList,$OneFormat;
	} 

	for (my $Index=0; $Index<$NbOutParam; $Index++) 
	{
		my $OneFormat = Translate_Type_To_Printf_Format($OutParamList[$Index]) ;
		push @OutPrintfFormatList,$OneFormat;
	} 
	# ----------------- CONSTRUCT the BUILTIN  BLOCK -----------
	my   $LastData = 0  ;
	my @DataIndex   = (0,0,0,0) ; # index of data in input file	 for each input
	do
	{
		if (open(SRCTEMPLATE,"<$SrcBuiltinTemplate")) 
		{
			my 	$OneLine ;
			while ($OneLine=<SRCTEMPLATE>) 
			{

				$OneLine =~ s/$BUILTIN_NAME/$BuiltinName/	;

				if ( $OneLine =~ /$INPUT_DATA(\d)/ ) 
				{
				 	my $nb = $1 ; 
					if ( $nb >	$NbInParam )
					{
						$OneLine = ""	;
					}
					elsif ( ($BuiltImmediate) and IsAnImmediate($nb) )
					{ # immediate parameter
						if ($OneLine =~ /$INPUT_DATA$nb$IS_NOT_IMMEDIATE/) 
						{
							$OneLine = ""	;
						}
						else 
						#($OneLine =~ /$INPUT_DATA$nb$IS_IMMEDIATE/) 
						{
							my $InDataStr = "";
							my $DataNbInFile = 0 ;

							($DataNbInFile,$InDataStr)  	= GetOneData($InDataFile[$nb-1],$DataIndex[$nb-1] ) ;
							
							if ($InParamMaskList[$nb-1] ne "")
							{ # we introduce the mask
								$InDataStr = "(".$InParamMaskList[$nb-1]." & ".$InDataStr.")" ;
							}
							if ($OneLine =~ /$INPUT_DATA$nb$DATAPRINTF/)
							{  # we increment only after we don't need this value again
								$DataIndex[$nb-1] = $DataIndex[$nb-1] + 1 ;

								# if 'number of data already get' is equal to 'number of Data in File'
								if ($DataIndex[$nb-1] == $DataNbInFile)
								{	# end of generation of call to builtin
									$LastData = 1 ;
									# number of data for test
									push @DataSize, $DataNbInFile ;
								}
							}
							my $PrintStr="" ;
							if ($InPrintfFormatList[$nb-1] ne $LONGLONG_HEX_PRINTF_FORMAT) 
							{
							 	$PrintStr= "%".$InPrintfFormatList[$nb-1]."  \"".",".$InDataStr ;
							}
							else
							{
							 	$PrintStr= "%lx%.8lx  \"".",(unsigned int)(".$InDataStr." >> 32 ) & 0x0FFFFFFFFu".",(unsigned int) ".$InDataStr." & 0x0FFFFFFFFu"	 ;
							}
						   	$OneLine =~ s/$INPUT_DATA$nb$DATAPRINTF/$PrintStr/  ;
							$OneLine =~ s/$INPUT_DATA$nb$IS_IMMEDIATE/$InDataStr/ ;
						}

					}
					else
					{ # normal parameter
						if ($OneLine =~ /$INPUT_DATA$nb$IS_IMMEDIATE/) 
						{
							$OneLine = ""	;
						}
						else
						{
							$OneLine =~ s/$INPUT_DATA$nb$IS_NOT_IMMEDIATE// ;
							my $PrintStr="" ;
							if ($InPrintfFormatList[$nb-1] ne $LONGLONG_HEX_PRINTF_FORMAT) 
							{
							 	$PrintStr= "%".$InPrintfFormatList[$nb-1]."  \"".",inp".$nb."[index]" ;
							}
							else
							{
							 	$PrintStr= "%lx%.8lx  \"".",(unsigned int)(inp".$nb."[index] >> 32 ) & 0x0FFFFFFFFu".",(unsigned int)inp".$nb."[index] & 0x0FFFFFFFFu" ;
							}
						   	$OneLine =~ s/$INPUT_DATA$nb$DATAPRINTF/$PrintStr/  ;
						   	$OneLine =~ s/$INPUT_DATA$nb//  ;
						}
					}
				}
				elsif ( $OneLine =~ /$OUTPUT_DATA(\d)/ ) 
				{
				 	my $nb = $1 ; 
					if ( $nb >	$NbOutParam )
					{
						$OneLine = ""	;
					}
					else 
					{
					  	$OneLine =~ s/$OUTPUT_DATA$nb$DATATYPE/$OutParamList[$nb-1]/  ;
						my $PrintStr="" ;
						if ($OutPrintfFormatList[$nb-1] ne $LONGLONG_HEX_PRINTF_FORMAT) 
						{
						 	$PrintStr= "%".$OutPrintfFormatList[$nb-1]." \"".",out".$nb ;
						}
						else
						{
						 	$PrintStr= "%lx%.8lx\"".",(unsigned int)(out".$nb." >> 32 ) & 0x0FFFFFFFFu".",(unsigned int)out".$nb." & 0x0FFFFFFFFu"	;
						}
					   	$OneLine =~ s/$OUTPUT_DATA$nb$DATAPRINTF/$PrintStr/  ;
					   	$OneLine =~ s/$OUTPUT_DATA$nb//  ;
					}
				}
				elsif ( ($BuiltImmediate) and ($OneLine =~ /$IS_NOT_IMMEDIATE/)  )
				{ #remove loop
						$OneLine = ""	;
				}
				elsif ( !($BuiltImmediate) and ($OneLine =~ /$IS_IMMEDIATE/)  )
				{ #remove return
						$OneLine = ""	;
				}
				else
				{
				 $OneLine =~ s/$IS_IMMEDIATE//		;
				 $OneLine =~ s/$IS_NOT_IMMEDIATE//	;
				}
				 $StrBuiltin.="$OneLine";
			}

		}
		else
		{
			print "ERROR Cannot OPEN Builtin Template Src  File\n";
		}
	  
	  $StrBuiltin.="//---------------------------------------\n";

	 } while  ( ($BuiltImmediate) and	($LastData == 0) )	 ;



	# ----------------- CONSTRUCT the MAIN  CODE -----------

	if (open(SRCTEMPLATE,"<$SrcTemplate")) 
	{
		if (open(SRCFILE,">$OneSrcBuiltinFile"))  
		{
		my 	$OneLine ;
		my $DataNb   = 0 ;
			while ($OneLine=<SRCTEMPLATE>) 
			{

				$OneLine =~ s/$BUILTIN_NAME/$BuiltinName/	;
				$OneLine =~ s/$GENERATION_DATE/$GenDate/	;
				
				$OneLine =~ s/$INPUT_PARAM_NB/$NbInParam/ 		; 	
				$OneLine =~ s/$OUTPUT_PARAM_NB/$NbOutParam/ 	; 	

				if ( $OneLine =~ /$INPUT_DATA(\d)/ ) 
				{
				 	my $nb = $1 ; 
					if ( $nb >	$NbInParam )
					{
						$OneLine = ""	;
					}
					else
					{
						if ( ($BuiltImmediate) and IsAnImmediate($nb) )
						{ # immediate parameter
							if ($OneLine =~ /$INPUT_DATA$nb$IS_NOT_IMMEDIATE/) 
							{
								$OneLine = ""	;
							}
						}
						elsif ($OneLine =~ /$INPUT_DATA$nb$DATALIST/) 
						{
							my $InDataStr = "";
							$DataNb   = 0 ;
							($DataNb,$InDataStr) 	= GetDataStr($InDataFile[$nb-1]) ;
							push @DataSize, $DataNb ;
						  	$OneLine =~ s/$INPUT_DATA$nb$DATALIST/$InDataStr/ ;
							$OneLine =~ s/$INPUT_DATA_TAB_SIZE/$DataNb/	;
						}
					   	$OneLine =~ s/$INPUT_DATA$nb$DATATYPE/$InParamList[$nb-1]/  ;
					   	$OneLine =~ s/$INPUT_DATA$nb$DATAPRINTF/$InPrintfFormatList[$nb-1]/  ;
					   	$OneLine =~ s/$INPUT_DATA$nb$IS_NOT_IMMEDIATE//	;
					   	$OneLine =~ s/$INPUT_DATA$nb//  ;
					}
				}
				elsif ( $OneLine =~ /$OUTPUT_DATA(\d)/ ) 
				{
				 	my $nb = $1 ; 
					if ( $nb >	$NbOutParam )
					{
						$OneLine = ""	;
					}
					else 
					{
					  	$OneLine =~ s/$OUTPUT_DATA$nb$DATATYPE/$OutParamList[$nb-1]/  ;
					  	$OneLine =~ s/$OUTPUT_DATA$nb$DATAPRINTF/$OutPrintfFormatList[$nb-1]/  ;
					   	$OneLine =~ s/$OUTPUT_DATA$nb//  ;
					}
				}

				my $TempTabSize = 0	 ;
				if (scalar(@DataSize) != 0) { $TempTabSize =$DataSize[0]; }

				$OneLine =~ s/$INPUT_DATA_TAB_SIZE/$TempTabSize/;

				$OneLine =~ s/$TAB_BUILTIN/$StrBuiltin/	;

				print SRCFILE "$OneLine";
 			}
			close(SRCFILE);
		}
		else
		{
			print "ERROR Cannot OPEN generated Src File\n";
		}
		close(SRCTEMPLATE);
	}
	else
	{
		print "ERROR Cannot OPEN Template Src  File\n";
	}

	my $NbSize  = scalar(@DataSize )  ;
	if ( $NbSize != $NbInParam  )
	{
		print $BuiltinName." Number of Input data size ($NbSize) shall be equal to Number of Input Parameters($NbInParam) \n" ;
		exit (1) ;								  
   	}
	else
	{
		for (my $Index=0; $Index<$NbSize; $Index++) 
		{
			if ( $DataSize[$Index] != $DataSize[0]  )
			{
				print $BuiltinName." Data size (".@DataSize.") shall be the same for all input  \n" ;
				exit (1) ;								  
   			}
		}
	}
	
	
} #end sub GenerateSrc


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


# Ensure correct parameter number.
if (scalar(@ARGV) > 0) 
{
	Usage();
	exit(1);
}


# Analyze Command Line
# AnalyzeCmdLine(\@ARGV);


# ------- Create test Source Dir
if (!-d $SrcDir) 
{
	my $Command = "mkdir $SrcDir";

	if (system("$Command") != 0) 
	{
		print "$Command\n";
		print "Make Test Source directory failed\n";
		exit(1);
	}
}


# -------- Read  Builtin list File -------------------

my $MyBuiltinListFile="$DirMain/$SrcDir/My_builtin.lst" ;

if (-f $MyBuiltinListFile) 
{
 	$BuiltinListFile =	$MyBuiltinListFile ;
}

open(LISTFILE,"<$BuiltinListFile") || die "ERROR $BuiltinListFile File couldn't be open " ;
my $OneLine;
my $BuiltinName  = "";
my $OnWork = 0 ;
my $BuiltImmediate = 0	;
my $ParamNb = 0	   		;
my $BuiltinCounter = 0 	;

while ($OneLine=<LISTFILE>)
{
	chomp($OneLine);
	if ($OneLine =~ /^#/) 
	{
		# Comments in this file begin with a #
		next;
	}
	elsif ($OneLine =~ /^__builtin_(\w+)/)
	{
		if ($OnWork == 1 )
		{
			print "ERROR end missing for $BuiltinName in $BuiltinListFile  \n";
			exit (1) ;
		}
		$BuiltinName = $1 ;
		print  "Reading $BuiltinName in builtin list file\n";
		@InParamList  = () ;
		@ImmedParamNb  = () ;
		@OutParamList = () ;
		@InDataFile   = () ;
		@InParamMaskList= ("","","","") ;
		$OnWork = 1	   ;
		$BuiltImmediate = 0	   ;
		$ParamNb = 0	   		;
		$BuiltinCounter ++ ;
	}
	elsif ($OnWork == 1 )
	{
	 	if ($OneLine =~ s/^immediate//)
		{
			$BuiltImmediate = 1	   ;
		}
	 	elsif ( ($BuiltImmediate == 1) and ($OneLine =~ s/^ImmedparamIn\s+//) )
		{
		 	push @InParamList , $OneLine ;
		 	$ParamNb ++ ;
		 	push @ImmedParamNb , $ParamNb ;
		}
	 	elsif ($OneLine =~ s/^paramIn\s+//)
		{
		 	push @InParamList , $OneLine ;
		 	$ParamNb ++ ;
		}
	 	elsif ($OneLine =~ s/^---mask=(\w+)//)
		{
		 	print "mask= $1 \n";
		 	$InParamMaskList[$ParamNb-1] = $1 ;
		}
	 	elsif ($OneLine =~ s/^paramOut\s+//)
		{
		 	push @OutParamList , $OneLine ;
		}
	 	elsif ($OneLine =~ /^dataIn\s+(\w+)/)
		{
			print "datafile:$1\n"  ;
		 	push @InDataFile , $1 ;
		}
	 	elsif ($OneLine =~ /^end/)
		{
			print  "Generation of $BuiltinName C file \n";
  			GenerateSrc(
					$BuiltinName, 
					$BuiltImmediate
					)	 ;
		 	$OnWork = 0 ;
			$BuiltImmediate = 0	;
			$ParamNb = 0	   	;
		}
	}
}
close(LISTFILE);
print "$BuiltinCounter builtins src files \n" ;

