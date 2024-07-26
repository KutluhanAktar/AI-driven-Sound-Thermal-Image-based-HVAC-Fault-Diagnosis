         /////////////////////////////////////////////  
        //  AI-driven Sound & Thermal Image-based  //
       //         HVAC Fault Diagnosis            //
      //           ----------------              //
     //           (Particle Photon 2)           //           
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
// Particle Photon 2 :  
//                                MLX90641 Thermal Imaging Camera (16x12 w/ 110° FOV)
// D1 / SCL   --------------------- SCL
// D0 / SDA   --------------------- SDA
//                                ST7735 1.8" Color TFT Display
// 3.3V --------------------------- LED
// D17 / SCK  --------------------- SCK
// D15 / MOSI --------------------- SDA
// D3   --------------------------- AO (DC)
// D4   --------------------------- RESET
// D2   --------------------------- CS
// GND  --------------------------- GND
// 3.3V --------------------------- VCC
//                                COM-09032 Analog Joystick
// A0   --------------------------- VRX
// A1   --------------------------- VRY
// D19  --------------------------- SW
//                                Control Button (OK)
// D9   --------------------------- +
//                                5mm Common Anode RGB LED
// D13  --------------------------- R
// D14  --------------------------- G
// D5   --------------------------- B  


// Include Particle Device OS APIs.
#include "Particle.h"
// Include the required libraries.
#include <Wire.h>
#include <MLX90641_API.h>
#include <MLX9064X_I2C_Driver.h>
#include "Adafruit_ST7735.h"

// Add the icons to be shown on the ST7735 TFT display.
#include "logo.h"

// Via Device OS, connect to the Particle Cloud automatically.
SYSTEM_MODE(AUTOMATIC);
// Then, enable threading to run the given code (application) and the system (network management) concurrently.
SYSTEM_THREAD(ENABLED);
// If required, display system, cloud connectivity, and application logs via the serial monitor.
//SerialLogHandler logHandler(LOG_LEVEL_INFO); // View logs with CLI using 'particle serial monitor --follow'

// Define the Particle Cloud variable configurations by creating a struct — _thermal.
struct _thermal{
  String buff_1 = "empty";
  String buff_2 = "empty";
  String buff_3 = "empty";
  String buff_4 = "empty";
  boolean buff_1_st = false;
  boolean buff_2_st = false;
  boolean buff_3_st = false;
  boolean buff_4_st = false;
};

// Define the MLX90641 thermal imaging camera configurations.
const byte MLX90641_address = 0x33; // Default 7-bit unshifted address of the MLX90641 camera.
#define TA_SHIFT 12 // Default shift value for the MLX90641 camera in the open air.
uint16_t eeMLX90641[832];
float MLX90641To[192];
uint16_t MLX90641Frame[242];
paramsMLX90641 MLX90641;
int errorno = 0;

// Define various temperature threshold ranges to design a specific color algorithm to generate thermal images within the given thermal buffer configurations.
int min_temp = 18, mod_temp_1 = 20, mod_temp_2 = 22, mod_temp_3 = 24, max_temp = 26;
#define thermal_buff_width   16
#define thermal_buff_height  12
#define thermal_buff_num     4
#define thermal_buff_size    thermal_buff_width * thermal_buff_height
#define thermal_img_size     thermal_buff_size * thermal_buff_num

// Define the ST7735 TFT screen settings.
#define SCREEN_HEIGHT 160
#define SCREEN_WIDTH  128
#define TFT_CS  D2
#define TFT_DC  D3
#define TFT_RST D4
Adafruit_ST7735 st7735(&SPI, TFT_CS, TFT_DC, TFT_RST);

// Define analog joystick pins.
#define JS_X A0
#define JS_Y A1
#define JS_B D19
#define C_B  D9
// Define RGB LED pins.
#define red_pin   D13
#define green_pin D14
#define blue_pin  D5

// Define the required menu design structure by creating a struct — _menu.
struct _menu {
  int background = st7735.color565(23, 30, 39);
  int border = st7735.color565(186, 12, 46);
  int scan_c = st7735.color565(174, 225, 205);
  int inspect_c = st7735.color565(243, 208, 40);
  int menu_c = st7735.color565(255, 255, 255);
  int highlight = st7735.color565(76, 74, 70);
  int text_c = st7735.color565(76, 74, 70);
  int text_c_h = st7735.color565(186, 12, 46);
  int status_c[2] = {ST77XX_RED, ST7735_GREEN};
};

// Define the data holders:
struct _thermal _thermal;
struct _menu _menu;
volatile boolean show_home = true, s_update = false, b_update = false, buff_update = false;
int menu_option = -1, buff_num = 0;
int j_x_read, j_y_read, j_b_read, c_b_read, j_min = 250, j_max = 3500;
int w_b_offset = 10, m_b_w = SCREEN_WIDTH-(2*w_b_offset), m_b_h = 45, m_b_r = 12, i_offset = 5;
int w_b_i_offset = w_b_offset+i_offset, m_b_i_w = SCREEN_WIDTH-(2*w_b_i_offset), m_b_i_h = m_b_h-(2*i_offset), m_b_i_r = m_b_r-i_offset;
int b_x = w_b_offset, b_y = 10, b_i_x = w_b_i_offset, b_i_y = b_y+i_offset;
int ic_w = 50, ic_h = 45, ic_x = (SCREEN_WIDTH-ic_w)/2, ic_y = (SCREEN_HEIGHT-ic_h)/2;

// To avoid errors due to threading, declare custom application functions before the setup function.
String get_and_display_data_from_MLX90641(int buffer_size);
int collect_thermal_buffers(String num);
void show_interface(String command);
boolean is_MLX_Connected();
void joystick_read();
void adjustColor(int r, int g, int b);


void setup(){
  Serial.begin(115200);

  // Assign the defined Particle Cloud variables.
  Particle.variable("thermal_buff_1", _thermal.buff_1);
  Particle.variable("thermal_buff_2", _thermal.buff_2);
  Particle.variable("thermal_buff_3", _thermal.buff_3);
  Particle.variable("thermal_buff_4", _thermal.buff_4);

  // Assign the defined Particle Cloud functions.
  Particle.function("collect_thermal_buffers", collect_thermal_buffers);

  // Register pin configurations.
  pinMode(JS_B, INPUT_PULLUP);
  pinMode(C_B, INPUT_PULLUP);
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  adjustColor(0,0,0);

  // Initialize the ST7735 screen.
  st7735.initR(INITR_BLACKTAB);
  st7735.setRotation(2);
  st7735.fillScreen(ST77XX_BLACK);
  st7735.setTextSize(1);
  st7735.setTextWrap(false);

  // Initiate the I2C communication and set the clock speed to 2M to generate accurate thermal scan (imaging) buffers via the MLX90641 thermal imaging camera.
  Wire.begin();
  Wire.setClock(2000000);

  // Check the connection status between the MLX90641 thermal imaging camera and Photon 2.
  if(is_MLX_Connected() == false){
    st7735.fillScreen(ST77XX_RED);
    st7735.setCursor(0, 20);
    st7735.setTextColor(ST77XX_BLACK);
    st7735.println("MLX90641 not detected at default I2C address!");
    st7735.println("Please check wiring. Freezing.");
    while (1);
  }

  // Obtain the MLX90641 thermal imaging camera parameters and check the parameter extraction status.
  int status;
  status = MLX90641_DumpEE(MLX90641_address, eeMLX90641);
  errorno = status;
  //MLX90641_CheckEEPROMValid(eeMLX90641);//eeMLX90641[10] & 0x0040;
 
  if(status != 0){
    st7735.fillScreen(ST77XX_RED);
    st7735.setCursor(0, 20);
    st7735.setTextColor(ST77XX_BLACK);
    st7735.println("Failed to load MLX90641 system parameters!");
    while(1);
  }
 
  status = MLX90641_ExtractParameters(eeMLX90641, &MLX90641);
  if(status != 0){
    st7735.fillScreen(ST77XX_RED);
    st7735.setCursor(0, 20);
    st7735.setTextColor(ST77XX_BLACK);
    st7735.println("MLX90641 parameter extraction failed!");
    while(1);
  }

  // Once the MLX90641 parameters are extracted successfully, release the eeMLX90641 array and set the refresh rate to 16 Hz.
  MLX90641_SetRefreshRate(MLX90641_address, 0x05);

}

void loop(){
  // Show the home screen.
  if(show_home) show_interface("home");

  // Adjust the highlighted menu option depending on the joystick movements.
  joystick_read();
  if(j_y_read > j_max) { menu_option = 0; b_update = true; delay(500); }
  if(j_y_read < j_min) { menu_option = 1; b_update = true; delay(500); }
  
  // If the joystick button is pressed, navigate to the highlighted interface (menu) option.
  if(menu_option == 0){
    if(b_update){
      st7735.fillRoundRect(b_i_x, b_i_y, m_b_i_w, m_b_i_h, m_b_i_r, _menu.highlight);
      st7735.setTextColor(_menu.text_c_h); st7735.setTextSize(2); st7735.setCursor(b_i_x+25, b_i_y+10); st7735.println("Scan");
      st7735.fillRoundRect(b_i_x, SCREEN_HEIGHT-b_i_y-m_b_i_h, m_b_i_w, m_b_i_h, m_b_i_r, _menu.inspect_c);
      st7735.setTextColor(_menu.text_c); st7735.setCursor(b_i_x+8, SCREEN_HEIGHT-b_i_y-m_b_i_h+10); st7735.println("Inspect");
      st7735.fillRect(ic_x, ic_y, ic_w, ic_h, _menu.background); int i_x = menu_option+1;
      st7735.drawBitmap((SCREEN_WIDTH-interface_widths[i_x])/2, (SCREEN_HEIGHT-interface_heights[i_x])/2, interface_logos[i_x], interface_widths[i_x], interface_heights[i_x], _menu.scan_c);
    }b_update = false;
    if(!j_b_read){
      s_update = true;
      show_interface("scan");
      while(s_update){
        joystick_read();
        // If the control button (OK) is pressed, generate thermal scan buffers
        // and assign the collected buffers to the associated arrays (strings) incrementally (from 1 to 4). 
        if(!c_b_read){
          if(buff_num == 0) { _thermal.buff_1 = get_and_display_data_from_MLX90641(thermal_buff_size); _thermal.buff_1_st = true;}
          if(buff_num == 1) { _thermal.buff_2 = get_and_display_data_from_MLX90641(thermal_buff_size); _thermal.buff_2_st = true;}
          if(buff_num == 2) { _thermal.buff_3 = get_and_display_data_from_MLX90641(thermal_buff_size); _thermal.buff_3_st = true;}
          if(buff_num == 3) { _thermal.buff_4 = get_and_display_data_from_MLX90641(thermal_buff_size); _thermal.buff_4_st = true;}
          // Change the associated array number.
          buff_num++; if(buff_num > 3) buff_num = 0;
          // Update the assigned buffer status icons after registering buffers to arrays (strings).
          buff_update = true;
          delay(250);
        }
        // Show the buffer status icons on the screen to inform the user of the ongoing procedure.
        if(buff_update){
          // Change the assigned status icon of the recently registered buffer.
          int i_x = menu_option+1, l_x = 5, l_y = 25+interface_heights[i_x], l_offset = 25;
          if(_thermal.buff_1_st){
            st7735.fillRect(SCREEN_WIDTH-status_widths[0]-l_x, l_y-(status_heights[0]/2), status_widths[0], status_heights[0], _menu.highlight);
            st7735.drawBitmap(SCREEN_WIDTH-status_widths[1]-l_x, l_y-(status_heights[1]/2), status_logos[1], status_widths[1], status_heights[1], _menu.status_c[1]);
          }
          if(_thermal.buff_2_st){
            st7735.fillRect(SCREEN_WIDTH-status_widths[0]-l_x, l_y-(status_heights[0]/2)+l_offset, status_widths[0], status_heights[0], _menu.highlight);
            st7735.drawBitmap(SCREEN_WIDTH-status_widths[1]-l_x, l_y-(status_heights[1]/2)+l_offset, status_logos[1], status_widths[1], status_heights[1], _menu.status_c[1]);
          }
          if(_thermal.buff_3_st){
            st7735.fillRect(SCREEN_WIDTH-status_widths[0]-l_x, l_y-(status_heights[0]/2)+(2*l_offset), status_widths[0], status_heights[0], _menu.highlight);
            st7735.drawBitmap(SCREEN_WIDTH-status_widths[1]-l_x, l_y-(status_heights[1]/2)+(2*l_offset), status_logos[1], status_widths[1], status_heights[1], _menu.status_c[1]);
          }
          if(_thermal.buff_4_st){
            st7735.fillRect(SCREEN_WIDTH-status_widths[0]-l_x, l_y-(status_heights[0]/2)+(3*l_offset), status_widths[0], status_heights[0], _menu.highlight);
            st7735.drawBitmap(SCREEN_WIDTH-status_widths[1]-l_x, l_y-(status_heights[1]/2)+(3*l_offset), status_logos[1], status_widths[1], status_heights[1], _menu.status_c[1]);
          }
          // Avoid flickering.
          buff_update = false;
        }
        // Do not forget to add delays in while loops. Otherwise, the while loop interrupts the Particle Cloud network connection.
        delay(2000);
        // If the X-axis of the joystick moves to the left, redirect the user to the home screen.
        if(j_x_read > j_max){
          s_update = false;
          show_home = true;
          menu_option = -1;
        }
      }
    }
  }

  if(menu_option == 1){
    if(b_update){
      st7735.fillRoundRect(b_i_x, b_i_y, m_b_i_w, m_b_i_h, m_b_i_r, _menu.scan_c);
      st7735.setTextColor(_menu.text_c); st7735.setTextSize(2); st7735.setCursor(b_i_x+25, b_i_y+10); st7735.println("Scan");
      st7735.fillRoundRect(b_i_x, SCREEN_HEIGHT-b_i_y-m_b_i_h, m_b_i_w, m_b_i_h, m_b_i_r, _menu.highlight);
      st7735.setTextColor(_menu.text_c_h); st7735.setCursor(b_i_x+8, SCREEN_HEIGHT-b_i_y-m_b_i_h+10); st7735.println("Inspect");
      st7735.fillRect(ic_x, ic_y, ic_w, ic_h, _menu.background); int i_x = menu_option+1;
      st7735.drawBitmap((SCREEN_WIDTH-interface_widths[i_x])/2, (SCREEN_HEIGHT-interface_heights[i_x])/2, interface_logos[i_x], interface_widths[i_x], interface_heights[i_x], _menu.inspect_c);
    }b_update = false;
    if(!j_b_read){
      s_update = true;
      show_interface("inspect");
      while(s_update){
        joystick_read();
        // If the control button (OK) is pressed, clear all thermal scan buffers and the latest generated thermal image.
        if(!c_b_read){
          _thermal.buff_1 = _thermal.buff_2 = _thermal.buff_3 = _thermal.buff_4 = "empty";
          _thermal.buff_1_st = _thermal.buff_2_st = _thermal.buff_3_st = _thermal.buff_4_st = false;
          buff_num = 0;
          delay(500);
          show_interface("inspect");
          delay(500);
        }
        // Do not forget to add delays in while loops. Otherwise, the while loop interrupts the Particle Cloud network connection.
        delay(2000);
        // If the X-axis of the joystick moves to the left, redirect the user to the home screen.
        if(j_x_read > j_max){
          s_update = false;
          show_home = true;
          menu_option = -1;
        }
      }
    }
  }
}

void show_interface(String command){
  if(command == "home"){
    adjustColor(0,0,0);
    st7735.fillScreen(_menu.background);
    // Define the menu option buttons.
    st7735.fillRoundRect(b_x, b_y, m_b_w, m_b_h, m_b_r, _menu.border);
    st7735.fillRoundRect(b_i_x, b_i_y, m_b_i_w, m_b_i_h, m_b_i_r, _menu.scan_c);
    st7735.setTextColor(_menu.text_c);
    st7735.setTextSize(2);
    st7735.setCursor(b_i_x+25, b_i_y+10);
    st7735.println("Scan");
    st7735.fillRoundRect(b_x, SCREEN_HEIGHT-b_y-m_b_h, m_b_w, m_b_h, m_b_r, _menu.border);
    st7735.fillRoundRect(b_i_x, SCREEN_HEIGHT-b_i_y-m_b_i_h, m_b_i_w, m_b_i_h, m_b_i_r, _menu.inspect_c);
    st7735.setCursor(b_i_x+8, SCREEN_HEIGHT-b_i_y-m_b_i_h+10);
    st7735.println("Inspect");
    // Show the interface (home) icon.
    st7735.fillRect(ic_x, ic_y, ic_w, ic_h, _menu.background);
    int i = 0;
    st7735.drawBitmap((SCREEN_WIDTH-interface_widths[i])/2, (SCREEN_HEIGHT-interface_heights[i])/2, interface_logos[i], interface_widths[i], interface_heights[i], _menu.menu_c);
    // Stop the screen flickering.
    show_home = false;
  }else if(command =="scan"){
    adjustColor(0,255,1);
    st7735.fillScreen(_menu.highlight);
    int i_x = menu_option+1;
    st7735.drawBitmap((SCREEN_WIDTH-interface_widths[i_x])/2, 10, interface_logos[i_x], interface_widths[i_x], interface_heights[i_x], _menu.scan_c);
    st7735.setTextSize(1); 
    st7735.setTextColor(_menu.scan_c);
    // According to the registered thermal scan buffers, show the assigned buffer status icons.
    int l_x = 5, l_y = 25+interface_heights[i_x], l_offset = 25;
    st7735.setCursor(l_x, l_y); st7735.println("Buffer [1] =>");
    st7735.drawBitmap(SCREEN_WIDTH-status_widths[_thermal.buff_1_st]-l_x, l_y-(status_heights[_thermal.buff_1_st]/2), status_logos[_thermal.buff_1_st], status_widths[_thermal.buff_1_st], status_heights[_thermal.buff_1_st], _menu.status_c[_thermal.buff_1_st]);
    st7735.setCursor(l_x, l_y+l_offset); st7735.println("Buffer [2] =>");
    st7735.drawBitmap(SCREEN_WIDTH-status_widths[_thermal.buff_2_st]-l_x, l_y-(status_heights[_thermal.buff_2_st]/2)+l_offset, status_logos[_thermal.buff_2_st], status_widths[_thermal.buff_2_st], status_heights[_thermal.buff_2_st], _menu.status_c[_thermal.buff_2_st]);
    st7735.setCursor(l_x, l_y+(2*l_offset)); st7735.println("Buffer [3] =>");
    st7735.drawBitmap(SCREEN_WIDTH-status_widths[_thermal.buff_3_st]-l_x, l_y-(status_heights[_thermal.buff_3_st]/2)+(2*l_offset), status_logos[_thermal.buff_3_st], status_widths[_thermal.buff_3_st], status_heights[_thermal.buff_3_st], _menu.status_c[_thermal.buff_3_st]);
    st7735.setCursor(l_x, l_y+(3*l_offset)); st7735.println("Buffer [4] =>");
    st7735.drawBitmap(SCREEN_WIDTH-status_widths[_thermal.buff_4_st]-l_x, l_y-(status_heights[_thermal.buff_4_st]/2)+(3*l_offset), status_logos[_thermal.buff_4_st], status_widths[_thermal.buff_4_st], status_heights[_thermal.buff_4_st], _menu.status_c[_thermal.buff_4_st]);
  }else if(command == "inspect"){
    adjustColor(255,255,0);
    st7735.fillScreen(_menu.highlight);
    int i_x = menu_option+1;
    st7735.drawBitmap(10, SCREEN_HEIGHT-interface_heights[i_x]-10, interface_logos[i_x], interface_widths[i_x], interface_heights[i_x], _menu.inspect_c);
    st7735.setTextSize(1);
    st7735.setTextColor(_menu.inspect_c);
    // Notify the user whether the required thermal scan buffers are registered or not.
    // If all of them registered successfully, generate and draw the preview thermal image from the passed buffers.
    int l_x = 20+interface_widths[i_x], l_y = SCREEN_HEIGHT-interface_heights[i_x]-10, l_offset = 10;
    if(_thermal.buff_1_st && _thermal.buff_2_st && _thermal.buff_3_st && _thermal.buff_4_st){
      st7735.setCursor(l_x, l_y); st7735.println("Press OK");
      st7735.setCursor(l_x, l_y+l_offset); st7735.println("to clear");
      st7735.setCursor(l_x, l_y+(2*l_offset)); st7735.println("thermal");
      st7735.setCursor(l_x, l_y+(3*l_offset)); st7735.println("image!");
      delay(500);
      // Obtain individual data points of each passed thermal buffer by converting them from strings to char arrays.
      const char *img_buff_points[] = {_thermal.buff_1.c_str(), _thermal.buff_2.c_str(), _thermal.buff_3.c_str(), _thermal.buff_4.c_str()};
      // Generate the preview thermal image [{16x12} x 4] by applying the specific color algorithm based on the defined temperature ranges.
      int p_w = 3, p_h = 4, img_x, img_x_s, img_y, img_y_s, p_num = 1, y_off = 10;
      int img_w = thermal_buff_width*p_w, img_h = thermal_buff_height*p_h;
      for(int t = 0; t < thermal_buff_num; t++){
        // Define buffer starting points.
        if(t==0) img_x = img_x_s = (SCREEN_WIDTH-(img_w*2))/2, img_y = img_y_s = y_off;
        if(t==1) img_x = img_x_s = (SCREEN_WIDTH/2), img_y = img_y_s = y_off;
        if(t==2) img_x = img_x_s = (SCREEN_WIDTH-(img_w*2))/2, img_y = img_y_s = y_off+img_h;
        if(t==3) img_x = img_x_s = (SCREEN_WIDTH/2), img_y = img_y_s = y_off+img_h;
        for(int i = 0; i < thermal_buff_size; i++){
          // Draw individual data points of each thermal buffer with the color indicator estimated by the given algorithm to generate a precise thermal image.
          switch(img_buff_points[t][i]){
            case 'w':
              st7735.fillRect(img_x, img_y, p_w, p_h, ST77XX_WHITE);
            break;
            case 'c':
              st7735.fillRect(img_x, img_y, p_w, p_h, ST77XX_CYAN);
            break;
            case 'b':
              st7735.fillRect(img_x, img_y, p_w, p_h, ST77XX_BLUE);
            break;
            case 'y':
              st7735.fillRect(img_x, img_y, p_w, p_h, ST77XX_YELLOW);
            break;
            case 'o':
              st7735.fillRect(img_x, img_y, p_w, p_h, st7735.color565(255, 165, 0));
            break;
            case 'r':
              st7735.fillRect(img_x, img_y, p_w, p_h, ST77XX_RED);
            break;
          }
          // Update the successive data point coordinates.
          img_x += p_w;
          if(p_num==thermal_buff_width){
            img_x = img_x_s;
            img_y += p_h;
            p_num=0;
          }
          p_num+=1;
        }
      }
    }else{
      st7735.setCursor(l_x, l_y); st7735.println("Please");
      st7735.setCursor(l_x, l_y+l_offset); st7735.println("register");
      st7735.setCursor(l_x, l_y+(2*l_offset)); st7735.println("all scan");
      st7735.setCursor(l_x, l_y+(3*l_offset)); st7735.println("buffers!");
      // If the registered buffers do not meet the requirements, show the blank thermal image — template.
      int p_w = 3, p_h = 4;
      int img_w = thermal_buff_width*p_w*2, img_h = thermal_buff_height*p_h*2, img_x = (SCREEN_WIDTH-img_w)/2, img_y = 10;
      st7735.fillRect(img_x, img_y, img_w, img_h, st7735.color565(144, 238, 144));
    }
  }
}

boolean is_MLX_Connected(){
  // Check whether the MLX90641 thermal imaging camera address is found on the I2C bus successfully.
  Wire.beginTransmission((uint8_t)MLX90641_address);
  if(Wire.endTransmission() != 0){
    return (false);
  }
  return (true);
}

String get_and_display_data_from_MLX90641(int buffer_size){
  String conv_buff;
  // Obtain the IR thermal imaging array (16x12 buffer) generated by the MLX90641 thermal imaging camera.
  for(byte x = 0 ; x < 2 ; x++){
    int status = MLX90641_GetFrameData(MLX90641_address, MLX90641Frame);
    // Get the required MLX90641 variables to calculate the thermal imaging buffer.
    float vdd = MLX90641_GetVdd(MLX90641Frame, &MLX90641);
    float Ta = MLX90641_GetTa(MLX90641Frame, &MLX90641);
    // Estimate the temperature reflection loss based on the sensor's ambient temperature.
    float tr = Ta - TA_SHIFT; 
    float emissivity = 0.95;
    // Generate the thermal imaging array (buffer).
    MLX90641_CalculateTo(MLX90641Frame, &MLX90641, emissivity, tr, MLX90641To);
  }
  // According to the declared temperature threshold ranges, define a specific algorithm to convert each data point of the given thermal buffer to color-based indicators.
  for(int i = 0 ; i < buffer_size ; i++){
    String _p;
    // Assess and assign a color-based indicator for the passed data point via the algorithm.
    if(MLX90641To[i] <= min_temp) _p = 'w';
    if(MLX90641To[i] > min_temp && MLX90641To[i] <= mod_temp_1) _p = 'c';
    if(MLX90641To[i] > mod_temp_1 && MLX90641To[i] <= mod_temp_2) _p = 'b';
    if(MLX90641To[i] > mod_temp_2 && MLX90641To[i] <= mod_temp_3) _p = 'y';
    if(MLX90641To[i] > mod_temp_3 && MLX90641To[i] <= max_temp) _p = 'o';
    if(MLX90641To[i] > max_temp) _p = 'r';
    // Append the evaluated indicator as a string item to register the given buffer as an array (string).
    conv_buff += _p;
  }
  // Return the generated array (string).
  return conv_buff;
}

void joystick_read(){
  j_b_read = digitalRead(JS_B);
  c_b_read = digitalRead(C_B);
  j_x_read = analogRead(JS_X);
  j_y_read = analogRead(JS_Y);
}

void adjustColor(int r, int g, int b){
  analogWrite(red_pin, (255-r));
  analogWrite(green_pin, (255-g));
  digitalWrite(blue_pin, 1-b);
}

// Define the Particle Cloud functions.
int collect_thermal_buffers(String num){
  // If requested by the user, generate thermal scan (imaging) buffers
  // and assign the collected buffers to the associated arrays (strings) according to the passed buffer number (from 1 to 4). 
  if(num == "1"){
    _thermal.buff_1 = get_and_display_data_from_MLX90641(thermal_buff_size); _thermal.buff_1_st = true;
    buff_num = 1;
    adjustColor(0,255,0);
    delay(1000);
    adjustColor(0,0,0);
    buff_update = true;
    return buff_num;
  }else if(num == "2"){
    _thermal.buff_2 = get_and_display_data_from_MLX90641(thermal_buff_size); _thermal.buff_2_st = true;
    buff_num = 2;
    adjustColor(0,255,0);
    delay(1000);
    adjustColor(0,0,0);
    buff_update = true;
    return buff_num;
  }else if(num == "3"){
    _thermal.buff_3 = get_and_display_data_from_MLX90641(thermal_buff_size); _thermal.buff_3_st = true;
    buff_num = 3;
    adjustColor(0,255,0);
    delay(1000);
    adjustColor(0,0,0);
    buff_update = true;
    return buff_num;
  }else if(num == "4"){
    _thermal.buff_4 = get_and_display_data_from_MLX90641(thermal_buff_size); _thermal.buff_4_st = true;
    buff_num = 4;
    adjustColor(0,255,0);
    delay(1000);
    adjustColor(0,0,0);
    buff_update = true;
    return buff_num;
  }else if(num == "clear"){
    // If requested, clear all thermal scan buffers.
    _thermal.buff_1 = _thermal.buff_2 = _thermal.buff_3 = _thermal.buff_4 = "empty";
    _thermal.buff_1_st = _thermal.buff_2_st = _thermal.buff_3_st = _thermal.buff_4_st = false;
    buff_num = 0;
    adjustColor(0,0,1);
    delay(1000);
    adjustColor(0,0,0);
    buff_update = true;
    return buff_num;
  }else{
    adjustColor(255,0,0);
    delay(1000);
    adjustColor(0,0,0);
    return -1;
  }
}