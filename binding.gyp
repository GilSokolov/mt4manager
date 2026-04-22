{
  "targets": [
    {
      "target_name": "mt4manager",
      "sources": [
        "native/addon.cc",
        "native/mt4_manager_wrap.cc",
        "native/mt4_client.cc",
        "native/async_job.cc",
        "native/memory.cc",
        "native/utils/win32_error.cc",
        "native/utils/mt4_errors.cc",
        "native/users/mt4_users.cc",
        "native/users/mt4_users_wrap.cc",
        "native/users/mt4_user_converter.cc",
        "native/pumping_options_napi.cc"
      ],
       "libraries": [
        "ws2_32.lib"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "include",
        "native"
      ],
      "defines": [
        "NAPI_CPP_EXCEPTIONS",
        # "MT4_DEBUG"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "conditions": [
        ["OS=='win'", {
          "msvs_settings": {
            "VCCLCompilerTool": {
              "ExceptionHandling": 1,
              "AdditionalOptions": ["/std:c++20"]
            }
          }
        }, {
          "cflags_cc": ["-std=c++20"]
        }]
      ]
    }
  ]
}
