<?php

include('conexao.php');

//Creating Array for JSON response
$response = array();

// Check if we got the field from the user
if (isset($_GET['hum']) && isset($_GET['temp']) && isset($_GET['volt']) && isset($_GET['pot']) ) {
    
    $hum= $_GET['hum'];
    $temp= $_GET['temp'];
    $volt= $_GET['volt'];
    $pot= $_GET['pot'];
        
    // Fire SQL query to insert data in weather
    $result = mysqli_query($conexao, "INSERT INTO gprsArduinoTest(hum,temp,volt,pot) VALUES('$hum','$temp','$volt','$pot')");
    
    // Check for succesfull execution of query
    if ($result) {
        // successfully inserted 
        $response["success"] = 1;
        $response["message"] = "Base de dados atualizada com sucesso";
        
        // Show JSON response
        echo json_encode($response);
    } else {
        // Failed to insert data in database
        $response["success"] = 0;
        $response["message"] = "Falha ao inserir na base de dados";
        
        // Show JSON response
        echo json_encode($response);
    }
} else {
    // If required parameter is missing
    $response["success"] = 0;
    $response["message"] = "Falta insercao de parametros na url";
    
    // Show JSON response
    echo json_encode($response);
}

?>
