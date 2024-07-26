<?php

// Include the Twilio PHP Helper Library. 
require_once 'twilio-php-main/src/Twilio/autoload.php';
use Twilio\Rest\Client;

// Define the dashboard class and its functions.
class dashboard{
	public $conn;
	private $twilio;
	private $table = "system_log";
	// Define the required configurations to communicate with Particle Photon 2 via the Particle Device Cloud API.
	private $Particle = array(
								"API" => "https://api.particle.io/v1/devices/",
								"device_id" => "<__DEVICE_ID__>/", // etc. 0a1111111111111111111111/
								"access_token" => "<__ACCESS_TOKEN__>",
								"variables" => array("thermal_buff_1", "thermal_buff_2", "thermal_buff_3", "thermal_buff_4"),
								"_function" => "collect_thermal_buffers"
							);
    
	public function __init__($conn){
		$this->conn = $conn;
		// Define the Twilio account credentials and object.
		$_sid    = "<__SID__>";
        $token  = "<__ACCESS_TOKEN__>";
        $this->twilio = new Client($_sid, $token);
		// Define the user and the Twilio-verified phone numbers.
		$this->user_phone = "+____________";
		$this->from_phone = "+____________";	
	}
	
    // Database -> Append new log update	
	public function append_log_update($type, $category, $class, $date, $info){
		// Insert new system log updates (sample collections or model inference results) into the system_log MariaDB database table.
		$sql = "INSERT INTO `$this->table` (`type`, `category`, `class`, `date`, `info`)
		        VALUES ('$type', '$category', '$class', '$date', '$info')";
	    mysqli_query($this->conn, $sql);
	}
	
	// Database -> Optain, modify, and pass system log updates
	public function optain_modify_log_updates(){
		$generated_html_elements = [];
		// Obtain all system log updates registered on the MariaDB database table — system_log.
		$sql = "SELECT * FROM `$this->table` ORDER BY `id` DESC";
		$result = mysqli_query($this->conn, $sql);
		$check = mysqli_num_rows($result);
		if($check > 0){
			while($row = mysqli_fetch_assoc($result)){
				$html_element = '';
				// Modify the fetched log updates as HTML elements according to the passed log category.
				if($row["type"] == "thermal_img" && $row["category"] == "detection"){
					$is_cooling_malfunction = ($row["class"] == "malfunction") ? '<p><i class="fa-solid fa-triangle-exclamation"></i> Cooling Malfunction Detected!</p>' : '<p><i class="fa-solid fa-circle-check"></i> Cooling Status is Stable!</p>';
					$html_element = '
										<section class="t_detection">
										<img src="generate_thermal_img/img_detection/'.$row["info"].'" />
										<h2><i class="fa-regular fa-image"></i> Thermal Image</h2>
										<p><i class="fa-solid fa-circle-info"></i> Malfunction Diagnosis</p>
										<p><i class="fa-solid fa-triangle-exclamation"></i> Anamolous Sound Detected!</p>
										'.$is_cooling_malfunction.'
										<p><i class="fa-regular fa-clock"></i> '.$row["date"].'</p>
										<div class="overlay thermal_detect"><a href="generate_thermal_img/img_detection/'.$row["info"].'" download><button><i class="fa-solid fa-cloud-arrow-down"></i></button></a></div>
										</section>
					                ';
				}else if($row["type"] == "thermal_img" && $row["category"] == "sample"){
					$html_element = '
										<section class="t_sample">
										<img src="generate_thermal_img/img_sample/'.$row["info"].'" />
										<h2><i class="fa-regular fa-image"></i> Thermal Image</h2>
										<p><i class="fa-solid fa-circle-info"></i> Sample Collection</p>
										<p><i class="fa-regular fa-clock"></i> '.$row["date"].'</p>
										<div class="overlay thermal_sample"><a href="generate_thermal_img/img_sample/'.$row["info"].'" download><button><i class="fa-solid fa-cloud-arrow-down"></i></button></a></div>
										</section>
					                ';					
				}else if($row["type"] == "audio_file"){
					$html_element = '
										<section class="a_sample">
										<img src="assets/audio_icon.jpg" />
										<h2><i class="fa-solid fa-music"></i> Anamolous Sound</h2>
										<p><i class="fa-solid fa-circle-info"></i> Sample Collection</p>
										<p><i class="fa-solid fa-volume-high"></i> Class: '.$row["class"].'</p>
										<p><i class="fa-regular fa-clock"></i> '.$row["date"].'</p>
										<div class="overlay audio_sample"><a href="sample_audio_files/files/'.$row["info"].'" download><button><i class="fa-solid fa-cloud-arrow-down"></i></button></a></div>
										</section>
					                ';						
				}
				// Append the most recently modified HTML element to the associated main element array so as to create a list of the generated HTML elements.
				array_push($generated_html_elements, $html_element);
			}
			// Finally, return the generated HTML element list (array).
			return $generated_html_elements;
		}else{
			return '
						<section><img src="assets/database_empty.jpg" /><h2>There are no system log updates on the database yet.</h2></section>
						<section><img src="assets/database_empty.jpg" /><h2>There are no system log updates on the database yet.</h2></section>
						<section><img src="assets/database_empty.jpg" /><h2>There are no system log updates on the database yet.</h2></section>
						<section><img src="assets/database_empty.jpg" /><h2>There are no system log updates on the database yet.</h2></section>
						<section><img src="assets/database_empty.jpg" /><h2>There are no system log updates on the database yet.</h2></section>
						<section><img src="assets/database_empty.jpg" /><h2>There are no system log updates on the database yet.</h2></section>
				   ';
		}		
	}

    // Via the Particle Cloud API, make Particle Photon 2 collect a thermal scan (imaging) buffer and save the collected buffer to the passed Cloud variable.
    public function particle_register_parameter($variable){
		// Define the required authorization configurations and function arguments (POST data parameters).
		$data = "access_token=".$this->Particle["access_token"]."&args=".$variable;
		// By making a cURL call (POST request), communicate with the Particle Cloud API to activate the given Cloud function on Photon 2.
		$url = $this->Particle["API"].$this->Particle["device_id"].$this->Particle["_function"];
		$curl = curl_init();
		curl_setopt($curl, CURLOPT_POST, 1);
		curl_setopt($curl, CURLOPT_POSTFIELDS, $data);
		curl_setopt($curl, CURLOPT_URL, $url);
		//curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
		curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
		curl_setopt($curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		// Execute the defined cURL call.
		$result = curl_exec($curl);
		if(!$result){ echo "Particle Cloud API => Connection Failed!"; }
		else{ echo "Particle Cloud API => Connection Successful!"; }
        curl_close($curl);
	}
	
	// Via the Particle Cloud API, obtain the Cloud variables (thermal imaging buffers) collected and registered by Particle Photon 2.
    public function particle_obtain_parameter($variable){
		// By making a cURL call (GET request), communicate with the Particle Cloud API to obtain the variables registered by Photon 2.
		$url = $this->Particle["API"].$this->Particle["device_id"].$this->Particle["variables"][$variable-1]
		       ."?access_token=".$this->Particle["access_token"];
		$curl = curl_init();
		curl_setopt($curl, CURLOPT_URL, $url);
		curl_setopt($curl, CURLOPT_HEADER, false);
		curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
		curl_setopt($curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		// Execute the defined cURL call.
		$result = curl_exec($curl);
		if(!$result){ return "Particle Cloud API => Connection Failed!"; }
		// If the Cloud connection is successful, decode the received JSON data packet to obtain the registered value of the passed variable. Then, return the obtained value.
		else{
			$data_packet = json_decode($result);
			return $data_packet->result;
		}
        curl_close($curl);
	}

    // Obtain all thermal scan (imaging) buffers registered by Photon 2 individually from the Particle Cloud.
	// Then, run a Python script (generate_thermal_image_and_run_model.py) to generate a thermal image from the fetched buffers.
	// If the passed process type is detection, run an inference with the Edge Impulse FOMO-AD (visual anomaly detection) model on LattePanda Mu via the same Python script.
    public function particle_generate_thermal_image_from_buffers($process_type){
		// Obtain thermal imaging buffers registered on the Particle Cloud.
		$thermal_buffers = [];
		for($i=0; $i<count($this->Particle["variables"]); $i++){
			$thermal_buffers[$i] = $this->particle_obtain_parameter($i+1);
		}
		// Generate and save a thermal image from the given buffers by executing the generate_thermal_image_and_run_model.py file.
		// As executing the Python script, transmit the obtained thermal buffers and the given process type as Python Arguments.
		$path = str_replace("/assets", "/generate_thermal_img", dirname(__FILE__));
		$arguments = '--buff_1='.$thermal_buffers[0].' --buff_2='.$thermal_buffers[1].' --buff_3='.$thermal_buffers[2].' --buff_4='.$thermal_buffers[3].' --process='.$process_type;
		$run_Python = shell_exec('sudo python3 "'.$path.'/generate_thermal_image_and_run_model.py" '.$arguments.' 2>&1'); // Add 2>&1 for debugging errors directly on the browser.
		// If the passed process type is detection, obtain and return the detected thermal cooling malfunction class after running the FOMO-AD (visual anomaly detection) model via the Python script.
		// Otherwise, obtain the default sample collection response.
		return $run_Python;
	}

	// Send an SMS to the registered phone number via Twilio so as to inform the user of the latest system log update regarding cooling status.
	public function Twilio_send_SMS($body){
		// Configure the SMS object.
        $sms_message = $this->twilio->messages
			->create($this->user_phone,
				array(
					   "from" => $this->from_phone,
                       "body" => $body
                     )
                );
		// Send the SMS.
		echo("SMS SID: ".$sms_message->sid);	  
	}		
}

// Define the MariaDB server and database connection settings.
$server = array(
	"server" => "localhost",
	"username" => "root",
	"password" => "",
	"database" => "hvac_system_updates"
);

$conn = mysqli_connect($server["server"], $server["username"], $server["password"], $server["database"]);

?>