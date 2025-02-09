# mstl-node
Javascript bindings of MSTL based on STL implementation from [ankane/stl-cpp](https://github.com/ankane/stl-cpp)

## Requirements
* C++ compiler compatible with `node-addon-api`
* NodeV20 or higher
* `npm`
* `clang-format` (for pre-commit hook)

## How to use
Install the package:
```sh
npm install --save @adtonos/mstl-node
```
Use it in your code:
```ts
import { fit } from '@adtonos/mstl-node';

// ...
const series = new Float32Array([
  5.0, 9.0, 2.0, 9.0, 0.0, 6.0, 3.0, 8.0, 5.0, 8.0, 7.0, 8.0, 8.0, 0.0, 2.0,
  5.0, 0.0, 5.0, 6.0, 7.0, 3.0, 6.0, 1.0, 4.0, 4.0, 4.0, 3.0, 7.0, 5.0, 8.0,
]);
const results = fit(series, [7, 10]);
results.seasonal
```

## Contributing
Any contributions are welcome, please submit a PR with ready suggestion from your fork and/or create an issue and we will be happy to discuss.

## How to compile/run
```sh
git clone git@github.com:adtonos/mstl-node.git
npm install
npm test
```
