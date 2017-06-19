#!/opt/bin/perl

use DBI;
use strict;
use Text::ParseWords;

my $litersinpulse = 10;

my $mqttpub = "/opt/bin/mosquitto_pub";
my $mqttuser = "test";
my $mqttpassword = "1111";

# define database name and driver
my $driver   = "SQLite";
my $db_name = "/opt/etc/watermeter/watermeter.db";
my $dbd = "DBI:$driver:dbname=$db_name";

# sqlite does not have a notion of username/password
my $username = "";
my $password = "";

my @msg = quotewords(" ", 0, @ARGV[0]);
my $topic = @ARGV[1];

my $tableName = $topic;

#my $time = @msg[0];
my $time = time();
my $water = @msg[1];
my $new = @msg[2];

$topic =~ s/\s*//;
$time =~ s/\s*//;
$water =~ s/\s*//;
my $tableName = uc($topic);
$tableName =~ s/\//_/g;
$topic =~ s/\/Out/\/In/;


my $dbh = DBI->connect($dbd, $username, $password, { RaiseError => 1 })
                      or die $DBI::errstr;
print STDERR "Database opened successfully\n";

# create a table
my $twm = qq(CREATE TABLE IF NOT EXISTS '$tableName'
             (ID INTEGER PRIMARY KEY     AUTOINCREMENT,
              TIME       TIMESHTAMP    NOT NULL,
              COUNTER      INT     NOT NULL););



my $ret = $dbh->do($twm);
              
if($ret < 0) {
   print STDERR $DBI::errstr;
} else {
   print STDERR "Table \'".$tableName."\' created successfully\n";
}


if (@msg == 3) {
  if ($new == "NEW") {
    my $stmt = qq(INSERT INTO '$tableName' (ID, TIME,COUNTER)
      VALUES (NULL, '$time', '$water' ) );
    my $rv = $dbh->do($stmt) or die $DBI::errstr;
  }
}


if (@msg < 3) {
  my $stmt = qq(SELECT * FROM '$tableName' ORDER BY ID DESC LIMIT 1;);
  my $sth = $dbh->prepare( $stmt );
  my $ret = $sth->execute() or die $DBI::errstr;
  if($ret < 0){
    print $DBI::errstr;
  }
  my @row = $sth->fetchrow_array();

  $sth->finish();
  
  my $swater = $row[2];
  my $stime = $row[1];
  
  if ($swater > $water) {
    $water = $swater;
    $time = $stime;
  } else  {
    if ($swater == $water) {
      $water = $water + $litersinpulse;
      $new = "NEW";
    }
    $stmt = qq(INSERT INTO '$tableName' (ID, TIME,COUNTER)
      VALUES (NULL, '$time', '$water' ) );
    my $rv = $dbh->do($stmt) or die $DBI::errstr;
  }

}

my $cmd = "$mqttpub -u $mqttuser -P $mqttpassword -m "."\'$time $water $new\'".' -t '.$topic."\n";

system($cmd);

$dbh->disconnect();
print STDERR "Exit the database\n";
