{
  "targets": [
    {
      "target_name": "mt4manager",
      "sources": [
        "native/trades/mt4_trades_from_napi.cc",
        "native/trades/mt4_trades_to_napi.cc",
        "native/trades/mt4_trades_wrap.cc",
        "native/trades/mt4_trades.cc",
        "native/trades/trade_request_to_mt4.cc",
        "native/addon.cc",
        "native/mt4_manager_wrap.cc",
        "native/mt4_client.cc",
        "native/mt4_client_config.cc",
        "native/utils/mt4_errors.cc",
        "native/users/mt4_users.cc",
        "native/users/mt4_users_wrap.cc",
        "native/users/mt4_user_to_napi.cc",
        "native/users/mt4_user_from_napi.cc",
        "native/pumping_options_napi.cc",
        "native/symbols/mt4_symbols.cc",
        "native/symbols/mt4_symbols_wrap.cc",
        "native/symbols/mt4_symbol_to_napi.cc",
        "native/symbols/mt4_symbol_convertor.cc",
       
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
