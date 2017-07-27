<?php

$sqliteBase = "/opt/etc/watermeter/watermeter.db";

date_default_timezone_set("Europe/Moscow");


function openBase() {
  global $sqliteBase;

  $db = new SQLite3($sqliteBase);
  return $db;
}

function closeBase() {
  global $db;
  $db->close();
}

function getTables() {
	
  global $db;
	
  $sql = "SELECT name FROM (SELECT * FROM sqlite_master UNION ALL SELECT * FROM sqlite_temp_master) WHERE type='table' ORDER BY name";

  $ret = $db->query($sql);

  $counterTables = 0;
  $arrayTables = array();
  while($row = $ret->fetchArray() ) {
	
	if ($row['name'] != "" && $row['name'] != "sqlite_sequence") {		
	  $arrayTables[$counterTables] = $row['name'];
	  $counterTables++;
	}
  }
  return $arrayTables;   
}

function getPeriod($waterTable) {
  
  global $db, $dateIn, $dateEnd;
  
  $sql = "SELECT * from $waterTable where TIME BETWEEN $dateIn AND $dateEnd ORDER by TIME";

  $ret = $db->query($sql);
  
  return $ret;

}

function getPreCount($waterTable, $id) {
  
  global $db;
  
  $id--;
  
  $sql = "SELECT * from $waterTable WHERE ID = $id";
  
  $ret = $db->query($sql);
  
  $row = $ret->fetchArray(SQLITE3_ASSOC);
  
  $preCount = $row['COUNTER'];
  
  return $preCount;
}

function getLastCounter($waterTable) {

  global $db;
  
  $sql = "SELECT * from $waterTable ORDER BY TIME DESC LIMIT 1";
  
  $ret = $db->query($sql);
  $row = $ret->fetchArray(SQLITE3_ASSOC);
  
  $counter = $row['COUNTER'];
  
  return $counter;
}


function makeColumns($waterTable, $ret, &$wArray) {
	
  global $period, $globCounter;
	
  $i = 0;
  $timeUpdate = 0;
  $inTime = 0;
  $endTime = 0;
  $counter = 0;

  while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
    if ($i == 0) {
      $firstID = $row['ID'];
	  if ($firstID != 1) {
        $preCount = getPreCount($waterTable, $firstID);
	  } else {
	    $preCount = 0;
	  }
	  $i++;
	  $ret->reset();
	  continue;
	}
	
nonfetch:

	if ($timeUpdate == 0) {
		
      $tt = $row['TIME'];
	  
	  $hour = date("G", $tt);
	  $day = date("j", $tt);
	  $nday = date("t", $tt);
	  $month = date("n", $tt);
	  $year = date("Y", $tt);

	  if ($period == "day") {
  
  
		$inTime = mktime($hour, 0, 0, $month, $day, $year);
		$endTime = mktime($hour, 59, 59, $month, $day, $year);
  
		$arr =  $hour;

	  } elseif ($period == "month") {
  
		$inTime = mktime(0, 0, 0, $month, $day, $year);
		$endTime = mktime(23, 59, 59, $month, $day, $year);
  
		$arr = $day;
		
	  } else {
		
		$inTime = mktime(0, 0, 0, $month, 1, $year);
		$endTime = mktime(23, 59, 59, $month, $nday, $year);
		
		$arr = $month;
		
	  }
	  
	  $timeUpdate++;
	}
	
	if ($row['TIME'] >= $inTime && $row['TIME'] <= $endTime) {
	  $count = $row['COUNTER'] - $preCount;
	  $preCount = $row['COUNTER'];
	  $counter += $count;
	} else {
	  $globCounter += $counter;
	  if ($period == "month" && $counter > 5000) {
	    $counter = 2000;
	  } elseif ($period == "day" && $counter > 300) {
		$counter = 300;
	  }
	  $wArray[$arr] = $counter;
	  $counter = 0;
	  $timeUpdate = 0;
	  goto nonfetch;
	}
	
  }
  $wArray[$arr] = $counter;
  $globCounter += $counter;
  
}

?>