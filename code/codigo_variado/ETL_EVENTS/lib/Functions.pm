#!/usr/bin/perl -w
#Functions
#DB LAST UPDATE MAR 2016
#Author: Mauricio Lopez
#FUNCTIONS AND TOOLS LIBRARY

package Functions;
use vars qw(@ISA @EXPORT @EXPORT_OK $VERSION);
use strict; 
use warnings;
use Log::Dispatch;
use Exporter 'import';

@ISA = qw(Exporter);
@EXPORT = qw(trim info debug printHash setLog setLogFilename logSeparator);
@EXPORT_OK = qw(ltrim rtrim);
$VERSION = 1.00;

my $ProcID = $$;
my $loglevel = 'info'; #'debug'
my $log;

##########################################################################
# SETLOGFILENAME
##########################################################################

sub setLogFilename
{
	my $fname = shift;
	#undef $log;
	$log = Log::Dispatch->new(
      outputs => [
          [ 'File',   min_level => $loglevel, filename =>$fname, mode => '>>', newline   => 1, ],
          #[ 'Screen', min_level => 'debug', newline   => 1,],              
      ],
	);
	#$log->info(getHeader($ProcID,$loglevel)."================================START=LOG================================");
	$log->info(getHeader($ProcID,$loglevel)."INIT [LOG MODULE]");
}

##########################################################################
# SETLOG
##########################################################################

sub setLog
{
	my $params = shift;
	$log = Log::Dispatch->new(
      outputs => %{$params}
	);
	$log->info(getHeader($ProcID,$loglevel)."INIT [LOG MODULE]: ".$ProcID);
}



##########################################################################
# CAPTURAR HORA PARA LOS LOGS
##########################################################################
sub getHeader{
	my $proc = shift;
	my $level = shift;
	my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
	$year=$year+1900;
	$mon=$mon+1;
	my $_time = sprintf("%02d%02d%02d", $hour, $min, $sec);	
	my $_date = sprintf("%02d%02d%02d", $year,$mon,$mday);
	my $header = "[".$_date.".".$_time."]"."[".$proc."]"."[".$level."] ";
	return $header;
}

sub info{
	my $str = shift;
	setLogFilename('default_log.log') if(!defined $log);
	$log->info(getHeader($ProcID,$loglevel).$str);
}
sub logSeparator{
	my $str = shift;
	setLogFilename('default_log.log') if(!defined $log);
	$log->info("================================");
}

sub debug{
	my $str = shift;
	setLogFilename('default_log.log') if(!defined $log);
	$log->debug(getHeader($ProcID,$loglevel).$str);
}

sub printHash
{
  my ($hash) = @_;
   while( my($k, $v) = each %$hash ) {
	    print "ROW: $k, value: $v.\n";
	    if(ref($v)){
	    	print "{\n";
	    	printHash($v);
	    	print "}\n";
	    } 
   }
}

sub ltrim {
    my $string = shift;
    $string =~ s/^\s*//g;
    return $string;
}


sub rtrim {
    my $string = shift;
    $string =~ s/\s*$//g;
    return $string;
}


sub trim {
    my $string = shift;
    $string =~ s/^\s*(\S+)\s*$/$1/g;
    return $string;
}

#############################################################
# FUNCION PARA SABER SI HACE PING                           #
#############################################################
sub checkIp
{
 my $_ipaddress = shift;
 my $p = Net::Ping->new("syn");
 my $res=0;
 	if(defined $p){
 		if($p->ping($_ipaddress,10)){
			$res=1;
		}
		$p->close();
	}
return $res;
}


1;
