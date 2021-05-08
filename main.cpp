#include "mbed.h"

#include "mbed_rpc.h"

#include "uLCD_4DGL.h"

////////
#include "accelerometer_handler.h"

#include "config.h"

#include "magic_wand_model_data.h"

#include "tensorflow/lite/c/common.h"

#include "tensorflow/lite/micro/kernels/micro_ops.h"

#include "tensorflow/lite/micro/micro_error_reporter.h"

#include "tensorflow/lite/micro/micro_interpreter.h"

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

#include "tensorflow/lite/schema/schema_generated.h"

#include "tensorflow/lite/version.h"

#include "stm32l475e_iot01_accelero.h"

#include <math.h>
//////
uLCD_4DGL uLCD(D1, D0, D2);

int N = 256;
float f[8] = {20, 25, 30, 35, 40, 45, 50, 55}; //list of threshold angles
int f_cur = 3;
int f_idx = 3;
Thread thread;
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalIn mypin(USER_BUTTON);
///////////////////////////////////
void tilt(Arguments *in, Reply *out);
void gestureUIMode(Arguments *in, Reply *out);
RPCFunction rpcTilt(&tilt, "tilt");
RPCFunction rpcGesture(&gestureUIMode, "gestureUIMode");
BufferedSerial pc(USBTX, USBRX);




void display_list()
{
    uLCD.locate(0, 0);
    for (int i = 0; i < 8; i++)
    {
        uLCD.color(GREEN);
        if (i == f_cur)
            uLCD.color(BLUE);
        if (i == f_idx)
            uLCD.color(RED);
        uLCD.printf("%.1f   \n", f[i]);
    }
}


void tilt(Arguments *in, Reply *out)
{
    int16_t pDataXYZ[3] = {0};
    float ang ;
    char buffer[200];

   //for initial reference vector
   BSP_ACCELERO_Init();

    while(1)
    {
        led1 = !led1;
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        ang = atan(float(pDataXYZ[0])/float(pDataXYZ[2]))/3.14159*180;
        printf("accelerometer data: %d, %d, %d \n", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
        printf(" angle is %3f\n" ,ang);
        //display_angle();
        uLCD.locate(0, 0);
        uLCD.color(BLUE);
        uLCD.printf("%.2f\n", ang);
        ThisThread::sleep_for(1000ms);
    }
    sprintf(buffer, "Accelerometer values: (%d, %d, %d)", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
   out->putData(buffer);
}

// Create an area of memory to use for input, output, and intermediate arrays.

// The size of this will depend on the model you're using, and may need to be

// determined by experimentation.

constexpr int kTensorArenaSize = 60 * 1024;

uint8_t tensor_arena[kTensorArenaSize];

// Return the result of the last prediction

int PredictGesture(float *output)
{

    // How many times the most recent gesture has been matched in a row

    static int continuous_count = 0;

    // The result of the last prediction

    static int last_predict = -1;

    // Find whichever output has a probability > 0.8 (they sum to 1)

    int this_predict = -1;

    for (int i = 0; i < label_num; i++)
    {

        if (output[i] > 0.8)
            this_predict = i;
    }

    // No gesture was detected above the threshold

    if (this_predict == -1)
    {

        continuous_count = 0;

        last_predict = label_num;

        return label_num;
    }

    if (last_predict == this_predict)
    {

        continuous_count += 1;
    }
    else
    {

        continuous_count = 0;
    }

    last_predict = this_predict;

    // If we haven't yet had enough consecutive matches for this gesture,

    // report a negative result

    if (continuous_count < config.consecutiveInferenceThresholds[this_predict])
    {

        return label_num;
    }

    // Otherwise, we've seen a positive result, so clear all our variables

    // and report it

    continuous_count = 0;

    last_predict = -1;

    return this_predict;
}
///


float gesture(TfLiteTensor *model_input, tflite::ErrorReporter *error_reporter, tflite::MicroInterpreter *interpreter)
{
    float result = 0;
    bool should_clear_buffer = false;
    bool got_data = false;
    int input_length = model_input->bytes / sizeof(float);
    int record_flag = 0;
    int gesture_index;
    while (1)
    {
        // Attempt to read new data from the accelerometer

        got_data = ReadAccelerometer(error_reporter, model_input->data.f,

                                     input_length, should_clear_buffer);

        // If there was no new data,

        // don't try to clear the buffer again and wait until next time

        if (!got_data)
        {
            should_clear_buffer = false;
            continue;
        }

        // Run inference, and report any error
        TfLiteStatus invoke_status = interpreter->Invoke();
        if (invoke_status != kTfLiteOk)
        {
            error_reporter->Report("Invoke failed on index: %d\n", begin_index);
            continue;
        }

        // Analyze the results to obtain a prediction
        gesture_index = PredictGesture(interpreter->output(0)->data.f);
        // Clear the buffer next time we read data
        should_clear_buffer = gesture_index < label_num;
        if (gesture_index < label_num)
        {

            error_reporter->Report(config.output_message[gesture_index]);
            printf("Current index %d\n", gesture_index);
            // check button
            if (gesture_index == 0) // up
            {
                f_cur = f_cur + 1;
                if (f_cur > 7)
                    f_cur = 7;
                display_list();
            }
            if (gesture_index == 1) // down
            {
                f_cur = f_cur - 1;
                if (f_cur < 0)
                    f_cur = 0;
                display_list();
                // ThisThread::sleep_for(10ms);
            }
            if (gesture_index == 2) // check
            {
                f_idx = f_cur;
                // freq = f[f_idx];
                // printf("Current Frequence : %.1f \n\r", freq);
                display_list();
                record_flag = 1;
                result = f[f_cur];
                // ThisThread::sleep_for(1ms);
                break;
            }
        }

        ThisThread::sleep_for(100ms);
    }
    return result;
}
void gestureUIMode(Arguments *in, Reply *out)
{
    char buffer[200];

      // Set up logging.

    static tflite::MicroErrorReporter micro_error_reporter;

    tflite::ErrorReporter *error_reporter = &micro_error_reporter;

    // Map the model into a usable data structure. This doesn't involve any

    // copying or parsing, it's a very lightweight operation.

    const tflite::Model *model = tflite::GetModel(g_magic_wand_model_data);

    if (model->version() != TFLITE_SCHEMA_VERSION)
    {

        error_reporter->Report(

            "Model provided is schema version %d not equal "

            "to supported version %d.",

            model->version(), TFLITE_SCHEMA_VERSION);

        return ;
    }

    // Pull in only the operation implementations we need.

    // This relies on a complete list of all the ops needed by this graph.

    // An easier approach is to just use the AllOpsResolver, but this will

    // incur some penalty in code space for op implementations that are not

    // needed by this graph.

    static tflite::MicroOpResolver<6> micro_op_resolver;

    micro_op_resolver.AddBuiltin(

        tflite::BuiltinOperator_DEPTHWISE_CONV_2D,

        tflite::ops::micro::Register_DEPTHWISE_CONV_2D());

    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,

                                 tflite::ops::micro::Register_MAX_POOL_2D());

    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,

                                 tflite::ops::micro::Register_CONV_2D());

    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,

                                 tflite::ops::micro::Register_FULLY_CONNECTED());

    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,

                                 tflite::ops::micro::Register_SOFTMAX());

    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,

                                 tflite::ops::micro::Register_RESHAPE(), 1);

    // Build an interpreter to run the model with

    static tflite::MicroInterpreter static_interpreter(

        model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);

    tflite::MicroInterpreter *interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors

    interpreter->AllocateTensors();

    // Obtain pointer to the model's input tensor

    TfLiteTensor *model_input = interpreter->input(0);

    if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||

        (model_input->dims->data[1] != config.seq_length) ||

        (model_input->dims->data[2] != kChannelNumber) ||

        (model_input->type != kTfLiteFloat32))
    {

        error_reporter->Report("Bad input tensor parameters in model");

        return ;
    }

    TfLiteStatus setup_status = SetupAccelerometer(error_reporter);

    if (setup_status != kTfLiteOk)
    {

        error_reporter->Report("Set up failed\n");

        return ;
    }

    error_reporter->Report("Set up successful...\n");

    float angle = gesture(model_input, error_reporter, interpreter);
    sprintf(buffer, "selected angle: %f \n", angle);
    out->putData(buffer);

}

int main(void)
{
 
    uLCD.locate(0, 0);
    uLCD.text_width(2); //4X size text
    uLCD.text_height(2);

    //////////////////////////////////////
    char buf[256], outbuf[256];

    FILE *devin = fdopen(&pc, "r");
    FILE *devout = fdopen(&pc, "w");
    while(1) {
        memset(buf, 0, 256);
        for (int i = 0; ; i++) {
            char recv = fgetc(devin);
            if (recv == '\n') {
                printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
        //Call the static call method on the RPC class
        RPC::call(buf, outbuf);
        printf("%s\r\n", outbuf);
    }
    ///////////////////////////////////////
    
    
  

    //display_list();
    // Main loop
    // while (1)
    // {
    //     //receive rpc code
    //     // if (1) gesture
    //     // else tilt
    // }
    //thread.start(tilt);
    
}