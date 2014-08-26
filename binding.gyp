{
  "targets": [
    {
      "target_name": "registwin",
      "sources": [
        "lib/chromium/base/win/registry.cc",
        "lib/chromium/base/win/registry.h",
        "lib/registwin.cc",
        "lib/registwin.h",
        "lib/util.cc",
        "lib/util.h",
      ],
      'include_dirs': [
        'lib',
        'lib/chromium',
        "<!(node -e \"require('nan')\")",
      ],
      'defines': [
        'UNICODE',
      ],
      'msvs_disabled_warnings': [
        4267,
        4506,
        4530,
      ],
    }
  ]
}