#!/usr/bin/perl

use lib './lib/';
use strict; 
use warnings;
use Data::Dumper;
use JSON;
use configloader;
use Functions;
use utf8;
use open qw(:std :utf8);
use Digest::MD5 qw(md5 md5_hex md5_base64);
use Time::HiRes qw/ time sleep /;
use database;
use DBI qw(:sql_types);
use File::Basename;

setLogFilename($0.".log");
info("===".basename($0)."===");
my $start = time;

#DEFAULT RESPONSE
my $response;
$response->{type}="error";
$response->{msg}="Bad Request";
$response->{data}="{}";
$response->{time}=0;

my $d = new configloader('./config.conf');

my $DBREF='MainDB';
my @_cfgppal = ( $d->{_CONFIG}{$DBREF}{ip},			#0
			$d->{_CONFIG}{$DBREF}{port}, 			#1
			$d->{_CONFIG}{$DBREF}{user},			#2
			$d->{_CONFIG}{$DBREF}{password}, 		#3
			$d->{_CONFIG}{$DBREF}{database},		#4
			$d->{_CONFIG}{$DBREF}{reference_table},
);
my $_dbh = new database($_cfgppal[0], $_cfgppal[1], $_cfgppal[2], $_cfgppal[3], $_cfgppal[4]);

my $TACACSDB='TACACSDB';
my @_cfgtacacs = ( $d->{_CONFIG}{$TACACSDB}{ip},		#0
			$d->{_CONFIG}{$TACACSDB}{port}, 			#1
			$d->{_CONFIG}{$TACACSDB}{user},				#2
			$d->{_CONFIG}{$TACACSDB}{password}, 		#3
			$d->{_CONFIG}{$TACACSDB}{database},			#4
);
my $_dbh_tacacs = new database($_cfgtacacs[0], $_cfgtacacs[1], $_cfgtacacs[2], $_cfgtacacs[3], $_cfgtacacs[4]);

# if (-e $filename) {
# 	ProcessFile($filename);
# } else {
#    $response->{msg}="the file $filename does not exist!";
# }

my $Htypes = loadTypes('dbdm_ref_type');
#print(to_json($Htypes)."\n");

my $Htables = loadTables('dbdm_ref_table');
#print(to_json($Htables)."\n");

my $Hmappers = loadMappers('dbdm_map_field');
#print(to_json($Hmappers)."\n");

my $refHash = loadHash('dbdm_reference');
#print(to_json($refHash)."\n");

my $controlHash = load_TLC_control('dbdm_etl_control');
#print(to_json($controlHash)."\n");
#print(to_json($controlHash->{data})."\n");

my $id_table = $Htables->{data}->{'tac_log_authentication'}->{id};
my $startrow = $controlHash->{data}->{$id_table}->{idrow};
my $nrows = $controlHash->{data}->{$id_table}->{numrows};

info("Process table: tac_log_authentication(".$id_table.")-start:".$startrow." Size:".$nrows);

$response->{data}={};
if(1)
{
	my $resHash = loadAuthentication($startrow,$nrows);
	if($resHash->{count}>0)
	{
		$response->{msg} = "" if($response->{msg} =~ /Bad/);
		$response->{type}="info";
		$response->{msg}.=" authentication: ".$resHash->{count};
		$response->{data}->{authentication}=$resHash->{count};
		$response->{data}->{authenticationlimits} = "$startrow,$nrows";
	}
	saveHash($refHash,'dbdm_reference') if($refHash->{save});
	updateControl($controlHash, $id_table,'dbdm_etl_control', $resHash->{count});
}


$id_table = $Htables->{data}->{'tac_log_authorization'}->{id};
$startrow = $controlHash->{data}->{$id_table}->{idrow};
$nrows = $controlHash->{data}->{$id_table}->{numrows};

info("Process table: tac_log_authorization(".$id_table.")-start:".$startrow." Size:".$nrows);

if(1)
{
	my $resHash = loadAuthorization($startrow,$nrows);
	if($resHash->{count}>0)
	{
		$response->{msg} = "" if($response->{msg} =~ /Bad/);
		$response->{type}="info";
		$response->{msg}.=" authorization: ".$resHash->{count};
		$response->{data}->{authorization}=$resHash->{count};
		$response->{data}->{authorizationlimits} = "$startrow,$nrows";
	}
	saveHash($refHash,'dbdm_reference') if($refHash->{save});
	updateControl($controlHash, $id_table,'dbdm_etl_control', $resHash->{count});
}

removeRows('authentication');
removeRows('authorization');

#print(to_json($refHash)."\n");

my $end   = time;
my $ttime = ($end - $start) * 1000;
info("The total time is ($start , $end) ".$ttime." ms.");
$response->{time} = sprintf("%0.4f",$ttime);
info to_json($response);
print to_json($response);


#############################################################
## MAIN HASH
#############################################################

sub searchInHash{
	my $hash = shift;
	my $type = shift;
	my $key = trim shift;
	my $response=0;
	$key='NULL' if (!defined $key);
	$key='NA' if ($key eq "");

	if(!defined $hash->{data}->{$key})
	{
		$hash->{data}->{$key}={};
		info("hash->{data}->{".$key."}-not exists adding");
	}

	if(!defined $hash->{data}->{$key}->{$type})
	{
		info(ref($hash)." = ".($hash->{n}+1)." key = ".$key." type = ".$type);
		$hash->{n}++;
		$hash->{data}->{$key}->{$type}=$hash->{n};
		$hash->{save}=1;	
	}
	$response = $hash->{data}->{$key}->{$type};
	return $response;
}

sub loadHash{
	my $table = shift;
	my $hash = {};	
	$hash->{n}=0;
	$hash->{save}=0;
	$hash->{data}={};
	my $count=0;

	$_dbh->connect();
	
	my $sql = "select id, name,fk_ref_type_id from ".$table;	
	my $sth =  $_dbh->{_dbh}->prepare($sql);
	
	# execute the query
  	$sth->execute();

	info $sth->rows . " rows found in ".$table;
	while (my $ref = $sth->fetchrow_hashref()) {
		#print to_json($ref);
		my $id = $ref->{id};
		my $name = $ref->{name};
		my $type = $ref->{fk_ref_type_id};
		$hash->{n} = $id if($id>$hash->{n});
		$hash->{data}->{$name}={} if(!defined $hash->{data}->{$name});
		$hash->{data}->{$name}->{$type}=$id;
		$count++;
	}
	$sth->finish();	

	$_dbh->disconnect();

	return $hash;
}

sub saveHash{
	my $hash = shift;
	my $table = shift;
	my $count=0;

	$_dbh->connect();
	
	my $sql = "INSERT IGNORE INTO ".$table."(id, name, fk_ref_type_id) VALUES(?,?,?)";	
	my $sth =  $_dbh->{_dbh}->prepare($sql);
	
	foreach my $name(keys %{$hash->{data}})
	{		
		foreach my $type(keys %{$hash->{data}->{$name}})
		{
			my $id = $hash->{data}->{$name}->{$type};
			#print "id: $id, Name: $name\n";
			$sth->bind_param( 1, int($id) );
			$sth->bind_param( 2, $name, SQL_VARCHAR);
			$sth->bind_param( 3, $type );
			# execute the query
			$sth->execute();
			$count++;
		}		
	}
	info("Table:".$table." ".$count." rows updated");

	$sth->finish();	

	$_dbh->disconnect();

}


sub loadTypes{
	my $table = shift;
	my $hash = {};	
	$hash->{n}=0;
	my $count=0;

	$_dbh->connect();
	
	my $sql = "select id, name from ".$table;	
	my $sth =  $_dbh->{_dbh}->prepare($sql);
	
	# execute the query
  	$sth->execute();

	info $sth->rows . " rows found in ".$table;
	while (my $ref = $sth->fetchrow_hashref()) {
		#print to_json($ref);
		my $id = $ref->{id};
		my $name = $ref->{name};
		$hash->{n} = $id if($id>$hash->{n});
		$hash->{data}->{$name}=$id;
		$count++;
	}
	$sth->finish();	

	$_dbh->disconnect();

	return $hash;
}

sub loadTables{
	my $table = shift;
	my $hash = {};	
	$hash->{n}=0;
	$hash->{data}={};
	my $count=0;

	$_dbh->connect();
	
	my $sql = "select id, `schema`, name from ".$table;	
	my $sth =  $_dbh->{_dbh}->prepare($sql);
	
	# execute the query
  	$sth->execute();

	info $sth->rows . " rows found in ".$table;
	while (my $ref = $sth->fetchrow_hashref()) {
		#print to_json($ref);
		my $id = $ref->{id};
		my $schema = $ref->{schema};
		my $name = $ref->{name};
		$hash->{n} = $id if($id>$hash->{n});
		$hash->{data}->{$name}={};
		$hash->{data}->{$name}->{id}=$id;
		$hash->{data}->{$name}->{schema}=$schema;		
		$count++;
	}
	$sth->finish();	

	$_dbh->disconnect();

	return $hash;
}

sub loadMappers{
	my $table = shift;
	my $hash = {};	
	$hash->{n}=0;
	$hash->{data}={};
	my $count=0;

	$_dbh->connect();
	
	my $sql = "SELECT dmf.id id, dmf.fieldname name, drt.name fromtable, drt2.name type FROM dbdm_map_field dmf ".
			"inner join dbdm_ref_table drt on dmf.fk_ref_table_id = drt.id ".
			"inner join dbdm_ref_type drt2 on dmf.fk_ref_type_id = drt2.id ";	
	my $sth =  $_dbh->{_dbh}->prepare($sql);
	
	# execute the query
  	$sth->execute();

	info $sth->rows . " rows found in ".$table;
	while (my $ref = $sth->fetchrow_hashref()) {
		#print to_json($ref);
		my $id = $ref->{id};
		my $name = $ref->{name};
		my $fromtable = $ref->{fromtable};
		my $type = $ref->{type};
		$hash->{n} = $id if($id>$hash->{n});
		$hash->{data}->{$name}={} if(!defined $hash->{data}->{$name});
		$hash->{data}->{$name}->{$fromtable}={} if(!defined $hash->{data}->{$name}->{$fromtable});
		$hash->{data}->{$name}->{$fromtable}->{id}=$id;
		$hash->{data}->{$name}->{$fromtable}->{type}=$type;		
		$count++;
	}
	$sth->finish();	

	$_dbh->disconnect();

	return $hash;
}

sub loadAuthentication{
	my $linf = shift;
	my $lsup = shift;
	my $hash = {};	
	$hash->{n}=0;
	$hash->{count}=0;
	my $count=0;

	$_dbh_tacacs->connect();
	#info("Load tac_log_authentication from:".$linf." size:".$lsup);
	my $sql_tacacts = "select * from tac_log_authentication limit ".$linf.",".$lsup;	
	my $sth_tacacs =  $_dbh_tacacs->{_dbh}->prepare($sql_tacacts);	
	# execute the query
  	$sth_tacacs->execute();

	############## REPLICA TABLE MINIMIZED
	$_dbh->connect();	
	my $sql = "INSERT IGNORE INTO DBDM_TACACS.authentication (id, server, `date`, nas, username, line, nac, `action`) VALUES(?, ?, ?, ?, ?, ?, ?, ?)";	
	my $sth =  $_dbh->{_dbh}->prepare($sql);	

	info $sth_tacacs->rows . " rows found in tac_log_authentication";
	while (my $ref = $sth_tacacs->fetchrow_hashref()) {

		#print to_json($ref)."\n";
		my $id = $ref->{id};
		my $id_server = searchInHash($refHash, $Htypes->{data}->{"SERVER"},$ref->{server});
		my $date = $ref->{date};
		my $id_nas = searchInHash($refHash, $Htypes->{data}->{"NAS"},$ref->{nas});
		my $id_username = searchInHash($refHash, $Htypes->{data}->{"USERNAME"},$ref->{username});
		my $id_line = searchInHash($refHash, $Htypes->{data}->{"LINE"},$ref->{line});
		my $id_nac = searchInHash($refHash, $Htypes->{data}->{"NAC"},$ref->{nac});
		my $id_action = searchInHash($refHash, $Htypes->{data}->{"ACTION"},$ref->{action});

		#my @par = ($id,$id_server,$date,$id_nas,$id_username,$id_line,$id_nac,$id_action);
		#print to_json(@par)."\n";

		$sth->bind_param( 1, int($id) );
		$sth->bind_param( 2, $id_server);
		$sth->bind_param( 3, $date );
		$sth->bind_param( 4, $id_nas );
		$sth->bind_param( 5, $id_username );
		$sth->bind_param( 6, $id_line );
		$sth->bind_param( 7, $id_nac );
		$sth->bind_param( 8, $id_action );

		$sth->execute();
	
		# my $name = $ref->{name};
		$hash->{n} = $id if($id>$hash->{n});
		# $hash->{data}->{$name}=$id;
		$count++;
	}
	
	info("Table: authentication ".$count." rows updated");
	$sth->finish();	
	$sth_tacacs->finish();	

	$_dbh_tacacs->disconnect();
	$hash->{count}=$count;

	return $hash;
}


sub load_TLC_control{
	my $table = shift;
	my $hash = {};	
	$hash->{n}=0;
	$hash->{save}=0;
	$hash->{data}={};
	my $count=0;

	$_dbh->connect();
	
	my $sql = "SELECT id, idrow, numrows, lastidrow, lastnumrows, created, modified, lastmodified, fk_ref_table_id FROM ".$table;	
	my $sth =  $_dbh->{_dbh}->prepare($sql);
	
	# execute the query
  	$sth->execute();

	info $sth->rows . " rows found in ".$table;
	while (my $ref = $sth->fetchrow_hashref()) {
		#print to_json($ref);
		
		my $id = $ref->{id};
		my $name = $ref->{fk_ref_table_id};
		$hash->{n} = $id if($id>$hash->{n});
		$hash->{data}->{$name}={} if(!defined $hash->{data}->{$name});
		foreach my $k (keys %{$ref})
		{
			#print to_json($k)."\n";
			$hash->{data}->{$name}->{$k}=$ref->{$k};
		}
		$count++;
	}
	$sth->finish();	

	$_dbh->disconnect();

	return $hash;
}

sub updateControl{
	my $hash = shift;
	my $id_tableref = shift;
	my $table = shift;
	my $numrows = shift;

	$_dbh->connect();

	#print("HASH:\n".to_json($hash)."\nDEBUG\n");
	my $idtable = $hash->{data}->{$id_tableref}->{fk_ref_table_id};

	info("Updating etl control: ".$table." start to ".$numrows);

	my $sql = "UPDATE ".$table." SET lastidrow=idrow, lastnumrows=numrows, lastmodified=modified, idrow=idrow+".$numrows.", modified=CURRENT_TIMESTAMP WHERE fk_ref_table_id=".$idtable;	
	my $sth =  $_dbh->{_dbh}->prepare($sql);	
	$sth->execute();

	info("Table:".$table." updated");

	$sth->finish();	

	$_dbh->disconnect();

}

sub loadAuthorization{
	my $linf = shift;
	my $lsup = shift;
	my $hash = {};	
	$hash->{n}=0;
	$hash->{count}=0;
	my $count=0;

	$_dbh_tacacs->connect();

	#info("Load tac_log_authorization from:".$linf." size:".$lsup);	
	my $sql_tacacts = "SELECT id, server, `date`, nas, username, line, nac, `action`, SUBSTRING_INDEX(cmd,' | ',1) cmd from tac_log_authorization limit ".$linf.",".$lsup;	
	my $sth_tacacs =  $_dbh_tacacs->{_dbh}->prepare($sql_tacacts);	
	# execute the query
  	$sth_tacacs->execute();

	############## REPLICA TABLE MINIMIZED
	$_dbh->connect();	
	my $sql = "INSERT IGNORE INTO DBDM_TACACS.authorization (id, server, `date`, nas, username, line, nac, `action`, cmd) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)";	
	my $sth =  $_dbh->{_dbh}->prepare($sql);	

	info $sth_tacacs->rows . " rows found in tac_log_authorization";
	while (my $ref = $sth_tacacs->fetchrow_hashref()) {

		#print to_json($ref)."\n";
		my $id = $ref->{id};
		my $id_server = searchInHash($refHash, $Htypes->{data}->{"SERVER"},$ref->{server});
		my $date = $ref->{date};
		my $id_nas = searchInHash($refHash, $Htypes->{data}->{"NAS"},$ref->{nas});
		my $id_username = searchInHash($refHash, $Htypes->{data}->{"USERNAME"},$ref->{username});
		my $id_line = searchInHash($refHash, $Htypes->{data}->{"LINE"},$ref->{line});
		my $id_nac = searchInHash($refHash, $Htypes->{data}->{"NAC"},$ref->{nac});
		my $id_action = searchInHash($refHash, $Htypes->{data}->{"ACTION"},$ref->{action});
		my $id_cmd = searchInHash($refHash, $Htypes->{data}->{"CMD"},$ref->{cmd});

		#my @par = ($id,$id_server,$date,$id_nas,$id_username,$id_line,$id_nac,$id_action);
		#print to_json(@par)."\n";

		$sth->bind_param( 1, int($id) );
		$sth->bind_param( 2, $id_server);
		$sth->bind_param( 3, $date );
		$sth->bind_param( 4, $id_nas );
		$sth->bind_param( 5, $id_username );
		$sth->bind_param( 6, $id_line );
		$sth->bind_param( 7, $id_nac );
		$sth->bind_param( 8, $id_action );
		$sth->bind_param( 9, $id_cmd );

		$sth->execute();
	
		# my $name = $ref->{name};
		$hash->{n} = $id if($id>$hash->{n});
		# $hash->{data}->{$name}=$id;
		$count++;
	}
	
	info("Table: authorization ".$count." rows updated");
	$sth->finish();	
	$sth_tacacs->finish();	

	$_dbh_tacacs->disconnect();
	$hash->{count}=$count;

	return $hash;
}


sub removeRows{
	my $table = shift;
	$_dbh->connect();
	
	my $sql = "DELETE FROM ".$table;	
	my $sth =  $_dbh->{_dbh}->prepare($sql);	
	$sth->execute();
	info("Remove rows in Table:".$table." Number of rows affected: " . $sth->rows);
	$sth->finish();	

	$_dbh->disconnect();
}
