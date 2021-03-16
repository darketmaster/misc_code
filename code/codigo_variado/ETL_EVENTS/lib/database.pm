#database
#Author: Mauricio Lopez 
#DATABASE CLASS


package database;
use Log::StdLog { level => 'info', file => "$0.log" };
use Log::Dispatch;
use Functions;

use DBI;

sub new
{
    my $this = shift;    
    my $class = ref($this) || $this;      
    my($ip,$port,$user,$password,$database)=@_;
    
    my $self = { 
    	_ip=>$ip,
    	_port=>$port,
    	_user=>$user,
        _password=>$password,
        _database=>$database,
      	_query=>{}               ###MODIFICADO
 	};
 	
 	$self->{_dbh}||0;
 	
    bless $self, $class;
    return $self;
}


sub setParameters{
	my($this,$ip,$port,$user,$password,$database)=@_;
	$this->{_ip}=$ip;
	$this->{_port}=$port;
	$this->{_user}=$user;
	$this->{_password}=$password;
	$this->{_database}=$database;
}

sub getDbh{
	my $this = shift;
	return $this->{_dbh};
}


sub connect{
	my $this = shift;
    $this->{_dbh} = DBI->connect("dbi:mysql:".$this->{_database}.":".$this->{_ip}.":".$this->{_port}, $this->{_user},$this->{_password});
}

sub connectODBC{
	my $this = shift;
    $this->{_dbh} = DBI->connect('dbi:ODBC:'.$this->{_database}, $this->{_user},$this->{_password});
}

sub dbQuerySql{
	my($this,$sql)=@_;
	my($res,$help);
	my $query={};

	eval {

	print {*STDLOG} info  => "SQL[".$sql."]";

	my $sth = $this->{_dbh}->prepare($sql);
	my $ejecutado = $sth->execute();
	
	my $i=0;
	if(defined $sth->{NUM_OF_FIELDS} && $sth->{NUM_OF_FIELDS} > 0){
		while (my $ref = $sth->fetchrow_hashref()) {
			   $query->{$i}=$ref;
			   # while( my ($k, $v) = each %$ref ) {
			   # print "$i- key: $k, value: $v.\n";
			   #	}
				$i++;
		}
	}
	};
	if($@) {
		print {*STDLOG} info  => "ERROR: SQL[".$sql."]";
		print {*STDLOG} info  => "ERROR: $@";
	}
	return $query;
}

sub dbQuerySql6{
my($this,$sql)=@_;
my($res,$help);

my $sth = $this->{_dbh}->prepare($sql);
$sth->execute();
my $query={};
my $i=0;
while (my $ref = $sth->fetchrow_hashref()) {
	  my $_is = sprintf("%06d", $i);
	   $query->{$_is}=$ref;
       # while( my ($k, $v) = each %$ref ) {
       # print "$i- key: $k, value: $v.\n";
       #	}
    	$i++;
}
return $query;
}

sub dbQueryCmd(){
my($this,$model,$cmd,@var)=@_;
my $res;
my $help;
$this->connect();
my $sth =  $this->{_dbh}->prepare("SELECT `command`,`help`  FROM `$model`  WHERE name regexp '$cmd'");
$sth->execute();
my $i=0;
while (my $ref = $sth->fetchrow_hashref()) {
       $res=$ref->{command};
	foreach (@var){
		$res=~ s/\$v$i/$_/g;
		$i=$i+1;
		$help=$ref->{help};
	};
}
$this->disconnect();
return $res,$help;
}


##################################################################
################nuevo  2013-09
#_query->{number}
sub dbBufferQueryCmd(){
my($this,$model,$cmd,@var)=@_;
my $res;
my $help;

_query->{number} = 0 if(!defined _query->{number});

	if(_query->{number}==0 || !(defined _query->{$cmd}))
	{
		#print ">>>>>>>NEW_COMMAND\n";
		$this->connect();
		my $sth =  $this->{_dbh}->prepare("SELECT `command`,`help`  FROM `$model`  WHERE name =  '$cmd'");
		$sth->execute();
		
		while (my $ref = $sth->fetchrow_hashref()) {
			my $obj;
			$obj->{command}=$ref->{command};
			$obj->{help}=$ref->{help};
			 _query->{$cmd} = 	$obj;
			 _query->{number}++;
		}
		$this->disconnect();
	}
	
	my $i=0;
	$res=_query->{$cmd}->{command};
	foreach (@var){
		$res=~ s/\$[vV]$i/$_/g;
		$i=$i+1;
		$help=_query->{$cmd}->{command};
	};
				
return $res,$help;
}
##################################################################

sub disconnect{
	my $this = shift;
	$this->{_dbh}->disconnect() if(defined $this->{_dbh});
}

sub _printHash{
  my ($self,$hash) = @_;
   while( my($k, $v) = each %$hash ) {
	    print "ROW: $k, value: $v.\n";
	    while( my($col, $val) = each %$v ) {
	    	print "[$col] : $val \n" if defined $val;
	    }
   }
}


#################################################################
# 2013-04-26 COMMANDS, HELP AND REGEXP
#################################################################

sub dbQueryCmdRegexpRaw(){
my($this,$model,$cmd,$raw)=@_;
my $cmds;
my $output_format;
my $res="";
$this->connect();
my $sth =  $this->{_dbh}->prepare("SELECT command, output_format  FROM `$model`  WHERE name regexp '$cmd'");
$sth->execute();

while (my $ref = $sth->fetchrow_hashref()) {
		$cmds=$ref->{command};
       	$output_format=$ref->{output_format};
       	my @line = split(/[\r\n]+/,$cmds);
	   	foreach (@line){
	   		######print $_."****\n";
			my @var = split('_:_',$_);
			##print $var[0]." --<<>>-- ".$var[1]."\n";
			if( $raw =~ /$var[1]/ )
			{				
				my $tmp = $var[0].":".$output_format."\n";
				eval "\$res .= \"$tmp\"";
			}
		}
}

$this->disconnect();
return $res;
}


sub dbQueryCmdRegexpRawMultiple(){
my($this,$model,$cmd,$raw)=@_;
my $cmds;
my $output_format;
my $res="";
$this->connect();
my $sth =  $this->{_dbh}->prepare("SELECT command, output_format  FROM `$model`  WHERE name regexp '$cmd'");
$sth->execute();

while (my $ref = $sth->fetchrow_hashref()) {
		$cmds=$ref->{command};
       	$output_format=$ref->{output_format};
       	my @line = split(/[\r\n]+/,$cmds);
	   	foreach (@line){
	   		######print $_."****\n";
			my @var = split('_:_',$_);
			##print $var[0]." --<<>>-- ".$var[1]."\n";
			while( $raw =~ m/$var[1]/g )
			{				
				my $tmp = $var[0].":".$output_format."\n";
				eval "\$res .= \"$tmp\"";
			}
		}
}

$this->disconnect();
return $res;
}


#bmultiline get raw info in multiline mode otherwise is take only a bigblock
#bcounter enables _03d counter for variable
#Replace enables a regexp match relacement example s/[.]//g
sub dbQueryCmdRegexp(){
my($this,$model,$cmd,$raw)=@_;
my $cmds;
my $output_format;
my $res="";
$this->connect();
my $sth =  $this->{_dbh}->prepare("SELECT command, output_format  FROM `$model`  WHERE name regexp '$cmd'");
$sth->execute();

while (my $ref = $sth->fetchrow_hashref()) {
		$cmds=$ref->{command};
       	$output_format=$ref->{output_format};
       	my @line = split(/[\r\n]+/,$cmds);
	   	foreach (@line){
	   		######print $_."****\n";
	   		#print {*STDLOG} info  => "line :".$_;
			info("line:<<<".$_.">>>");
			my @var = split('_:_',$_);
			my $bmultiline = 0;
			if(defined $var[2])
			{
			   if($var[2] =~ /1|multi/i)
			   {
			   	$bmultiline = 1;
			   }				
			}
			
			my $bcounter = 0;
			if(defined $var[3])
			{
			   if($var[3] =~ /1|count/i)
			   {
			   	$bcounter = 1;
			   }				
			}
			
			my $bregexpmatch = 0;
			if(defined $var[4] && defined $var[5])
			{
			   $bregexpmatch = 1;				
			}
			
			##print $var[0]." --<<>>-- ".$var[1]."\n";
			if($bmultiline)
			{
				my @rawlines = split('\n',$raw);
				my $counter = 0;
				foreach my $_rawline( @rawlines)
				{
					if( $_rawline =~ /$var[1]/ )
					{	
						my $tmp;
						my $strOutTmp = $output_format;
						eval "\$strOutTmp = \"$output_format\"";
						$var[5] =~ s/ //g if($bregexpmatch);
						$strOutTmp =~ s/$var[4]/$var[5]/g if($bregexpmatch);
						#$res .=  " ".$strOutTmp."\n" if($bregexpmatch);
						if($bcounter)		
						{
							$tmp = $var[0].sprintf("_%03d",$counter).":".$strOutTmp."\n";
							$counter++;
						}
						else
						{
							$tmp = $var[0].":".$strOutTmp."\n";
						} 	
						#eval "\$res .= \"$tmp\"";
						$res .= "$tmp";
					}
				}				
			}
			else
			{
				#info("CHECK REGEXP =~".$var[1]."?");
				if( $raw =~ /$var[1]/ )
				{						
					my $strOutTmp = $output_format;
					eval "\$strOutTmp = \"$output_format\"";
					$var[5] =~ s/ //g if($bregexpmatch);
					$strOutTmp =~ s/$var[4]/$var[5]/g if($bregexpmatch);	
					#$res .=  " ".$strOutTmp."\n" if($bregexpmatch);
					my $tmp = $var[0].":".$strOutTmp."\n";
					#eval "\$res .= \"$tmp\"";
					$res .= "$tmp";
					#info("PASSED!!! : ".$tmp);
				}				
			}			
		}
}

$this->disconnect();
return $res;
}



1;

