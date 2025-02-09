#include "include/stl.hpp"
#include <napi.h>

std::vector<size_t> js_array_to_uint_vector(const Napi::Array &arr) {
  std::vector<size_t> vec;
  for (uint32_t i = 0; i < arr.Length(); i++) {
    vec.push_back(arr.Get(i).As<Napi::Number>().Uint32Value());
  }
  return vec;
}

std::vector<size_t>
js_typed_array_to_size_t_vector(const Napi::TypedArray &arr) {
  std::vector<size_t> vec;
  for (uint32_t i = 0; i < arr.ElementLength(); i++) {
    vec.push_back(arr.Get(i).ToNumber().Uint32Value());
  }
  return vec;
}

Napi::Object mstl_result_to_js(Napi::Env env, stl::MstlResult<float> &&result) {
  Napi::Object obj = Napi::Object::New(env);

  const auto convert_to_float_typed_array =
      [env](std::unique_ptr<std::vector<float>> in) {
        const auto length = in->size();
        const auto array_buff = Napi::ArrayBuffer::New(
            env, in->data(), length * sizeof(float),
            [](Napi::Env, void *, std::vector<float> *hint) {
              std::unique_ptr<std::vector<float>> vectorPtrToDelete(hint);
            },
            in.get());
        static_cast<void>(in.release());
        return Napi::Float32Array::New(env, length, array_buff, 0);
      };

  auto trend_ptr =
      std::make_unique<std::vector<float>>(std::move(result.trend));
  auto remainder_ptr =
      std::make_unique<std::vector<float>>(std::move(result.remainder));

  Napi::Array seasonal_arr = Napi::Array::New(env, result.seasonal.size());
  for (size_t i = 0; i < result.seasonal.size(); i++) {
    auto seasonal_ptr =
        std::make_unique<std::vector<float>>(std::move(result.seasonal[i]));
    seasonal_arr.Set(i, convert_to_float_typed_array(std::move(seasonal_ptr)));
  }

  obj.Set("trend", convert_to_float_typed_array(std::move(trend_ptr)));
  obj.Set("seasonal", seasonal_arr);
  obj.Set("remainder", convert_to_float_typed_array(std::move(remainder_ptr)));

  return obj;
}

Napi::Object mstl_result_to_js(Napi::Env env,
                               stl::MstlResult<double> &&result) {
  Napi::Object obj = Napi::Object::New(env);
  const auto convert_to_float_typed_array =
      [env](std::unique_ptr<std::vector<double>> in) {
        const auto length = in->size();
        const auto array_buff = Napi::ArrayBuffer::New(
            env, in->data(), length * sizeof(double),
            [](Napi::Env, void *, std::vector<double> *hint) {
              std::unique_ptr<std::vector<double>> vectorPtrToDelete(hint);
            },
            in.get());
        static_cast<void>(in.release());
        return Napi::Float64Array::New(env, length, array_buff, 0);
      };

  auto trend_ptr =
      std::make_unique<std::vector<double>>(std::move(result.trend));
  auto remainder_ptr =
      std::make_unique<std::vector<double>>(std::move(result.remainder));
  Napi::Array seasonal_arr = Napi::Array::New(env, result.seasonal.size());
  for (size_t i = 0; i < result.seasonal.size(); i++) {
    auto seasonal_ptr =
        std::make_unique<std::vector<double>>(std::move(result.seasonal[i]));
    seasonal_arr.Set(i, convert_to_float_typed_array(std::move(seasonal_ptr)));
  }

  obj.Set("trend", convert_to_float_typed_array(std::move(trend_ptr)));
  obj.Set("seasonal", seasonal_arr);
  obj.Set("remainder", convert_to_float_typed_array(std::move(remainder_ptr)));

  return obj;
}
template <typename T, napi_typedarray_type AType>
Napi::Object fit(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  // convert all exceptions into javascript ones
  try {
    if (!info[0].IsTypedArray()) {
      Napi::Error::New(env, "Expected series to be typed array")
          .ThrowAsJavaScriptException();
      return Napi::Object::New(env);
    }
    if (info[0].As<Napi::TypedArray>().TypedArrayType() != AType) {
      Napi::Error::New(env, "Expected series to be typed array of float64")
          .ThrowAsJavaScriptException();
      return Napi::Object::New(env);
    }
    if (!info[1].IsTypedArray() && !info[1].IsArray()) {
      Napi::Error::New(env,
                       "Expected periods to be typed array or javascript array")
          .ThrowAsJavaScriptException();
      return Napi::Object::New(env);
    }
    const auto series = info[0].As<T>();
    const auto periods =
        info[1].IsArray()
            ? js_array_to_uint_vector(info[1].As<Napi::Array>())
            : js_typed_array_to_size_t_vector(info[1].As<Napi::TypedArray>());
    stl::MstlParams params;
    if (info[2].IsObject()) {
      const auto options_obj = info[2].As<Napi::Object>();
      stl::StlParams stl_params;

      if (options_obj.Has("robust")) {
        const auto value = options_obj["robust"];
        if (!value.IsBoolean()) {
          Napi::Error::New(env, "options.robust must be a boolean")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        stl_params.robust(value.As<Napi::Boolean>().Value());
      }

      if (options_obj.Has("trendJump")) {
        const auto value = options_obj["trendJump"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.trendJump must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        stl_params.trend_jump(value.As<Napi::Number>().Uint32Value());
      }

      if (options_obj.Has("trendDegree")) {
        const auto value = options_obj["trendDegree"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.trendDegree must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        stl_params.trend_degree(value.As<Napi::Number>().Uint32Value());
      }

      if (options_obj.Has("trendLength")) {
        const auto value = options_obj["trendLength"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.trendLength must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        stl_params.trend_length(value.As<Napi::Number>().Uint32Value());
      }

      if (options_obj.Has("innerLoops")) {
        const auto value = options_obj["innerLoops"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.innerLoops must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        stl_params.inner_loops(value.As<Napi::Number>().Uint32Value());
      }

      if (options_obj.Has("outerLoops")) {
        const auto value = options_obj["outerLoops"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.outerLoops must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        stl_params.outer_loops(value.As<Napi::Number>().Uint32Value());
      }

      if (options_obj.Has("lowPassJump")) {
        const auto value = options_obj["lowPassJump"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.lowPassJump must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        stl_params.low_pass_jump(value.As<Napi::Number>().Uint32Value());
      }

      if (options_obj.Has("lowPassDegree")) {
        const auto value = options_obj["lowPassDegree"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.lowPassDegree must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        stl_params.low_pass_degree(value.As<Napi::Number>().Uint32Value());
      }

      if (options_obj.Has("lowPassLength")) {
        const auto value = options_obj["lowPassLength"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.lowPassLength must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        stl_params.low_pass_length(value.As<Napi::Number>().Uint32Value());
      }

      if (options_obj.Has("seasonalJump")) {
        const auto value = options_obj["seasonalJump"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.seasonalJump must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        stl_params.seasonal_jump(value.As<Napi::Number>().Uint32Value());
      }

      if (options_obj.Has("seasonalDegree")) {
        const auto value = options_obj["seasonalDegree"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.seasonalDegree must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        stl_params.seasonal_degree(value.As<Napi::Number>().Uint32Value());
      }

      if (options_obj.Has("lambda")) {
        const auto value = options_obj["lambda"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.lambda must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        params.lambda(value.As<Napi::Number>().FloatValue());
      }

      if (options_obj.Has("iterations")) {
        const auto value = options_obj["iterations"];
        if (!value.IsNumber()) {
          Napi::Error::New(env, "options.iterations must be a number")
              .ThrowAsJavaScriptException();
          return Napi::Object::New(env);
        }
        params.iterations(value.As<Napi::Number>().Uint32Value());
      }

      params.stl_params(stl_params);
    }
    auto result = params.fit(series.Data(), series.ElementLength(),
                             periods.data(), periods.size());
    return mstl_result_to_js(env, std::move(result));
  } catch (const std::invalid_argument &e) {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    return Napi::Object::New(env);
  }
}

// Initialize module
Napi::Object init(Napi::Env env, Napi::Object exports) {
  exports.Set("fit64", Napi::Function::New(
                           env, fit<Napi::Float64Array, napi_float64_array>));
  exports.Set("fit32", Napi::Function::New(
                           env, fit<Napi::Float32Array, napi_float32_array>));
  return exports;
}

NODE_API_MODULE(addon, init)
