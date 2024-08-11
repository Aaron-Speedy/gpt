#include <stdio.h>
#include <stdint.h>

#define MATH_IMPL
#include "math.h"

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

char *read_file(const char *path) {
  FILE *fp = fopen(path, "rb");
  if (fp == NULL) {
    fprintf(stderr, "Error: %s does not exist!\n", path);
    exit(1);
  }
  fseek(fp, 0, SEEK_END);

  size_t len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *str = malloc(sizeof(char) * len);
  fread(str, len, 1, fp);

  fclose(fp);

  return str;
}

#define CL_ERR_TO_STR(err) case err: return #err
const char *cl_get_err_str(cl_int err) {
  switch (err) {
  CL_ERR_TO_STR(CL_SUCCESS);
  CL_ERR_TO_STR(CL_DEVICE_NOT_FOUND);
  CL_ERR_TO_STR(CL_DEVICE_NOT_AVAILABLE);
  CL_ERR_TO_STR(CL_COMPILER_NOT_AVAILABLE);
  CL_ERR_TO_STR(CL_MEM_OBJECT_ALLOCATION_FAILURE);
  CL_ERR_TO_STR(CL_OUT_OF_RESOURCES);
  CL_ERR_TO_STR(CL_OUT_OF_HOST_MEMORY);
  CL_ERR_TO_STR(CL_PROFILING_INFO_NOT_AVAILABLE);
  CL_ERR_TO_STR(CL_MEM_COPY_OVERLAP);
  CL_ERR_TO_STR(CL_IMAGE_FORMAT_MISMATCH);
  CL_ERR_TO_STR(CL_IMAGE_FORMAT_NOT_SUPPORTED);
  CL_ERR_TO_STR(CL_BUILD_PROGRAM_FAILURE);
  CL_ERR_TO_STR(CL_MAP_FAILURE);
  CL_ERR_TO_STR(CL_MISALIGNED_SUB_BUFFER_OFFSET);
  CL_ERR_TO_STR(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
  CL_ERR_TO_STR(CL_COMPILE_PROGRAM_FAILURE);
  CL_ERR_TO_STR(CL_LINKER_NOT_AVAILABLE);
  CL_ERR_TO_STR(CL_LINK_PROGRAM_FAILURE);
  CL_ERR_TO_STR(CL_DEVICE_PARTITION_FAILED);
  CL_ERR_TO_STR(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
  CL_ERR_TO_STR(CL_INVALID_VALUE);
  CL_ERR_TO_STR(CL_INVALID_DEVICE_TYPE);
  CL_ERR_TO_STR(CL_INVALID_PLATFORM);
  CL_ERR_TO_STR(CL_INVALID_DEVICE);
  CL_ERR_TO_STR(CL_INVALID_CONTEXT);
  CL_ERR_TO_STR(CL_INVALID_QUEUE_PROPERTIES);
  CL_ERR_TO_STR(CL_INVALID_COMMAND_QUEUE);
  CL_ERR_TO_STR(CL_INVALID_HOST_PTR);
  CL_ERR_TO_STR(CL_INVALID_MEM_OBJECT);
  CL_ERR_TO_STR(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
  CL_ERR_TO_STR(CL_INVALID_IMAGE_SIZE);
  CL_ERR_TO_STR(CL_INVALID_SAMPLER);
  CL_ERR_TO_STR(CL_INVALID_BINARY);
  CL_ERR_TO_STR(CL_INVALID_BUILD_OPTIONS);
  CL_ERR_TO_STR(CL_INVALID_PROGRAM);
  CL_ERR_TO_STR(CL_INVALID_PROGRAM_EXECUTABLE);
  CL_ERR_TO_STR(CL_INVALID_KERNEL_NAME);
  CL_ERR_TO_STR(CL_INVALID_KERNEL_DEFINITION);
  CL_ERR_TO_STR(CL_INVALID_KERNEL);
  CL_ERR_TO_STR(CL_INVALID_ARG_INDEX);
  CL_ERR_TO_STR(CL_INVALID_ARG_VALUE);
  CL_ERR_TO_STR(CL_INVALID_ARG_SIZE);
  CL_ERR_TO_STR(CL_INVALID_KERNEL_ARGS);
  CL_ERR_TO_STR(CL_INVALID_WORK_DIMENSION);
  CL_ERR_TO_STR(CL_INVALID_WORK_GROUP_SIZE);
  CL_ERR_TO_STR(CL_INVALID_WORK_ITEM_SIZE);
  CL_ERR_TO_STR(CL_INVALID_GLOBAL_OFFSET);
  CL_ERR_TO_STR(CL_INVALID_EVENT_WAIT_LIST);
  CL_ERR_TO_STR(CL_INVALID_EVENT);
  CL_ERR_TO_STR(CL_INVALID_OPERATION);
  CL_ERR_TO_STR(CL_INVALID_GL_OBJECT);
  CL_ERR_TO_STR(CL_INVALID_BUFFER_SIZE);
  CL_ERR_TO_STR(CL_INVALID_MIP_LEVEL);
  CL_ERR_TO_STR(CL_INVALID_GLOBAL_WORK_SIZE);
  CL_ERR_TO_STR(CL_INVALID_PROPERTY);
  CL_ERR_TO_STR(CL_INVALID_IMAGE_DESCRIPTOR);
  CL_ERR_TO_STR(CL_INVALID_COMPILER_OPTIONS);
  CL_ERR_TO_STR(CL_INVALID_LINKER_OPTIONS);
  CL_ERR_TO_STR(CL_INVALID_DEVICE_PARTITION_COUNT);
  CL_ERR_TO_STR(CL_INVALID_PIPE_SIZE);
  CL_ERR_TO_STR(CL_INVALID_DEVICE_QUEUE);

  default:
    return "UNKNOWN ERROR CODE";
  }
}

void clerr(cl_int err, const char *str) {
  if (err != CL_SUCCESS) {
    fprintf(stderr, "Error: %s: %s!\n", str, cl_get_err_str(err));
    exit(1);
  }
}

typedef struct {
  cl_device_id id;
  cl_context ctx;
  cl_command_queue queue;
} CL_Device;

typedef struct {
  cl_program program;
  cl_kernel kernel;
} CL_Program;

CL_Device cl_init_device(cl_device_type device_type) {
  CL_Device dev = {0};
  cl_int err;

  err = clGetDeviceIDs(NULL, device_type, 1, &dev.id, NULL);
    clerr(err, "Failed to get GPU for OpenCL");
  dev.ctx = clCreateContext(NULL, 1, &dev.id, NULL, NULL, &err);
    clerr(err, "Failed to create OpenCL context");
  dev.queue = clCreateCommandQueueWithProperties(dev.ctx, dev.id, 0, &err);
    clerr(err, "Failed to create OpenCL command queue");

  return dev;
}

CL_Program cl_init_program(CL_Device *dev, const char *code) {
  CL_Program program;
  const char *codes[] = { code };
  cl_int err;

  program.program = clCreateProgramWithSource(dev->ctx, 1, codes, NULL, &err);
    clerr(err, "Failed to create OpenCL program");
  err = clBuildProgram(program.program, 0, NULL, NULL, NULL, NULL);
    clerr(err, "Failed to build OpenCL program");

  return program;
}

int main() {
  cl_int err;
  CL_Device dev = cl_init_device(CL_DEVICE_TYPE_GPU);
  CL_Program program = cl_init_program(&dev, read_file("kernel.cl"));

  const int K = 32, M = 32, N = 32;
  Matrix a = {.w = K, .h = M};
  Matrix b = {.w = N, .h = K};
  Matrix c = {.w = M, .h = N};

  cl_mem buf_in = clCreateBuffer(dev.ctx, CL_MEM_READ_ONLY, sizeof(in), NULL, &err);
    clerr(err, "Failed to create OpenCL input buffer");
  cl_mem buf_out = clCreateBuffer(dev.ctx, CL_MEM_WRITE_ONLY, sizeof(out), NULL, &err);
    clerr(err, "Failed to create OpenCL output buffer");

  err = clEnqueueWriteBuffer(
    dev.queue, buf_in, CL_TRUE, 0,
    sizeof(in), in, 
    0, NULL, NULL
  );
  clerr(err, "Failed to enqueue OpenCL output buffer");

  cl_kernel kernel = clCreateKernel(program.program, "root", NULL);
  err = clSetKernelArg(kernel, 0, sizeof(buf_in), &buf_in);
  err |= clSetKernelArg(kernel, 1, sizeof(buf_out), &buf_out);
  clerr(err, "Failed to set OpenCL kernel arguments");

  clEnqueueNDRangeKernel(dev.queue, kernel, 1, NULL, &in_len, NULL, 0, NULL, NULL);
  clFinish(dev.queue);

  clEnqueueReadBuffer(dev.queue, buf_out, CL_TRUE, 0, sizeof(out), out, 0, NULL, NULL);

  printf("{ ");
  for (int i = 0; i < arrlen(out); i++) {
    printf("%f, ", out[i]);
  }
  printf("}\n");

  return 0;
}
