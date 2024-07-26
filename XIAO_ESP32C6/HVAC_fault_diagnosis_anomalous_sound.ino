         /////////////////////////////////////////////  
        //  AI-driven Sound & Thermal Image-based  //
       //         HVAC Fault Diagnosis            //
      //           ----------------              //
     //             (XIAO ESP32C6)              //           
    //             by Kutluhan Aktar           // 
   //                                         //
  /////////////////////////////////////////////

//
// Identify the faulty components via anomalous sound detection and diagnose ensuing cooling malfunctions via thermal visual anomaly detection.
//
// For more information:
// https://www.hackster.io/kutluhan-aktar
//
//
// Connections
// XIAO ESP32C6 :
//                                Fermion: I2S MEMS Microphone
// 3.3V    ------------------------ 3V3
// D1      ------------------------ WS (+20K)
// 3.3V    ------------------------ SEL
// D0      ------------------------ SCK
// D2      ------------------------ DO (+220Ω)
//                                SSD1306 OLED Display (128x64)
// D4/SDA  ------------------------ SDA
// D5/SCL  ------------------------ SCL
//                                Control Button (A)
// D8      ------------------------ +
//                                Control Button (B)
// D9      ------------------------ +
//                                Control Button (C)
// D10     ------------------------ +
//                                Control Button (D)
// D3      ------------------------ +
//                                Arduino Nano
// RX (D7) ------------------------ TX (D4)
// TX (D6) ------------------------ RX (D2)


// Include the required libraries:
#include <WiFi.h>
#include <driver/i2s.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Add the icons to be shown on the SSD1306 OLED display.
#include "logo.h"

// Include the Edge Impulse neural network model (Audio MFE) converted to an Arduino library.
#include <AI-driven_HVAC_Fault_Diagnosis_Audio__inferencing.h>

// Define the required parameters to run an inference with the Edge Impulse neural network model.
#define buf_multiplier   5
#define audio_buff_size  512
int16_t sample_audio_buffer[audio_buff_size];

// Define the threshold value for the model outputs (predictions).
float threshold = 0.60;

// Define the anomalous sound (audio) class names.
String classes[] = {"defective", "normal"};

char ssid[] = "<__SSID__>";      // your network SSID (name)
char pass[] = "<__PASSWORD__>";  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

// Define the server on LattePanda Mu (w/ Lite Carrier).
char server[] = "192.168.1.21";
// Define the web application (HVAC malfunction dashboard) path.
String application = "/HVAC_malfunction_diagnosis_dashboard/";

// Initialize the WiFiClient object.
WiFiClient client; /* WiFiSSLClient client; */

// Define the Fermion I2S MEMS microphone configurations.
#define I2S_SCK    D0
#define I2S_WS     D1
#define I2S_DO     D2
#define DATA_BIT   (16) //16-bit
// Define the I2S processor port.
#define I2S_PORT I2S_NUM_0

// Define the SSD1306 screen settings:
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET    -1  // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the control button pins.
#define control_button_A  D8
#define control_button_B  D9
#define control_button_C  D10
#define control_button_D  D3

// Define the required CNC configurations by creating a struct — _CNC.
struct _CNC{
  String pos_command[5] = {"111111111", "222222222", "333333333", "444444444", "hhhhhhhhh"};
  int pos_status[4] = {4, 4, 4, 4};
  boolean serial_update = false;
};

// Define the data holders:
struct _CNC _CNC;
int predicted_class = -1;
int menu_option = 0, position_start = 0;
volatile boolean option_update = false, zeroing = false, defective_sound = false;

void setup(){
  Serial.begin(115200);

  // Via the hardware serial port, initiate the serial communication between XIAO ESP32C6 and Arduino Nano.
  Serial1.begin(115200, SERIAL_8N1, /*RX=*/D7,/*TX=*/D6);

  // Register pin configurations.
  pinMode(control_button_A, INPUT_PULLUP); pinMode(control_button_B, INPUT_PULLUP); pinMode(control_button_C, INPUT_PULLUP); pinMode(control_button_D, INPUT_PULLUP);
  
  // Initialize the SSD1306 screen:
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);

  // Configure the I2S port for the I2S MEMS microphone.
  i2s_install(22000);
  i2s_setpin();
  i2s_start(I2S_PORT);
  delay(1000);

  // Connect to WPA/WPA2 network. Change this line if using an open or WEP network.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  // Attempt to connect to the given Wi-Fi network.
  while(WiFi.status() != WL_CONNECTED){
    // Wait for the network connection.
    delay(500);
    Serial.print(".");
  }
  // If connected to the network successfully:
  Serial.println("Connected to the Wi-Fi network successfully!");
}

void loop(){
  // Adjust the highlighted menu option by utilizing the control buttons — A and C.
  if(!digitalRead(control_button_A)){
    menu_option-=1;
    if(menu_option < 0) menu_option = 3;
    delay(500);
  }
  if(!digitalRead(control_button_C)){
    menu_option+=1;
    if(menu_option > 3) menu_option = 0;
    delay(500);
  }

  // Show the interface (home) screen.
  show_interface("home", menu_option);

  // If the control button B is pressed, navigate to the highlighted interface (menu) option.
  if(menu_option == 1 && !digitalRead(control_button_B)){
    option_update = true;
    while(option_update){
      // Inform the user of the data collection settings.
      int l_offset = 5;
      show_interface("collect", menu_option);
      delay(2000);
      // According to the pressed control button (A or C), generate an audio sample from the collected raw audio buffers
      // and transfer the generated sample with the selected audio class to the web dashboard (application). 
      if(!digitalRead(control_button_A) || !digitalRead(control_button_C)){
        // Get the selected audio class.
        String selected_class = (!digitalRead(control_button_A)) ? "normal" : "defective";
        // Before proceeding with generating an audio sample, check the I2S microphone status.
        if(microphone_sample(false)){
          // After collecting data successfully, notify the user via the screen.
          display.clearDisplay();
          display.drawBitmap((SCREEN_WIDTH-collected_width)/2, l_offset, collected_bits, collected_width, collected_height, SSD1306_WHITE);   
          display.setTextSize(1);
          display.setCursor(0, collected_height+(2*l_offset));
          display.println("I2S microphone\ncollecting data!");
          display.display();
          delay(3000);
          // If the I2S microphone is operating precisely, generate a one-second audio sample by utilizing the buffer multiplier.
          // Simultaneously, transfer the collected raw data buffers to the web dashboard (application) while making an HTTP POST request in order to avoid memory allocation errors.
          if(make_a_post_request("sample_audio_files/save_audio_sample.php?audio=new&class="+selected_class)){
            // If successful:
            display.clearDisplay();
            display.drawBitmap((SCREEN_WIDTH-connected_width)/2, l_offset, connected_bits, connected_width, connected_height, SSD1306_WHITE);   
            display.setTextSize(1);
            display.setCursor(0, connected_height+(2*l_offset));
            display.println("Sample Transferred\nSuccessfully!");
            display.display();
            delay(5000);
          }else{
            display.clearDisplay();
            display.drawBitmap((SCREEN_WIDTH-error_width)/2, l_offset, error_bits, error_width, error_height, SSD1306_WHITE);   
            display.setTextSize(1);
            display.setCursor(0, error_height+(2*l_offset));
            display.println("Server => Connection\nFailed!");
            display.display();
            delay(5000);
          }
        }else{
          display.clearDisplay();
          display.drawBitmap((SCREEN_WIDTH-error_width)/2, l_offset, error_bits, error_width, error_height, SSD1306_WHITE);   
          display.setTextSize(1);
          display.setCursor(0, error_height+(2*l_offset));
          display.println("Sample Collection\nFailed!");
          display.display();
          delay(3000);
        }
      }
      // If the control button D is pressed, redirect the user to the home screen.
      if(!digitalRead(control_button_D)){
        option_update = false;
      }
    }
  }

  if(menu_option == 2 && !digitalRead(control_button_B)){
    option_update = true;
    while(option_update){
      // Run inference.
      run_inference_to_make_predictions();
      show_interface("run", menu_option);
      delay(5000);
      // If the Edge Impulse neural network model (Audio MFE) detects anomalous sound originating from the cooling fans, initiate the thermal imaging process.
      if(predicted_class == 0){
        position_start = 0;
        zeroing = false;
        // Clear the previously assigned buffer status indicators.
        for(int i=0; i<4; i++) _CNC.pos_status[i] = 4;
        delay(2000);
        defective_sound = true;
        // Activate the CNC positioning process.
        while(defective_sound){
          // Notify the user of the CNC positioning status of each individual point by showing their associated status icons on the SSD1306 screen — Waiting, Ongoing, Saved, or Image Ready.
          show_interface("CNC", menu_option);
          delay(2000);
          // Start the CNC positioning sequence and collect thermal scan (imaging) buffers on predefined locations (points) through the Particle Cloud API.
          thermal_buffer_collection_via_CNC("detection");
        }
      }
      // If the control button D is pressed, redirect the user to the home screen.
      if(!digitalRead(control_button_D)){
        option_update = false;
      }
    }
  }
  
  if(menu_option == 3 && !digitalRead(control_button_B)){
    position_start = 0;
    zeroing = false;
    option_update = true;
    // Clear the previously assigned buffer status indicators.
    for(int i=0; i<4; i++) _CNC.pos_status[i] = 4;
    while(option_update){
      // Notify the user of the CNC positioning status of each individual point by showing their associated status icons on the SSD1306 screen — Waiting, Ongoing, Saved, or Image Ready.
      show_interface("CNC", menu_option);
      delay(2000);
      // Start the CNC positioning sequence and collect thermal scan (imaging) buffers on predefined locations (points) through the Particle Cloud API.
      thermal_buffer_collection_via_CNC("sample");
      // If the control button D is pressed, redirect the user to the home screen.
      if(!digitalRead(control_button_D)){
        option_update = false;
      }
    }
  }
  // Wait.
  delay(1000);
}

void run_inference_to_make_predictions(){
  // Summarize the Edge Impulse neural network model (Audio MFE) inference settings (from model_metadata.h):
  ei_printf("\nInference settings:\n");
  ei_printf("\tInterval: "); ei_printf_float((float)EI_CLASSIFIER_INTERVAL_MS); ei_printf(" ms.\n");
  ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
  ei_printf("\tSample length: %d ms.\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);
  ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

  // If the I2S microphone generates a raw audio (data) buffer successfully:
  if(microphone_sample(false)){
    // Run inference:
    ei::signal_t signal;
    // Create a signal object from the resized (scaled) audio buffer.
    signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
    signal.get_data = &microphone_audio_signal_get_data;
    // Run the classifier:
    ei_impulse_result_t result = { 0 };
    EI_IMPULSE_ERROR _err = run_classifier(&signal, &result, false);
    if(_err != EI_IMPULSE_OK){
      ei_printf("ERR: Failed to run classifier (%d)\n", _err);
      return;
    }

    // Print the inference timings on the serial monitor.
    ei_printf("\nPredictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
        result.timing.dsp, result.timing.classification, result.timing.anomaly);

    // Obtain the prediction results for each label (class).
    for(size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++){
      // Print the prediction results on the serial monitor.
      ei_printf("%s:\t%.5f\n", result.classification[ix].label, result.classification[ix].value);
      // Get the imperative predicted label (class).
      if(result.classification[ix].value >= threshold) predicted_class = ix;
    }
    ei_printf("\nPredicted Class: %d [%s]\n", predicted_class, classes[predicted_class]);  

    // Detect classifier anomalies, if any:
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
      ei_printf("Anomaly: ");
      ei_printf_float(result.anomaly);
      ei_printf("\n");
    #endif 

    // Release the audio buffer.
    //ei_free(sample_audio_buffer);
  }
}

static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr){
  // Convert the given microphone (audio) data (buffer) to the out_ptr format required by the Edge Impulse neural network model (Audio MFE).
  numpy::int16_to_float(&sample_audio_buffer[offset], out_ptr, length);
  return 0;
}

boolean make_a_post_request(String request){
  // Connect to the web application named HVAC_malfunction_diagnosis_dashboard. Change '80' with '443' if you are using SSL connection.
  if(client.connect(server, 80)){
    // If successful:
    Serial.println("\nConnected to the web application successfully!\n");
    // Create the query string:
    String query = application + request;
    // Make an HTTP POST request:
    String head = "--AudioSample\r\nContent-Disposition: form-data; name=\"audio_sample\"; filename=\"new_sample.txt\";\r\nContent-Type: text/plain;\r\n\r\n";
    String tail = "\r\n--AudioSample--\r\n";
    // Get the total message length.
    uint32_t totalLen = head.length() + audio_buff_size*buf_multiplier + tail.length();
    // Start the request:
    client.println("POST " + query + " HTTP/1.1");
    client.println("Host: 192.168.1.21");
    client.println("Content-Length: " + String(totalLen));
    client.println("Connection: Keep-Alive");
    client.println("Content-Type: multipart/form-data; boundary=AudioSample");
    client.println();
    client.print(head);
    // According to the given buffer multiplier, collect and transfer I2S raw audio buffers consecutively to prevent memory allocation issues.
    for(int t=0; t<buf_multiplier; t++){
      microphone_sample(false);
      for(int i=0; i<audio_buff_size; i++) client.print(sample_audio_buffer[i]);
    }
    // Complete data (buffer) writing.
    client.print(tail);
    // Wait until transferring the generated (multiplied) raw audio sample.
    delay(5000);
    // If successful:
    Serial.println("HTTP POST => Data transfer completed!\n");
    return true;
  }else{
    Serial.println("\nConnection failed to the web application!\n");
    delay(2000);
    return false;
  }
}

boolean make_a_get_request(String request){
  // Connect to the web application named HVAC_malfunction_diagnosis_dashboard. Change '80' with '443' if you are using SSL connection.
  if(client.connect(server, 80)){
    // If successful:
    Serial.println("\nConnected to the web application successfully!\n");
    // Create the query string:
    String query = application + request;
    // Make an HTTP GET request:
    client.println("GET " + query + " HTTP/1.1");
    client.println("Host: 192.168.1.21");
    client.println("Connection: close");
    client.println();
    // Wait until completing the request process.
    delay(2000);
    // If successful:
    Serial.println("HTTP GET => Connection established!\n");
    return true;
  }else{
    Serial.println("\nConnection failed to the web application!\n");
    delay(2000);
    return false;
  }
}

void thermal_buffer_collection_via_CNC(String process_type){
  // Initiate the four-step CNC positioning sequence so as to move the thermal camera container head to the predefined points for consecutive data (thermal imaging buffer) collection.
  if(position_start < 4){
    // Transfer CNC commands to Arduino Nano via serial communication.
    Serial1.print(_CNC.pos_command[position_start]);
    delay(2000);
    // Update the given position status to Ongoing.
    _CNC.pos_status[position_start] = 0;
    show_interface("CNC", menu_option);
    delay(500);
    // Wait until Arduino Nano returns the coordinate update confirmation message via the serial communication.
    _CNC.serial_update = true;
    String pos_confirmation = nano_update_response();
    // If Arduino Nano transfers the coordinate confirmation message, update the given position status to Completed.
    // Then, increase the point (position) number.
    if(pos_confirmation == "CNC_OK"){
      _CNC.pos_status[position_start] = 1;
      show_interface("CNC", menu_option);
      delay(5000);
      // After positioning the container head on the given location (point), make an HTTP GET request to the web dashboard in order to make Photon 2 collect and register the associated thermal imaging buffer through the Particle Cloud API.
      // If registered successfully, update the given position status to Saved.
      String request = "assets/Particle_cloud_connection.php?collect_particle_cloud_variable="+String(position_start+1);
      if(make_a_get_request(request)) _CNC.pos_status[position_start] = 2;
      // Update the position (point) number.
      position_start++;
      if(position_start == 4){ show_interface("CNC", menu_option); delay(500); zeroing = true; }
    }
  }
  // After passing all four position points successfully, return the thermal camera container head to the starting point (zeroing).
  if(zeroing){
    Serial1.print(_CNC.pos_command[position_start]);
    delay(4000);
    // Wait until Arduino Nano returns the zeroing confirmation message via the serial communication.
    _CNC.serial_update = true;
    String zero_confirmation = nano_update_response();
    if(zero_confirmation == "CNC_OK"){
      // After the container head returns to the starting point, update all position status indicators (icons) to Image Ready.
      for(int i=0; i<4; i++) _CNC.pos_status[i] = 3;
      position_start++;
      delay(1000);
      zeroing = false;
      // Notify the user of the latest updated status indicators.
      show_interface("CNC", menu_option);
      delay(3000);
    }        
  }
  // If Photon 2 registers all thermal imaging buffers successfully and the web dashboard is ready to generate a thermal image from the passed buffers,
  // make an HTTP GET request to the web dashboard to initiate the thermal image conversion process.
  if(_CNC.pos_status[0] == 3 && _CNC.pos_status[1] == 3 && _CNC.pos_status[2] == 3 && _CNC.pos_status[3] == 3){
    // If the web dashboard generates the thermal image successfully, redirect the user to the home screen.
    String request = "assets/Particle_cloud_connection.php?generate_cloud_thermal_image="+process_type;
    if(make_a_get_request(request)){ delay(5000); option_update = false; defective_sound = false; }
  }
}

void show_interface(String com, int menu_option){
  // Get the assigned interface logo information.
  int l_w = interface_widths[menu_option];
  int l_h = interface_heights[menu_option];
  if(com == "home"){
    display.clearDisplay();
    display.drawBitmap(0, (SCREEN_HEIGHT-l_h)/2, interface_logos[menu_option], l_w, l_h, SSD1306_WHITE);   
    display.setTextSize(1); 
    (menu_option == 1) ? display.setTextColor(SSD1306_BLACK, SSD1306_WHITE) : display.setTextColor(SSD1306_WHITE);
    display.setCursor(l_w+5, 5); display.println("Collect Audio");
    (menu_option == 2) ? display.setTextColor(SSD1306_BLACK, SSD1306_WHITE) : display.setTextColor(SSD1306_WHITE);
    display.setCursor(l_w+5, 20); display.println("Faulty Sound");
    (menu_option == 3) ? display.setTextColor(SSD1306_BLACK, SSD1306_WHITE) : display.setTextColor(SSD1306_WHITE);
    display.setCursor(l_w+5, 35); display.println("CNC Positioning");
    display.setCursor(l_w+5, 45); display.println("&Thermal Buffer");
    display.setCursor(l_w+5, 55); display.println("Collection");
    display.display();
  }else if(com == "collect"){
    int l_offset = 1;
    display.clearDisplay();
    display.drawBitmap((SCREEN_WIDTH-l_w)/2, l_offset, interface_logos[menu_option], l_w, l_h, SSD1306_WHITE);   
    display.setTextSize(1);
    display.setCursor((SCREEN_WIDTH/2)-45, (2*l_offset)+l_h+5);
    display.println("[A] => normal");
    display.setCursor((SCREEN_WIDTH/2)-45, (8*l_offset)+l_h+15);
    display.println("[C] => defective");
    display.display();
  }else if(com == "CNC"){
    int l_offset = 2, h_offset = 16;
    // Depending on the status of the CNC positioning point (Waiting, Ongoing, Saved, or Image Ready), display the associated CNC status icon on the screen.
    int i_1 = _CNC.pos_status[0], i_2 = _CNC.pos_status[1], i_3 = _CNC.pos_status[2], i_4 = _CNC.pos_status[3];
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(l_offset, l_offset); display.println("Position [1] =>");
    display.drawBitmap(SCREEN_WIDTH-CNC_widths[i_1]-l_offset, l_offset, CNC_logos[i_1], CNC_widths[i_1], CNC_heights[i_1], SSD1306_WHITE);
    display.setCursor(l_offset, l_offset + h_offset); display.println("Position [2] =>");
    display.drawBitmap(SCREEN_WIDTH-CNC_widths[i_2]-l_offset, l_offset+h_offset, CNC_logos[i_2], CNC_widths[i_2], CNC_heights[i_2], SSD1306_WHITE);
    display.setCursor(l_offset, l_offset + (2*h_offset)); display.println("Position [3] =>");
    display.drawBitmap(SCREEN_WIDTH-CNC_widths[i_3]-l_offset, l_offset+(2*h_offset), CNC_logos[i_3], CNC_widths[i_3], CNC_heights[i_3], SSD1306_WHITE);
    display.setCursor(l_offset, l_offset + (3*h_offset)); display.println("Position [4] =>");
    display.drawBitmap(SCREEN_WIDTH-CNC_widths[i_4]-l_offset, l_offset+(3*h_offset), CNC_logos[i_4], CNC_widths[i_4], CNC_heights[i_4], SSD1306_WHITE);
    display.display();
  }else if(com == "run"){
    int l_c_w = class_widths[predicted_class], l_c_h = class_heights[predicted_class], l_offset = 2;
    String p_c = "[ "+classes[predicted_class]+" ]"; p_c.toUpperCase();
    int p_c_l = p_c.length()*5;  
    display.clearDisplay();
    display.drawBitmap((SCREEN_WIDTH-l_c_w)/2, l_offset, class_logos[predicted_class], l_c_w, l_c_h, SSD1306_WHITE);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor((SCREEN_WIDTH-p_c_l)/2, SCREEN_HEIGHT-(6*l_offset)); display.println(p_c);
    display.display();
  }
}

String nano_update_response(){
  // Wait until Arduino Nano transfers a data packet via serial communication.
  String data_packet = "";
  while(_CNC.serial_update){
    if(Serial1.available() > 0){
      data_packet = Serial1.readString();
    }
    if(data_packet != ""){
      _CNC.serial_update = false;
    }
    delay(1000); 
  }
  // Then, return the obtained data packet.
  return data_packet;
}

bool microphone_sample(bool _debug){ 
  // Obtain the information generated by the I2S microphone and save it to the input buffer — sample_audio_buffer.
  size_t bytesIn = 0;
  esp_err_t result = i2s_read(I2S_PORT, &sample_audio_buffer, audio_buff_size, &bytesIn, portMAX_DELAY);
  // If the I2S microphone generates raw audio data successfully:
  if(result == ESP_OK){
    Serial.println("\nAudio Data Generated Successfully!");
    // Depending on the given model, scale (resize) the raw audio buffer (data) collected by the I2S microphone. Otherwise, the sound might be too quiet for classification.
    for(int x = 0; x < bytesIn/2; x++){
      sample_audio_buffer[x] = (int16_t)(sample_audio_buffer[x]) * 8;
    }
    // If requested, display the average (mean) audio data reading on the serial plotter.
    if(_debug){
      int16_t samples_read = bytesIn / 8;
      if(samples_read > 0){
        float mean = 0;
        for(int16_t i = 0; i < samples_read; ++i){ mean += (sample_audio_buffer[i]); }
        mean /= samples_read;
        Serial.println(mean);
      }
    }
    // Exit. 
    return true;
  }else{
    Serial.println("\nAudio Data Collection Failed!");
    // Exit.
    return false;
  }
}

void i2s_install(uint32_t sampling_rate){
  // Configure the I2S processor port for the I2S microphone (ONLY_RIGHT).
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = sampling_rate,
    .bits_per_sample = (i2s_bits_per_sample_t)DATA_BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,
    .dma_buf_count = 16,
    .dma_buf_len = audio_buff_size,
    .use_apll = false
  };
 
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}
 
void i2s_setpin(){
  // Set the I2S microphone pin configurations.
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_DO
  };
 
  i2s_set_pin(I2S_PORT, &pin_config);
}
