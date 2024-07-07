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
    $api_key_value = "api-key";

    $query = 'SELECT * FROM pms5003 WHERE datetime > DATE_SUB(NOW(), INTERVAL 24 HOUR) AND datetime <= NOW();';

    $result = $conn->query($query); 
    
    $conn->close();
    
    require_once ('jpgraph/jpgraph.php');
    require_once ('jpgraph/jpgraph_line.php');
    require_once( "jpgraph/jpgraph_date.php" );
    
    while ($row = $result->fetch_assoc()) {
        $datay1[] = $row["batteryvolts"];
        $date = new DateTime($row["datetime"]);
        $hours[] = $date->format('H:m:s');
    }
    
    $graph = new Graph(1200,400);
    $graph->SetScale("datlin"); 
    $theme_class=new UniversalTheme;
    $graph->SetTheme($theme_class);
    $graph->img->SetAntiAliasing(false);
    $graph->title->Set('Particulate Matter Sensor - Battery Info');
    $graph->SetBox(false);
    $graph->SetMargin(40,20,66,100);
    $graph->img->SetAntiAliasing();
    $graph->yaxis->HideZeroLabel();
    $graph->yaxis->HideLine(false);
    $graph->yaxis->HideTicks(false,false);
    $graph->xgrid->Show();
    $graph->xgrid->SetLineStyle("solid");
    $graph->xaxis->SetTickLabels($hours);
    $graph->xaxis->HideLastTickLabel();
    $graph->xgrid->SetColor('#E3E3E3');
    $graph->xaxis->SetTextLabelInterval(2);

    // Create the first line
    $p1 = new LinePlot($datay1);
    $graph->Add($p1);
    $p1->SetColor("#FF1493");
    $p1->SetLegend('Battery Volts (mV)');

    $graph->legend->SetFrameWeight(1);

    // Output line
    $graph->Stroke();    
?>
