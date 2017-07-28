<?php

include ('lib/module.php');

$hotWaterTable = $_POST['hot_water_table'];
$coldWaterTable = $_POST['cold_water_table'];

$date = explode("-", $_POST['date']);
$year = $date[0];
$month = $date[1];
$day = $date[2];
$period = $_POST['period'];

$db = openBase();

$hotArray = array();
$coldArray = array();

$globCounter = 0;
  



if ($period == "day") {

  $dateIn = mktime(0, 0, 0, $month, $day, $year);
  $dateEnd = mktime(23, 59, 59, $month, $day, $year);
  
  $xAxis = "0";
  for($i=1; $i < 24; $i++) {
    $xAxis = $xAxis.", $i";
  }
  
  $titleText = "00:00:00 $day-$month-$year - 23:59:59 $day-$month-$year";
  
  $retHot = getPeriod($hotWaterTable);
  $retCold = getPeriod($coldWaterTable);

  for($i = 0; $i < 24; $i++) {
    $hotArray[$i] = 0;
    $coldArray[$i] = 0;
  }
    
} elseif ($period == "month") {
  
  $d = date("t", strtotime("$year-$month"));
  
  $dateIn = mktime(0, 0, 0, $month, 1, $year);
  $dateEnd = mktime(23, 59, 59, $month, $d, $year);
  
  $xAxis = "1";
  for ($i=2; $i <= $d; $i++) {
    $xAxis = $xAxis.", $i";
  }
  
  $titleText = "00:00:00 01-$month-$year - 23:59:59 $d-$month-$year";
  
  $retHot = getPeriod($hotWaterTable);
  $retCold = getPeriod($coldWaterTable);
  
  for($i = 1; $i <= $d; $i++) {
    $hotArray[$i] = 0;
    $coldArray[$i] = 0;
  }


} elseif ($period == "year") {
  
  $dateIn = mktime(0, 0, 0, 1, 1, $year);
  $dateEnd = mktime(23, 59, 59, 12, 31, $year);
  
  $xAxis = "1";
  for($i=2; $i <= 12; $i++) {
    $xAxis = $xAxis.", $i";
  }
  
  $titleText = "00:00:00 01-01-$year - 23:59:59 31-12-$year";

  $retHot = getPeriod($hotWaterTable);
  $retCold = getPeriod($coldWaterTable);
  
  for($i = 1; $i <= 12; $i++) {
    $hotArray[$i] = 0;
    $coldArray[$i] = 0;
  }
    
}

if ($period != "alltime") {

  $globCounter = 0;
  makeColumns($hotWaterTable, $retHot, $hotArray);
  $titleText = $titleText."<br>Total $globCounter liters of hot water ";

  $globCounter = 0;
  makeColumns($coldWaterTable, $retCold, $coldArray);
  $titleText = $titleText." and $globCounter liters of cold water";

} else {
	
  $globCounter = getLastCounter($hotWaterTable);
  $titleText = $titleText."<br>Total $globCounter liters of hot water ";
  $hotArray[0] = $globCounter;;

  $globCounter = getLastCounter($coldWaterTable);
  $titleText = $titleText." and $globCounter liters of cold water";
  $coldArray[0] = $globCounter;

  $xAxis = "1";

}
  

?>
<!DOCTYPE HTML>
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<title>Highcharts WaterMeter</title>

		<style type="text/css">

		</style>
	</head>
	<body>
<script src="https://code.highcharts.com/highcharts.js"></script>
<script src="https://code.highcharts.com/modules/exporting.js"></script>

<div id="container" style="min-width: 310px; height: 400px; margin: 0 auto"></div>



<script type="text/javascript">

Highcharts.chart('container', {
    chart: {
        type: 'column'
    },
    title: {
        text: 'WaterMeter'
    },
    subtitle: {
        text: '<?php echo $titleText ?>'
    },
    xAxis: {
        categories: [
		  <?php
		  echo $xAxis;
		  ?>
        ],
        crosshair: true
    },
    yAxis: {
        min: 0,
        title: {
            text: 'Liters'
        }
    },
    tooltip: {
        headerFormat: '<span style="font-size:10px">{point.key}</span><table>',
        pointFormat: '<tr><td style="color:{series.color};padding:0">{series.name}: </td>' +
            '<td style="padding:0"><b>{point.y:.1f} liters</b></td></tr>',
        footerFormat: '</table>',
        shared: true,
        useHTML: true
    },
    plotOptions: {
        column: {
            pointPadding: 0.2,
            borderWidth: 0
        }
    },
    series: [{
        name: 'Hot Water',
        color: '#FF0000',
        data: [<?php
		foreach ($hotArray as $value) {
		  echo "$value, ";
		}
		?>]

    }, {
        name: 'Cold Water',
        color: '#00008B',
        data: [<?php
		foreach ($coldArray as $value) {
		  echo "$value, ";
		}
		?>]

    }]
});
		</script>
	</body>
</html>

<?php
  closeBase();
?>

