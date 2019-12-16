# Metal FS Aggregate Operator

This operator computes the count, sum, minimum and maximum of a stream of incoming integer values.

## Installation
```
npm install @metalfs/aggregate
```

## Operator Details

| | Description |
 -| -
Input  | Stream of 64-bit big endian unsigned integer values
Output | Four 64-bit big endian unsigned integer values: count, sum, min, max
Stream Width | Adaptable

### Options

No options are available.
