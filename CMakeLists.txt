include(../ao.common.cmake)

set(CURRENT_TARGET "Utils")

# 添加库目标
add_ao_target(${CURRENT_TARGET} "SHARED")

# 添加依赖
target_link_libraries(${CURRENT_TARGET} PRIVATE dwrite wbemuuid)