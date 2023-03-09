# BioEye assignment

## Overview
In this application, frames are processed one-by-one from a web camera.  
The video capture and image processing works on different threads.  
Using a shared resource (```TSQueue```) to control the queue and prevent consumer-producer problem.
<br>   
The frame processing pipeline includes:
1. **Image manipulation** - convert to greyscale and resize.
2. **Face detection** - by dlib HOG model.
3. **Facial landmarks detection** - by dlib 5 facial landmarks model.
4. **Compute the bounding rectangle** alinged to each eye.
5. **Rotate** each eye rectangle to be horizontal.
6. **Crop** each eye rectangle to image.

The application saves a CSV file containing the following data for each frame:
- **frame-number** - Unique identifier for each frame 
- **start_time** - Starting time of processing
- **processing_duration** - Total time took for processing
- **face_detected** - 1 - face detected, 0 - face not detected


In addition to the CSV, the application saves both eyes every 30 frames in which face was detected.  
The image name format is ``` {left/right}_{frame_number}.png```.

The CSV file is saved in ```.\data``` dir.  
The images are saved in ```.\data\img``` dir.  


## Requirements
- C++ 20
- dlib library
- OpenCV library

In this solution both additional libraries installed in ```"C:\lib\"```  .

