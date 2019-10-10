#include <metal/stream.h>

//#define LIMIT(LO,VAL,HI) ( ((VAL)>(LO))? (((VAL)<(HI))?(VAL):(HI)) : (LO) )

#define VALUE_BYTES 8
#define VALUE_BITS (VALUE_BYTES*8)
#define VALUE_COUNT (STREAM_BYTES/VALUE_BYTES)
//#define VUP(IDX) LIMIT(0, (VALUE_BITS * (IDX) + VALUE_BITS - 1), (STREAM_BYTES*8))
//#define VLO(IDX) LIMIT(0, (VALUE_BITS * (IDX)), (STREAM_BYTES*8))
#define VUP(IDX) (VALUE_BITS * (IDX) + VALUE_BITS - 1)
#define VLO(IDX) (VALUE_BITS * (IDX))

typedef ap_uint<6> ValueCount; //Assume up to 64 Values (accomodating DoubleRow, this supports up to 32 * 64 = 2048bit streams)
typedef ap_uint<VALUE_BITS> Value;
typedef ap_uint<VALUE_BITS*VALUE_COUNT> Row;
typedef ap_uint<VALUE_BITS*VALUE_COUNT*2> DoubleRow;
typedef ap_uint<VALUE_COUNT> Mask;

void hls_operator_aggregate(mtl_stream &in, mtl_stream &out) {
#pragma HLS INTERFACE axis port=in name=axis_input
#pragma HLS INTERFACE axis port=out name=axis_output
#pragma HLS INTERFACE s_axilite port=return bundle=control

  mtl_stream_element input;
  mtl_stream_element output;

  Value count = 0;
  Value sum = 0;
  Value min = 0xffffffffffffffffull;
  Value max = 0;
  do {
#pragma HLS PIPELINE
    input = in.read();

    for (int i = 0; i < VALUE_COUNT; ++i) {
#pragma HLS UNROLL
      Value value = input.data(VUP(i), VLO(i));
      count += 1;
      sum += value;
      if (value < min) {
        min = value;
      }
      if (value > max) {
        max = value;
      }
    }

  } while (!input.last);

  output.data = count;
  output.keep = (mtl_stream_keep)-1;
  output.last = 0;
  out.write(output);

  output.data = sum;
  output.keep = (mtl_stream_keep)-1;
  output.last = 0;
  out.write(output);

  output.data = min;
  output.keep = (mtl_stream_keep)-1;
  output.last = 0;
  out.write(output);

  output.data = max;
  output.keep = (mtl_stream_keep)-1;
  output.last = 1;
  out.write(output);

}

#include "testbench.cpp"
