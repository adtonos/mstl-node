{
  "targets": [
    {
      "target_name": "mstl",
      "sources": ["native/mstl.cpp"],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "cflags_cc": ["-std=c++20", "-fexceptions"],
    }
  ]
}

