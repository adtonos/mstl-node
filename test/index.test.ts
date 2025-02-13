import assert from "assert";
import { fit, MstlFitOptions } from "../src/index";
import { describe, it } from "mocha";

const generateSeriesF32 = () =>
  new Float32Array([
    5.0, 9.0, 2.0, 9.0, 0.0, 6.0, 3.0, 8.0, 5.0, 8.0, 7.0, 8.0, 8.0, 0.0, 2.0,
    5.0, 0.0, 5.0, 6.0, 7.0, 3.0, 6.0, 1.0, 4.0, 4.0, 4.0, 3.0, 7.0, 5.0, 8.0,
  ]);

const generateSeriesF64 = () =>
  new Float64Array([
    5.0, 9.0, 2.0, 9.0, 0.0, 6.0, 3.0, 8.0, 5.0, 8.0, 7.0, 8.0, 8.0, 0.0, 2.0,
    5.0, 0.0, 5.0, 6.0, 7.0, 3.0, 6.0, 1.0, 4.0, 4.0, 4.0, 3.0, 7.0, 5.0, 8.0,
  ]);

const basicOptions = () => {
  const options: MstlFitOptions = {
    trendLength: 13,
    trendDegree: 1,
    seasonalDegree: 1,
    iterations: 2,
    lowPassLength: 13,
    trendJump: 1,
    lowPassJump: 1,
    seasonalJump: 1,
    robust: false,
    lowPassDegree: 1,
    innerLoops: 2,
    outerLoops: 0,
  };
  return options;
};

const expectArrayElementsClose = (
  actual: ArrayLike<number>,
  expected: ArrayLike<number>,
  tolerance = 0.01,
) => {
  for (let i = 0; i < actual.length; ++i) {
    assert.strictEqual(
      Math.abs(actual[i]! - expected[i]!) < tolerance,
      true,
      `Expected ${actual[i]} to be equal to ${expected[i]} with tolerance of ${tolerance}. Element ${i}`,
    );
  }
};

describe("fit function with real data handling", () => {
  it("handles multiple periods correctly", () => {
    const series = generateSeriesF32();
    const result = fit(series, [7, 10], basicOptions());

    expectArrayElementsClose(
      result.seasonal[0]!.slice(0, 5),
      [1.02957645, 1.58052462, -2.58504053, 3.82336372, -1.37414519],
    );
    expectArrayElementsClose(
      result.seasonal[1]!.slice(0, 5),
      [-1.13068049, 2.4459641, 0.32451697, -0.93648035, -4.19763815],
    );

    expectArrayElementsClose(
      result.trend.slice(0, 5),
      [4.899, 5.027, 5.151, 5.27, 5.387],
    );
    expectArrayElementsClose(
      result.remainder.slice(0, 5),
      [0.20186475, -0.05349705, -0.8779612, 0.84224536, 0.18390715],
    );
  });

  it("handles Cox transformation correctly", () => {
    const series = generateSeriesF64();
    const options: MstlFitOptions = {
      ...basicOptions(),
      lambda: 0.5,
    };
    const result = fit(series, [7, 10], options);

    expectArrayElementsClose(
      result.seasonal[0]!.slice(0, 5),
      [1.03454373, 1.00230502, -1.28675536, 2.36520888, -1.55556466],
    );
    expectArrayElementsClose(
      result.seasonal[1]!.slice(0, 5),
      [-0.73107267, 1.2115821, 0.45351811, -1.36553556, -2.62265205],
    );
    expectArrayElementsClose(
      result.trend.slice(0, 5),
      [1.97986303, 2.05898726, 2.13443353, 2.20569258, 2.27523968],
    );
    expectArrayElementsClose(
      result.remainder.slice(0, 5),
      [0.18880186, -0.27287437, -0.47276916, 0.79463409, -0.09702297],
    );
  });
});
