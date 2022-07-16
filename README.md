#Activity Classification 
This project aims to classify the current activity of the person using accelerometer data only. 
The board used in the project is an Arduino Nano BLE Sense board which has an in-built accelerometer sensor which has a sampling frequency of 119 Hz and can detect accelerations in all three directions. The model was trained on Google colab using data collected from a Smart-phone which has a sample rate of 3.2 kHz. The data was preprocessed to make the data around 80-90 Hz.

Currently, the model can predict 3 activities:
1. Standing
2. Walking
3. Running

The model was trained on roughly 24,000 samples and had an accuracy of 95.98% and an accuracy of 89.5% on unseen data which had roughly 1 million samples from the given link: . 

The model accuracies for each of the activities are as follows:
1. Standing - 99.98%
2. Walking - 71.16%
3. Jogging - 86.49%

The model is currently not very accurate in predicting walking and will require further tuning of the hyperparamters.

The model was converted using tflite and optimized to support 8 bit operations which are what most microcontrollers support. The accuracy for this was 95.55 %; a small dip due to quantization of values.

The preprocessing of data included removing the effect of gravity from the sensor data by subtracting from the mean value and calculating the mod squared of the acceleration.

Future changes that I will try to bring to the project are:
1. Re-training to get better results for walking detection
2. Replacement of the list for feeding data with queue which may improve the model behaviour in microcontroller
3. Making an appropriate command responder for the model output values to a web server using the in-built Bluetooth module
4. Make a steps detection model to count number of steps taken when walking/jogging.