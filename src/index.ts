export interface MstlFitOptions {
  robust?: boolean;
  trendJump?: number;
  trendLength?: number;
  trendDegree?: number;
  innerLoops?: number;
  outerLoops?: number;
  lowPassJump?: number;
  lowPassLength?: number;
  lowPassDegree?: number;
  seasonalJump?: number;
  seasonalDegree?: number;

  lambda?: number;
  iterations?: number;
}

export type TypedFloatArray = Float32Array | Float64Array;

export interface MstlResult {
  seasonal: TypedFloatArray[];
  trend: TypedFloatArray;
  remainder: TypedFloatArray;
}

const nativeMstl = require("bindings")("mstl") as {
  fit64: (
    series: Float64Array,
    periods: number[] | Uint8Array | Uint16Array | Uint32Array,
    options: MstlFitOptions,
  ) => {
    seasonal: Float64Array[];
    trend: Float64Array;
    remainder: Float64Array;
  };
  fit32: (
    series: Float32Array,
    periods: number[] | Uint8Array | Uint16Array | Uint32Array,
    options: MstlFitOptions,
  ) => {
    seasonal: Float32Array[];
    trend: Float32Array;
    remainder: Float32Array;
  };
};

/**
 * Performs MSTL fitting
 * @param series Array of floating point numbers, typed arrays are preferred for performance reasons
 * @param periods Array of periods for which MSTL is performed, typed arrays are preferred for performance reasons
 * @param [options={}] Parameters of the MSTL algorithm, see possible values in MstlFitOptions
 * @returns MstlResult
 */
export function fit(
  series: number[] | Float32Array | Float64Array,
  periods: number[] | Uint8Array | Uint16Array | Uint32Array,
  options: MstlFitOptions = {},
): MstlResult {
  const seriesTyped = (() => {
    if (Array.isArray(series)) {
      return new Float32Array(series);
    }
    return series;
  })();
  if (seriesTyped instanceof Float64Array) {
    return nativeMstl.fit64(seriesTyped, periods, options);
  }
  return nativeMstl.fit32(seriesTyped, periods, options);
}
