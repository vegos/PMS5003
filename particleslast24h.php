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
        $datay1[] = $row["particles_03um"];
        $datay2[] = $row["particles_05um"];
        $datay3[] = $row["particles_10um"];
        $datay4[] = $row["particles_25um"];
        $datay5[] = $row["particles_50um"];
        $datay6[] = $row["particles_100um"];
        $date = new DateTime($row["datetime"]);
        $hours[] = $date->format('H:m:s');
    }
    
    $graph = new Graph(1200,400);
    $graph->SetScale("datlin"); 
    $theme_class=new UniversalTheme;
    $graph->SetTheme($theme_class);
    $graph->img->SetAntiAliasing(false);
    $graph->title->Set('Particulate Matter Sensor - Particles / 0.1Lt air');
    $graph->SetBox(false);
    $graph->SetMargin(60,30,66,100); //36,63);
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
    $p1->SetLegend('0.3 μm');

    // Create the second line
    $p2 = new LinePlot($datay2);
    $graph->Add($p2);
    $p2->SetLegend('0.5 μm');

    // Create the third line
    $p3 = new LinePlot($datay3);
    $graph->Add($p3);
    $p3->SetLegend('1.0 μm');

    // Create the fourth line
    $p4 = new LinePlot($datay4);
    $graph->Add($p4);
    $p4->SetLegend('2.5 μm');

    // Create the fifth line
    $p5 = new LinePlot($datay5);
    $graph->Add($p5);
    $p5->SetLegend('5.0 μm');

    // Create the sixth line
    $p6 = new LinePlot($datay6);
    $graph->Add($p6);
    $p6->SetLegend('10.0 μm');

    $graph->legend->SetFrameWeight(1);

    // Output line
    $graph->Stroke();    
?>
