#define INPUT_LENGTH 30
#define ARRAY_SIZE 90

#include <Arduino_LSM9DS1.h>

#include <TensorFlowLite.h>

#include  "accel_model.h"

#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
namespace{
  
  float dataAcc[ARRAY_SIZE];

  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* model_input = nullptr;


  constexpr int kTensorArenaSize = 20 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];
  float* model_input_buffer = nullptr; 
  float mean[3] = {0,0,0};
 }
void setup() {
  // put your setup code here, to run once:
  //accelerometer setup
  Serial.begin(9600);
  IMU.begin();
  
  
  

  //setting up the model
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  model = tflite::GetModel(g_activity_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }
  tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter static_interpreter(model, resolver,tensor_arena,kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;
 
  
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    

    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }
   model_input = interpreter->input(0);
  model_input_buffer = model_input->data.f;
 

}

void loop() {
  // put your main code here, to run repeatedly:
  float x,y,z;
  int counter = INPUT_LENGTH;
  mean[0] = 0;
  mean[1] = 0;
  mean[2] = 0;
  //reading INPUT_LENGTH number of samples at a time
  int counter_dash = 2*counter;
  while(counter_dash){
    if(IMU.accelerationAvailable()){
      
      IMU.readAcceleration(x,y,z);
      if(counter_dash%2 == 0){
      dataAcc[3*(INPUT_LENGTH - counter) ] = x;
      dataAcc[3*(INPUT_LENGTH - counter) + 1 ] = y;
      dataAcc[3*(INPUT_LENGTH - counter) + 2 ] = z;
      mean[0]+=x/INPUT_LENGTH;
      mean[1]+=y/INPUT_LENGTH;
      mean[2]+=z/INPUT_LENGTH;
      counter--;
      }
      counter_dash--;
    }
  }
  
  for(int i = 0;i<ARRAY_SIZE;i++){
    dataAcc[i] = dataAcc[i] - mean[i%3];
  }
  for(int i = 0; i<INPUT_LENGTH;i++){
    model_input_buffer[i] = dataAcc[3*i]*dataAcc[3*i] + dataAcc[3*i + 1]*dataAcc[3*i + 1] + dataAcc[3*i + 2]*dataAcc[3*i + 2];
  }
  
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
    return;
  }
  TfLiteTensor* output = interpreter->output(0);
  
  Serial.print("Stand:");
  Serial.print(output->data.f[0]);
  Serial.print(' ');
  Serial.print("walk:");
  Serial.print(output->data.f[1]);
  Serial.print(' ');
  Serial.print("jog:");
  Serial.println(output->data.f[2]);

  

  
  
  
  
  
}
