# AI-driven Sound & Thermal Image-based HVAC Fault Diagnosis
#
# LattePanda Mu (x86 Compute Module)
#
# By Kutluhan Aktar
#
# Identify the faulty components via anomalous sound detection
# and diagnose ensuing cooling malfunctions via thermal visual anomaly detection.
# 
#
# For more information:
# https://www.hackster.io/kutluhan-aktar


import cv2
import numpy
from edge_impulse_linux.image import ImageImpulseRunner
import argparse
import os
import datetime
from time import sleep

class thermal_img():
    def __init__(self, model_file):
        # Get the absolute folder path to avoid errors while running this script via the web dashboard (application).
        self.path = str(os.path.dirname(os.path.realpath(__file__)))
        # Define the required configurations to run the Edge Impulse FOMO-AD (visual anomaly detection) model.
        self.model_file = os.path.join(self.path, model_file).replace("/generate_thermal_img", "")
        self.threshold = 5
        self.detected_class = ""
        self.__debug = False
        # Define the required variables to generate a thermal image from the given thermal scan (imaging) buffers.
        self.t_img = {"w": 192, "h": 192, "p_w": 6, "p_h": 8, "temp_img": self.path+"/thermal_template.jpg"}
        self.thermal_buff_width = 16
        self.thermal_buff_height = 12
    
    def run_inference(self, process):
        # Run inference to identify HVAC cooling malfunctions based on the generated thermal images via visual anomaly detection.
        with ImageImpulseRunner(self.model_file) as runner:
            try:
                resulting_image = ""
                # Print the information of the Edge Impulse FOMO-AD model converted to a Linux (x86_64) application (.eim).
                model_info = runner.init()
                if(self.__debug): print('\nLoaded runner for "' + model_info['project']['owner'] + ' / ' + model_info['project']['name'] + '"')
                labels = model_info['model_parameters']['labels']
                # Get the latest registered thermal image (frame) generated from the passed thermal imaging buffers.
                latest_img = self.generated_thermal_image
                # After obtaining the latest image, resize (if necessary) and generate features from the retrieved frame depending on the provided model so as to run an inference.
                features, cropped = runner.get_features_from_image(latest_img)
                res = runner.classify(features)
                # Since the Edge Impulse FOMO-AD (visual anomaly detection) model categorizes given image samples by individual cells (grids)
                # based on the dichotomy between two predefined classes (anomaly and no anomaly), utilize the mean visual anomaly value to detect overall (high-risk) thermal cooling malfunctions.
                if res["result"]["visual_anomaly_mean"] >= self.threshold:
                    # If the given thermal image sample indicates a thermal cooling malfunction:
                    self.detected_class = "malfunction"
                    # Obtain the cells with their assigned labels and anomaly scores evaluated by the FOMO-AD (visual anomaly detection) model.
                    intensity = ""
                    c_offset = 5
                    for cell in res["result"]["visual_anomaly_grid"]:
                        # Draw each cell assigned with an anomaly score greater than the given model threshold on the resulting image.
                        if cell["label"] == "anomaly" and cell["value"] >= self.threshold:
                            cv2.rectangle(cropped, (cell["x"], cell["y"]), (cell["x"]+cell["width"], cell["y"]+cell["height"]), (0,255,0), 2)
                            # According to the given threshold, calculate the anomaly intensity level — Low (L), Moderate (M), High (H) — for each individual cell provided by the FOMO-AD model.
                            if(cell["value"] >= self.threshold and cell["value"] < self.threshold+c_offset):
                                intensity = "L"
                            elif(cell["value"] >= self.threshold+c_offset and cell["value"] < self.threshold+(2*c_offset)):
                                intensity = "M"
                            elif(cell["value"] >= self.threshold+(2*c_offset)):
                                intensity = "H"
                            # Then, draw the estimated anomaly intensity level to the top-left corner of the passed cell.
                            cv2.putText(cropped, intensity, (cell["x"]+2, cell["y"]+10), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (0,255,0), 1, cv2.LINE_AA)
                else:
                    # If the given thermal image sample indicates a stable cooling process:
                    self.detected_class = "normal"
                # Save the generated model resulting image modified with the passed cells and their evaluated anomaly intensity levels (if applicable) to the img_detection folder on the web dashboard.
                if self.detected_class != "":
                    if(self.__debug): print("\nFOMO-AD Model Detection Result => " + self.detected_class + "\n")
                    self.generated_thermal_image = cropped
                    self.save_thermal_img(self.detected_class, process)
            # Stop the running inference.    
            finally:
                if(runner):
                    runner.stop()
    
    def generate_thermal_img(self, thermal_buff):
        # Get the template (blank) thermal image (192 x 192).
        template = cv2.imread(self.t_img["temp_img"])
        # Generate the thermal image from the given buffers ((16x12) x 4).
        p_num = 1
        for t in range(len(thermal_buff)):
            # Define buffer starting points.
            if(t==0): img_x, img_x_s, img_y, img_y_s = 0, 0, 0, 0
            if(t==1): img_x, img_x_s, img_y, img_y_s = int(self.t_img["w"]/2), int(self.t_img["w"]/2), 0, 0
            if(t==2): img_x, img_x_s, img_y, img_y_s = 0, 0, int(self.t_img["h"]/2), int(self.t_img["h"]/2)
            if(t==3): img_x, img_x_s, img_y, img_y_s = int(self.t_img["w"]/2), int(self.t_img["w"]/2), int(self.t_img["h"]/2), int(self.t_img["h"]/2)
            for p in thermal_buff[t]:
                # Draw individual data points of each thermal buffer with the color indicator estimated by the specific color algorithm based on the defined temperature ranges to generate a precise thermal image.
                # Note: Indicators are defined in the BGR format.
                match p:
                    case 'w':
                        cv2.rectangle(template, (img_x,img_y), (img_x+self.t_img["p_w"],img_y+self.t_img["p_h"]), (255,255,255), -1)
                    case 'c':
                        cv2.rectangle(template, (img_x,img_y), (img_x+self.t_img["p_w"],img_y+self.t_img["p_h"]), (255,255,0), -1)
                    case 'b':
                        cv2.rectangle(template, (img_x,img_y), (img_x+self.t_img["p_w"],img_y+self.t_img["p_h"]), (255,0,0), -1)
                    case 'y':
                        cv2.rectangle(template, (img_x,img_y), (img_x+self.t_img["p_w"],img_y+self.t_img["p_h"]), (0,255,255), -1)
                    case 'o':
                        cv2.rectangle(template, (img_x,img_y), (img_x+self.t_img["p_w"],img_y+self.t_img["p_h"]), (0,165,255), -1)
                    case 'r':
                        cv2.rectangle(template, (img_x,img_y), (img_x+self.t_img["p_w"],img_y+self.t_img["p_h"]), (0,0,255), -1)
                # Update the successive data point coordinates.
                img_x += self.t_img["p_w"]
                if(p_num==self.thermal_buff_width):
                    img_x = img_x_s
                    img_y += self.t_img["p_h"]
                    p_num = 0
                p_num += 1
        # After generating the thermal image, register the modified frame before saving an image file.
        self.generated_thermal_image = template
        
    def save_thermal_img(self, img_tag, _type):
        # Depending on the passed process type (sample or detection), save the produced (registered) frame to the img_sample or img_detection folder by adding the creation date to the file name.
        folder = "img_sample" if _type=="sample" else "img_detection"
        date = datetime.datetime.now().strftime("%Y_%m_%d_%H_%M_%S")
        file_name = "{}/{}/{}__{}.jpg".format(self.path, folder, img_tag, date)
        cv2.imwrite(file_name, self.generated_thermal_image)
        print(img_tag+":"+date)

# Define the thermal_img object.
thermal_img = thermal_img("model/ai-driven-hvac-fault-diagnosis-(thermal)-linux-x86_64-v1.eim")

# Obtain and decode thermal scan (imaging) buffers and the process type transferred by the web dashboard via Python Arguments.
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--buff_1", required=True, help="thermal image buffer (16x12)")
    parser.add_argument("--buff_2", required=True, help="thermal image buffer (16x12)")
    parser.add_argument("--buff_3", required=True, help="thermal image buffer (16x12)")
    parser.add_argument("--buff_4", required=True, help="thermal image buffer (16x12)")
    parser.add_argument("--process", required=True, help="1) sample=only generate thermal image to collect data 2) detection=generate thermal image and run an inference")
    args = parser.parse_args()
    buff_1 = args.buff_1
    buff_2 = args.buff_2
    buff_3 = args.buff_3
    buff_4 = args.buff_4
    process = args.process
    # After obtaining the required parameters via Python Arguments, generate a thermal image from the given thermal imaging buffers.
    thermal_img.generate_thermal_img([buff_1, buff_2, buff_3, buff_4])
    # Depending on the passed process type (sample or detection), run an inference with the Edge Impulse FOMO-AD (visual anomaly detection) model
    # to diagnose cooling malfunctions or save the produced thermal image directly as a sample.
    if(process=="detection"):
        thermal_img.run_inference(process)
    elif(process=="sample"):
        thermal_img.save_thermal_img(process, process)
