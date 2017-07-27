<?php

include ('lib/module.php');

$db = openBase();
$arrayTables = getTables();
  
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0"; charset=utf-8>
    <title>WaterMeter statistics</title>
    <style>
        form {
            width: 35em;
            margin: 0 auto;
            text-align: center;
            font: 12pt/10pt sans-serif;
        }

        form {
            max-width: 1000px;
            height: auto;
        }

		a.hot {
			color: #FF0000;
		}
		
		a.cold {
			color: #00008B;
		}

        @media (max-width: 1024px) {
            form {
                width: 800px;
                height: auto;
            }
        }

        @media (max-width: 768px) {
            form {
                width: 600px;
                height: auto;
            }
        }

        @media (max-width: 480px) {
            form {
                width: 100%;
                height: auto;
            }
        }

        @media (max-width: 320px) {
            form {
                width: 100%;
                height: auto;
            }
        }
    </style>
</head>
	<body>

<form action="column.php" method="POST">
   <p><a class="hot"><b>Hot Water Table </b></a><select size="1" name="hot_water_table">
	<?php
	  $i = 0;
	  foreach($arrayTables as $value) {
		if ($i == 0) {
          echo "<option value=\"$value\" selected>$value</option>\n";
		  $i++;
		}
        echo "<option value=\"$value\">$value</option>\n";
      }
	?>
   </select></p>
   <p><a class="cold"><b>Cold Water Table </b></a><select size="1" name="cold_water_table">
	<?php
	  $i = 0;
	  foreach($arrayTables as $value) {
		if ($i == 0) {
          echo "<option value=\"$value\" selected>$value</option>\n";
		  $i++;
		}
        echo "<option value=\"$value\">$value</option>\n";
      }
	?>
   </select></p>
   
   
   <p>Select a date: <input required type="date" name="date"></p>
   
   <p><input type="radio" name="period" id="day" value="day" checked>
   <label for="day">Day</label>
   <input type="radio" name="period" id="month" value="month">
   <label for="month">Month</label>
   <input type="radio" name="period" id="year" value="year">
   <label for="year">Year</label>
   <input type="radio" name="period" id="alltime" value="alltime">
   <label for="alltime">All Time</label></p>
   
   <p><input type="submit" value="Submit"></p>

  </form>

  </body>
</html>

<?php
  closeBase();
?>
