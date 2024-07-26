<?php

include_once "class.php";

// Define the dashboard object of the dashboard class.
$dashboard = new dashboard();
$dashboard->__init__($conn);

// Retrieve the system log update list on the MariaDB database, modify the fetched log updates as HTML elements, and pass the generated HTML elements as a JSON object.
if(isset($_GET["new_update"])){
	$generated_html_elements = $dashboard->optain_modify_log_updates();
	
	// Create a JSON object from the generated HTML elements.
	$data = array("generated_html_elements" => $generated_html_elements);
	$j_data = json_encode($data);
    
	// Return the recently generated JSON object.
	echo($j_data);
}
?>