# Metal FS Aggregate Operator

This operator computes the count, sum, minimum and maximum of a stream of incoming integer values.

For general information on Metal FS, please visit the [project website](https://metalfs.github.io).

## Installation
```
npm install @metalfs/aggregate
```

Example `image.json`:
```json
{
    "streamBytes": 8,
    "target": "SNAP/WebPACK_Sim",
    "operators": {
        "aggregate": "@metalfs/aggregate"
    }
}
```

## Operator Details

| | Description |
 -| -
Input  | Stream of 64-bit big endian unsigned integer values
Output | Four 64-bit big endian unsigned integer values: count, sum, min, max
Stream Width | Adaptable

### Options

No options are available.

## See also

 - [Filter Operator](https://github.com/metalfs/filter)
