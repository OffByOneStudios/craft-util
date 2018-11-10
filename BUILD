cc_library(
    name = "util",
    visibility = ["//visibility:public"],
    srcs = glob([
      "src/util/**/*.cpp",
      "src/util/**/*.h",
      "src/util/**/*.hpp",
    ]),
    includes = ["src"],
    
    deps = ["@spdlog//:spdlog"],
    
    linkopts = ["-DEFAULTLIB:Ws2_32.lib", "-DEFAULTLIB:pathcch.lib", "-DEFAULTLIB:shlwapi.lib", "-DEFAULTLIB:Shell32.lib"]
)

cc_binary(
    name = "test",
    visibility = ["//visibility:public"],
    srcs = ["test/graph.cpp"],
    deps = ["util"],

    copts = ["/std:c++latest"]
)