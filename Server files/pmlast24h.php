<?php
    $servername = "localhost";
    // REPLACE with your Database name
    $dbname = "particlesensor";
    // REPLACE with Database user
    $username = "pmsuser";
    // REPLACE with Database user password
    $password = "password";

    // Create connection
    $conn = new mysqli($servername, $username, $password, $dbname);
    // Check connection
    $api_key_value = "apikey";

    $query = 'SELECT * FROM pms5003 WHERE datetime > DATE_SUB(NOW(), INTERVAL 24 HOUR) AND datetime <= NOW();';

    $result = $conn->query($query); 
    
    $conn->close();
    
    require_once ('jpgraph/jpgraph.php');
    require_once ('jpgraph/jpgraph_line.php');
    require_once( "jpgraph/jpgraph_date.php" );
    
    while ($row = $result->fetch_assoc()) {
        $datay1[] = $row["pm10_env"];
        $datay2[] = $row["pm25_env"];
        $datay3[] = $row["pm100_env"];
        $date = new DateTime($row["datetime"]);
        $hours[] = $date->format('H:m:s');
    }
    
    $graph = new Graph(1200,400);
    $graph->SetScale("datlin");    
    $theme_class=new UniversalTheme;
    $graph->SetTheme($theme_class);
    $graph->img->SetAntiAliasing(false);
    $graph->title->Set('Particulate Matter Sensor');
    $graph->SetBox(false);    
    $graph->SetMargin(40,20,66,100); //36,63);
    $graph->img->SetAntiAliasing();
    $graph->yaxis->HideZeroLabel();
    $graph->yaxis->HideLine(false);
    $graph->yaxis->HideTicks(false,false);

    $graph->xgrid->Show();
    $graph->xgrid->SetLineStyle("solid");
    $graph->xaxis->SetTickLabels($hours);
    $graph->xgrid->SetColor('#E3E3E3');
    $graph->xaxis->SetTextLabelInterval(2);
    $graph->xaxis->HideLastTickLabel();
    
    // Create the first line
    $p1 = new LinePlot($datay1); 
    $graph->Add($p1);
    $p1->SetColor("#6495ED");
    $p1->SetLegend('PM 1.0 μg/m³');

    // Create the second line
    $p2 = new LinePlot($datay2);
    $graph->Add($p2);
    $p2->SetColor("#B22222");
    $p2->SetLegend('PM 2.5 μg/m³');

    // Create the third line
    $p3 = new LinePlot($datay3);
    $graph->Add($p3);
    $p3->SetColor("#FF1493");
    $p3->SetLegend('PM 10 μg/m³');

    $graph->legend->SetFrameWeight(1);

    // Output line
    $graph->Stroke();    
?>
