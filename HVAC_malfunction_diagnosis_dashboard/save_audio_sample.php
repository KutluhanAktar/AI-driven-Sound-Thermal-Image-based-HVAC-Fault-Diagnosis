<?php

include_once "../assets/class.php";

// Define the dashboard object of the dashboard class.
$dashboard = new dashboard();
$dashboard->__init__($conn);

# Get the current date and time.
$date = date("Y_m_d_H_i_s");

# Define the text file name of the received raw audio buffer (I2S).
$txt_file = "audio_%s__".$date;

// If XIAO ESP32C6 transfers the raw audio buffer (data) with the selected audio class, save the received buffer as a text (TXT) file and modify the file name accordingly. 
if(isset($_GET["audio"]) && isset($_GET["class"])){
	$txt_file = sprintf($txt_file, $_GET["class"]);
}
if(!empty($_FILES["audio_sample"]["name"])){
	// Text File:
	$received_buffer_properties = array(
	    "name" => $_FILES["audio_sample"]["name"],
	    "tmp_name" => $_FILES["audio_sample"]["tmp_name"],
		"size" => $_FILES["audio_sample"]["size"],
		"extension" => pathinfo($_FILES["audio_sample"]["name"], PATHINFO_EXTENSION)
	);
    // Check whether the uploaded file's extension is in the allowed file formats.
	$allowed_formats = array('jpg', 'png', 'bmp', 'txt');
	if(!in_array($received_buffer_properties["extension"], $allowed_formats)){
		echo "FILE => File Format Not Allowed!";
	}else{
		// Check whether the uploaded file size exceeds the 5 MB data limit.
		if($received_buffer_properties["size"] > 5000000){
			echo "FILE => File size cannot exceed 5MB!";
		}else{
			// Save the uploaded file (TXT).
			move_uploaded_file($received_buffer_properties["tmp_name"], "./".$txt_file.".".$received_buffer_properties["extension"]);
			echo "FILE => Saved Successfully!";
		}
	}
	// Convert the recently saved raw audio buffer (TXT file) to a WAV audio file by executing a Python script — convert_raw_to_wav.py.
	// As executing the Python script, transmit the required audio conversion parameters for the Fermion I2S MEMS microphone as Python Arguments.
	$path = dirname(__FILE__);
	$arguments = '--nchannels=2 --sampwidth=2 --framerate=22000';
	$run_Python = shell_exec('sudo python3 "'.$path.'/convert_raw_to_wav.py" '.$arguments.' 2>&1'); // Add 2>&1 for debugging errors directly on the browser.
	// After generating the WAV audio file from the raw audio buffer, remove the converted text file from the server.
	if(file_exists("./".$txt_file.".txt")) unlink("./".$txt_file.".txt");
	// After completing the audio conversion process successfully, update the system log on the MariaDB database accordingly.
	$dashboard->append_log_update("audio_file", "sample", $_GET["class"], $date, $txt_file.".wav");
}

?>