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

typedef ap_uint<VALUE_BITS*4> Stats;

Stats aggregate_row(Row row) {
  Stats stats;
  Value local_cnt[VALUE_COUNT+1];
  Value local_sum[VALUE_COUNT+1];
  Value local_min[VALUE_COUNT+1];
  Value local_max[VALUE_COUNT+1];
  local_cnt[0] = 0;
  local_sum[0] = 0;
  local_min[0] = 0xffffffffffffffffull;
  local_max[0] = 0;

#pragma HLS PIPELINE
  for (int i = 0; i < VALUE_COUNT; ++i) {
#pragma HLS UNROLL
    Value value = row(VUP(i), VLO(i));
    local_cnt[i+1] = local_cnt[i] + 1;
    local_sum[i+1] = local_sum[i] + value;
    local_min[i+1] = (value < local_min[i]) ? value : local_min[i];
    local_max[i+1] = (value > local_max[i]) ? value : local_max[i];
  }

  stats(VUP(0), VLO(0)) = local_cnt[VALUE_COUNT];
  stats(VUP(1), VLO(1)) = local_sum[VALUE_COUNT];
  stats(VUP(2), VLO(2)) = local_min[VALUE_COUNT];
  stats(VUP(3), VLO(3)) = local_max[VALUE_COUNT];
  return stats;
}


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

    Stats stats = aggregate_row(input.data);
    count += stats(VUP(0), VLO(0));
    sum += stats(VUP(1), VLO(1));
    min = (stats(VUP(2), VLO(2)) < min) ? stats(VUP(2), VLO(2)) : min;
    max = (stats(VUP(3), VLO(3)) > max) ? stats(VUP(3), VLO(3)) : max;

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
